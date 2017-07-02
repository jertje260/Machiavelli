#pragma once
#include "Game.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;


Game::Game()
{
	cerr << "Creating a new game\n";
	Running = false;
	Active = true;
	LoadResources();
}


Game::~Game()
{
}

bool Game::IsAvailable()
{
	return Players.size() < 2;
}

bool Game::IsStarted()
{
	return Running;
}

std::vector<std::shared_ptr<Player>> Game::GetCurrentPlayers()
{
	return Players;
}

void Game::AddPlayer(std::shared_ptr<Player> p)
{
	Players.push_back(p);
	King = p;
	cerr << "Player " << p->GetName() << " joined the game\n";
}


std::shared_ptr<Player> Game::GetCurrentPlayer()
{
	return currentPlayer;
}

void Game::HandleCommand(std::shared_ptr<Player> player, std::string command)
{
	if (command == "hand") {
		player->ShowHandCards();
	}
	else if (command == "board") {
		player->ShowBuiltBuildings();
	}
	else if (command == "chars") {
		player->ShowCharacterCards();
	}
	else {
		cerr << "Dont know what to do with: " << command;
	}
}

void Game::Start()
{
	Running = true;
	auto p1 = Players[0];
	auto p2 = Players[1];

	// players draw their build cards.
	for (int i = 0; i < 4; i++)
	{
		DrawBuildCard(p1);
		DrawBuildCard(p2);
	}
	// give the players their coins
	GiveGold(p1, 2);
	GiveGold(p2, 2);

	TellAll("The game has started. It will be " + p1->GetName() + " vs " + p2->GetName() + ".\r\n");
	TellAll("You have been given four build cards and 2 gold coins.\r\n");
	p1->GiveOverview();
	p2->GiveOverview();

	SetupRound();
}

void Game::Quit(std::shared_ptr<Player> p)
{
	std::shared_ptr<Socket> client;
	if (Players.size() > 1) {
		if (Players[0] == p) {
			client = Players[1]->GetClient();
		}
		else {
			client = Players[0]->GetClient();
		}

		client->write("Your opponent " + p->GetName() + " quit. Now stopping game and disconnecting you from the server.\r\n");
	}
	Active = false;
	Running = false;
}

void Game::Quit()
{
	for each (std::shared_ptr<Player> p in Players)
	{
		p->GetClient()->write("Server Quitting, stopping all games.\r\n");
	}
	Active = false;
	Running = false;
}

void Game::ExecuteSpecial(shared_ptr<Bouwmeester> spec)
{
	//bouwmeester has no special action
	spec->DoneSpecial = true;
}

void Game::ExecuteSpecial(shared_ptr<Koopman> spec)
{
	//koopman has no special action
	spec->DoneSpecial = true;
}

void Game::ExecuteSpecial(shared_ptr<Magier> spec)
{
	string message = "You are the " + CharacterToString(spec->Type) + ". You have " + to_string(currentPlayer->GetHandCards().GetDeckSize()) + " cards and " + notCurrentPlayer->GetName() + " has " + to_string(notCurrentPlayer->GetHandCards().GetDeckSize()) + " cards.\r\n";
	message += "[1] Switch cards with " + notCurrentPlayer->GetName() + "\r\n[2] Switch cards with the deck\r\n";
	Tell(currentPlayer, message);
	string response = currentPlayer->GetResponse();

	try {
		int num = stoi(response);

		switch (num)
		{
		case 1: // switch with other player
		{
			auto cards = currentPlayer->GetHandCards();
			currentPlayer->SetHandCards(notCurrentPlayer->GetHandCards());
			notCurrentPlayer->SetHandCards(cards);
			Tell(currentPlayer, "You switched cards with " + notCurrentPlayer->GetName() + ".\r\n");
			currentPlayer->ShowHandCards();
			Tell(notCurrentPlayer, "Your cards have been switched.\r\n");
			notCurrentPlayer->ShowHandCards();

			spec->DoneSpecial = true;
		}
		case 2: // switch with the deck
		{
			bool done = false;
			while (!done && Active)
			{
				string m = "Select which cards you want to switch by typing their numbers sperated with a space.\r\n";
				size_t decksize = currentPlayer->GetHandCards().GetDeckSize();
				for (size_t j = 0; j < decksize; j++) {
					m += "[" + to_string(j + 1) + "] " + currentPlayer->GetHandCards().GetDeck()[j]->GetCardInfo() + "\r\n";
				}
				Tell(currentPlayer, m);
				string response = currentPlayer->GetResponse();
				string workingResponse = response;
				size_t pos = 0;
				vector<int> cardsToSwitch;
				try {
					while ((pos = response.find(' ')) != string::npos) {
						int num = stoi(workingResponse.substr(0, pos)) - 1;
						if (num >= decksize) {
							throw invalid_argument("number " + to_string(num) + " is not a card in your deck.\r\n");
						}

						cardsToSwitch.push_back(num);

						workingResponse.erase(0, pos + 1);
					}
					sort(cardsToSwitch.rbegin(), cardsToSwitch.rend());
					for (size_t j = 0; j < cardsToSwitch.size(); j++) {
						auto card = currentPlayer->GetHandCards().Pop(cardsToSwitch[j]);
						disposedCards.AddCard(card);
						currentPlayer->AddCard(buildCards.Pop());
					}
					done = true;
				}
				catch (invalid_argument e) {
					Tell(currentPlayer, e.what());
					continue;
				}
				catch (...) {
					Tell(currentPlayer, WrongInput(response));
					continue;
				}

			}
			Tell(currentPlayer, "You switched cards with the deck.\r\n");
			Tell(notCurrentPlayer, currentPlayer->GetName() + " switched cards with the deck.\r\n");
			currentPlayer->ShowHandCards();

			spec->DoneSpecial = true;
		}
		default:
			Tell(currentPlayer, WrongInput(response));
			return;
		}


	}
	catch (exception e) {
		Tell(currentPlayer, WrongInput(response));
		return;
	}
}

void Game::ExecuteSpecial(shared_ptr<Condottiere> spec)
{
	if (!notCurrentPlayer->HasCharacter(CharacterType::Prediker) || notCurrentPlayer->GetBuildings().GetDeckSize() < 8) {
		string message = "You are the " + CharacterToString(spec->Type) + ". You have " + to_string(currentPlayer->GetGold()) + " gold. Which building do you want to destroy?\r\n";
		message += "[0] None;";
		for (size_t i = 0; i < notCurrentPlayer->GetBuildings().GetDeckSize(); i++) {
			auto building = notCurrentPlayer->GetBuildings().GetDeck()[i];
			message += "[" + to_string(i + 1) + "] " + building->GetName() + " cost: " + to_string(building->GetCoins() - 1) + " points: " + to_string(building->GetCoins()) + "\r\n";
		}
		Tell(currentPlayer, message);
		string response = currentPlayer->GetResponse();

		try {
			int num = stoi(response);
			if (num == 0) {
				Tell(currentPlayer, "You decided not to destroy a building of " + notCurrentPlayer->GetName() + ".\r\n");
				Tell(notCurrentPlayer, currentPlayer->GetName() + " did not destroy one of your buildings.\r\n");
				spec->DoneSpecial = true;
			}
			else {
				num--;
				if (num >= 0 && num < notCurrentPlayer->GetBuildings().GetDeckSize()) {
					auto building = notCurrentPlayer->GetBuildings().GetDeck()[num];
					if (currentPlayer->GetGold() >= building->GetCoins() - 1) {
						currentPlayer->AddGold(-(building->GetCoins() - 1));
						goldPiecesLeft += (building->GetCoins() - 1);

						notCurrentPlayer->GetBuildings().RemoveCard(building);
						disposedCards.AddCard(building);
						spec->DoneSpecial = true;
					}
					else {
						throw invalid_argument("You dont have enough gold to do that.\r\n");
					}



				}
				else {
					Tell(currentPlayer, WrongInput(response));
				}
			}
		}
		catch (invalid_argument e) {
			Tell(currentPlayer, e.what());
		}
		catch (exception e) {
			Tell(currentPlayer, WrongInput(response));
		}
	}
	else {
		Tell(currentPlayer, notCurrentPlayer->GetName() + " is immune to the " + CharacterToString(spec->Type) + ". So there is no use for your special :(\r\n");
		Tell(notCurrentPlayer, "You are immune to the " + CharacterToString(spec->Type) + ". So he cannot destroy any of your buildings.\r\n");
		spec->DoneSpecial = true;
	}
}

void Game::ExecuteSpecial(shared_ptr<Koning> spec)
{
	//koning has no special action
	spec->DoneSpecial = true;
}

void Game::ExecuteSpecial(shared_ptr<Dief> spec)
{
	string message = "You are the " + CharacterToString(spec->Type) + ", Who do you want to steal?\r\n";
	for (size_t i = 2; i < characterOrder.size(); i++) {
		message += "[" + to_string(i - 1) + "] " + CharacterToString(characterOrder[i]) + "\r\n";
	}
	Tell(currentPlayer, message);
	string response = currentPlayer->GetResponse();

	try {
		int num = stoi(response);
		num++;
		if (num > 1 && num < characterOrder.size() - 1) {

			for each (auto c in notCurrentPlayer->GetCharacters().GetDeck())
			{
				if (c->Type == characterOrder[num]) {
					c->Stolen = true;
					Tell(currentPlayer, "You have stolen from the " + CharacterToString(characterOrder[num]) + ".\r\n");
					Tell(notCurrentPlayer, "The " + CharacterToString(characterOrder[num]) + " has been stolen.\r\n");
					break;
				}
			}
			spec->DoneSpecial = true;

		}
		else {
			Tell(currentPlayer, WrongInput(response));
		}

	}
	catch (exception e) {
		Tell(currentPlayer, WrongInput(response));
		return;
	}

}

void Game::ExecuteSpecial(shared_ptr<Moordenaar> spec)
{
	string message = "You are the" + CharacterToString(spec->Type) + ", Who do you want to kill?\r\n";
	for (size_t i = 1; i < characterOrder.size(); i++) {
		message += "[" + to_string(i) + "] " + CharacterToString(characterOrder[i]) + "\r\n";
	}
	Tell(currentPlayer, message);
	string response = currentPlayer->GetResponse();

	try {
		int num = stoi(response);
		if (num > 0 && num < characterOrder.size() - 1) {
			for each (auto c in notCurrentPlayer->GetCharacters().GetDeck())
			{
				if (c->Type == characterOrder[num]) {
					c->Killed = true;
					Tell(currentPlayer, "You have murdered the " + CharacterToString(characterOrder[num]) + ".\r\n");
					Tell(notCurrentPlayer, "The " + CharacterToString(characterOrder[num]) + " has been murdered.\r\n");
					break;
				}
			}
			spec->DoneSpecial = true;

		}
		else {
			Tell(currentPlayer, WrongInput(response));
		}

	}
	catch (exception e) {
		Tell(currentPlayer, WrongInput(response));
		return;
	}

}
void Game::ExecuteSpecial(shared_ptr<Prediker> spec)
{
	//prediker has no special action
	spec->DoneSpecial = true;
}
void Game::ExecuteSpecial(shared_ptr<CharacterCard> spec)
{
	//charactercard doesnt exist.
	spec->DoneSpecial = true;
}

void Game::LoadResources()
{
	cerr << "Creating build deck\r\n";
	CreateBuildDeck();
	cerr << "Created build deck " << buildCards.GetDeck().size() << " \r\n";
	cerr << "Creating Character deck\r\n";
	CreateCharacterDeck();
	cerr << "Created Character deck " << characters.GetDeck().size() << " \r\n";
}

void Game::FinishGame(shared_ptr<Player> p)
{
	auto p1Buildings = currentPlayer->GetBuildings();
	int p1bonuspoints = 0;
	int p1BuildingTypes = currentPlayer->GetBuildingTypes();
	auto p2Buildings = notCurrentPlayer->GetBuildings();
	int p2bonuspoints = 0;
	int p2BuildingTypes = notCurrentPlayer->GetBuildingTypes();
	string message = currentPlayer->GetName() + " has " + to_string(p1Buildings.GetDeckSize()) + " buildings of " + to_string(p1BuildingTypes) + " colors.\r\n";
	message += "The buildings give " + to_string(currentPlayer->GetPoints()) + " points.\r\n";
	if (p1BuildingTypes == 5) {
		p1bonuspoints += 3;
	}
	if (p == currentPlayer) {
		message += currentPlayer->GetName() + " was the first with 8 buildings, therefore (s)he gets 4 bonus points\r\n";
		p1bonuspoints += 4;
	}
	else if (p1Buildings.GetDeckSize() >= 8) {
		message += currentPlayer->GetName() + " has 8 buildings but was not the first, therefore (s)he gets 2 bonus points\r\n";
		p1bonuspoints += 2;
	}

	message += notCurrentPlayer->GetName() + " has " + to_string(p2Buildings.GetDeckSize()) + " buildings of " + to_string(p2BuildingTypes) + " colors.\r\n";
	message += "The buildings give " + to_string(notCurrentPlayer->GetPoints()) + " points.\r\n";
	if (p2BuildingTypes == 5) {
		p2bonuspoints += 3;
	}
	if (p == notCurrentPlayer) {
		message += notCurrentPlayer->GetName() + " was the first with 8 buildings, therefore (s)he gets 4 bonus points\r\n";
		p2bonuspoints += 4;
	}
	else if (p2Buildings.GetDeckSize() >= 8) {
		message += notCurrentPlayer->GetName() + " has 8 buildings but was not the first, therefore (s)he gets 2 bonus points\r\n";
		p2bonuspoints += 2;
	}
	string winner = "";
	if (currentPlayer->GetPoints() + p1bonuspoints == notCurrentPlayer->GetPoints() + p2bonuspoints) {
		if (currentPlayer->GetPoints() > notCurrentPlayer->GetPoints()) {
			winner = currentPlayer->GetName();
		}
		else {
			winner = notCurrentPlayer->GetName();
		}
	}
	else if(currentPlayer->GetPoints() + p1bonuspoints > notCurrentPlayer->GetPoints() + p2bonuspoints){
		winner = currentPlayer->GetName();
	}
	else {
		winner = notCurrentPlayer->GetName();
	}

	message += currentPlayer->GetName() + ": " + to_string(currentPlayer->GetPoints()) + " + " + to_string(p1bonuspoints) + " bonus VS " + notCurrentPlayer->GetName() + ": " + to_string(notCurrentPlayer->GetPoints()) + " + " + to_string(p2bonuspoints) + " bonus\r\n";
	message += "The winner is: " + winner + "!\r\n";
	message += "The game is over, the connection will be terminated.\r\n";

	TellAll(message);
	Active = false;
	Running = false;
}

void Game::SetupRound()
{
	currentPlayer = King;

	cerr << "Current player is: " << currentPlayer->GetName() << "\r\n";

	// resetting character cards.
	while (disposedCharacters.GetDeckSize() > 0)
	{
		characters.AddCard(disposedCharacters.Pop());
	}
	for each (auto p in Players)
	{
		while (p->GetCharacters().GetDeckSize() > 0) {
			characters.AddCard(p->GetCharacters().Pop());
		}
	}
	for each (auto c in characters.GetDeck()) {
		c->Reset();
	}
	cerr << "Reset the characters: " << characters.GetDeckSize() << "\r\n";
	ChooseCharacters();

}

void Game::ChooseCharacters() {
	while (characters.GetDeckSize() > 0) {
		auto c = characters.Pop();
		string info = "You draw the top card: " + CharacterToString(c->Type) + " and put it away, now choose one of the following cards.\r\n";
		disposedCharacters.AddCard(c);
		for (size_t i = 0; i < characters.GetDeckSize(); i++) {
			info += "[" + to_string(i + 1) + "] " + CharacterToString(characters.GetDeck()[i]->Type) + "\r\n";
		}
		info += "Pick by telling the card number you want.\r\n";
		Tell(currentPlayer, info);
		bool goodResponse = false;
		while (!goodResponse && Active) {
			auto response = currentPlayer->GetResponse();
			try {
				int num = stoi(response) - 1;
				if (num >= 0 && num < characters.GetDeckSize()) {
					currentPlayer->AddCharacter(characters.Pop(num));
					goodResponse = true;
					characters.Shuffle();
				}
				else {
					Tell(currentPlayer, WrongInput(response));
					continue;
				}
			}
			catch (exception e) {
				Tell(currentPlayer, WrongInput(response));
				continue;
			}

		}
		SwitchPlayer();

	}
	PlayRounds();
}

void Game::PlayRounds()
{
	shared_ptr<Player> firstWinner;
	bool finished = false;
	for (size_t i = 0; i < characterOrder.size(); i++) {
		TellAll("King " + King->GetName() + ": " + CharacterToString(characterOrder[i]) + " show yourself!\r\n");
		bool switched = false;
		shared_ptr<CharacterCard> currentChar;
		for each (auto p in Players)
		{
			currentChar = p->GetCharacter(characterOrder[i]);
			if (currentChar != nullptr) {
				currentPlayer = p;
				if (characterOrder[i] == CharacterType::Koning) {
					King = currentPlayer;
				}
				switched = true;
				Tell(p, "You have this card, it is your turn.\r\n");
				Tell(notCurrentPlayer, p->GetName() + " has this card, it is his/her turn.\r\n");
			}
		}

		if (!switched) {
			TellAll("The " + CharacterToString(characterOrder[i]) + " did not respond to " + King->GetName() + "\'s call.\r\n");
		}
		else {
#pragma region First Action
			if (currentChar->Stolen) {
				if (notCurrentPlayer->HasCharacter(CharacterType::Dief)) {
					auto gold = currentPlayer->GetGold();
					currentPlayer->AddGold(-gold);
					notCurrentPlayer->AddGold(gold);
					Tell(currentPlayer, "You have been stolen by " + notCurrentPlayer->GetName() + " for " + to_string(gold) + "coins.\r\n");
					Tell(notCurrentPlayer, "You have stolen " + to_string(gold) + " coins from " + currentPlayer->GetName() + ".\r\n");
				}
			}
			GetBonusCoins(currentChar, currentPlayer);

			bool first = false;
			while (!first && Active) {
				string message = "You have " + to_string(currentPlayer->GetGold()) + " coins\r\n";
				message += "What would you like to do?\r\n";
				message += "[1] Get 2 gold coins.\r\n";
				message += "[2] Get 1 building card\r\n";
				message += "[3] Check character cards\r\n";
				message += "[4] Check building cards\r\n";
				message += "[5] Check your built buildings\r\n";
				message += "[6] Check " + notCurrentPlayer->GetName() + " built buildings\r\n";
				message += "[7] Check all built buildings\r\n";

				auto response = currentPlayer->GetResponse();
				int choice;
				try {
					choice = stoi(response);
				}
				catch (exception e) {
					Tell(currentPlayer, WrongInput(response));
					continue;
				}
				switch (choice)
				{
				case 1: {
					GiveGold(currentPlayer, 2);
					Tell(currentPlayer, "You took 2 gold coins.\r\n");
					Tell(notCurrentPlayer, currentPlayer->GetName() + " took 2 gold coins.\r\n");
					first = true;
				}
				case 2: {
					auto card1 = buildCards.Pop();
					auto card2 = buildCards.Pop();
					bool cardChosen = false;
					while (!cardChosen && Active) {
						string choice = "You drew 2 cards, please make your choice by typing the number.\r\n";
						choice += "[1]" + card1->GetCardInfo() + "\r\n";
						choice += "[2]" + card2->GetCardInfo() + "\r\n";

						Tell(currentPlayer, choice);
						auto response = currentPlayer->GetResponse();
						int res;
						try {
							res = stoi(response);
						}
						catch (exception e) {
							Tell(currentPlayer, WrongInput(response));
							continue;
						}
						if (res != 1 || res != 2) {
							Tell(currentPlayer, WrongInput(response));
							continue;
						}
						else {
							if (res == 1) {
								currentPlayer->AddCard(card1);
								disposedCards.AddCard(card2);
							}
							else {
								currentPlayer->AddCard(card2);
								disposedCards.AddCard(card1);
							}
							cardChosen = true;
						}
					}

					Tell(notCurrentPlayer, currentPlayer->GetName() + " drew a card.\r\n");
					first = true;
				}
				case 3:
					currentPlayer->ShowCharacterCards();
				case 4:
					currentPlayer->ShowHandCards();
				case 5:
					currentPlayer->ShowBuiltBuildings();
				case 6:
				{
					auto built = notCurrentPlayer->BuiltBuildings();
					built = notCurrentPlayer->GetName() + " has built the following buildings, scoring " + to_string(notCurrentPlayer->GetPoints()) + " points\r\n" + built;
					Tell(currentPlayer, built);
				}
				case 7:
				{
					auto builtncp = notCurrentPlayer->BuiltBuildings();
					builtncp = notCurrentPlayer->GetName() + " has built the following buildings, scoring " + to_string(notCurrentPlayer->GetPoints()) + " points\r\n" + builtncp;
					auto built = currentPlayer->BuiltBuildings();
					built = "You built the following buildings, scoring " + to_string(currentPlayer->GetPoints()) + " points\r\n" + built;
					Tell(currentPlayer, builtncp + built);
				}
				default:
					Tell(currentPlayer, WrongInput(response));
					continue;
				}

			}
#pragma endregion
#pragma region Second Action
			bool second = false;
			int amountOfBuildingsToBuild = 1;
			if (currentChar->Type == CharacterType::Bouwmeester) {
				amountOfBuildingsToBuild = 3;
			}
			while (!second && Active) {
				string q = "You have " + to_string(currentPlayer->GetGold()) + " gold and you can build " + to_string(amountOfBuildingsToBuild) + " buildings. Do you want to build a building?\r\n";
				q += "[1] Yes.\r\n[2] No.\r\n";
				auto response = currentPlayer->GetResponse();
				int val;
				try {
					val = stoi(response);
				}
				catch (exception e) {
					Tell(currentPlayer, WrongInput(response));
					continue;
				}
				switch (val)
				{
				case 1:
				{

					string canBuild = "Which of the following buildings do you want to build?.\r\n[0] None\r\n";
					for (size_t j = 0; j < currentPlayer->GetHandCards().GetDeckSize(); j++) {
						canBuild += "[" + to_string(j + 1) + "] " + currentPlayer->GetHandCards().GetDeck()[j]->GetCardInfo() + "\r\n";
					}
					Tell(currentPlayer, canBuild);
					bool goodResponse = false;
					while (!goodResponse && Active) {
						auto res = currentPlayer->GetResponse();
						int val;
						try {
							val = stoi(res);
						}
						catch (exception e) {
							Tell(currentPlayer, WrongInput(res));
							continue;
						}
						if (val < 0 || val > currentPlayer->GetHandCards().GetDeckSize()) {
							Tell(currentPlayer, WrongInput(res));
							continue;
						}
						try {
							Build(val);

							goodResponse = true;
							Tell(currentPlayer, "You build the building. Now you have " + to_string(currentPlayer->GetPoints()) + " points.\rn");
							Tell(notCurrentPlayer, currentPlayer->GetName() + " has built a building and has" + to_string(currentPlayer->GetPoints()) + " points\r\n");
						}
						catch (exception e) {
							Tell(currentPlayer, "You dont have enough gold coins to build that building.");
							continue;
						}

					}
				}
				case 2: {
					Tell(currentPlayer, "You wont build a building this round.");
					Tell(notCurrentPlayer, currentPlayer->GetName() + " did not build a building.");
					second = true;
				}
				default:
					Tell(currentPlayer, WrongInput(response));
					continue;
				}
			}
#pragma endregion
#pragma region Special Action
			while (!currentChar->DoneSpecial && Active) {
				ExecuteSpecial(currentChar);
			}
#pragma endregion
			if (!finished && currentPlayer->GetBuildings().GetDeckSize() >= 8) {
				finished = true;
				firstWinner = currentPlayer;
				Tell(currentPlayer, "You have built eight or more buildings. This will be the final round.\r\n");
				Tell(notCurrentPlayer, currentPlayer->GetName() + " has built eigh or more buildings. This will be the final round.\r\n");
			}

		}

	}
	if (currentPlayer->HasCharacter(CharacterType::Koning)) {
		King = currentPlayer;
	}
	else if (notCurrentPlayer->HasCharacter(CharacterType::Koning)) {
		King = notCurrentPlayer;
	}
	if (!finished) {
		SetupRound();
	}
	else {
		FinishGame(firstWinner);
	}
}

void Game::SwitchPlayer()
{
	if (currentPlayer == Players[0]) {
		currentPlayer = Players[1];
		notCurrentPlayer = Players[0];
	}
	else {
		currentPlayer = Players[0];
		notCurrentPlayer = Players[1];
	}
}

void Game::DrawBuildCard(std::shared_ptr<Player> p)
{
	p->AddCard(buildCards.Pop());
}

bool Game::GiveGold(std::shared_ptr<Player> p, int amount)
{
	if (goldPiecesLeft >= amount) {
		p->AddGold(amount);
		goldPiecesLeft -= amount;

		return true;
	}
	else {
		return false;
	}
}

void Game::Tell(std::shared_ptr<Player> p, std::string what)
{
	p->GetClient()->write(what);
}

void Game::TellAll(std::string what)
{
	for each (std::shared_ptr<Player> p in Players)
	{
		Tell(p, what);
	}
}

void Game::CreateBuildDeck() {
	auto info = ReadCsv("Bouwkaarten.csv");
	auto factory = CardFactory::Get();

	for (vector<vector<string>>::size_type i = 0; i != info.size(); i++) {
		auto card = factory->CreateBuildCard(info[i][0], info[i][1], info[i][2]);
		if (info[i].size() > 3) {
			auto c = card.get();
			c->SetDescription(info[i][3]);
		}
		buildCards.AddCard(card);

	}
	buildCards.Shuffle();
}

void Game::CreateCharacterDeck() {
	auto info = ReadCsv("karakterkaarten.csv");

	auto factory = CardFactory::Get();

	for (vector<vector<string>>::size_type i = 0; i != info.size(); i++) {
		auto card = factory->CreateCharacterCard(info[i][1]);
		characters.AddCard(card);
	}
	characters.Shuffle();
}

void Game::Build(int number)
{
	auto card = currentPlayer->GetHandCards().GetDeck()[number];

	currentPlayer->AddGold(-card->GetCoins());
	goldPiecesLeft += card->GetCoins();
	currentPlayer->Build(number);
}

void Game::GetBonusCoins(shared_ptr<CharacterCard> c, shared_ptr<Player> p)
{
	int points = 0;
	switch (c->Type)
	{
	case CharacterType::Koning:
		points = p->GetPoints(CardColor::yellow);
	case CharacterType::Prediker:
		points = p->GetPoints(CardColor::blue);
	case CharacterType::Koopman:
		points = 1 + p->GetPoints(CardColor::green);
	default:
		break;
	}

	while (points > 0 && goldPiecesLeft > 0) {
		goldPiecesLeft--;
		points--;
		p->AddGold(1);
	}

}

vector<vector<string>> Game::ReadCsv(string path) {
	ifstream infile(path);
	vector<vector<string>> returnValue;
	while (infile)
	{
		string line;
		vector<string> values;
		if (!getline(infile, line)) break;

		stringstream iss(line);

		while (iss)
		{
			string value;
			if (!getline(iss, value, ';')) break;

			values.push_back(value);

		}
		returnValue.push_back(values);

	}

	return returnValue;
}


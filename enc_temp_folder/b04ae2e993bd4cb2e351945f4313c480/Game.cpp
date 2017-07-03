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
	string name = "Player ";
	name += to_string(Players.size() + 1);
	p->SetName(name);
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
	Tell(player, "\r");
	if (command == "hand") {
		player->ShowHandCards();
	}
	else if (command == "cmds") {
		Tell(player, "\rThe following commands are there beside game state based ones:\r\n\'quit\': quits the game\r\n\'hand\': shows your hand cards\r\n\'chars\': shows your characters\r\n\'board\': shows your buildings on the board.\r\n");
	}
	else if (command == "board") {
		player->ShowBuiltBuildings();
	}
	else if (command == "chars") {
		player->ShowCharacterCards();
	}
	// starts with "name"
	else if (strncmp(command.c_str(), string("name").c_str(),4)==0) {
		//split on space
		string name = "";
		size_t pos = 0;
		if ((pos = command.find(' ')) != string::npos) {
			name = command.substr(pos+1, command.size());
		}
		player->SetName(name);
	}
	else if (player == currentPlayer) {
		StateCommand(command);
	}
	else {
		Tell(player, "Your command \'" + command + "\' cannot be processed. Try \'cmds\' for more info.\r\n");
		cerr << "Dont know what to do with: " << command << "\r\n";
	}
	Tell(player, "\rMachiavelli> ");
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
	Players.clear();
	King = nullptr;
	currentPlayer = nullptr;
	notCurrentPlayer = nullptr;
	firstWinner = nullptr;
	Active = false;
	Running = false;
}

void Game::Quit()
{
	for each (std::shared_ptr<Player> p in Players)
	{
		p->GetClient()->write("Server Quitting, stopping all games.\r\n");
	}
	Players.clear();
	King = nullptr;
	currentPlayer = nullptr;
	notCurrentPlayer = nullptr;
	firstWinner = nullptr;
	Active = false;
	Running = false;
}
#pragma region Specials
void Game::BuilderSpec()
{
	//bouwmeester has no special action
	currentChar->DoneSpecial = true;
	SpecPhase();
}

void Game::MerchantSpec()
{
	//koopman has no special action
	currentChar->DoneSpecial = true;
	SpecPhase();
}

void Game::MageSpec()
{
	if (currentChar->phaseNumber == 0)
	{
		string message = "You are the " + CharacterToString(currentChar->Type) + ". You have " + to_string(currentPlayer->GetHandCards().GetDeckSize()) + " cards and " + notCurrentPlayer->GetName() + " has " + to_string(notCurrentPlayer->GetHandCards().GetDeckSize()) + " cards.\r\n";
		message += "[1] Switch cards with " + notCurrentPlayer->GetName() + "\r\n[2] Switch cards with the deck\r\n[3] Do nothing\r\n";
		Tell(currentPlayer, message);
	}
	else if(currentChar->phaseNumber == 1){
		string m = "Select which cards you want to switch by typing their numbers sperated with a space.\r\n";
		size_t decksize = currentPlayer->GetHandCards().GetDeckSize();
		for (size_t j = 0; j < decksize; j++) {
			m += "[" + to_string(j + 1) + "] " + currentPlayer->GetHandCards().GetDeck()[j]->GetCardInfo() + "\r\n";
		}
		Tell(currentPlayer, m);
	}
}

void Game::CondottiereSpec()
{
	if (!notCurrentPlayer->HasCharacter(CharacterType::Prediker) || notCurrentPlayer->GetBuildings().GetDeckSize() < 8) {
		string message = "You are the " + CharacterToString(currentChar->Type) + ". You have " + to_string(currentPlayer->GetGold()) + " gold. Which building do you want to destroy?\r\n";
		message += "[0] None;\r\n";
		for (size_t i = 0; i < notCurrentPlayer->GetBuildings().GetDeckSize(); i++) {
			auto building = notCurrentPlayer->GetBuildings().GetDeck()[i];
			message += "[" + to_string(i + 1) + "] " + building->GetName() + " cost: " + to_string(building->GetCoins() - 1) + " points: " + to_string(building->GetCoins()) + "\r\n";
		}
		Tell(currentPlayer, message);
		
	}
	else {
		Tell(currentPlayer, notCurrentPlayer->GetName() + " is immune to the " + CharacterToString(currentChar->Type) + ". So there is no use for your special :(\r\n");
		Tell(notCurrentPlayer, "You are immune to the " + CharacterToString(currentChar->Type) + ". So he cannot destroy any of your buildings.\r\n");
		currentChar->DoneSpecial = true;
	}
}

void Game::KingSpec()
{
	//koning has no special action
	currentChar->DoneSpecial = true;
	SpecPhase();
}

void Game::ThiefSpec()
{
	string message = "You are the " + CharacterToString(currentChar->Type) + ", Who do you want to steal?\r\n";
	for (size_t i = 2; i < characterOrder.size(); i++) {
		message += "[" + to_string(i - 1) + "] " + CharacterToString(characterOrder[i]) + "\r\n";
	}
	Tell(currentPlayer, message);
}

void Game::MurdererSpec()
{
	string message = "You are the " + CharacterToString(currentChar->Type) + ", Who do you want to kill?\r\n";
	for (size_t i = 1; i < characterOrder.size(); i++) {
		message += "[" + to_string(i) + "] " + CharacterToString(characterOrder[i]) + "\r\n";
	}
	Tell(currentPlayer, message);
}

void Game::PreacherSpec()
{
	//prediker has no special action
	currentChar->DoneSpecial = true;
	SpecPhase();
}

void Game::BuilderHandle(string input)
{
	// Nothing to do, but for completionist sake.
}

void Game::MerchantHandle(string input)
{
	// Nothing to do, but for completionist sake.
}

void Game::MageHandle(string input)
{
	if (currentChar->phaseNumber == 0) {
		try {
			int num = stoi(input);

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

				currentChar->DoneSpecial = true;
				SpecPhase();
				break;
			}
			case 2: // switch with the deck
			{
				string m = "Select which cards you want to switch by typing their numbers sperated with a space.\r\n";
				size_t decksize = currentPlayer->GetHandCards().GetDeckSize();
				for (size_t j = 0; j < decksize; j++) {
					m += "[" + to_string(j + 1) + "] " + currentPlayer->GetHandCards().GetDeck()[j]->GetCardInfo() + "\r\n";
				}
				Tell(currentPlayer, m);

				currentChar->phaseNumber = 1;
				break;
			}
			case 3: {
				Tell(currentPlayer, "You dont switch any cards.\r\nMachiavelli> ");
				Tell(notCurrentPlayer, currentPlayer->GetName() + " decided not to switch any cards.\r\nMachiavelli> ");
				currentChar->DoneSpecial = true;
				SpecPhase();
				break;
			}
			default:
				Tell(currentPlayer, WrongInput(input));
				return;
			}


		}
		catch (exception e) {
			Tell(currentPlayer, WrongInput(input));
			return;
		}
	}
	else if (currentChar->phaseNumber == 1) {
		// switching cards with deck;

		string workingResponse = input;
		size_t pos = 0;
		vector<int> cardsToSwitch;
		try {
			while ((pos = workingResponse.find(' ')) != string::npos) {
				int num = stoi(workingResponse.substr(0, pos)) - 1;
				if (num >= currentPlayer->GetHandCards().GetDeckSize()) {
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
			Tell(currentPlayer, "You switched cards with the deck.\r\n");
			Tell(notCurrentPlayer, currentPlayer->GetName() + " switched cards with the deck.\r\n");
			currentPlayer->ShowHandCards();

			currentChar->DoneSpecial = true;
			SpecPhase();
		}
		catch (invalid_argument e) {
			Tell(currentPlayer, e.what());
		}
		catch (...) {
			Tell(currentPlayer, WrongInput(input));
		}
		
	}
	
}

void Game::CondottiereHandle( string input)
{
	try {
		int num = stoi(input);
		if (num == 0) {
			Tell(currentPlayer, "You decided not to destroy a building of " + notCurrentPlayer->GetName() + ".\r\n");
			Tell(notCurrentPlayer, currentPlayer->GetName() + " did not destroy one of your buildings.\r\n");
			currentChar->DoneSpecial = true;
			SpecPhase();
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
					Tell(currentPlayer, "You destroyed " + building->GetName() + ".\r\n");
					Tell(notCurrentPlayer, "Your " + building->GetName() + " was destroyed.\r\n");
					currentChar->DoneSpecial = true;
					SpecPhase();
				}
				else {
					throw invalid_argument("You dont have enough gold to do that.\r\n");
				}
			}
			else {
				Tell(currentPlayer, WrongInput(input));
			}
		}
	}
	catch (invalid_argument e) {
		Tell(currentPlayer, e.what());
	}
	catch (exception e) {
		Tell(currentPlayer, WrongInput(input));
	}
}

void Game::KingHandle(string input)
{
	// Nothing to do, but for completionist sake.
}

void Game::ThiefHandle(string input)
{
	try {
		int num = stoi(input);
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
			currentChar->DoneSpecial = true;
			SpecPhase();
		}
		else {
			Tell(currentPlayer, WrongInput(input));
		}

	}
	catch (exception e) {
		Tell(currentPlayer, WrongInput(input));
		return;
	}
}

void Game::MurdererHandle(string input)
{
	try {
		int num = stoi(input);
		if (num > 0 && num < characterOrder.size()) {
			for each (auto c in notCurrentPlayer->GetCharacters().GetDeck())
			{
				if (c->Type == characterOrder[num]) {
					c->Killed = true;
					Tell(currentPlayer, "You have murdered the " + CharacterToString(characterOrder[num]) + ".\r\n");
					Tell(notCurrentPlayer, "The " + CharacterToString(characterOrder[num]) + " has been murdered.\r\n");
					break;
				}
			}
			currentChar->DoneSpecial = true;
			SpecPhase();
		}
		else {
			Tell(currentPlayer, WrongInput(input));
		}

	}
	catch (exception e) {
		Tell(currentPlayer, WrongInput(input));
		return;
	}
}

void Game::PreacherHandle(string input)
{
	// Nothing to do, but for completionist sake.
}

#pragma endregion
void Game::StateCommand(string command)
{
	switch (State)
	{
	case GameState::Character:
	{
		int number;
		try {
			number = stoi(command);
			HandleCharacterChoice(number);
		}
		catch (...) {
			Tell(currentPlayer, WrongInput(command));
		}
		break;
	}
	case GameState::StartPhase:
	{
		int number;
		try {
			number = stoi(command);
			HandleStartPhase(number);
		}
		catch (...) {
			Tell(currentPlayer, WrongInput(command));
		}
		break;
	}
	case GameState::ChooseCard:
	{
		int number;
		try {
			number = stoi(command);
			HandleCardChoice(number);
		}
		catch (...) {
			Tell(currentPlayer, WrongInput(command));
		}
		break;
	}
	case GameState::BuildPhase:
	{
		int number;
		try {
			number = stoi(command);
			HandleBuildPhase(number);
		}
		catch (...) {
			Tell(currentPlayer, WrongInput(command));
		}
		break;
	}
	case GameState::SpecialPhase:
	{
		HandleSpecPhase(command);
		break;
	}
	default:
		break;
	}
}

void Game::HandleCharacterChoice(int number)
{

	int num = number - 1;
	if (num >= 0 && num < characters.GetDeckSize()) {
		auto c = characters.Pop(num);
		currentPlayer->AddCharacter(c);
		Tell(currentPlayer, "You took the " + CharacterToString(c->Type) + ".\r\n");
		characters.Shuffle();
	}
	else {
		Tell(currentPlayer, WrongInput(to_string(number)));
		return;
	}

	SwitchPlayer();
	ChooseCharacters();
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
	else if (currentPlayer->GetPoints() + p1bonuspoints > notCurrentPlayer->GetPoints() + p2bonuspoints) {
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
	if (notCurrentPlayer == nullptr) {
		if (currentPlayer = Players[0]) {
			notCurrentPlayer = Players[1];
		}
		else {
			notCurrentPlayer = Players[0];
		}
	}

	cerr << "Current player is: " << currentPlayer->GetName() << "\r\n";

	// resetting character cards.
	while (disposedCharacters.GetDeckSize() > 0)
	{
		characters.AddCard(disposedCharacters.Pop());
	}
	for each (auto p in Players)
	{
		while (p->GetCharacters().GetDeckSize() > 0) {
			characters.AddCard(p->PopCharacter());
		}
	}
	for each (auto c in characters.GetDeck()) {
		c->Reset();
	}
	cerr << "Reset the characters: " << characters.GetDeckSize() << "\r\n";
	ChooseCharacters();

}

void Game::ChooseCharacters() {
	if (characters.GetDeckSize() > 0) {
		characters.Shuffle();
		State = GameState::Character;
		auto c = characters.Pop();
		string info = "You draw the top card: " + CharacterToString(c->Type) + " and put it away, now choose one of the following cards.\r\n";
		disposedCharacters.AddCard(c);
		for (size_t i = 0; i < characters.GetDeckSize(); i++) {
			info += "[" + to_string(i + 1) + "] " + CharacterToString(characters.GetDeck()[i]->Type) + "\r\n";
		}
		info += "Pick by telling the card number you want.\r\nMachiavelli> ";
		Tell(currentPlayer, info);
		Tell(notCurrentPlayer, "Machiavelli> ");



	}
	else {
		PlayRound();
	}
}

void Game::PlayRound()
{
	characterNumber++;
	if (characterNumber < characterOrder.size()) {
		TellAll("\rKing \"" + King->GetName() + "\": " + CharacterToString(characterOrder[characterNumber]) + " show yourself!\r\n");
		bool switched = false;

		for (size_t n = 0; n < Players.size(); n++)
		{
			currentChar = Players[n]->GetCharacter(characterOrder[characterNumber]);
			if (currentChar != nullptr) {
				currentPlayer = Players[n];
				if (n == 0) {
					notCurrentPlayer = Players[1];
				}
				else {
					notCurrentPlayer = Players[0];
				}
				if (characterOrder[characterNumber] == CharacterType::Koning) {
					King = currentPlayer;
				}
				switched = true;
				Tell(currentPlayer, "You have this card, it is your turn.\r\n");
				Tell(notCurrentPlayer, currentPlayer->GetName() + " has this card, it is his/her turn.\r\n");
				break;
			}
		}

		if (!switched) {
			TellAll("The " + CharacterToString(characterOrder[characterNumber]) + " did not respond to the King\'s call.\r\n");
			PlayRound();
		}
		else {
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
			if (currentChar->Type == CharacterType::Bouwmeester) {
				DrawBuildCard(currentPlayer);
				DrawBuildCard(currentPlayer);
			}
			StartPhase();
			// next split
		}
	}
	else {
		characterNumber = -1;
		TellAll("The round ended\r\nMachiavelli> ");
		if (currentPlayer->HasCharacter(CharacterType::Koning)) {
			King = currentPlayer;
		}
		else if (notCurrentPlayer->HasCharacter(CharacterType::Koning)) {
			King = notCurrentPlayer;
		}
		if (firstWinner == nullptr) {
			SetupRound();
		}
		else {
			FinishGame(firstWinner);
		}
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

void Game::StartPhase()
{
	State = GameState::StartPhase;
	string message = "You have " + to_string(currentPlayer->GetGold()) + " gold.\r\n";
	message += "What would you like to do?\r\n";
	message += "[1] Get 2 gold coins.\r\n";
	message += "[2] Get 1 building card\r\n";
	message += "[3] Check character cards\r\n";
	message += "[4] Check building cards\r\n";
	message += "[5] Check your built buildings\r\n";
	message += "[6] Check " + notCurrentPlayer->GetName() + "\'s built buildings\r\n";
	message += "[7] Check all built buildings\r\nMachiavelli> ";
	Tell(currentPlayer, message);
}

void Game::HandleStartPhase(int number)
{
	switch (number)
	{
	case 1: {
		GiveGold(currentPlayer, 2);
		Tell(currentPlayer, "You took 2 gold coins.\r\n");
		Tell(notCurrentPlayer, currentPlayer->GetName() + " took 2 gold coins.\r\n");
		State = GameState::BuildPhase;
		BuildPhase();
		break;
	}
	case 2: {

		auto card1 = buildCards.GetDeck()[0];
		auto card2 = buildCards.GetDeck()[1];

		string choice = "You drew 2 cards, please make your choice by typing the number.\r\n";
		choice += "[1]" + card1->GetCardInfo() + "\r\n";
		choice += "[2]" + card2->GetCardInfo() + "\r\n";

		Tell(currentPlayer, choice);
		State = GameState::ChooseCard;
		break;
	}
	case 3:
		currentPlayer->ShowCharacterCards();
		break;
	case 4:
		currentPlayer->ShowHandCards();
		break;
	case 5:
		currentPlayer->ShowBuiltBuildings();
		break;
	case 6:
	{
		auto built = notCurrentPlayer->BuiltBuildings();
		built = notCurrentPlayer->GetName() + " has built the following buildings, scoring " + to_string(notCurrentPlayer->GetPoints()) + " points\r\n" + built;
		Tell(currentPlayer, built);
		break;
	}
	case 7:
	{
		auto builtncp = notCurrentPlayer->BuiltBuildings();
		builtncp = notCurrentPlayer->GetName() + " has built the following buildings, scoring " + to_string(notCurrentPlayer->GetPoints()) + " points\r\n" + builtncp;
		auto built = currentPlayer->BuiltBuildings();
		built = "You built the following buildings, scoring " + to_string(currentPlayer->GetPoints()) + " points\r\n" + built;
		Tell(currentPlayer, builtncp + built);
		break;
	}
	default:
		Tell(currentPlayer, WrongInput(to_string(number)));
		break;
	}
}

void Game::HandleCardChoice(int number)
{
	if (number == 1 || number == 2) {
		auto card1 = buildCards.Pop();
		auto card2 = buildCards.Pop();
		if (number == 1) {
			currentPlayer->AddCard(card1);
			disposedCards.AddCard(card2);
		}
		else {
			currentPlayer->AddCard(card2);
			disposedCards.AddCard(card1);
		}
		State = GameState::BuildPhase;
		BuildPhase();
	}
	else {
		Tell(currentPlayer, WrongInput(to_string(number)));
	}
}

void Game::BuildPhase()
{
	if (currentChar->maxBuildings > currentChar->buildingsBuilt && State == GameState::BuildPhase) {
		string q = "You have " + to_string(currentPlayer->GetGold()) + " gold and you can build " + to_string((currentChar->maxBuildings - currentChar->buildingsBuilt)) + " building(s). Which building do you want to build?\r\n";
		q += "[1] None.\r\n";
		for (size_t i = 0; i < currentPlayer->GetHandCards().GetDeckSize(); i++)
		{
			q += "[" + to_string(i + 2) + "] " + currentPlayer->GetHandCards().GetDeck()[i]->GetCardInfo() + "\r\n";
		}
		Tell(currentPlayer, q);
	}
	else {
		// building phase is over.
		State = GameState::SpecialPhase;
		SpecPhase();
	}
}

void Game::HandleBuildPhase(int number)
{
	switch (number)
	{
	case 1: {
		Tell(currentPlayer, "You wont build a building this round.\r\n");
		Tell(notCurrentPlayer, currentPlayer->GetName() + " did not build a building.\r\nMachiavelli> ");
		State = GameState::SpecialPhase;
		BuildPhase();
		break;
	}
	default:
	{
		// all other numbers.
		number -= 2;
		if (number >= 0 && number < currentPlayer->GetHandCards().GetDeckSize()) {
			auto card = currentPlayer->GetHandCards().GetDeck()[number];
			try {
				currentPlayer->AddGold(-card->GetCoins());
				goldPiecesLeft += card->GetCoins();
				currentPlayer->Build(number);
				currentChar->buildingsBuilt++;
				Tell(currentPlayer, "You build " + card->GetName() + ".\r\n");
				Tell(notCurrentPlayer, currentPlayer->GetName() + " has build " + card->GetName() + "\r\nMachiavelli> ");
				BuildPhase();
			}
			catch (...) {
				Tell(currentPlayer, "You dont have enough gold to build the " + card->GetName() + ". Try again.\r\n");

			}
		}
		else {
			Tell(currentPlayer, WrongInput(to_string(number + 2)));
		}
		break;
	}
	}
}

void Game::SpecPhase()
{
	if (!currentChar->DoneSpecial) {
		// doesnt work cos inheritance dunno....
		//ExecuteSpecial(currentChar);

		switch (currentChar->Type)
		{
		case CharacterType::Moordenaar:
			MurdererSpec();
			break;
		case CharacterType::Dief:
			ThiefSpec();
			break;
		case CharacterType::Magiër:
			MageSpec();
			break;
		case CharacterType::Koning:
			KingSpec();
			break;
		case CharacterType::Prediker:
			PreacherSpec();
			break;
		case CharacterType::Koopman:
			MerchantSpec();
			break;
		case CharacterType::Bouwmeester:
			BuilderSpec();
			break;
		case CharacterType::Condottiere:
			CondottiereSpec();
			break;
		default:
			break;
		}
	}
	else {
		TurnEnd();
	}
}

void Game::HandleSpecPhase(string input)
{
	switch (currentChar->Type)
	{
	case CharacterType::Moordenaar:
		MurdererHandle(input);
		break;
	case CharacterType::Dief:
		ThiefHandle(input);
		break;
	case CharacterType::Magiër:
		MageHandle(input);
		break;
	case CharacterType::Koning:
		KingHandle(input);
		break;
	case CharacterType::Prediker:
		PreacherHandle(input);
		break;
	case CharacterType::Koopman:
		MerchantHandle(input);
		break;
	case CharacterType::Bouwmeester:
		BuilderHandle(input);
		break;
	case CharacterType::Condottiere:
		CondottiereHandle(input);
		break;
	default:
		break;
	}
}

void Game::TurnEnd()
{
	if (firstWinner == nullptr && currentPlayer->GetBuildings().GetDeckSize() >= 8) {
		firstWinner = currentPlayer;
		Tell(currentPlayer, "You have built eight or more buildings. This will be the final round.\r\n");
		Tell(notCurrentPlayer, currentPlayer->GetName() + " has built eigh or more buildings. This will be the final round.\r\n");
	}
	PlayRound();
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
		points = p->GetCount(CardColor::yellow);
		break;
	case CharacterType::Prediker:
		points = p->GetCount(CardColor::blue);
		break;
	case CharacterType::Koopman:
		points = 1 + p->GetCount(CardColor::green);
		break;
	case CharacterType::Condottiere:
		points = p->GetCount(CardColor::red);
		break;
	default:
		break;
	}

	int given = 0;

	while (points > 0 && goldPiecesLeft > 0) {
		goldPiecesLeft--;
		points--;
		p->AddGold(1);
		given++;
	}
	Tell(p, "You have been given " + to_string(given) + " bonus gold.\r\n");

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


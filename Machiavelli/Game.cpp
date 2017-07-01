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

}

void Game::Start()
{
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

}

void Game::Quit()
{
	for each (std::shared_ptr<Player> p in Players)
	{
		p->GetClient()->write("Server Quitting, stopping all games.\r\n");
	}
	Active = false;
}

void Game::ExecuteSpecial(shared_ptr<Bouwmeester> spec)
{
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

void Game::NewRound()
{

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

	cerr << "Reset the characters: " << characters.GetDeckSize() << "\r\n";
	ChooseCharacters();

}

void Game::ChooseCharacters() {
	while (characters.GetDeckSize() > 0) {
		auto c = characters.Pop();
		string info = "You draw the top card: " + CharacterToString(c->Type) + " and put it away, now choose one of the following cards.\r\n";
		disposedCharacters.AddCard(c);
		for (int i = 0; i < characters.GetDeckSize(); i++) {
			info += "[" + to_string(i + 1) + "] " + CharacterToString(characters.GetDeck()[i]->Type) + "\r\n";
		}
		info += "Pick by telling the card number you want.\r\n";
		Tell(currentPlayer, info);
		bool goodResponse = false;
		while (!goodResponse) {
			auto response = currentPlayer->GetResponse();
			try {
				int num = stoi(response) - 1;
				if (num >= 0 && num < characters.GetDeckSize()) {
					currentPlayer->AddCharacter(characters.Pop(num));
					goodResponse = true;
					characters.Shuffle();
				}
				else {
					Tell(currentPlayer, "I cannot parse \'" + response + "\' into a right character, try again\r\n");
					continue;
				}
			}
			catch (exception e) {
				Tell(currentPlayer, "I cannot parse that \'" + response + "\' a right character, try again\r\n");
				continue;
			}

		}
		SwitchPlayer();

	}
	PlayRounds();
}

void Game::PlayRounds()
{
	for (int i = 0; i < characterOrder.size(); i++) {
		TellAll("King " + King->GetName() + ": " + CharacterToString(characterOrder[i]) + " show yourself!\r\n");
		bool switched = false;
		shared_ptr<CharacterCard> currentChar;
		for each (auto p in Players)
		{
			if (p->HasCharacter(characterOrder[i])) {
				currentPlayer = p;
				switched = true;
				Tell(p, "You have this card, it is your turn.\r\n");
				Tell(notCurrentPlayer, p->GetName() + " has this card, it is his/her turn.\r\n");
			}
		}

		if (!switched) {
			TellAll("The " + CharacterToString(characterOrder[i]) + " did not respond to " + King->GetName() + "\'s call.\r\n");
		}
		else {
			while (!currentPlayer->EndedTurn) {
				bool first = false;
				while (!first) {
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
						Tell(currentPlayer, "Your input of " + response + "cannot be used.");
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
						while (!cardChosen) {
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
								Tell(currentPlayer, "Your input of " + response + "cannot be used.");
								continue;
							}
							if (res != 1 || res != 2) {
								Tell(currentPlayer, "Your input of " + response + "cannot be used.");
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
					case 4:
					case 5:
					case 6:
					case 7:
					default:
						break;
					}

				}
			}
			currentPlayer->EndedTurn = false;
		}
	}

	NewRound();
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


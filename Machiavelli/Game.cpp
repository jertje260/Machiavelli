#pragma once
#include "Game.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;


Game::Game() 
{
	Running = false;
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

std::vector<Player> Game::GetCurrentPlayers()
{
	return Players;
}

void Game::AddPlayer(Player p)
{
	Players.push_back(p);
}


Player Game::GetCurrentPlayer()
{
	return Players[currentPlayer];
}

void Game::HandleCommand(Player player, std::string command)
{

}

void Game::Start()
{
	// players draw their build cards.
	for (int i = 0; i < 4; i++)
	{
		DrawBuildCard(Players[0]);
		DrawBuildCard(Players[1]);
	}
	// give the players their coins
	GiveGold(Players[0], 2);
	GiveGold(Players[1], 2);

	
}

void Game::LoadResources()
{
	CreateBuildDeck();
}

void Game::NewGame()
{

}

void Game::SetupRound()
{

}

void Game::PlayRound()
{

}

void Game::DrawBuildCard(Player p)
{
	p.AddCard(buildCards.Pop());
}

bool Game::GiveGold(Player p, int amount)
{
	if (goldPiecesLeft >= amount) {
		p.AddGold(amount);
		goldPiecesLeft -= amount;

		return true;
	}
	else {
		return false;
	}
}

void Game::CreateBuildDeck() {
	auto info = ReadCsv("Bouwkaarten.csv");
	auto factory = CardFactory::Get();

	for (vector<vector<string>>::size_type i = 0; i != info.size(); i++) {
		auto card = factory->CreateBuildCard(info[i][0], info[i][1], info[i][2]);
		if (info[i].size() >3) {
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


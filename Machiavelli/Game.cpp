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

std::vector<std::shared_ptr<Player>> Game::GetCurrentPlayers()
{
	return Players;
}

void Game::AddPlayer(std::shared_ptr<Player> p)
{
	Players.push_back(p);
}


std::shared_ptr<Player> Game::GetCurrentPlayer()
{
	return std::shared_ptr<Player>();
}

void Game::HandleCommand(std::shared_ptr<Player> player, std::string command)
{

}

void Game::LoadResources()
{
	CreateBuildDeck();
}

void Game::NewRound()
{

}

void Game::SetupRound()
{

}

void Game::PlayRound()
{

}

void Game::CreateBuildDeck() {
	auto info = ReadCsv("../Resources/Bouwkaarten.csv");
	auto factory = CardFactory::Get();

	for (vector<vector<string>>::size_type i = 0; i != info.size(); i++) {
		auto card = factory->CreateBuildCard(info[i][0], info[i][1], info[i][2]);
		if (info[i].size() >3) {
			auto c = card.get();
			c->SetDescription(info[i][3]);
		}
		buildCards.AddCard(card);

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


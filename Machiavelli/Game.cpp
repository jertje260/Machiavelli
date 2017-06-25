#include "Game.h"



Game::Game() : Running{ false }
{
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


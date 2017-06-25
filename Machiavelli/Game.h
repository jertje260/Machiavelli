#pragma once
#include "Player.h"
#include <memory>
#include <vector>
#include "Deck.h"
#include "BuildCard.h"
#include "CharacterCard.h"

class Game
{
public:
	Game();
	~Game();

	bool IsAvailable();
	bool IsStarted();
	std::vector<std::shared_ptr<Player>> GetCurrentPlayers();
	void AddPlayer(std::shared_ptr<Player> p);
	std::shared_ptr<Player> GetCurrentPlayer();
	void HandleCommand(std::shared_ptr<Player> player, std::string command);


private:
	bool Running;
	std::vector<std::shared_ptr<Player>> Players;
	void LoadResources();
	void NewRound();
	void SetupRound();
	void PlayRound();
	

	Deck<CharacterCard> Characters;
	Deck<BuildCard> BuildCards;


};


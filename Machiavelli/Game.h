#pragma once
class Player;
#include <memory>
#include <vector>
#include "Deck.h"
#include "BuildCard.h"
#include "CharacterCard.h"
#include "CardFactory.h"
#include "Player.h"

using namespace std;
class Game
{
public:
	Game();
	~Game();

	bool IsAvailable();
	bool IsStarted();
	vector<shared_ptr<Player>> GetCurrentPlayers();
	void AddPlayer(shared_ptr<Player> p);
	shared_ptr<Player> GetCurrentPlayer();
	void HandleCommand(shared_ptr<Player> player, string command);


private:
	bool Running;
	vector<shared_ptr<Player>> Players;
	void LoadResources();
	void NewRound();
	void SetupRound();
	void PlayRound();

	void CreateBuildDeck();

	vector<vector<string>> ReadCsv(string path);
	

	Deck<CharacterCard> characters;
	Deck<BuildCard> buildCards;


};


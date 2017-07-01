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
	vector<std::shared_ptr<Player>> GetCurrentPlayers();
	void AddPlayer(std::shared_ptr<Player> p);
	std::shared_ptr<Player> GetCurrentPlayer();
	void HandleCommand(std::shared_ptr<Player> player, string command);
	void Start();


private:
	bool Running;
	vector<std::shared_ptr<Player>> Players;
	void LoadResources();
	void NewGame();
	void SetupRound();
	void PlayRound();
	int goldPiecesLeft = 30;

	void DrawBuildCard(std::shared_ptr<Player> p);
	bool GiveGold(std::shared_ptr<Player> p, int amount);


	void CreateBuildDeck();
	void CreateCharacterDeck();

	int currentPlayer = 0;

	vector<vector<string>> ReadCsv(string path);
	

	Deck<CharacterCard> characters;
	Deck<BuildCard> buildCards;


};


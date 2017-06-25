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
	vector<Player> GetCurrentPlayers();
	void AddPlayer(Player p);
	Player GetCurrentPlayer();
	void HandleCommand(Player player, string command);
	void Start();


private:
	bool Running;
	vector<Player> Players;
	void LoadResources();
	void NewGame();
	void SetupRound();
	void PlayRound();
	int goldPiecesLeft = 30;

	void DrawBuildCard(Player p);
	bool GiveGold(Player p, int amount);


	void CreateBuildDeck();
	void CreateCharacterDeck();

	int currentPlayer = 0;

	vector<vector<string>> ReadCsv(string path);
	

	Deck<CharacterCard> characters;
	Deck<BuildCard> buildCards;


};


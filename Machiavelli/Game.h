#pragma once
class Player;
#include <memory>
#include <vector>
#include "Deck.h"
#include "BuildCard.h"
#include "CharacterCard.h"
#include "CardFactory.h"
#include "Player.h"
#include "Bouwmeester.h"
#include "Condottiere.h"
#include "Dief.h"
#include "Koning.h"
#include "Koopman.h"
#include "Magier.h"
#include "Moordenaar.h"
#include "Prediker.h"

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
	void Start();
	void Quit(shared_ptr<Player> p);
	void Quit();
	bool Running;
	bool Active;

	void ExecuteSpecial(shared_ptr<Bouwmeester> spec);
	void ExecuteSpecial(shared_ptr<Koopman> spec);
	void ExecuteSpecial(shared_ptr<Magier> spec);
	void ExecuteSpecial(shared_ptr<Condottiere> spec);
	void ExecuteSpecial(shared_ptr<Koning> spec);
	void ExecuteSpecial(shared_ptr<Dief> spec);
	void ExecuteSpecial(shared_ptr<Moordenaar> spec);
	void ExecuteSpecial(shared_ptr<Prediker> spec);
	void ExecuteSpecial(shared_ptr<CharacterCard> spec);

private:

	vector<shared_ptr<Player>> Players;
	void LoadResources();
	void FinishGame(shared_ptr<Player> p);
	void SetupRound();
	void ChooseCharacters();
	void PlayRounds();
	int goldPiecesLeft = 30;
	shared_ptr<Player> notCurrentPlayer;
	string WrongInput(string input) { return "Your input of " + input + "cannot be used, please try again.\r\n"; }
	void SwitchPlayer();
	void DrawBuildCard(shared_ptr<Player> p);
	bool GiveGold(shared_ptr<Player> p, int amount);
	void Tell(shared_ptr<Player>p, string what);
	void TellAll(string what);
	shared_ptr<Player> King;

	void CreateBuildDeck();
	void CreateCharacterDeck();
	void Build(int number);
	void GetBonusCoins(shared_ptr<CharacterCard> c, shared_ptr<Player> p);
	vector<CharacterType> characterOrder{CharacterType::Moordenaar,CharacterType::Dief, CharacterType::Magiër,CharacterType::Koning, CharacterType::Prediker, CharacterType::Koopman, CharacterType::Bouwmeester, CharacterType::Condottiere };

	shared_ptr<Player> currentPlayer;

	vector<vector<string>> ReadCsv(string path);
	

	Deck<CharacterCard> characters;
	Deck<CharacterCard> disposedCharacters;
	Deck<BuildCard> buildCards;
	Deck<BuildCard> disposedCards;

	bool sortDescending(int i, int j) { return i > j; }
};


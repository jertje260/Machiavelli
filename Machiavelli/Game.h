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

enum class GameState { Character, StartPhase, ChooseCard, BuildPhase, SpecialPhase };
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

	void SetCharacters(Deck<CharacterCard> d) { characters = d; }
	void SetBuildCards(Deck<BuildCard> d) { buildCards = d; }


private:
	void StateCommand(string command);
	void HandleCharacterChoice(int number);
	vector<shared_ptr<Player>> Players;
	shared_ptr<Player> notCurrentPlayer;
	shared_ptr<Player> King;

	void FinishGame(shared_ptr<Player> p);
	void SetupRound();
	void ChooseCharacters();
	void PlayRound();
	int goldPiecesLeft = 30;
	
	string WrongInput(string input) { return "Your input of \'" + input + "\' cannot be used, please try again.\r\n"; }
	void SwitchPlayer();
	void DrawBuildCard(shared_ptr<Player> p);
	bool GiveGold(shared_ptr<Player> p, int amount);
	void Tell(shared_ptr<Player>p, string what);
	void TellAll(string what);
	



	GameState State;
	// Phases
	void StartPhase();
	void HandleStartPhase(int number);
	void HandleCardChoice(int number);
	void BuildPhase();
	void HandleBuildPhase(int number);
	void SpecPhase();
	void HandleSpecPhase(string input);
	void TurnEnd();



	void Build(int number);
	void GetBonusCoins(shared_ptr<CharacterCard> c, shared_ptr<Player> p);
	vector<CharacterType> characterOrder{CharacterType::Moordenaar,CharacterType::Dief, CharacterType::Magiër,CharacterType::Koning, CharacterType::Prediker, CharacterType::Koopman, CharacterType::Bouwmeester, CharacterType::Condottiere };

	shared_ptr<Player> currentPlayer;


	

	Deck<CharacterCard> characters;
	Deck<CharacterCard> disposedCharacters;
	Deck<BuildCard> buildCards;
	Deck<BuildCard> disposedCards;
	shared_ptr<Player> firstWinner;
	shared_ptr<CharacterCard> currentChar;
	size_t characterNumber = -1;

	// Specs
	void BuilderSpec();
	void MerchantSpec();
	void MageSpec();
	void CondottiereSpec();
	void KingSpec();
	void ThiefSpec();
	void MurdererSpec();
	void PreacherSpec();

	void BuilderHandle(string input);
	void MerchantHandle(string input);
	void MageHandle(string input);
	void CondottiereHandle(string input);
	void KingHandle(string input);
	void ThiefHandle(string input);
	void MurdererHandle(string input);
	void PreacherHandle(string input);
};
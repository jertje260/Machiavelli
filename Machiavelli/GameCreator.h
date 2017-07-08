#pragma once
#include "Game.h"
using namespace std;

class GameCreator
{
public:

	~GameCreator();

	static GameCreator *Get() {
		static GameCreator instance;
		return &instance;
	}

	shared_ptr<Game> CreateGame();




private:
	GameCreator();
	GameCreator(const GameCreator &) {}
	GameCreator &operator=(const GameCreator &) { return *this; }

	vector<vector<string>> ReadCsv(string path);
	void CreateBuildDeck(shared_ptr<Game> g);
	void CreateCharacterDeck(shared_ptr<Game> g);
	void LoadResources(shared_ptr<Game> g);

	vector<vector<string>> Characters;
	vector<vector<string>> BuildCards;
};


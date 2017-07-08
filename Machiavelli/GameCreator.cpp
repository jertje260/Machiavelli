#include "GameCreator.h"
#include <fstream>
#include <iostream>
#include <sstream>


shared_ptr<Game> GameCreator::CreateGame()
{
	auto g = make_shared<Game>();
	LoadResources(g);
	return g;
}

GameCreator::GameCreator()
{
	Characters = ReadCsv("karakterkaarten.csv");
	BuildCards = ReadCsv("Bouwkaarten.csv");
}


GameCreator::~GameCreator()
{
}


vector<vector<string>> GameCreator::ReadCsv(string path) {
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

void GameCreator::CreateCharacterDeck(shared_ptr<Game> g) {
	auto factory = CardFactory::Get();
	Deck<CharacterCard> d;
	for (vector<vector<string>>::size_type i = 0; i != Characters.size(); i++) {
		auto card = factory->CreateCharacterCard(Characters[i][1]);
		d.AddCard(card);
	}
	d.Shuffle();

	g->SetCharacters(d);
}

void GameCreator::CreateBuildDeck(shared_ptr<Game> g) {
	auto factory = CardFactory::Get();
	Deck<BuildCard> d;
	for (vector<vector<string>>::size_type i = 0; i != BuildCards.size(); i++) {
		auto card = factory->CreateBuildCard(BuildCards[i][0], BuildCards[i][1], BuildCards[i][2]);
		if (BuildCards[i].size() > 3) {
			auto c = card.get();
			c->SetDescription(BuildCards[i][3]);
		}
		d.AddCard(card);

	}
	d.Shuffle();
	g->SetBuildCards(d);
}

void GameCreator::LoadResources(shared_ptr<Game> g)
{
	cerr << "Creating build deck\r\n";
	CreateBuildDeck(g);
	cerr << "Creating Character deck\r\n";
	CreateCharacterDeck(g);

}
//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <string>
#include <memory>
#include "BuildCard.h"
#include "CharacterCard.h"
#include "Deck.h"
#include "Socket.h"
using namespace std;

class Player {
public:
	Player() {}
	Player(const string& name) : name{ name } {}

	string GetName() const { return name; }
	void SetName(const string& new_name) { name = new_name; }
	void AddCard(shared_ptr<BuildCard> b);
	void AddGold(int amount) {
		if (gold + amount < 0) {
			throw invalid_argument("The player will get negative gold.");
		}
		gold += amount;
	}
	int GetGold() {
		return gold;
	}

	void Build(int number) {
		playedCards.AddCard(handCards.Pop(number));
	}
	shared_ptr<Socket> GetClient() { return client; }
	void AddClient(shared_ptr<Socket> socket) { client = socket; }
	void GiveOverview();
	void AddCharacter(shared_ptr<CharacterCard> character) { characters.AddCard(character); }
	Deck<CharacterCard> GetCharacters() { return characters; }
	bool HasCharacter(CharacterType c);
	Deck<BuildCard> GetHandCards() { return handCards; }
	void SetHandCards(Deck<BuildCard> cards) { handCards = cards; }
	bool EndedTurn = false;
	int GetPoints();
	shared_ptr<CharacterCard> GetCharacter(CharacterType type);
	int GetCount(CardColor color);
	int GetBuildingTypes();
	Deck<BuildCard> GetBuildings() { return playedCards; }
	void ShowHandCards();
	void ShowCharacterCards();
	void ShowBuiltBuildings();
	string BuiltBuildings();
	shared_ptr<CharacterCard> PopCharacter() { return characters.Pop(); }
	shared_ptr<BuildCard> PopBuilding(int number) { return playedCards.Pop(number); }



private:
	shared_ptr<Socket> client;
	string name;
	Deck<CharacterCard> characters;
	Deck<BuildCard> handCards;
	int gold = 0;
	Deck<BuildCard> playedCards;
	void Tell(string str) { client->write(str); }
};

#endif /* Player_hpp */

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
using namespace std;

class Player {
public:
	Player() {}
	Player(const std::string& name) : name{ name } {}

	std::string get_name() const { return name; }
	void set_name(const std::string& new_name) { name = new_name; }
	void AddCard(shared_ptr<BuildCard> b);
	void AddGold(int amount) {
		gold += amount;
	}

private:
	std::string name;
	Deck<CharacterCard> caracters;
	Deck<BuildCard> handCards;
	int gold = 0;
	Deck<BuildCard> playedCards;
};

#endif /* Player_hpp */

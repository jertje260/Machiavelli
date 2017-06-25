//
//  Player.cpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include "Player.h"
using namespace std;

Player::~Player()
{
}


Player::Player(std::string name, std::shared_ptr<Game> game, std::shared_ptr<Socket> client) {
	{
		Name = name;
		Game = game;
		Client = client;
	}
}
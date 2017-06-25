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
#include "Game.h"
#include "Socket.h"

class Player {
public:
	Player(std::string name, std::shared_ptr<Game> game, std::shared_ptr<Socket> client);
	~Player();

    std::string get_name() const { return Name; }
    void set_name(const std::string& new_name) { Name = new_name; }

private:
    std::string Name;
	std::shared_ptr<Game> Game;
	std::shared_ptr<Socket> Client;
};

#endif /* Player_hpp */

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
#include "Socket.h"

class Player {
public:
	Player(std::string name, Socket client);
	Player(std::string name);
	Player() {};
	~Player();

    std::string get_name() const { return Name; }
    void set_name(const std::string& new_name) { Name = new_name; }

private:
    std::string Name;
	Socket Client;
};

#endif /* Player_hpp */

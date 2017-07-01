//
// ClientInfo.h
//
// Created by Jeroen de Haas on 22/11/2016.
// Copyright (c) 2016 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef _CLIENT_INFO_H
#define _CLIENT_INFO_H

#include <string>
#include <utility>
#include <memory>

#include "Socket.h"
#include "Player.h"

class ClientInfo {
    std::shared_ptr<Socket> _socket;
    std::shared_ptr<Player> _player;
public:
    ClientInfo(std::shared_ptr<Socket> socket, std::shared_ptr<Player> player)
        : _socket{std::move(socket)}, _player{std::move(player)} {}
    std::shared_ptr<Socket> get_socket() { return _socket; }
    std::shared_ptr<Player> get_player() { return _player; }
};

#endif

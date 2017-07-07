//
//  main.cpp
//  socketexample
//
//  Created by Bob Polis on 16/09/14.
//  Revised by Jeroen de Haas on 22/11/2016
//  Copyright (c) 2014 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

// Memory Leaks
#include <vld.h>



#include <thread>
#include <iostream>
#include <exception>
#include <memory>
#include <utility>
#include <chrono>
#include <algorithm>
using namespace std;

#include "Socket.h"
#include "Sync_queue.h"
#include "ClientCommand.h"
#include "Player.h"
#include "ClientInfo.h"
#include "Game.h"

namespace machiavelli {
	const int tcp_port{ 1080 };
	const string prompt{ "machiavelli> " };
}

static bool running = true;

static Sync_queue<ClientCommand> queue;

static vector<shared_ptr<Game>> games;

void consume_command() // runs in its own thread
{
	try {
		while (running) {
			ClientCommand command{ queue.get() }; // will block here unless there are still command objects in the queue
			if (command.get_cmd() == "quit_server") { // quit server command to trigger the stopping of this thread.
				break;
			}
			if (auto clientInfo = command.get_client_info().lock()) {
				auto client = clientInfo->get_socket();
				auto player = clientInfo->get_player();
				auto game = clientInfo->get_game();
				try {
					game->HandleCommand(player, command.get_cmd());
					
				}
				catch (const exception& ex) {
					cerr << "*** exception in consumer thread for player " << player->GetName() << ": " << ex.what() << '\r\n';
					if (client->is_open()) {
						client->write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
				catch (...) {
					cerr << "*** exception in consumer thread for player " << player->GetName() << '\n';
					if (client->is_open()) {
						client->write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
			}

		}
	}
	catch (...) {
		cerr << "consume_command crashed\n";
	}
}

std::shared_ptr<ClientInfo> init_client_session(std::shared_ptr<Socket> client) {
	client->write("Welcome to Server 1.0! To quit, type 'quit'.\r\n");
	return make_shared<ClientInfo>(move(client), make_shared<Player>());
}

void handle_client(Socket client) // this function runs in a separate thread
{
	try {
		shared_ptr<Game> curGame;
		if (games.size() > 0) {
			 curGame = games.back();
		}
		if (curGame == nullptr || !curGame->IsAvailable()) {
			cerr << "No free game available, creating new one.\r\n";
			curGame = make_shared<Game>();
			games.push_back(curGame);
		}

		auto client_info = init_client_session(std::make_shared<Socket>(move(client)));
		auto socket = client_info->get_socket();
		auto player = client_info->get_player();
		client_info->set_game(curGame);
		player->AddClient(socket);
		curGame->AddPlayer(player);

		socket->write("Welcome, have fun playing our game!\r\nSet your name with the \'name\' <myname>\r\n");
		if (!curGame->IsAvailable()) {
			curGame->Start();
		}
		else
		{
			socket->write("Waiting for another player to join and start the game. Type \'cmds\' for the command info.\r\nMachiavelli> ");
		}
		while (curGame->Active) { // game loop
			try {

				// read first line of request
				std::string cmd;
				if (socket->readline([&cmd](std::string input) { cmd = input; })) {
					cerr << '[' << socket->get_dotted_ip() << " (" << socket->get_socket() << ") " << player->GetName() << "] " << cmd << "\r\n";

					if (cmd == "quit") {
						curGame->Quit(player);

						socket->write("Bye!\r\n");
						games.erase(remove(games.begin(), games.end(), curGame), games.end());
					}
					else if (cmd == "quit_server") {
						for each (std::shared_ptr<Game> g in games)
						{
							g->Quit();
						}
						running = false;
						ClientCommand command{ cmd, client_info };
						queue.put(command);

					}
					else if (curGame->Active) {
						ClientCommand command{ cmd, client_info };
						queue.put(command);
					}
				};

			}
			catch (const exception& ex) {
				*socket << "ERROR: " << ex.what() << "\r\n";
			}
			catch (...) {
				socket->write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
			}
			// stop eating my cpu!
			this_thread::sleep_for(chrono::milliseconds(10));
		}
		// close weg
	}
	catch (std::exception &ex) {
		cerr << "handle_client " << ex.what() << "\r\n";
	}
	catch (...) {
		cerr << "handle_client crashed\r\n";
	}
}

int main(int argc, const char * argv[])
{


	// start command consumer thread
	vector<thread> all_threads;
	all_threads.emplace_back(consume_command);

	// create a server socket
	ServerSocket server{ machiavelli::tcp_port };

	try {
		cerr << "server listening\r\n";
		while (running) {
			// wait for connection from client; will create new socket
			server.accept([&all_threads](Socket client) {
				std::cerr << "Connection accepted from " << client.get_dotted_ip() << "\r\n";
				all_threads.emplace_back(handle_client, move(client));
			});
			this_thread::sleep_for(chrono::milliseconds(100));
		}


	}
	catch (const exception& ex) {
		cerr << ex.what() << ", resuming..." << '\r\n';
	}
	catch (...) {
		cerr << "problems, problems, but: keep calm and carry on!\r\n";
	}

	for (auto &t : all_threads) {
		t.join();
	}

	return 0;
}


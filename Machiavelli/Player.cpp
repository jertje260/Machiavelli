//
//  Player.cpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include "Player.h"
#include "Enums.h"
using namespace std;

void Player::AddCard(std::shared_ptr<BuildCard> b)
{
	handCards.AddCard(b);
}

string Player::GetResponse()
{
	client->write("Machiavelli> ");
	string str;
	bool done{ false };
	while (!done) {
		done = client->readline([&str](std::string input) {
			str = input;
		});
	}
	client->write("\r");
	return str;
}

void Player::GiveOverview()
{
	std::string overview = "Gold: " + std::to_string(gold) + "\r\n";
	overview += "You have " + std::to_string(handCards.GetDeckSize()) + " handcards:\r\n";
	for each (auto c in handCards.GetDeck())
	{
		overview += c->GetCardInfo() + "\r\n";
	}
	int points = 0;
	for each (auto c in playedCards.GetDeck())
	{
		points += c->GetCoins();
	}
	overview += "You have " + std::to_string(playedCards.GetDeckSize()) + " cards played resulting in " + std::to_string(points) + " points:\r\n";
	for each (auto c in playedCards.GetDeck())
	{
		overview += c->GetCardInfo() + "\r\n";
	}
	Tell(overview);

}

bool Player::HasCharacter(CharacterType c)
{
	for each (auto character in characters.GetDeck())
	{
		if (character->Type == c) {
			return true;
		}
	}
	return false;
}

int Player::GetPoints()
{
	int p = 0;
	for each (auto c in playedCards.GetDeck())
	{
		p += c->GetCoins();
	}
	return p;
}

shared_ptr<CharacterCard> Player::GetCharacter(CharacterType type)
{
	shared_ptr<CharacterCard> c = nullptr;
	for each (auto character in characters.GetDeck())
	{
		if (character->Type == type && !character->Killed) {
			c = character;
			break;
		}
	}
	return c;
}

int Player::GetPoints(CardColor color)
{
	int points = 0;
	for each (auto card in playedCards.GetDeck())
	{
		if (card->GetCardColor() == color) {
			points += card->GetCoins();
		}
	}
	return points;
}

int Player::GetBuildingTypes()
{
	bool yellow = false;
	bool red = false;
	bool green = false;
	bool lila = false;
	bool blue = false;
	for each (auto b in playedCards.GetDeck())
	{
		switch (b->GetCardColor())
		{
		case CardColor::blue:
			blue = true;
		case CardColor::green:
			green = true;
		case CardColor::yellow:
			yellow = true;
		case CardColor::red:
			red = true;
		case CardColor::lila:
			lila = true;
		default:
			break;
		}
	}

	int value = 0;
	if (yellow) {
		value++;
	}
	if (red) {
		value++;
	}
	if (green) {
		value++;
	}
	if (lila) {
		value++;
	}
	if (blue) {
		value++;
	}
	return value;
}

void Player::ShowHandCards()
{
	string overview = "You have " + std::to_string(handCards.GetDeckSize()) + " handcards:\r\n";
	for each (auto c in handCards.GetDeck())
	{
		overview += c->GetCardInfo() + "\r\n";
	}
	client->write(overview);
}

void Player::ShowCharacterCards()
{
	string chars = "You have the following characters:\r\n";
	for each (auto c in characters.GetDeck())
	{
		string status;
		if (c->Killed) {
			status = "Killed.";
		}
		else if (c->Stolen) {
			status = "Stolen.";
		}
		else if (c->Played) {
			status = "Played.";
		}
		else {
			status = "Not played.";
		}
		chars += CharacterToString(c->Type) + " : " + status + "\r\n";
	}
	client->write(chars);
}

void Player::ShowBuiltBuildings()
{
	string string = "You have build the following buildings and scored " +  to_string(GetPoints()) + " points.\r\n";
	string += BuiltBuildings();
	client->write(string);
}

string Player::BuiltBuildings()
{
	string result = "";
	for each (auto building in playedCards.GetDeck())
	{
		result += building->GetCardInfo() + "\r\n";
	}
	return result;
}

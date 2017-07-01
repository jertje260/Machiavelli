#pragma once
#include "BuildCard.h"



BuildCard::BuildCard()
{
}


BuildCard::~BuildCard()
{
}

BuildCard::BuildCard(string name, CardColor color, int points)
{
	this->name = name;
	this->color = color;
	this->coins = points;
}

string BuildCard::GetCardInfo()
{
	auto string = std::to_string(coins) + " : " + ColorToString(color) + " : " + name;
	if (!description.empty()) {
		string += " : " + description;
	}
	return string;
}



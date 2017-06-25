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



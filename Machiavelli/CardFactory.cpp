#pragma once
#include "CardFactory.h"
#include "Enums.h"


CardFactory::CardFactory()
{
	characterCards = map<string, CharacterCard>();
}
 

CardFactory::~CardFactory()
{
}

std::shared_ptr<BuildCard> CardFactory::CreateBuildCard(std::string card, std::string color, std::string points)
{
	CardColor c = StringToColor(color);
	int p = std::stoi(points);
	return std::make_shared<BuildCard>(card, c, p);
}

std::shared_ptr<CharacterCard> CardFactory::CreateCharacterCard(std::string card)
{
	return std::shared_ptr<CharacterCard>();
}

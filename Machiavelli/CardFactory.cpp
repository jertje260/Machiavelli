#pragma once
#include "CardFactory.h"
#include "Enums.h"
#include "Moordenaar.h"
#include "Bouwmeester.h"
#include "Condottiere.h"
#include "Dief.h"
#include "Prediker.h"
#include "Koning.h"
#include "Koopman.h"
#include "Magier.h"


CardFactory::CardFactory()
{
	characterCards = map<string, CharacterCard>();
}
 

CardFactory::~CardFactory()
{
}

std::shared_ptr<BuildCard> CardFactory::CreateBuildCard(std::string card, std::string points, std::string color)
{
	CardColor c = StringToColor(color);
	int p = std::stoi(points);
	return std::make_shared<BuildCard>(card, c, p);
}

std::shared_ptr<CharacterCard> CardFactory::CreateCharacterCard(std::string character)
{
	CharacterType type = StringToCharacter(character);

	switch (type)
	{
	case CharacterType::Moordenaar:
		return make_shared<Moordenaar>();
	case CharacterType::Dief:
		return make_shared<Dief>();
	case CharacterType::Magiër:
		return make_shared<Magier>();
	case CharacterType::Koning:
		return make_shared<Koning>();
	case CharacterType::Prediker:
		return make_shared<Prediker>();
	case CharacterType::Koopman:
		return make_shared<Koopman>();
	case CharacterType::Bouwmeester:
		return make_shared<Bouwmeester>();
	case CharacterType::Condottiere:
		return make_shared<Condottiere>();
	default:
		return nullptr;
	}
}

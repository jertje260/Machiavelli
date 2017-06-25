#include "CardFactory.h"



CardFactory::CardFactory()
{
}
 

CardFactory::~CardFactory()
{
}

std::shared_ptr<BuildCard> CardFactory::CreateBuildCard(std::string card)
{
	return 
}

std::shared_ptr<CharacterCard> CardFactory::CreateCharacterCard(std::string card)
{
	return std::shared_ptr<CharacterCard>();
}

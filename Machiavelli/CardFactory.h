#pragma once
#include "BuildCard.h"
#include "CharacterCard.h"
#include <map>
#include <memory>

using namespace std;
class CardFactory
{
public:

	~CardFactory();
	static CardFactory *Get() {
		static CardFactory instance;
		return &instance;
	}


	std::shared_ptr<BuildCard> CreateBuildCard(std::string card, std::string color, std::string points);
	shared_ptr<CharacterCard> CreateCharacterCard(string card);

	


private:
	CardFactory();
	CardFactory(const CardFactory &) {}
	CardFactory &operator=(const CardFactory &){ return *this; }

	map<string, CharacterCard> characterCards;

	

};


#pragma once
#include "BuildCard.h"
#include "CharacterCard.h"
#include <map>
#include <memory>

using namespace std;
class CardFactory
{
public:

	~CardFactory() {}
	static CardFactory *Get() {
		static CardFactory instance;
		return &instance;
	}
	void RegisterBuildCard(const string &building, );


	shared_ptr<BuildCard> CreateBuildCard(string card);
	shared_ptr<CharacterCard> CreateCharacterCard(string card);

	


private:
	CardFactory();
	CardFactory(const CardFactory &) {}
	CardFactory &operator=(const CardFactory &){ return *this; }

	map<string, BuildCard> BuildCards;
	map<string, CharacterCard> CharacterCards;

	

};


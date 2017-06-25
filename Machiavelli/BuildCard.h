#pragma once
#include <memory>
#include "Enums.h"
using std::string;
class BuildCard
{
public:
	BuildCard();
	~BuildCard();
	BuildCard(string name, CardColor color, int points);
	string GetName() { return name; }
	int GetCoins() { return coins; }
	CardColor GetCardColor() { return color; }
	string GetDescription() { return description; }
	void SetDescription(string description) {
		this->description = description;
	}


private:
	string name;
	int coins;
	CardColor color;
	string description;

};


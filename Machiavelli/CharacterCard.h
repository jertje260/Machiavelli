#pragma once
#include "Enums.h"
class CharacterCard
{
public:
	CharacterCard();
	~CharacterCard();

	CharacterType Type;
	bool Killed = false;
	bool Stolen = false;
	bool DoneSpecial = false;
	bool Played = false;
	int maxBuildings = 1;
	int buildingsBuilt = 0;
	int phaseNumber = 0;

	void Reset() {
		Killed = false;
		Stolen = false;
		DoneSpecial = false;
		Played = false;
		buildingsBuilt = 0;
		phaseNumber = 0;
	}

	virtual void ExecuteSpecial();
};


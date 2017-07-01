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

	void Reset() {
		Killed = false;
		Stolen = false;
		DoneSpecial = false;
		Played = false;
	}

	virtual void ExecuteSpecial();
};


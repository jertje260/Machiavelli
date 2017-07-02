#pragma once
#include <string>
#include <algorithm>

enum class CardColor { yellow, green, blue, red, lila };
enum class CharacterType { NONE, Moordenaar, Dief, Magiër, Koning, Prediker, Koopman, Bouwmeester, Condottiere };

inline const std::string CharacterToString(CharacterType c) {
	switch (c)
	{
	case CharacterType::Moordenaar:
		return "Murderer";
	case CharacterType::Dief:
		return "Thief";
	case CharacterType::Magiër:
		return "Magician";
	case CharacterType::Koning:
		return "King";
	case CharacterType::Prediker:
		return "Preacher";
	case CharacterType::Koopman:
		return "Merchant";
	case CharacterType::Bouwmeester:
		return "Builder";
	case CharacterType::Condottiere:
		return "Condottiere";
	default:
		return "NONE";
	}
}

inline const CharacterType StringToCharacter(std::string c) {
	std::transform(c.begin(), c.end(), c.begin(), ::tolower);

	if (c == "moordenaar") {
		return CharacterType::Moordenaar;
	}
	else if (c == "dief") {
		return CharacterType::Dief;
	}
	else if (c == "magiër") {
		return CharacterType::Magiër;
	}
	else if (c == "koning") {
		return CharacterType::Koning;
	}
	else if (c == "prediker") {
		return CharacterType::Prediker;
	}
	else if (c == "koopman") {
		return CharacterType::Koopman;
	}
	else if (c == "bouwmeester") {
		return CharacterType::Bouwmeester;
	}
	else if (c == "condottiere") {
		return CharacterType::Condottiere;
	}
	else {
		return CharacterType::NONE;
	}
}

inline const std::string ColorToString(CardColor c) {
	switch (c)
	{
	case CardColor::yellow:
		return "Yellow";
	case CardColor::green:
		return "Green";
	case CardColor::blue:
		return "Blue";
	case CardColor::red:
		return "Red";
	case CardColor::lila:
		return "Lila";
	default:
		return "White";
	}
}

inline const CardColor StringToColor(std::string c) {
	std::transform(c.begin(), c.end(), c.begin(), ::tolower);
	
	if (c == "geel") {
		return CardColor::yellow;
	}
	else if (c == "groen") {
		return CardColor::green;
	}
	else if (c == "blauw") {
		return CardColor::blue;
	}
	else if (c == "rood") {
		return CardColor::red;
	}
	else {
		return CardColor::lila;
	}
}
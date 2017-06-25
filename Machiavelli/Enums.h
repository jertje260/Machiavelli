#pragma once
#include <string>
#include <algorithm>

enum class CardColor { white, yellow, green, blue, red, lila };
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

inline const std::string ColorToString(CardColor c) {
	switch (c)
	{
	case CardColor::white:
		return "White";
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
	
	if (c == "wit") {
		return CardColor::white;
	}
	else if (c == "geel") {
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
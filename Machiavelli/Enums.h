#pragma once
#include <string>

enum class CardColor { white, yellow, green, blue, red,lila};
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
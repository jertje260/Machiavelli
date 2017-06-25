#pragma once
#include <vector>
#include <memory>
#include <algorithm>


template <class T>
class Deck
{
public:
	Deck() {};
	~Deck() {};

	void Shuffle() { random_shuffle(deck.begin(), deck.end()); }
	void AddCard(std::shared_ptr<T> card) { deck.push_back(card); }
	void RemoveCard(std::shared_ptr<T> card) { deck.erase(find(deck.begin(), deck.end(), card)); }
	void RemoveCardAt(int index) { deck.erase(deck.begin() + index); }
	int GetDeckSize() { return deck.size(); }
	void ClearDeck() { deck.clear(); }
	std::vector<std::shared_ptr<T>> GetDeck() { return deck; }


private:
	std::vector<std::shared_ptr<T>> deck;
};


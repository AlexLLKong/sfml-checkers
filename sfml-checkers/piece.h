#pragma once
#include "SFML/Graphics.hpp"

namespace my{

	enum class Colour {
		RED,
		BLACK
	};
	struct Piece {
		std::shared_ptr<sf::Sprite> Sprite;
		Colour Side;
		bool isKing = false;
		Piece() = delete;
		Piece(Colour side, std::shared_ptr<sf::Sprite> sprite)
		{
			Side = side;
			Sprite = sprite;
		}
	};
}
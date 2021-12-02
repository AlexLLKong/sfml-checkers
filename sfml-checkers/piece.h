#pragma once
#include "SFML/Graphics.hpp"

namespace my{

	enum class Side {
		RED,
		BLACK
	};
	struct Piece {
		sf::Sprite* sprite;
		Side side;
	};
}
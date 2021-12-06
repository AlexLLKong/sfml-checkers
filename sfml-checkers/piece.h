#pragma once
#include "SFML/Graphics.hpp"

namespace my{

	enum class Colour {
		RED,
		BLACK
	};
	struct Piece {
		struct PossibleMove {
			int x, y;
		};
		struct PossibleTake {
			int landX, landY;
			std::shared_ptr<my::Piece> pieceToBeTaken;
		};
		std::shared_ptr<sf::Sprite> Sprite;
		Colour Side;
		bool isKing = false;
		std::vector<my::Piece::PossibleTake> PossibleTakes;
		std::vector<my::Piece::PossibleMove> PossibleMoves;
		Piece() = delete;
		Piece(Colour side, std::shared_ptr<sf::Sprite> sprite)
		{
			Side = side;
			Sprite = sprite;
		}
	};
}
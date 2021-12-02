#pragma once
#include "SFML/Graphics.hpp"
#include <memory>
namespace my {
	struct TextureHolder {
		std::unique_ptr<sf::Texture> redSquareTexture;
		std::unique_ptr<sf::Texture> blackSquareTexture;
		std::unique_ptr<sf::Texture> redPieceTexture;
		std::unique_ptr<sf::Texture> blackPieceTexture;
		std::unique_ptr<sf::Texture> uiBackgroundTexture;
		std::unique_ptr<sf::Texture> resignButtonTexture;
        bool allTexturesLoaded = true;
		TextureHolder() {
            redSquareTexture = std::make_unique<sf::Texture>();
            blackSquareTexture = std::make_unique<sf::Texture>();
            redPieceTexture = std::make_unique<sf::Texture>();
            blackPieceTexture = std::make_unique<sf::Texture>();
            uiBackgroundTexture = std::make_unique<sf::Texture>();
            resignButtonTexture = std::make_unique<sf::Texture>();

            if (!redSquareTexture->loadFromFile("sprites/red-square.png"))
            {
                allTexturesLoaded = false;
            }
            if (!blackSquareTexture->loadFromFile("sprites/black-square.png"))
            {
                allTexturesLoaded = false;
            }
            // init piece textures
            if (!redPieceTexture->loadFromFile("sprites/red-piece.png"))
            {
                allTexturesLoaded = false;
            }
            if (!blackPieceTexture->loadFromFile("sprites/black-piece.png"))
            {
                allTexturesLoaded = false;
            }
            // ui textures
            if (!uiBackgroundTexture->loadFromFile("sprites/ui-background.png"))
            {
                allTexturesLoaded = false;
            }
            if (!resignButtonTexture->loadFromFile("sprites/resign-button.png"))
            {
                allTexturesLoaded = false;
            }
		}
	};
}
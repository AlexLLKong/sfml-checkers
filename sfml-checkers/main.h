#pragma once
#include "SFML/Graphics.hpp"
#include "piece.h"
#include "textures.h"
void CreateBoard(std::vector<sf::Sprite*>& sprites, my::TextureHolder& textureHolder);

void CreateUI(std::vector<sf::Sprite*>& sprites, my::TextureHolder& textureHolder);

void CreatePieces(std::vector<my::Piece*>& pieces, my::TextureHolder& textureHolder, bool* grid, std::vector<sf::Sprite*>& sprites);

void CreateText(std::vector<sf::Text*>& text, sf::Font& font);

my::Piece* GetHeldSprite(std::vector<my::Piece*>& pieces, bool* grid, int x, int y);

void RenderSprites(std::vector<sf::Sprite*>& sprites, sf::RenderWindow& window);

void RenderText(std::vector<sf::Text*>& text, sf::RenderWindow& window);

bool CheckPos(bool* grid, int x, int y, my::Piece* heldPiece, sf::Vector2i* prevPosition);

bool IsInvalidRedMove(sf::Vector2i* prevPosition, int x, int y);

bool IsInvalidBlackMove(sf::Vector2i* prevPosition, int x, int y);

// DEBUG FUNCTIONS

void DebugGrid(std::vector<sf::CircleShape>& shapes, bool* grid);

void DebugClickOnGrid(std::vector<sf::CircleShape>& shapes, bool* grid, int x, int y);

void DrawDebugShapes(std::vector<sf::CircleShape>& shapes, sf::RenderWindow& window);

// UTILITY FUNCTIONS

int ToGridCoordinate(int n);
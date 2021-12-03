#pragma once
#include "SFML/Graphics.hpp"
#include "piece.h"
#include "textures.h"
void CreateBoard(std::vector<std::unique_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder);

void CreateUI(std::vector<std::shared_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder);

void CreatePieces(std::vector<std::shared_ptr<my::Piece>>& pieces, my::TextureHolder& textureHolder, bool* grid, std::vector<std::shared_ptr<sf::Sprite>>& sprites);

void CreateText(std::vector<std::unique_ptr<sf::Text>>& text, sf::Font& font);

std::shared_ptr<my::Piece> GetHeldPiece(std::vector<std::shared_ptr<my::Piece>>& pieces, bool* grid, int x, int y);

void ToggleTurnUI(std::vector< std::shared_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder, my::Colour turn);

void RenderSprites(std::vector<std::shared_ptr<sf::Sprite>>& sprites, sf::RenderWindow& window);

void RenderSprites(std::vector<std::unique_ptr<sf::Sprite>>& sprites, sf::RenderWindow& window);

void RenderText(std::vector<std::unique_ptr<sf::Text>>& text, sf::RenderWindow& window);

bool CheckPos(bool* grid, int x, int y, my::Piece* heldPiece, sf::Vector2i* prevPosition);

bool IsInvalidRedMove(sf::Vector2i* prevPosition, int x, int y);

bool IsInvalidBlackMove(sf::Vector2i* prevPosition, int x, int y);

// DEBUG FUNCTIONS

void DebugGrid(std::vector<std::shared_ptr<sf::CircleShape>>& shapes, bool* grid);

void DebugClickOnGrid(std::vector<std::shared_ptr<sf::CircleShape>>& shapes, bool* grid, int x, int y);

void DrawDebugShapes(std::vector<std::shared_ptr<sf::CircleShape>>& shapes, sf::RenderWindow& window);

// UTILITY FUNCTIONS

int ToGridCoordinate(int n);
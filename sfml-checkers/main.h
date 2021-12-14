#pragma once
#include "SFML/Graphics.hpp"
#include "piece.h"
#include "button.h"
#include "textures.h"
void CreateBoard(std::vector<std::unique_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder);

void CreateUI(std::vector<std::shared_ptr<sf::Sprite>>& sprites, std::vector<std::shared_ptr<my::Button>>& buttons, my::TextureHolder& textureHolder);

void CreatePieces(std::vector<std::shared_ptr<my::Piece>>& pieces, my::TextureHolder& textureHolder, bool* grid, std::vector<std::shared_ptr<sf::Sprite>>& sprites);

void CreateText(std::vector<std::shared_ptr<sf::Text>>& text, sf::Font& font);

std::shared_ptr<my::Piece> GetHeldPiece(std::vector<std::shared_ptr<my::Piece>>& pieces, bool* grid, int x, int y);

void ToggleTurnUI(std::vector< std::shared_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder, my::Colour turn);

void RenderSprites(std::vector<std::shared_ptr<sf::Sprite>>& sprites, sf::RenderWindow& window);

void RenderSprites(std::vector<std::unique_ptr<sf::Sprite>>& sprites, sf::RenderWindow& window);

void RenderText(std::vector<std::shared_ptr<sf::Text>>& text, sf::RenderWindow& window);

void ClearMoves(std::vector<std::shared_ptr<my::Piece>> pieces);

bool IsValidMove(std::shared_ptr<my::Piece> heldPiece, int x, int y);

void GenerateMoves(std::vector<std::shared_ptr<my::Piece>> pieces, bool* grid);

std::shared_ptr<my::Piece> IsValidJump(int x, int y, std::shared_ptr<my::Piece> heldPiece);

bool AreAnyJumps(std::vector<std::shared_ptr<my::Piece>>& pieces);

void ClearJumps(std::vector<std::shared_ptr<my::Piece>>& pieces);

void GenerateJumps(bool* grid, std::vector<std::shared_ptr<my::Piece>>& pieces, my::Colour heldPieceSide);

void GenerateDownJumps(bool* grid, my::Piece* heldPiece, std::vector<std::shared_ptr<my::Piece>>& pieces);

void GenerateUpJumps(bool* grid, my::Piece* heldPiece, std::vector<std::shared_ptr<my::Piece>>& pieces);

void GenerateJump(bool* grid, my::Piece* heldPiece, std::vector<std::shared_ptr<my::Piece>>& pieces, int testTakeX, int testTakeY, int testLandX, int testLandY);

void CheckAndMakeKing(int y, std::shared_ptr<my::Piece> heldPiece, my::TextureHolder& textureHolder);

void CheckForWin(std::vector<std::shared_ptr<my::Piece>>& pieces, std::shared_ptr<sf::Text> winText, bool& gameOver);

void GameWon(my::Colour side, std::shared_ptr<sf::Text> winText, bool& gameOver);

void GameDrawn(std::shared_ptr<sf::Text> winText, bool& gameOver);

// DEBUG FUNCTIONS

void DebugGrid(std::vector<std::shared_ptr<sf::CircleShape>>& shapes, bool* grid);

void DebugClickOnGrid(std::vector<std::shared_ptr<sf::CircleShape>>& shapes, bool* grid, int x, int y);

void DebugDrawJumps(std::vector<std::shared_ptr<my::Piece>>& pieces, std::vector<std::unique_ptr<sf::CircleShape>>& shapes);

void DebugDrawMoves(std::vector<std::shared_ptr<my::Piece>>& pieces, std::vector<std::unique_ptr<sf::CircleShape>>& shapes, my::Colour side);

void DrawDebugShapes(std::vector<std::unique_ptr<sf::CircleShape>>& shapes, sf::RenderWindow& window);

// UTILITY FUNCTIONS

int ToGridCoordinate(int n);

bool IsWithinGrid(int n);

bool IsSquareOccupied(int x, int y, bool* grid);

std::shared_ptr<my::Piece> GetPieceAtPosition(int x, int y, std::vector<std::shared_ptr<my::Piece>>& pieces);

void CheckButtonClick(int x, int y, std::vector<std::shared_ptr<my::Button>>& buttons);

void PerformButtonClick(int x, int y, std::vector<std::shared_ptr<my::Button>>& buttons, my::ButtonEvent& buttonEvent);

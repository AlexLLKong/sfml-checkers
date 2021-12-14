
#include "SFML/Graphics.hpp"
#include "piece.h"
#include "button.h"
#include "textures.h"
const int BOARD_LENGTH = 8;
const int SPRITE_LENGTH = 16;
const float SCALE = 4.0f;
const int GUI_WIDTH = 256;

// UTILITY FUNCTIONS

int ToGridCoordinate(int n)
{
    return n / (SPRITE_LENGTH * SCALE);
}

bool IsWithinGrid(int n)
{
    return n >= 0 && n <= BOARD_LENGTH - 1;
}

bool IsSquareOccupied(int x, int y, bool* grid)
{
    return grid[y * BOARD_LENGTH + x];
}

std::shared_ptr<my::Piece> GetPieceAtPosition(int x, int y, std::vector<std::shared_ptr<my::Piece>>& pieces)
{
    for (auto& piece : pieces)
    {
        if (ToGridCoordinate(piece->Sprite->getPosition().x) == x &&
            ToGridCoordinate(piece->Sprite->getPosition().y) == y)
            return piece;
    }
    return nullptr;
}

void CheckButtonClick(int x, int y, std::vector<std::shared_ptr<my::Button>>& buttons)
{
    for (auto& btn : buttons)
    {
        if (btn->Sprite->getGlobalBounds().contains(x, y))
        {
            btn->Pressed = true;
        }
    }
}

void PerformButtonClick(int x, int y, std::vector<std::shared_ptr<my::Button>>& buttons, my::ButtonEvent& buttonEvent)
{
    for (auto& btn : buttons)
    {
        if (btn->Sprite->getGlobalBounds().contains(x, y) && btn->Pressed)
        {
            buttonEvent = btn->BtnEvent;
        }
    }
}

void CreatePieces(std::vector<std::shared_ptr<my::Piece>>& pieces, my::TextureHolder& textureHolder, bool* grid, std::vector<std::shared_ptr<sf::Sprite>>& sprites)
{
    int z = 0;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z; j < BOARD_LENGTH / 2 - 1; j += 2)
        {
            std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(*textureHolder.redPieceTexture);
            std::shared_ptr<my::Piece> piece = std::make_shared<my::Piece>(my::Colour::RED, sprite);
            sprite->setPosition(sf::Vector2f(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE));
            sprite->setScale(SCALE, SCALE);
            pieces.push_back(piece);
            sprites.push_back(sprite);
            grid[j * BOARD_LENGTH + i] = true;
        }
        z ^= 1;
    }
    z = 1;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z + 5; j < BOARD_LENGTH; j += 2)
        {
            std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(*textureHolder.blackPieceTexture);
            std::shared_ptr<my::Piece> piece = std::make_shared<my::Piece>(my::Colour::BLACK, sprite);
            sprite->setPosition(sf::Vector2f(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE));
            sprite->setScale(SCALE, SCALE);
            pieces.push_back(piece);
            sprites.push_back(sprite);
            grid[j * BOARD_LENGTH + i] = true;
        }
        z ^= 1;
    }
}

void CreateBoard(std::vector<std::unique_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder)
{
    int z = 0;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z; j < BOARD_LENGTH; j += 2)
        {
            std::unique_ptr<sf::Sprite> sprite = std::make_unique<sf::Sprite>(*textureHolder.redSquareTexture);
            sprite->setPosition(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE);
            sprite->setScale(SCALE, SCALE);
            sprites.push_back(std::move(sprite));
        }
        z ^= 1;
    }
    z = 1;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z; j < BOARD_LENGTH; j += 2)
        {
            std::unique_ptr<sf::Sprite> sprite = std::make_unique<sf::Sprite>(*textureHolder.blackSquareTexture);
            sprite->setPosition(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE);
            sprite->setScale(SCALE, SCALE);
            sprites.push_back(std::move(sprite));
        }
        z ^= 1;
    }
}

void CreateUI(std::vector<std::shared_ptr<sf::Sprite>>& sprites, std::vector<std::shared_ptr<my::Button>>& buttons, my::TextureHolder& textureHolder)
{
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(*textureHolder.uiBackgroundTexture);
    sprite->setPosition(sf::Vector2f(BOARD_LENGTH * SPRITE_LENGTH * SCALE, 0));
    sprite->setScale(SCALE, SCALE);
    sprites.push_back(sprite);

    sprite = std::make_shared<sf::Sprite>(*textureHolder.resignButtonTexture);
    sprite->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.155 - sprite->getGlobalBounds().width / 2, 4 * SPRITE_LENGTH * SCALE);
    sprite->setScale(SCALE, SCALE);
    sprites.push_back(sprite);
    std::shared_ptr<my::Button> resignButton = std::make_shared<my::Button>(sprite, my::ButtonEvent::RESIGN);
    buttons.push_back(resignButton);

    sprite = std::make_shared<sf::Sprite>(*textureHolder.redSquareTexture);
    sprite->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE + 1.5 * SPRITE_LENGTH * SCALE, 2 * SPRITE_LENGTH * SCALE);
    sprite->setScale(SCALE, SCALE);
    sprites.push_back(sprite);

    sprite = std::make_shared<sf::Sprite>(*textureHolder.blackPieceTexture);
    sprite->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE + 1.5 * SPRITE_LENGTH * SCALE, 2 * SPRITE_LENGTH * SCALE);
    sprite->setScale(SCALE, SCALE);
    sprites.push_back(sprite);
}

void CreateText(std::vector<std::shared_ptr<sf::Text>>& text, sf::Font& font)
{
    std::shared_ptr<sf::Text> txt = std::make_shared<sf::Text>("TURN", font);
    txt->setCharacterSize(48);
    txt->setStyle(sf::Text::Bold);
    txt->setFillColor(sf::Color::Red);
    txt->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.25 - txt->getGlobalBounds().width / 2, SPRITE_LENGTH * SCALE); // center in the ui a square down from the top
    text.push_back(txt);

    txt = std::make_shared<sf::Text>("RESIGN", font);
    txt->setCharacterSize(28);
    txt->setStyle(sf::Text::Bold);
    txt->setFillColor(sf::Color::Red);
    txt->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.25 - txt->getGlobalBounds().width / 2, 4 * SPRITE_LENGTH * SCALE + txt->getGlobalBounds().height * 0.8); // center in the ui 4 squares down from the top
    text.push_back(txt);

    txt = std::make_shared<sf::Text>("", font);
    txt->setCharacterSize(64);
    txt->setStyle(sf::Text::Bold);
    txt->setFillColor(sf::Color::Red);
    text.push_back(txt);
}
std::shared_ptr<my::Piece> GetHeldPiece(std::vector<std::shared_ptr<my::Piece>>& pieces, bool* grid, int x, int y)
{
    if (grid[y * BOARD_LENGTH + x])
    {
        for (auto& spr : pieces)
        {
            int sprX = spr->Sprite->getPosition().x / (SPRITE_LENGTH * SCALE);
            int sprY = spr->Sprite->getPosition().y / (SPRITE_LENGTH * SCALE);
            if (x == sprX && y == sprY)
                return spr;
        }
    }
    return nullptr;
}

void ToggleTurnUI(std::vector<std::shared_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder, my::Colour turn)
{
    if ((int)turn) // currently black turn
    {
        sprites[3]->setTexture(*textureHolder.redPieceTexture);
    }
    else // currently red turn
    {
        sprites[3]->setTexture(*textureHolder.blackPieceTexture);
    }
}

void RenderSprites(std::vector<std::unique_ptr<sf::Sprite>>& sprites, sf::RenderWindow& window)
{
    for (auto& spr : sprites)
    {
        window.draw(*spr);
    }
}

void RenderSprites(std::vector<std::shared_ptr<sf::Sprite>>& sprites, sf::RenderWindow& window)
{
    for (auto spr : sprites)
    {
        window.draw(*spr);
    }
}

void RenderText(std::vector<std::shared_ptr<sf::Text>>& text, sf::RenderWindow& window)
{
    for (auto& t : text)
    {
        window.draw(*t);
    }
}

void ClearMoves(std::vector<std::shared_ptr<my::Piece>> pieces)
{
    for (auto& piece : pieces)
    {
        piece->PossibleMoves.clear();
    }
}

bool IsValidMove(std::shared_ptr<my::Piece> heldPiece, int x, int y)
{
    for (auto& pm : heldPiece->PossibleMoves)
    {
        if (pm.x == x && pm.y == y)
            return true;
    }
    return false;
}

void GenerateMoves(std::vector<std::shared_ptr<my::Piece>> pieces, bool* grid)
{
    ClearMoves(pieces); // always start with clean slate
    for (auto& piece : pieces)
    {
        int curX = ToGridCoordinate(piece->Sprite->getPosition().x);
        int curY = ToGridCoordinate(piece->Sprite->getPosition().y);
        if (piece->Side == my::Colour::BLACK)
        {
            // check up moves
            if (IsWithinGrid(curX - 1) && IsWithinGrid(curY - 1))
                if (!grid[(curY - 1) * BOARD_LENGTH + (curX - 1)]) // up left is empty
                    piece->PossibleMoves.push_back({ curX - 1, curY - 1 });
            if (IsWithinGrid(curX + 1) && IsWithinGrid(curY - 1))
                if (!grid[(curY - 1) * BOARD_LENGTH + (curX + 1)]) // up left is empty
                    piece->PossibleMoves.push_back({ curX + 1, curY - 1 });
            // check down moves
            if (piece->isKing)
            {
                if (IsWithinGrid(curX - 1) && IsWithinGrid(curY + 1))
                    if (!grid[(curY + 1) * BOARD_LENGTH + (curX - 1)]) // up left is empty
                        piece->PossibleMoves.push_back({ curX - 1, curY + 1 });
                if (IsWithinGrid(curX + 1) && IsWithinGrid(curY + 1))
                    if (!grid[(curY + 1) * BOARD_LENGTH + (curX + 1)]) // up left is empty
                        piece->PossibleMoves.push_back({ curX + 1, curY + 1 });
            }
        }
        else
        {
            // check down moves
            if (IsWithinGrid(curX - 1) && IsWithinGrid(curY + 1))
                if (!grid[(curY + 1) * BOARD_LENGTH + (curX - 1)]) // up left is empty
                    piece->PossibleMoves.push_back({ curX - 1, curY + 1 });
            if (IsWithinGrid(curX + 1) && IsWithinGrid(curY + 1))
                if (!grid[(curY + 1) * BOARD_LENGTH + (curX + 1)]) // up left is empty
                    piece->PossibleMoves.push_back({ curX + 1, curY + 1 });
            if (piece->isKing)
            {
                // check up moves
                if (IsWithinGrid(curX - 1) && IsWithinGrid(curY - 1))
                    if (!grid[(curY - 1) * BOARD_LENGTH + (curX - 1)]) // up left is empty
                        piece->PossibleMoves.push_back({ curX - 1, curY - 1 });
                if (IsWithinGrid(curX + 1) && IsWithinGrid(curY - 1))
                    if (!grid[(curY - 1) * BOARD_LENGTH + (curX + 1)]) // up left is empty
                        piece->PossibleMoves.push_back({ curX + 1, curY - 1 });
            }
        }
    }
}

std::shared_ptr<my::Piece> IsValidJump(int x, int y, std::shared_ptr<my::Piece> heldPiece)
{
    if (heldPiece->PossibleTakes.size() > 0)
    {
        for (auto pt : heldPiece->PossibleTakes)
        {
            if (x == pt.landX && y == pt.landY)
                return pt.pieceToBeTaken;
        }
    }
    return nullptr;
}

bool AreAnyJumps(std::vector<std::shared_ptr<my::Piece>>& pieces)
{
    for (auto& piece : pieces)
    {
        if (piece->PossibleTakes.size() > 0)
            return true;
    }
    return false;
}

void ClearJumps(std::vector<std::shared_ptr<my::Piece>>& pieces)
{
    for (auto& piece : pieces)
    {
        piece->PossibleTakes.clear();
    }
}

void GenerateJump(bool* grid, my::Piece* heldPiece, std::vector<std::shared_ptr<my::Piece>>& pieces, int testTakeX, int testTakeY, int testLandX, int testLandY)
{
    // make sure not landing off board
    if (!IsWithinGrid(testLandX) || !IsWithinGrid(testLandY)) return;
    // make sure landing isn't occupied
    if (IsSquareOccupied(testLandX, testLandY, grid)) return;
    std::shared_ptr<my::Piece> pieceToTake = GetPieceAtPosition(testTakeX, testTakeY, pieces);
    // make sure piece to take exists
    if (!pieceToTake) return;
    // make sure piece to take is not same side
    if (pieceToTake->Side == heldPiece->Side) return;
    // take is now valid
    my::Piece::PossibleTake possibleTake = { testLandX, testLandY, pieceToTake };
    heldPiece->PossibleTakes.push_back(possibleTake);
}

void GenerateDownJumps(bool* grid, my::Piece* heldPiece, std::vector<std::shared_ptr<my::Piece>>& pieces)
{
    int curX = ToGridCoordinate(heldPiece->Sprite->getPosition().x);
    int curY = ToGridCoordinate(heldPiece->Sprite->getPosition().y);
    // test down left
    int testTakeX = curX - 1;
    int testTakeY = curY + 1;
    int testLandX = curX - 2;
    int testLandY = curY + 2;
    GenerateJump(grid, heldPiece, pieces, testTakeX, testTakeY, testLandX, testLandY);
    // test down right
    testTakeX = curX + 1;
    testTakeY = curY + 1;
    testLandX = curX + 2;
    testLandY = curY + 2;
    GenerateJump(grid, heldPiece, pieces, testTakeX, testTakeY, testLandX, testLandY);
}

void GenerateUpJumps(bool* grid, my::Piece* heldPiece, std::vector<std::shared_ptr<my::Piece>>& pieces)
{
    int curX = ToGridCoordinate(heldPiece->Sprite->getPosition().x);
    int curY = ToGridCoordinate(heldPiece->Sprite->getPosition().y);
    // test up left
    int testTakeX = curX - 1;
    int testTakeY = curY - 1;
    int testLandX = curX - 2;
    int testLandY = curY - 2;
    GenerateJump(grid, heldPiece, pieces, testTakeX, testTakeY, testLandX, testLandY);
    // test up right
    testTakeX = curX + 1;
    testTakeY = curY - 1;
    testLandX = curX + 2;
    testLandY = curY - 2;
    GenerateJump(grid, heldPiece, pieces, testTakeX, testTakeY, testLandX, testLandY);
}

void GenerateJumps(bool* grid, std::vector<std::shared_ptr<my::Piece>>& pieces, my::Colour heldPieceSide)
{
    ClearJumps(pieces); // always start with no jumps
    for (auto& piece : pieces)
    {
        if (piece->Side != heldPieceSide) // only generate jumps for the side whose turn it is next
        {
            if (piece->isKing)
            {
                GenerateDownJumps(grid, piece.get(), pieces);
                GenerateUpJumps(grid, piece.get(), pieces);
            }
            // else if piece is black, check up jumps
            else if (piece->Side == my::Colour::BLACK)
            {
                GenerateUpJumps(grid, piece.get(), pieces);
            }
            // else red, check down jumps
            else
            {
                GenerateDownJumps(grid, piece.get(), pieces);
            }
        }
    }
}

void CheckAndMakeKing(int y, std::shared_ptr<my::Piece> heldPiece, my::TextureHolder& textureHolder)
{
    if (heldPiece->Side == my::Colour::RED && y == BOARD_LENGTH - 1)
    {
        heldPiece->isKing = true;
        heldPiece->Sprite->setTexture(*textureHolder.redKingPieceTexture);
    }
    else if (heldPiece->Side == my::Colour::BLACK && y == 0)
    {
        heldPiece->isKing = true;
        heldPiece->Sprite->setTexture(*textureHolder.blackKingPieceTexture);
    }
}

void GameWon(my::Colour side, std::shared_ptr<sf::Text> winText, bool& gameOver)
{
    if (side == my::Colour::BLACK)
    {
        winText->setString("BLACK WINS");
    }
    else
    {
        winText->setString("RED WINS");
    }
    winText->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE / 2 - winText->getGlobalBounds().width / 2,
        BOARD_LENGTH * SPRITE_LENGTH * SCALE / 2 - winText->getGlobalBounds().height * 1.5); // center in the board
    gameOver = true;
}

void GameDrawn(std::shared_ptr<sf::Text> winText, bool& gameOver)
{
    winText->setString("GAME IS DRAWN");
    winText->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE / 2 - winText->getGlobalBounds().width / 2,
        BOARD_LENGTH * SPRITE_LENGTH * SCALE / 2 - winText->getGlobalBounds().height * 1.5); // center in the board
    gameOver = true;
}

void CheckForWin(std::vector<std::shared_ptr<my::Piece>>& pieces, std::shared_ptr<sf::Text> winText, bool& gameOver)
{
    int redCount = 0;
    int blackCount = 0;
    int redHasMovesOrTakes = 0;
    int blackHasMovesOrTakes = 0;
    for (auto& piece : pieces)
    {
        if (piece->Side == my::Colour::BLACK)
        {
            blackCount++;
            if (piece->PossibleMoves.size() > 0 || piece->PossibleTakes.size() > 0)
                blackHasMovesOrTakes++;
        }
        else
        {
            redCount++;
            if (piece->PossibleMoves.size() > 0 || piece->PossibleTakes.size() > 0)
                redHasMovesOrTakes++;
        }
    }
    if (redHasMovesOrTakes == 0 && blackHasMovesOrTakes == 0)
        GameDrawn(winText, gameOver);
    else if (redCount == 0 || redHasMovesOrTakes == 0)
        GameWon(my::Colour::BLACK, winText, gameOver);
    else if (blackCount == 0 || blackHasMovesOrTakes == 0)
        GameWon(my::Colour::RED, winText, gameOver);
}

void CreateRestartButton(std::vector<std::shared_ptr<sf::Sprite>>& uiSprites, 
                         std::vector<std::shared_ptr<my::Button>>& buttons, 
                         my::TextureHolder& textureHolder, 
                         std::vector<std::shared_ptr<sf::Text>>& text, 
                         sf::Font& font)
{
    


}

// DEBUG FUNCTIONS
void DebugGrid(std::vector<std::unique_ptr<sf::CircleShape>>& shapes, bool* grid)
{
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = 0; j < BOARD_LENGTH; j++)
        {
            if (grid[j * BOARD_LENGTH + i] == true)
            {
                std::unique_ptr<sf::CircleShape> circle = std::make_unique<sf::CircleShape>();
                circle->setRadius(SPRITE_LENGTH / 2 * SCALE);
                circle->setFillColor(sf::Color::Green);
                circle->setPosition(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE);
                shapes.push_back(std::move(circle));
            }
        }
    }
}

void DebugCircleOnGrid(std::vector<std::unique_ptr<sf::CircleShape>>& shapes, bool* grid, int x, int y)
{
    if (grid[y * BOARD_LENGTH + x])
    {
        std::unique_ptr<sf::CircleShape> circle = std::make_unique<sf::CircleShape>();
        circle->setRadius(SPRITE_LENGTH / 2 * SCALE);
        circle->setFillColor(sf::Color::Green);
        circle->setPosition(x * SPRITE_LENGTH * SCALE, y * SPRITE_LENGTH * SCALE);
        shapes.push_back(std::move(circle));
    }
}

void DebugDrawJumps(std::vector<std::shared_ptr<my::Piece>>& pieces, std::vector<std::unique_ptr<sf::CircleShape>>& shapes)
{
    for (auto& piece : pieces)
    {
        if (piece->PossibleTakes.size() > 0)
        {
            for (auto pt : piece->PossibleTakes)
            {
                std::unique_ptr<sf::CircleShape> circle = std::make_unique<sf::CircleShape>();
                circle->setRadius(SPRITE_LENGTH / 2 * SCALE);
                circle->setOutlineColor(sf::Color::Green);
                circle->setOutlineThickness(5.0f);
                circle->setFillColor(sf::Color::Transparent);
                circle->setPosition(pt.landX * SPRITE_LENGTH * SCALE, pt.landY * SPRITE_LENGTH * SCALE);
                shapes.push_back(std::move(circle));
            }
        }
    }
}

void DebugDrawMoves(std::vector<std::shared_ptr<my::Piece>>& pieces, std::vector<std::unique_ptr<sf::CircleShape>>& shapes, my::Colour side)
{
    for (auto& piece : pieces)
    {
        if (piece->PossibleMoves.size() > 0 && piece->Side == side)
        {
            for (auto pm : piece->PossibleMoves)
            {
                std::unique_ptr<sf::CircleShape> circle = std::make_unique<sf::CircleShape>();
                circle->setRadius(SPRITE_LENGTH / 2 * SCALE);
                circle->setOutlineColor(sf::Color::Blue);
                circle->setOutlineThickness(5.0f);
                circle->setFillColor(sf::Color::Transparent);
                circle->setPosition(pm.x * SPRITE_LENGTH * SCALE, pm.y * SPRITE_LENGTH * SCALE);
                shapes.push_back(std::move(circle));
            }
        }
    }
}

void DrawDebugShapes(std::vector<std::unique_ptr<sf::CircleShape>>& shapes, sf::RenderWindow& window)
{
    for (auto& shape : shapes)
    {
        window.draw(*shape);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(BOARD_LENGTH * SPRITE_LENGTH * SCALE + GUI_WIDTH, BOARD_LENGTH * SPRITE_LENGTH * SCALE), "sfml-checkers", sf::Style::Close);

    // init textures
    my::TextureHolder textureHolder;
    if (!textureHolder.allTexturesLoaded) return 0;
    // init font
    sf::Font font;
    if (!font.loadFromFile("font/upheavtt.ttf")) return 0;

    // setup grid tracker
    bool grid[BOARD_LENGTH * BOARD_LENGTH] = {false};
    // setup board sprites
    std::vector<std::unique_ptr<sf::Sprite>> boardSprites;
    CreateBoard(boardSprites, textureHolder);
    // setup piece sprites
    std::vector<std::shared_ptr<sf::Sprite>> pieceSprites;
    std::vector<std::shared_ptr<my::Piece>> pieces;
    CreatePieces(pieces, textureHolder, grid, pieceSprites);
    // setup ui sprites
    std::vector<std::shared_ptr<sf::Sprite>> uiSprites;
    std::vector<std::shared_ptr<my::Button>> buttons;
    CreateUI(uiSprites, buttons, textureHolder);
    // debug shapes
    std::vector<std::unique_ptr<sf::CircleShape>> shapes;
    // setup text
    std::vector<std::shared_ptr<sf::Text>> text;
    CreateText(text, font);

    // black goes first
    my::Colour turn = my::Colour::BLACK;

    my::ButtonEvent buttonEvent = my::ButtonEvent::NONE;
    bool gameOver = false;
    // generate moves
    GenerateMoves(pieces, grid);

    std::shared_ptr<my::Piece> heldPiece = nullptr;
    std::unique_ptr<sf::Vector2i> prevPosition = std::make_unique<sf::Vector2i>(0, 0);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.waitEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed && !heldPiece)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;
                    if (!gameOver)
                    {
                        int gridX = ToGridCoordinate(x);
                        int gridY = ToGridCoordinate(y);
                        prevPosition->x = gridX * SPRITE_LENGTH * SCALE;
                        prevPosition->y = gridY * SPRITE_LENGTH * SCALE;
                        heldPiece = GetHeldPiece(pieces, grid, gridX, gridY); 
                        if (heldPiece != nullptr) {
                            if(heldPiece->Side != turn)  
                                heldPiece = nullptr;
                        }
                    }
                    CheckButtonClick(x, y, buttons);
                }
            }
            else if (event.type == sf::Event::MouseMoved && heldPiece)
            {       
                int x = sf::Mouse::getPosition(window).x;
                int y = sf::Mouse::getPosition(window).y;
                int max = BOARD_LENGTH * SCALE * SPRITE_LENGTH;
                if(x >= 0 && x <= max && y >= 0 && y <= max)
                    heldPiece->Sprite->setPosition(x - SPRITE_LENGTH/2 * SCALE, y - SPRITE_LENGTH/2 * SCALE);
            }
            else if (event.type == sf::Event::MouseButtonReleased && heldPiece)
            {
                int x = sf::Mouse::getPosition(window).x;
                int y = sf::Mouse::getPosition(window).y;

                int gridX = ToGridCoordinate(x);
                int gridY = ToGridCoordinate(y);
                if (AreAnyJumps(pieces)) // check for any jumps
                {
                    std::shared_ptr<my::Piece> pieceToTake = IsValidJump(gridX, gridY, heldPiece);
                    if (pieceToTake) // this is one of the jumps
                    {
                        heldPiece->Sprite->setPosition(gridX * SPRITE_LENGTH * SCALE, gridY * SPRITE_LENGTH * SCALE);
                        grid[gridY * BOARD_LENGTH + gridX] = true;
                        grid[ToGridCoordinate(prevPosition->y) * BOARD_LENGTH + ToGridCoordinate(prevPosition->x)] = false;
                        prevPosition->x = 0;
                        prevPosition->y = 0;
                        
                        // remove pieceToTake from pieces
                        grid[ToGridCoordinate(pieceToTake->Sprite->getPosition().y) * BOARD_LENGTH + 
                             ToGridCoordinate(pieceToTake->Sprite->getPosition().x)] = false;
                        pieceSprites.erase(std::remove(pieceSprites.begin(), pieceSprites.end(), pieceToTake->Sprite));
                        pieces.erase(std::remove(pieces.begin(), pieces.end(), pieceToTake));
                        shapes.clear();
                        // TODO: check and make king here!
                        CheckAndMakeKing(gridY, heldPiece, textureHolder);
                        GenerateJumps(grid, pieces, heldPiece->Side == my::Colour::BLACK ? my::Colour::RED : my::Colour::BLACK); // still jumps to make?
                        // change the turn if there are no more jumps
                        if (!AreAnyJumps(pieces))
                        {
                            GenerateJumps(grid, pieces, heldPiece->Side); // generate jumps for next turn
                            GenerateMoves(pieces, grid); // generate moves for next turn
                            CheckForWin(pieces, text[2], gameOver);
                            ToggleTurnUI(uiSprites, textureHolder, turn);
                            turn = (int)turn ? my::Colour::RED : my::Colour::BLACK;
                        }
                    }
                    else // attempted move was not one of the jumps and therefore is cancelled
                    {
                        heldPiece->Sprite->setPosition(prevPosition->x, prevPosition->y);
                    }
                    heldPiece = nullptr;
                }
                else // check for normal movement
                {
                    // invalid move pos
                    if (!IsValidMove(heldPiece, gridX, gridY))
                    {
                        heldPiece->Sprite->setPosition(prevPosition->x, prevPosition->y);
                    }
                    else //valid pos
                    {
                        heldPiece->Sprite->setPosition(gridX * SPRITE_LENGTH * SCALE, gridY * SPRITE_LENGTH * SCALE);
                        grid[gridY * BOARD_LENGTH + gridX] = true;
                        grid[ToGridCoordinate(prevPosition->y) * BOARD_LENGTH + ToGridCoordinate(prevPosition->x)] = false;
                        prevPosition->x = 0;
                        prevPosition->y = 0;
                        // TODO: check and make king here!
                        CheckAndMakeKing(gridY, heldPiece, textureHolder);
                        GenerateJumps(grid, pieces, heldPiece->Side); // generate jumps for next turn
                        GenerateMoves(pieces, grid); // generate moves for next turn
                        CheckForWin(pieces, text[2], gameOver);
                        // change the turn
                        ToggleTurnUI(uiSprites, textureHolder, turn);
                        turn = (int)turn ? my::Colour::RED : my::Colour::BLACK;
                        shapes.clear();
                    }
                    heldPiece = nullptr;
                }
                DebugDrawJumps(pieces, shapes);
                //DebugDrawMoves(pieces, shapes, turn);
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                int x = sf::Mouse::getPosition(window).x;
                int y = sf::Mouse::getPosition(window).y;
                PerformButtonClick(x, y, buttons, buttonEvent);
                if ((int)buttonEvent)
                {
                    switch (buttonEvent)
                    {
                        case my::ButtonEvent::RESIGN:
                        {
                            if (!gameOver)
                            {
                                if ((int)turn) // black resign
                                    GameWon(my::Colour::RED, text[2], gameOver);
                                else
                                    GameWon(my::Colour::BLACK, text[2], gameOver);

                                std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(*textureHolder.resignButtonTexture);
                                sprite->setPosition(BOARD_LENGTH* SPRITE_LENGTH* SCALE * 1.155 - sprite->getGlobalBounds().width / 2, 5 * SPRITE_LENGTH * SCALE);
                                sprite->setScale(SCALE, SCALE);
                                uiSprites.push_back(sprite);
                                std::shared_ptr<my::Button> restartButton = std::make_shared<my::Button>(sprite, my::ButtonEvent::RESTART);
                                buttons.push_back(restartButton);

                                std::shared_ptr<sf::Text> txt = std::make_shared<sf::Text>("RESTART", font);
                                txt->setCharacterSize(28);
                                txt->setStyle(sf::Text::Bold);
                                txt->setFillColor(sf::Color::Red);
                                txt->setPosition(BOARD_LENGTH* SPRITE_LENGTH* SCALE * 1.25 - txt->getGlobalBounds().width / 2, 5 * SPRITE_LENGTH * SCALE + txt->getGlobalBounds().height * 0.8); // center in the ui 5 squares down from the top
                                text.push_back(txt);
                            }
                        }break;
                        case my::ButtonEvent::RESTART:
                        {
                            shapes.clear();
                            pieces.clear();
                            pieceSprites.clear();
                            for (int i = 0; i < BOARD_LENGTH * BOARD_LENGTH; i++)
                            {
                                grid[i] = false;
                            }
                            CreatePieces(pieces, textureHolder, grid, pieceSprites);
                            turn = my::Colour::BLACK;
                            gameOver = false;
                            text[2]->setString("");
                            std::shared_ptr<my::Button> restartButton;
                            for (auto& btn : buttons)
                            {
                                if (btn->BtnEvent == my::ButtonEvent::RESTART)
                                {
                                    restartButton = btn;
                                }
                            }
                            uiSprites.erase(std::remove(uiSprites.begin(), uiSprites.end(), restartButton->Sprite));
                            buttons.erase(std::remove(buttons.begin(), buttons.end(), restartButton));
                            std::shared_ptr<sf::Text> restartTxt;
                            for (auto& txt : text)
                            {
                                if (txt->getString() == "RESTART")
                                {
                                    restartTxt = txt;
                                }
                            }
                            text.erase(std::remove(text.begin(), text.end(), restartTxt));
                            GenerateMoves(pieces, grid);
                        }break;
                        default:
                        {
                            return 0;
                        }
                    }
                    buttonEvent = my::ButtonEvent::NONE;
                }
            }
            window.clear();
            RenderSprites(boardSprites, window);
            RenderSprites(pieceSprites, window);
            RenderSprites(uiSprites, window);
            RenderText(text, window);
            DrawDebugShapes(shapes, window);
            window.display();
        }
    }

    return 0;
}


#include "main.h"
const int BOARD_LENGTH = 8;
const int SPRITE_LENGTH = 16;
const float SCALE = 4.0f;
const int GUI_WIDTH = 256;
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
    CreateUI(uiSprites, textureHolder);
    // debug shapes
    std::vector<std::unique_ptr<sf::CircleShape>> shapes;
    // setup text
    std::vector<std::shared_ptr<sf::Text>> text;
    CreateText(text, font);

    // black goes first
    my::Colour turn = my::Colour::BLACK;

    std::shared_ptr<my::Piece> heldPiece = nullptr;
    std::unique_ptr<sf::Vector2i> prevPosition = std::make_unique<sf::Vector2i>(0, 0);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.waitEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed && heldPiece == nullptr)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int x = ToGridCoordinate(event.mouseButton.x);
                    int y = ToGridCoordinate(event.mouseButton.y);
                    prevPosition->x = x * SPRITE_LENGTH * SCALE;
                    prevPosition->y = y * SPRITE_LENGTH * SCALE;
                    heldPiece = GetHeldPiece(pieces, grid, x, y); 
                    if (heldPiece != nullptr) {
                        if(heldPiece->Side != turn)  
                            heldPiece = nullptr;
                    }
                }
            }
            else if (event.type == sf::Event::MouseMoved && heldPiece != nullptr)
            {       
                int x = sf::Mouse::getPosition(window).x;
                int y = sf::Mouse::getPosition(window).y;
                int max = BOARD_LENGTH * SCALE * SPRITE_LENGTH;
                if(x >= 0 && x <= max && y >= 0 && y <= max)
                    heldPiece->Sprite->setPosition(x - SPRITE_LENGTH/2 * SCALE, y - SPRITE_LENGTH/2 * SCALE);
            }
            else if (event.type == sf::Event::MouseButtonReleased && heldPiece != nullptr)
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
                            ToggleTurnUI(uiSprites, textureHolder, turn);
                            turn = (int)turn ? my::Colour::RED : my::Colour::BLACK;
                        }
                        CheckForWin(pieces, text[2]);
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
                    if (!CheckPos(grid, gridX, gridY, heldPiece.get(), prevPosition.get()))
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
                        // change the turn
                        ToggleTurnUI(uiSprites, textureHolder, turn);
                        turn = (int)turn ? my::Colour::RED : my::Colour::BLACK;
                    }
                    heldPiece = nullptr;
                }
                DebugDrawJumps(pieces, shapes);
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

void CreateUI(std::vector<std::shared_ptr<sf::Sprite>>& sprites, my::TextureHolder& textureHolder)
{
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(*textureHolder.uiBackgroundTexture);
    sprite->setPosition(sf::Vector2f(BOARD_LENGTH * SPRITE_LENGTH * SCALE, 0));
    sprite->setScale(SCALE, SCALE);
    sprites.push_back(sprite);

    sprite = std::make_shared<sf::Sprite>(*textureHolder.resignButtonTexture);
    sprite->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.155 - sprite->getGlobalBounds().width/2, 4 * SPRITE_LENGTH * SCALE);
    sprite->setScale(SCALE, SCALE);
    sprites.push_back(sprite);

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
    txt->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.25 - txt->getGlobalBounds().width/2, SPRITE_LENGTH * SCALE); // center in the ui a square down from the top
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

// returns false if pos is occupied
bool CheckPos(bool* grid, int x, int y, my::Piece* heldPiece, sf::Vector2i* prevPosition)
{
    if (!IsWithinGrid(x) || !IsWithinGrid(y)) // off the board
    {
        return false;
    }
    else if (grid[y * BOARD_LENGTH + x]) // spot occupied
    {
        return false;
    }
    else if (heldPiece->isKing && (IsInvalidRedMove(prevPosition, x, y) && IsInvalidBlackMove(prevPosition, x, y)))
    {
        return false;
    }
    else if (heldPiece->Side == my::Colour::RED && !heldPiece->isKing && IsInvalidRedMove(prevPosition, x, y)) // Make sure normal red pieces only go diagonaly down
    {
        return false;
    }
    else if (heldPiece->Side == my::Colour::BLACK && !heldPiece->isKing && IsInvalidBlackMove(prevPosition, x, y)) // Make sure normal black pieces only go diagonaly up
    {
        return false;
    }
    return true;
}

bool IsInvalidRedMove(sf::Vector2i* prevPosition, int x, int y)
{
    return prevPosition->x - x * SPRITE_LENGTH * SCALE != -1 * SPRITE_LENGTH * SCALE && prevPosition->y - y * SPRITE_LENGTH * SCALE != -1 * SPRITE_LENGTH * SCALE || // not diagonal right
        prevPosition->x - x * SPRITE_LENGTH * SCALE != 1 * SPRITE_LENGTH * SCALE && prevPosition->y - y * SPRITE_LENGTH * SCALE != -1 * SPRITE_LENGTH * SCALE || // not diagonal left
        prevPosition->x - x * SPRITE_LENGTH * SCALE == 0 || // direct down not allowed
        abs(prevPosition->x - x * SPRITE_LENGTH * SCALE) > 1 * SPRITE_LENGTH * SCALE; // cant move more than 1 left or right
}

bool IsInvalidBlackMove(sf::Vector2i* prevPosition, int x, int y)
{
    return prevPosition->x - x * SPRITE_LENGTH * SCALE != -1 * SPRITE_LENGTH * SCALE && prevPosition->y - y * SPRITE_LENGTH * SCALE != 1 * SPRITE_LENGTH * SCALE || // not diagonal right
        prevPosition->x - x * SPRITE_LENGTH * SCALE != 1 * SPRITE_LENGTH * SCALE && prevPosition->y - y * SPRITE_LENGTH * SCALE != 1 * SPRITE_LENGTH * SCALE || // not diagonal left
        prevPosition->x - x * SPRITE_LENGTH * SCALE == 0 ||  // direct up not allowed
        abs(prevPosition->x - x * SPRITE_LENGTH * SCALE) > 1 * SPRITE_LENGTH * SCALE; // cant move more than 1 left or right
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

void CheckForWin(std::vector<std::shared_ptr<my::Piece>>& pieces, std::shared_ptr<sf::Text> winText)
{
    int redCount = 0;
    int blackCount = 0;
    for (auto& piece : pieces)
    {
        if (piece->Side == my::Colour::BLACK)
            blackCount++;
        else
            redCount++;
    }
    if (redCount == 0)
        GameWon(my::Colour::BLACK, winText);
    else if (blackCount == 0)
        GameWon(my::Colour::RED, winText);
}

void GameWon(my::Colour side, std::shared_ptr<sf::Text> winText)
{
    if (side == my::Colour::BLACK)
    {
        winText->setString("BLACK WINS");
    }
    else
    {
        winText->setString("RED WINS");
    }
    winText->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE / 2 - winText->getGlobalBounds().width / 2, BOARD_LENGTH * SPRITE_LENGTH * SCALE / 2 - winText->getGlobalBounds().height * 1.5); // center in the ui 4 squares down from the top

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
                circle->setRadius(SPRITE_LENGTH/2 * SCALE);
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

void DrawDebugShapes(std::vector<std::unique_ptr<sf::CircleShape>>& shapes, sf::RenderWindow& window)
{
    for (auto& shape : shapes)
    {
        window.draw(*shape);
    }
}

// UTILITY FUNCTIONS

int ToGridCoordinate(int n)
{
    return n / (SPRITE_LENGTH * SCALE);
}

bool IsWithinGrid(int n)
{
    return n >= 0 && n <= BOARD_LENGTH - 1;
}

bool IsSquareOccupied(int x, int y, bool *grid)
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
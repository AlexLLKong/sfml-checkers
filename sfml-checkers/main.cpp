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

    // init font
    sf::Font font;
    if (!font.loadFromFile("font/upheavtt.ttf"))
    {
        return 0;
    }

    // setup grid tracker
    bool grid[BOARD_LENGTH * BOARD_LENGTH] = {false};
    // setup board sprites
    std::vector<sf::Sprite*> boardSprites;
    CreateBoard(boardSprites, textureHolder);
    // setup piece sprites
    std::vector<sf::Sprite*> pieceSprites;
    std::vector<my::Piece*> pieces;
    CreatePieces(pieces, textureHolder, grid, pieceSprites);
    // setup ui sprites
    std::vector<sf::Sprite*> uiSprites;
    CreateUI(uiSprites, textureHolder);
    // debug shapes
    std::vector<sf::CircleShape> shapes;
    // setup text
    std::vector<sf::Text*> text;
    CreateText(text, font);

    // black goes first
    my::Side turn = my::Side::BLACK;

    my::Piece* heldPiece = nullptr;
    sf::Vector2i* prevPosition = new sf::Vector2i(0, 0);
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
                    heldPiece = GetHeldSprite(pieces, grid, x, y); 
                    if (heldPiece != nullptr) {
                        if(heldPiece->side != turn)  
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
                    heldPiece->sprite->setPosition(x - SPRITE_LENGTH/2 * SCALE, y - SPRITE_LENGTH/2 * SCALE);
            }
            else if (event.type == sf::Event::MouseButtonReleased && heldPiece != nullptr)
            {
                int x = sf::Mouse::getPosition(window).x;
                int y = sf::Mouse::getPosition(window).y;
                int gridX = ToGridCoordinate(x);
                int gridY = ToGridCoordinate(y);
                // invalid pos
                if (!CheckPos(grid, gridX, gridY, heldPiece, prevPosition))
                {
                    heldPiece->sprite->setPosition(prevPosition->x, prevPosition->y);
                }
                else //valid pos
                {
                    heldPiece->sprite->setPosition(gridX * SPRITE_LENGTH * SCALE, gridY * SPRITE_LENGTH * SCALE);
                    grid[gridY * BOARD_LENGTH + gridX] = true;
                    grid[ToGridCoordinate(prevPosition->y) * BOARD_LENGTH + ToGridCoordinate(prevPosition->x)] = false;
                    prevPosition->x = 0;
                    prevPosition->y = 0;

                    // change the turn
                    ToggleTurnUI(uiSprites, textureHolder, turn);
                    turn = (int)turn ? my::Side::RED : my::Side::BLACK;
                }
                heldPiece = nullptr;
            }
            window.clear();
            RenderSprites(boardSprites, window);
            RenderSprites(pieceSprites, window);
            RenderSprites(uiSprites, window);
            RenderText(text, window);
            //DrawDebugShapes(shapes, window);
            window.display();
        }
    }

    return 0;
}

void CreatePieces(std::vector<my::Piece*>& pieces, my::TextureHolder& textureHolder, bool* grid, std::vector<sf::Sprite*>& sprites)
{
    int z = 0;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z; j < BOARD_LENGTH / 2 - 1; j += 2)
        {
            my::Piece* piece = new my::Piece();
            piece->side = my::Side::RED;
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setTexture(*textureHolder.redPieceTexture);
            sprite->setPosition(sf::Vector2f(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE));
            sprite->setScale(SCALE, SCALE);
            piece->sprite = sprite;
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
            my::Piece* piece = new my::Piece();
            piece->side = my::Side::BLACK;
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setTexture(*textureHolder.blackPieceTexture);
            sprite->setPosition(sf::Vector2f(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE));
            sprite->setScale(SCALE, SCALE);
            piece->sprite = sprite;
            pieces.push_back(piece);
            sprites.push_back(sprite);
            grid[j * BOARD_LENGTH + i] = true;
        }
        z ^= 1;
    }
}

void CreateBoard(std::vector<sf::Sprite*>& sprites, my::TextureHolder& textureHolder)
{
    int z = 0;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z; j < BOARD_LENGTH; j += 2)
        {
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setTexture(*textureHolder.redSquareTexture);
            sprite->setPosition(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE);
            sprite->setScale(SCALE, SCALE);
            sprites.push_back(sprite);
        }
        z ^= 1;
    }
    z = 1;
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = z; j < BOARD_LENGTH; j += 2)
        {
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setTexture(*textureHolder.blackSquareTexture);
            sprite->setPosition(sf::Vector2f(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE));
            sprite->setScale(SCALE, SCALE);
            sprites.push_back(sprite);
        }
        z ^= 1;
    }
}

void CreateUI(std::vector<sf::Sprite*>& sprites, my::TextureHolder& textureHolder)
{
    sf::Sprite* uiBackground = new sf::Sprite();
    uiBackground->setTexture(*textureHolder.uiBackgroundTexture);
    uiBackground->setPosition(sf::Vector2f(BOARD_LENGTH * SPRITE_LENGTH * SCALE, 0));
    uiBackground->setScale(SCALE, SCALE);
    sprites.push_back(uiBackground);

    sf::Sprite* resignButton = new sf::Sprite();
    resignButton->setTexture(*textureHolder.resignButtonTexture);
    resignButton->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.155 - resignButton->getGlobalBounds().width/2, 4 * SPRITE_LENGTH * SCALE);
    resignButton->setScale(SCALE, SCALE);
    sprites.push_back(resignButton);

    sf::Sprite* turnBackground = new sf::Sprite();
    turnBackground->setTexture(*textureHolder.redSquareTexture);
    turnBackground->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE + 1.5 * SPRITE_LENGTH * SCALE, 2 * SPRITE_LENGTH * SCALE);
    turnBackground->setScale(SCALE, SCALE);
    sprites.push_back(turnBackground);

    sf::Sprite* turnPiece = new sf::Sprite();
    turnPiece->setTexture(*textureHolder.blackPieceTexture);
    turnPiece->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE + 1.5 * SPRITE_LENGTH * SCALE, 2 * SPRITE_LENGTH * SCALE);
    turnPiece->setScale(SCALE, SCALE);
    sprites.push_back(turnPiece);
}

void CreateText(std::vector<sf::Text*>& text, sf::Font& font)
{
    sf::Text* turnText = new sf::Text("TURN", font);
    turnText->setCharacterSize(48);
    turnText->setStyle(sf::Text::Bold);
    turnText->setFillColor(sf::Color::Red);
    turnText->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.25 - turnText->getGlobalBounds().width/2, SPRITE_LENGTH * SCALE); // center in the ui a square down from the top
    text.push_back(turnText);

    sf::Text* resignText = new sf::Text("RESIGN", font);
    resignText->setCharacterSize(28);
    resignText->setStyle(sf::Text::Bold);
    resignText->setFillColor(sf::Color::Red);
    resignText->setPosition(BOARD_LENGTH * SPRITE_LENGTH * SCALE * 1.25 - resignText->getGlobalBounds().width / 2, 4 * SPRITE_LENGTH * SCALE + resignText->getGlobalBounds().height * 0.8); // center in the ui 4 squares down from the top
    text.push_back(resignText);
}
my::Piece* GetHeldSprite(std::vector<my::Piece*>& pieces, bool* grid, int x, int y)
{
    if (grid[y * BOARD_LENGTH + x])
    {
        for (int k = 0; k < pieces.size(); k++)
        {
            int sprX = pieces[k]->sprite->getPosition().x / (SPRITE_LENGTH * SCALE);
            int sprY = pieces[k]->sprite->getPosition().y / (SPRITE_LENGTH * SCALE);
            if (x == sprX && y == sprY)
                return pieces[k];
        }
    }
    return nullptr;
}

void ToggleTurnUI(std::vector<sf::Sprite*>& sprites, my::TextureHolder& textureHolder, my::Side turn)
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

void RenderSprites(std::vector<sf::Sprite*>& sprites, sf::RenderWindow& window)
{
    for (auto spr : sprites)
    {
        window.draw(*spr);
    }
}

void RenderText(std::vector<sf::Text*>& text, sf::RenderWindow& window)
{
    for (auto t : text)
    {
        window.draw(*t);
    }
}

// returns false if pos is occupied
bool CheckPos(bool* grid, int x, int y, my::Piece* heldPiece, sf::Vector2i* prevPosition)
{
    if (x < 0 || x > BOARD_LENGTH - 1) // off the board
    {
        return false;
    }
    else if (grid[y * BOARD_LENGTH + x]) // spot occupied
    {
        return false;
    }
    else if (heldPiece->side == my::Side::RED && IsInvalidRedMove(prevPosition, x, y)) // Make sure normal red pieces only go diagonaly down
    {
        return false;
    }
    else if (heldPiece->side == my::Side::BLACK && IsInvalidBlackMove(prevPosition, x, y)) // Make sure normal black pieces only go diagonaly up
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

// DEBUG FUNCTIONS
void DebugGrid(std::vector<sf::CircleShape>& shapes, bool* grid)
{
    for (int i = 0; i < BOARD_LENGTH; i++)
    {
        for (int j = 0; j < BOARD_LENGTH; j++)
        {
            if (grid[j * BOARD_LENGTH + i] == true)
            {
                sf::CircleShape circle;
                circle.setRadius(SPRITE_LENGTH/2 * SCALE);
                circle.setFillColor(sf::Color::Green);
                circle.setPosition(i * SPRITE_LENGTH * SCALE, j * SPRITE_LENGTH * SCALE);
                shapes.push_back(circle);
            }
        }
    }
}

void DebugClickOnGrid(std::vector<sf::CircleShape>& shapes, bool* grid, int x, int y)
{
    if (grid[y * BOARD_LENGTH + x])
    {
        sf::CircleShape circle;
        circle.setRadius(SPRITE_LENGTH / 2 * SCALE);
        circle.setFillColor(sf::Color::Green);
        circle.setPosition(x * SPRITE_LENGTH * SCALE, y * SPRITE_LENGTH * SCALE);
        shapes.push_back(circle);
    }
}

void DrawDebugShapes(std::vector<sf::CircleShape>& shapes, sf::RenderWindow& window)
{
    for (auto shape : shapes)
    {
        window.draw(shape);
    }
}

// UTILITY FUNCTIONS

int ToGridCoordinate(int n)
{
    return n / (SPRITE_LENGTH * SCALE);
}
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <raylib.h>
#include <limits>
#include <tuple>

enum class Type {
    Pawn, Rook, Knight, Bishop, Queen, King
};

struct Piece
{
    bool isMoving = false;
    bool color;
    Type name;
    Vector2 Position;
    Texture2D tex;
    size_t id;
    static inline size_t index = 0;

    Piece() = default;
    Piece(bool n_color, Type n_nam, const Vector2 &n_position, const Texture2D &n_tex)
        : color(n_color), name(n_nam), Position(n_position), tex(n_tex)
        {
            id = index;
            index = index + 1;
        };

    bool operator==(const Piece& other) const 
    {
        return (id == other.id);
    }

};

static void displayBoard() noexcept;
static void resetBoard() noexcept;
static bool nTimesAwayLiniar(size_t dx, size_t dy, size_t px, size_t py, size_t n) noexcept;
static bool nTimesAwayDiagonal(size_t dx, size_t dy, size_t px, size_t py, size_t n) noexcept;
void firstGrater(size_t &x, size_t &y) noexcept;
bool isNotJumpingOver(size_t dx, size_t dy, size_t px, size_t py) noexcept;

static void move(Piece &p1, const Vector2& mousePos, std::vector<Piece>& piecesSame, std::vector<Piece> &piecesOpposite) noexcept;
static bool moveIsValid(const Piece& p1, const Vector2 &prev) noexcept;
static bool areEqual(double a, double b, double epsilon = 1e-9) noexcept;
static std::tuple<size_t, size_t> getCoord(const Piece &p) noexcept;
static std::tuple<size_t, size_t> getCoord(const Vector2 &p) noexcept;

static std::vector<Piece> generateAllPieces() noexcept;
static void correctPosition(Piece& p1) noexcept;

static constexpr size_t boardSize = 700;
static constexpr float cellSize = static_cast<float>(boardSize)/8;

bool moving = false;
bool turn = true;

size_t board[8][8];
int main()
{
  
    resetBoard();
    displayBoard();
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(boardSize, boardSize, "Chess");
    SetTargetFPS(60);

    const std::vector<Piece> whitePiecesStarter = generateAllPieces();
    const std::vector<Piece> blackPiecesStarter = generateAllPieces();

    std::vector<Piece> whitePieces = whitePiecesStarter;
    std::vector<Piece> blackPieces = blackPiecesStarter;


    float scaleX = 1.f, scaleY = 1.f;
    Texture2D boardTexture = LoadTexture("chess.png");
    Rectangle prev{0.f, 0.f, static_cast<float>(boardSize), static_cast<float>(boardSize)};


    while (!WindowShouldClose())
    {
        
        if(IsKeyPressed(KEY_D))
            displayBoard();
        size_t width = GetScreenWidth(), height = GetScreenHeight();
        if(width < 700)
            width = 700;
        if(height < 700)
            height = 700;
        SetWindowSize(width, height);            
        BeginDrawing();

        if(IsKeyPressed(KEY_R))
        {
            whitePieces = whitePiecesStarter;
            blackPieces = blackPiecesStarter;
            turn = true;
            resetBoard();
        }
        
        if(IsKeyPressed(KEY_C))
        {
            if(turn)
                whitePieces.clear();
            else
                blackPieces.clear();
        }

        ClearBackground(RAYWHITE);

        if(!areEqual(prev.width, width) || !areEqual(prev.height, height))
        {   
            scaleX = static_cast<float>(width)/boardSize;
            scaleY = static_cast<float>(height)/boardSize;
        }
        Rectangle source{0.f, 0.f, prev.width * scaleX, prev.height * scaleY};
        
        DrawTexturePro(boardTexture, prev, source, {0.f, 0.f}, 0.f, WHITE);
        
        prev.width = width;
        prev.height = height;
        
        Vector2 mousePos = GetMousePosition();

        for(size_t i = 0; i < whitePieces.size(); ++i)
        {
            if(turn)
                move(whitePieces[i], mousePos, whitePieces, blackPieces);
                      
            DrawTexture(whitePieces[i].tex, whitePieces[i].Position.x - cellSize/2, whitePieces[i].Position.y - cellSize/2, WHITE);
        }
    

       for(size_t i = 0; i < blackPieces.size(); ++i)
        {
            if(!turn)
                move(blackPieces[i], mousePos, blackPieces, whitePieces);
            
            DrawTexture(blackPieces[i].tex, blackPieces[i].Position.x - cellSize/2, blackPieces[i].Position.y - cellSize/2, WHITE);
        }

        if(whitePieces.size() == 0)
        {          
            DrawRectangle(width / 2 - 135, height / 2 - 50, 275, 50, GREEN);
            DrawText("Black wins!", width/2 - 130, height/2 - 50, 50, RAYWHITE);
        }

        if(blackPieces.size() == 0)
        {
            DrawRectangle(width/2 - 135, height/2 - 50, 275, 50, GREEN);
            DrawText("White wins!", width/2 - 130, height/2 - 50, 50, BLACK);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
}

void correctPosition(Piece& p1) noexcept
{   
    auto [dx, dy] = getCoord(p1);

    // board[dx][dy] = p1.color;
    p1.Position.x = dx * cellSize + cellSize/2;
    p1.Position.y = dy * cellSize + cellSize/2;

}

std::vector<Piece> generateAllPieces() noexcept
{
    static size_t times = 0;// If this is even we generate for White, otherwise we generate for BLACK

    std::vector<Piece> vect;

    vect.reserve(16);    

    Texture2D pawnTexture, kingTexture, rookTexture, queenTextutre, knightTexture, bishopTexture;
    if(times%2)
    {
        pawnTexture = LoadTexture("blackPawn.png");
        kingTexture = LoadTexture("kingChessBlack.png");
        rookTexture = LoadTexture("rookBlack.png");
        queenTextutre = LoadTexture("BlackQueen.png");
        knightTexture = LoadTexture("knightBlack.png");
        bishopTexture = LoadTexture("Bishop.png");
    }
    else
    {
        pawnTexture = LoadTexture("White_pawn.png");
        kingTexture = LoadTexture("kingWhite.png");
        rookTexture = LoadTexture("rookWhite.png");
        queenTextutre = LoadTexture("WhiteQueen.png");
        knightTexture = LoadTexture("knightWhite.png");
        bishopTexture = LoadTexture("whiteBishop.png");
    }

    for(size_t i = 1; i <= 8; ++i)
    {
        Piece pawn{static_cast<bool>(times%2), Type::Pawn, {i * cellSize, (times%2 * 5 + 1) * cellSize + cellSize/2}, pawnTexture};

        correctPosition(pawn);

        vect.push_back(pawn);
    }

    Texture2D texes[] = {rookTexture, knightTexture, bishopTexture};
    //Add Texture for the rest
    for(size_t i = 1; i <= 3; ++i)
    {  
        Piece Piece{static_cast<bool>(times%2), static_cast<Type>(i), {i * cellSize, (times%2 * 7 ) * cellSize + cellSize/2}, texes[i - 1]};
        correctPosition(Piece);
        vect.push_back(Piece);

        Piece.index++;
        Piece.id++;
        Piece.Position.x = cellSize * (9 - i);
        correctPosition(Piece);
        vect.push_back(Piece);
    }

    Piece KingQueen{static_cast<bool>(times%2), Type::King, {4 * cellSize, (times%2 * 7 ) * cellSize + cellSize/2}, kingTexture};
    correctPosition(KingQueen);
    vect.push_back(KingQueen);
    
    KingQueen.index++;
    KingQueen.id++;
    
    KingQueen.tex = queenTextutre;
    KingQueen.name = Type::Queen;
    KingQueen.Position.x = 5 * cellSize;
    correctPosition(KingQueen);
    vect.push_back(KingQueen);;

    ++times;

  
    return vect;
}

void resetBoard() noexcept
{
  for(size_t i = 0; i < 8; ++i)
    {
        for(size_t j = 0; j < 8; ++j)
        {
            board[i][j] = 2;
            if(j == 0 || j == 1)
               board[i][j] = 0;
            if(j == 6 || j == 7)
                board[i][j] = 1;
            
        }
    }
}
void displayBoard() noexcept
{
    std::cout << "-----------------------------\n";
    for(size_t i = 0; i < 8; ++i)
    {
        for(size_t j = 0; j < 8; ++j)
        {
            std::cout << board[i][j] << ' ';
        }
        std::cout << '\n';
    }
}

void firstGrater(size_t &x, size_t &y) noexcept
{
    if(x > y)
    {
        x = x + y;
        y = x - y;
        x = x - y;
    }
}

bool nTimesAwayLiniar(size_t dx, size_t dy, size_t px, size_t py, size_t n) noexcept
{
    if((px - dx <= n) && (py == dy))
        return true;

    if((py - dy <= n) && px == dx)
        return true;

    return false;
}


bool nTimesAwayDiagonal(size_t dx, size_t dy, size_t px, size_t py, size_t n) noexcept
{
    if((px - dx <= n) && (py - dy <= n) && (px - dx == py - dy))
        return true;
    
    return false;
}

bool isNotJumpingOver(size_t dx, size_t dy, size_t px, size_t py) noexcept
{

    if(dx == px)
    {
        
        for(size_t j = dy + 1; j < py; ++j)
        {
            if(board[dx][j] != 2)
            {
                return false;
            }
        }

    }else
    if(dy == py)
    {
        
        for(size_t j = dx + 1; j < px; ++j)
        {
            if(board[j][dy] != 2)
            {
                return false;
            }
        }

    }
    else
    {
        size_t i = px + 1, j = py + 1;

        std::cout << i << ' ' << j << '\n';
        while (i < dx)
        {
            if(board[i][j] != 2)
                return false;
            
            ++i;
            ++j;
        }
        
        // std::cout << dx << ' ' << dy << ' ' << py << '\n';
        // for(size_t i = dx + 1; i < px; ++i)
        // {
        //     for(size_t j = dy + 1; j < py; ++j)
        //     {
        //         std::cout << "Problema e la mine C: \n";
        //         std::cout << "i = " << i << "j = " << j << '\n';
        //         if(board[i][j] != 2)
        //         {
        //             //std::cout << board[i][j] << ' ' << i << ' ' << j << '\n';
        //             return false;
        //         }
        //     }
        // }
    }
    // std::cout << "dx = " << dx << " dy = " << dy << "px = " << px << " py = " << py << '\n'; 

    return true;
}

bool moveIsValid(const Piece &p1, const Vector2 &prev) noexcept
{

    Vector2 pos = GetMousePosition();
    auto[dx0, dy0] = getCoord(pos);
    auto[px0, py0] = getCoord(prev);

    size_t dx = dx0, px = px0, dy = dy0, py = py0;

    firstGrater(dx, px);
    firstGrater(dy, py);

    if(dx == px && dy == py)
        return true;

    switch (p1.name)
    {
    case Type::Pawn:

        //if(!isNotJumpingOver(dx, dy, px, py))
        //    return false;
        // if(p1.color && py == 6 && py - 2 == dy && dx == px && board[dx][dy] == 2)
        //     return true;
        // if(!p1.color && py == 1 && py + 2 == dy && dx == px && board[dx][dy] == 2)
        //     return true;

        // if(p1.color && py - 1 == dy && dx == px && board[dx][dy] == 2)
        //     return true;
        // if(!p1.color && py + 1 == dy && dx == px && board[dx][dy] == 2)
        //     return true;
        
        // if(p1.color && py - 1 == dy && (dx == px + 1 || dx == px - 1) && board[dx][dy] == 0)
        //     return true;
        // if(!p1.color && py + 1 == dy && (dx == px + 1 || dx == px - 1) && board[dx][dy] == 1)
        //     return true;
        
        // return false;   
        return true;
    case Type::King:
        return nTimesAwayLiniar(dx, dy, px, py, 1) || nTimesAwayDiagonal(dx, dy, px, py, 1);
    case Type::Bishop:
        return (nTimesAwayDiagonal(dx, dy, px, py, 8) && isNotJumpingOver(dx0, dy0, px0, py0));
    case Type::Rook:
        return (nTimesAwayLiniar(dx, dy, px, py, 8) && isNotJumpingOver(dx, dy, px, py));
    case Type::Queen:
        return ((nTimesAwayLiniar(dx, dy, px, py, 8) || nTimesAwayDiagonal(dx, dy, px, py, 8)) && isNotJumpingOver(dx0, dy0, px0, py0));
    case Type::Knight:       
        if((px - dx == 1 && py - dy == 2) || (px - dx == 2  && py - dy == 1))
            return true;
        return false;
    default:
        std::cerr << "How did you get here?\n";
        exit(1);
    }
    
}
void move(Piece &p1, const Vector2& mousePos, std::vector<Piece> &pieceSame, std::vector<Piece> &piecesOpposite) noexcept
{    
    static Vector2 prevPos;
    
    if(!moving && !p1.isMoving && CheckCollisionPointCircle(mousePos, p1.Position, 30) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        moving = true;
        prevPos = p1.Position;
        p1.isMoving = true;
    }
    else if(p1.isMoving && !IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        p1.Position = mousePos;          
    }
    else if (p1.isMoving && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if(moveIsValid(p1, prevPos))
        {           
            moving = false;
            turn = !turn;
            p1.isMoving = false;
            p1.Position = mousePos;
            correctPosition(p1);
            auto [dx, dy] = getCoord(p1);
            auto [x, y] = getCoord(prevPos);
            
            if(areEqual(p1.Position.x, prevPos.x) && areEqual(p1.Position.y, prevPos.y))
            {
                turn = !turn;
                return;
            }

            for(size_t i = 0; i < pieceSame.size(); ++i)
            {
                if((p1.id != pieceSame[i].id) && areEqual(p1.Position.x, pieceSame[i].Position.x) && areEqual(p1.Position.y, pieceSame[i].Position.y))
                {
                    p1.Position = prevPos;
                    turn = !turn;
                    return;
                }
            }

            for(size_t i = 0; i < piecesOpposite.size(); ++i)
            {
                if(areEqual(p1.Position.x, piecesOpposite[i].Position.x) && areEqual(p1.Position.y, piecesOpposite[i].Position.y))
                {
                    board[x][y] = 2;
                    board[dx][dy] = p1.color;
                    piecesOpposite.erase(std::remove(piecesOpposite.begin(), piecesOpposite.end(), piecesOpposite[i]), piecesOpposite.end());
                    board[dx][dy] = p1.color;
                    return;
                }
            }
            board[x][y] = 2;
            board[dx][dy] = p1.color;
            
        }
    
    }

}

bool areEqual(double a, double b, double epsilon) noexcept
{
    return std::abs(a - b) < epsilon;
}

std::tuple<size_t, size_t> getCoord(const Piece &p) noexcept
{

    size_t x = p.Position.x, y = p.Position.y;

    size_t currMaxX = std::numeric_limits<size_t>::max();
    size_t currMaxY = std::numeric_limits<size_t>::max();

    size_t xCoord = 0, yCoord = 0;
    for(size_t i = 1; i <= 8; ++i)
    {
        size_t val = i * cellSize - 40;
        size_t abValX = std::abs(static_cast<int>(x - val));
        size_t abValY = std::abs(static_cast<int>(y - val));

        if(abValX < currMaxX)
        {
            currMaxX = abValX;
            xCoord = i;
        }

        if(abValY < currMaxY)
        {
            currMaxY = abValY;
            yCoord = i;
        }
    }

    return std::make_tuple(xCoord - 1, yCoord - 1);
}


std::tuple<size_t, size_t> getCoord(const Vector2 &p) noexcept
{

    size_t x = p.x, y = p.y;

    size_t currMaxX = std::numeric_limits<size_t>::max();
    size_t currMaxY = std::numeric_limits<size_t>::max();

    size_t xCoord = 0, yCoord = 0;
    for(size_t i = 1; i <= 8; ++i)
    {
        size_t val = i * cellSize - 40;
        size_t abValX = std::abs(static_cast<int>(x - val));
        size_t abValY = std::abs(static_cast<int>(y - val));

        if(abValX < currMaxX)
        {
            currMaxX = abValX;
            xCoord = i;
        }

        if(abValY < currMaxY)
        {
            currMaxY = abValY;
            yCoord = i;
        }
    }

    return std::make_tuple(xCoord - 1, yCoord - 1);
}
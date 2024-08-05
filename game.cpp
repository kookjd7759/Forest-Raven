#include <iostream>
#include <random>
#include <windows.h>

using namespace std;

/*
* (x, y)
* 
* y     (7, 7) 
* ^
* | WR WN ...
* (0,0) -> x 
*/

struct Position {
    int x, y;
};

const bool PosVecFind(vector<Position>* vec, Position pos) {
    for (const auto iter : *vec)
        if (iter.x == pos.x && iter.y == pos.y)
            return true;
    return false;
}

class Game {
private:
    enum Type {
        EMPTY,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    char typeToChar[7] = { '-', 'P', 'N', 'B', 'R', 'Q', 'K' };

    enum Team {
        NONE,
        WHITE,
        BLACK,
    };
    char teamToChar[3] = { '-', 'w', 'b' };

    const bool boundaryCheck(const Position pos) {
        return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true;
    }

    const bool isEnemy(const Position a, const Position b) {
        return ((board[a.y][a.x].team != NONE && board[b.y][b.x].team != NONE) &&
            (board[a.y][a.x].team != board[b.y][b.x].team));
    }

    const bool isAlly(const Position a, const Position b) {
        return ((board[a.y][a.x].team != NONE && board[b.y][b.x].team != NONE) &&
            board[a.y][a.x].team == board[b.y][b.x].team);
    }

    const bool isEmpty(const Position pos) {
        return (board[pos.y][pos.x].team == NONE ? true : false);
    }

    class Square {
    public:
        Type type = EMPTY;
        Team team = NONE;
    };

    Square** board = new Square *[8];
    Position prevMove[2];
    bool isWhiteTurn = true;


    int en_passant(const Position* pos, bool isWhite) const {
        auto comp = [&](const Position* a, const Position* b) -> bool {
            return (a->x == b->x) && (a->y == b->y);
            };

        if (prevMove[0].x == -1) // first move
            return false;

        Position kingSide[2], queenSide[2];
        if (isWhite) {
            kingSide[0] = { pos->x + 1, pos->y + 2 };
            kingSide[1] = { pos->x + 1, pos->y };

            queenSide[0] = { pos->x - 1, pos->y + 2 };
            queenSide[1] = { pos->x - 1, pos->y };
        }
        else {
            kingSide[0] = { pos->x + 1, pos->y - 2 };
            kingSide[1] = { pos->x + 1, pos->y };

            queenSide[0] = { pos->x - 1, pos->y - 2 };
            queenSide[1] = { pos->x - 1, pos->y };
        }


        if (comp(&kingSide[0], &prevMove[0]) && comp(&kingSide[1], &prevMove[1]))
            return 1;
        else if (comp(&queenSide[0], &prevMove[0]) && comp(&queenSide[1], &prevMove[1]))
            return -1;
        return 0;
    }
    

    vector<Position> move_pawn_b(const Position pos) {
        cout << "pawn_b !\n";
        vector<Position> vec;

        // Plain move
        if (isEmpty({ pos.x, pos.y - 1 })) {
            vec.push_back({ pos.x, pos.y - 1 });
            // First move
            if (pos.y == 6) vec.push_back({ pos.x, pos.y - 2 });
        }

        // Attack
        if (boundaryCheck({ pos.x - 1, pos.y - 1 }) && isEnemy(pos, { pos.x - 1, pos.y - 1 }))
            vec.push_back({ pos.x - 1, pos.y - 1 });

        if (boundaryCheck({ pos.x + 1, pos.y - 1 }) && isEnemy(pos, { pos.x + 1, pos.y - 1 }))
            vec.push_back({ pos.x + 1, pos.y - 1 });

        // en_passant Check
        int en_check = en_passant(&pos, false);
        if (en_check != 0)
            vec.push_back({ pos.x + en_check, pos.y - 1 });

        return vec;
    }

    vector<Position> move_pawn_w(const Position pos) {
        cout << "pawn_w !\n";
        vector<Position> vec;

        // Plain move
        if (isEmpty({ pos.x, pos.y + 1 })) {
            vec.push_back({ pos.x, pos.y + 1 });
            // First move
            if (pos.y == 1) vec.push_back({ pos.x, pos.y + 2 });
        }

        // Attack
        if (boundaryCheck({ pos.x + 1, pos.y + 1 }) && isEnemy(pos, { pos.x + 1, pos.y + 1 }))
            vec.push_back({ pos.x + 1, pos.y + 1 });

        if (boundaryCheck({ pos.x - 1, pos.y + 1 }) && isEnemy(pos, { pos.x - 1, pos.y + 1 }))
            vec.push_back({ pos.x - 1, pos.y + 1 });

        // en_passant Check
        int en_check = en_passant(&pos, true);
        if (en_check != 0)
            vec.push_back({ pos.x + en_check, pos.y + 1 });

        return vec;
    }

    vector<Position> move_knight(const Position pos) {
        cout << "knight !\n";
        vector<Position> vec;
        Position posList[8] = {
            {pos.x + 1, pos.y + 2},
            {pos.x - 1, pos.y + 2},
            {pos.x + 2, pos.y + 1},
            {pos.x - 2, pos.y + 1},
            {pos.x + 2, pos.y - 1},
            {pos.x - 2, pos.y - 1},
            {pos.x + 1, pos.y - 2},
            {pos.x - 1, pos.y - 2}
        };

        for (int i = 0; i < 8; i++) if (boundaryCheck(posList[i]) && !isAlly(pos, posList[i]))
                vec.push_back(posList[i]);

        return vec;
    }

    vector<Position> move_rook(const Position pos) {
        cout << "rook !\n";
        vector<Position> vec;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos) || isAlly(pos, nextPos)) return false;

            vec.push_back(nextPos);
            if (isEmpty(nextPos))
                return true;
            else return false;
            };

        Position nextPos = pos;
        cout << "R\n";
        while (true) {
            nextPos.x++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        return vec;
    }

    vector<Position> move_bishop(const Position pos) {
        cout << "bishop !\n";
        vector<Position> vec;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos) || isAlly(pos, nextPos)) return false;

            vec.push_back(nextPos);
            if (isEmpty(nextPos))
                return true;
            else return false;
            };

        Position nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        return vec;
    }

    vector<Position> move_queen(const Position pos) {
        cout << "queen !\n";
        vector<Position> vec;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos) || isAlly(pos, nextPos)) return false;

            vec.push_back(nextPos);
            if (isEmpty(nextPos))
                return true;
            else return false;
            };

        Position nextPos = pos;
        while (true) {
            nextPos.x++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        return vec;
    }

    vector<Position> move_king(const Position pos) {
        cout << "king !\n";
        vector<Position> vec;
        Position posList[8] = {
            {pos.x, pos.y + 1},
            {pos.x + 1, pos.y + 1},
            {pos.x + 1, pos.y},
            {pos.x + 1, pos.y - 1},
            {pos.x, pos.y - 1},
            {pos.x - 1, pos.y - 1},
            {pos.x - 1, pos.y},
            {pos.x - 1, pos.y + 1}
        };

        for (int i = 0; i < 8; i++) if (boundaryCheck(posList[i]) && !isAlly(pos, posList[i]))
                vec.push_back(posList[i]);

        return vec;
    }



    bool notationCheck(const string st) {
        if (st.size() == 2) {
            if ((st[0] >= 'a' && st[0] <= 'h') &&
                (st[1] >= '1' && st[1] <= '8')) {
                return true;
            }
        }
        return false;
    }

    Position convertPos(const string st) {
        Position pos{ st[0] - 'a', st[1] - '1' };
        return pos;
    }

    string convertPos(const Position pos) {
        string st = "";
        st += pos.x + 'a';
        st += pos.y + '1';
        return st;
    }



public:
    Game() : board(new Square* [8]) {
        for (int i = 0; i < 8; ++i) 
            board[i] = new Square[8];
        init();
    }

    ~Game() {
        for (int i = 0; i < 8; ++i) 
            delete[] board[i];
        delete[] board;
    }

    void reset() {
        // piece init
        Type list[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            board[0][i].type = list[i], board[0][i].team = WHITE;
            board[1][i].type = PAWN, board[1][i].team = WHITE;

            board[7][i].type = list[i], board[7][i].team = BLACK;
            board[6][i].type = PAWN, board[6][i].team = BLACK;
        }
        for (int i = 2; i < 6; i++) for (int j = 0; j < 8; j++) {
            board[i][j].type = EMPTY, board[i][j].team = NONE;
        }

        prevMove[0] = { -1, -1 }, prevMove[1] = { -1, -1 };
        isWhiteTurn = true;
    }

    void init() {
        reset();
    }

    void print_board(bool isW) {
        if (isW) {
            for (int i = 7; i >= 0; i--) {
                for (int j = 0; j < 8; j++) {
                    cout << teamToChar[board[i][j].team] << typeToChar[board[i][j].type] << " ";
                }
                cout << "\n";
            }
            for (int i = 0; i < 8; i++)
                cout << (char)('a' + i) << "  ";
            cout << "\n\n";
        }
        else {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    cout << teamToChar[board[i][j].team] << typeToChar[board[i][j].type] << " ";
                }
                cout << "\n";
            }
            for (int i = 7; i >= 0; i--)
                cout << (char)('a' + i) << "  ";
            cout << "\n\n";
        }
    }

    void move(const Position* now, const Position* dest) {
        cout << "Move : " << convertPos(*now) << " -> " << convertPos(*dest) << "\n";
        // cout << "MOVE : " << now->x << " " << now->y << " " << dest->x << " " << dest->y << "\n";
        board[dest->y][dest->x] = board[now->y][now->x];
        board[now->y][now->x].team = NONE;
        board[now->y][now->x].type = EMPTY;

        isWhiteTurn = isWhiteTurn ? false : true;
    }

    void AI_move(const Position* now, const Position* dest) {
        move(now, dest);
        cout << "MOVE : " << now->x << " " << now->y << " " << dest->x << " " << dest->y << "\n";
    }

    void capture(const Position* now, const Position* dest) {
        cout << teamToChar[board[now->y][now->x].team] << typeToChar[board[now->y][now->x].type] << " Take "
            << teamToChar[board[dest->y][dest->x].team] << typeToChar[board[dest->y][dest->x].type] << "\n";
        move(now, dest);
    }

    void command() {
        cout << (isWhiteTurn ? "[White move]" : "[Black move]") << "\n";

        string now, next; 
        while (true) { // For Legal move
            while (true) { // For Correct Input
                cout << "enter the move command >> "; cin >> now >> next;
                if (notationCheck(now) && notationCheck(next)) break;
                else cout << "move()::Notation Error\n";
            }

            Position selected = convertPos(now);
            if ((isWhiteTurn && board[selected.y][selected.x].team == BLACK) ||
                (!isWhiteTurn && board[selected.y][selected.x].team == WHITE)) {
                cout << "Is not your piece\n";
                continue;
            }

            vector<Position> vec_legalMove;
            switch (board[selected.y][selected.x].type) {
                case NONE: cout << "There is no piece, Enter position again\n"; continue;
                case PAWN: vec_legalMove = board[selected.y][selected.x].team == BLACK ?
                    move_pawn_b(selected) : move_pawn_w(selected); break;
                case KNIGHT: vec_legalMove = move_knight(selected); break;
                case BISHOP: vec_legalMove = move_bishop(selected); break;
                case ROOK: vec_legalMove = move_rook(selected); break;
                case QUEEN: vec_legalMove = move_queen(selected); break;
                case KING: vec_legalMove = move_king(selected); break;
                default: cout << "I don't know, Enter position again\n"; continue;
            }

            cout << "Legal move [ ";
            for (const auto iter : vec_legalMove)
                cout << convertPos(iter) << " ";
            cout << "]\n";

            Position dest = convertPos(next);
            if (PosVecFind(&vec_legalMove, dest)) {
                if (board[dest.y][dest.x].type != EMPTY) { // take the piece
                    capture(&selected, &dest);
                }
                else {
                    move(&selected, &dest);
                }
                break;
            }
            else cout << "It can't move there\n";
        }
    }

    void start() {
        while (true) {
            print_board(true);
            command();
        }
    }

    void startAI_test_scotchGame() {
        random_device rd; mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 3);
        int idx(0);
        Position Black_scotch[4][2] = { // scotch gambit Bc5 variation
            {4, 6, 4, 4},
            {1, 7, 2, 5},
            {4, 4, 3, 3},
            {5, 7, 2, 4}
        };
        while (true) {
            print_board(true);
            if (isWhiteTurn) {
                command();
            }
            else {
                Sleep(dis(gen) * 100 + 300);
                AI_move(&Black_scotch[idx][0], &Black_scotch[idx][1]);
                idx++;
            }
        }
    }
};

int main() {
    Game* chess = new Game();
    chess->startAI_test_scotchGame();
}
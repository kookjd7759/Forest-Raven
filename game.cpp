#include <iostream>
#include <windows.h>
#include <random>
#include <set>

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
    bool operator<(const Position& other) const {
        if (x < other.x) return true;
        if (x > other.x) return false;
        return y < other.y;
    }
};

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
    

    set<Position> move_pawn_b(const Position pos) {
        cout << "pawn_b !\n";
        set<Position> s;

        // Plain move
        if (isEmpty({ pos.x, pos.y - 1 })) {
            s.insert({ pos.x, pos.y - 1 });
            // First move
            if (pos.y == 6) s.insert({ pos.x, pos.y - 2 });
        }

        // Attack
        if (boundaryCheck({ pos.x - 1, pos.y - 1 }) && isEnemy(pos, { pos.x - 1, pos.y - 1 }))
            s.insert({ pos.x - 1, pos.y - 1 });

        if (boundaryCheck({ pos.x + 1, pos.y - 1 }) && isEnemy(pos, { pos.x + 1, pos.y - 1 }))
            s.insert({ pos.x + 1, pos.y - 1 });

        // en_passant Check
        int en_check = en_passant(&pos, false);
        if (en_check != 0)
            s.insert({ pos.x + en_check, pos.y - 1 });

        return s;
    }

    set<Position> move_pawn_w(const Position pos) {
        cout << "pawn_w !\n";
        set<Position> s;

        // Plain move
        if (isEmpty({ pos.x, pos.y + 1 })) {
            s.insert({ pos.x, pos.y + 1 });
            // First move
            if (pos.y == 1) s.insert({ pos.x, pos.y + 2 });
        }

        // Attack
        if (boundaryCheck({ pos.x + 1, pos.y + 1 }) && isEnemy(pos, { pos.x + 1, pos.y + 1 }))
            s.insert({ pos.x + 1, pos.y + 1 });

        if (boundaryCheck({ pos.x - 1, pos.y + 1 }) && isEnemy(pos, { pos.x - 1, pos.y + 1 }))
            s.insert({ pos.x - 1, pos.y + 1 });

        // en_passant Check
        int en_check = en_passant(&pos, true);
        if (en_check != 0)
            s.insert({ pos.x + en_check, pos.y + 1 });

        return s;
    }

    set<Position> move_knight(const Position pos) {
        cout << "knight !\n";
        set<Position> s;
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
                s.insert(posList[i]);

        return s;
    }

    set<Position> move_rook(const Position pos) {
        cout << "rook !\n";
        set<Position> s;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos) || isAlly(pos, nextPos)) return false;

            s.insert(nextPos);
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

        return s;
    }

    set<Position> move_bishop(const Position pos) {
        cout << "bishop !\n";
        set<Position> s;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos) || isAlly(pos, nextPos)) return false;

            s.insert(nextPos);
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

        return s;
    }

    set<Position> move_queen(const Position pos) {
        cout << "queen !\n";
        set<Position> s;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos) || isAlly(pos, nextPos)) return false;

            s.insert(nextPos);
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

        return s;
    }

    set<Position> move_king(const Position pos) {
        cout << "king !\n";
        set<Position> s;
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
                s.insert(posList[i]);

        return s;
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
        for (int i = 0; i < 8; i++) 
            board[i] = new Square[8];
        init();
    }

    ~Game() {
        for (int i = 0; i < 8; i++) 
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

    void move(const Position* now, const Position* dest) {
        // cout << "MOVE : " << now->x << " " << now->y << " " << dest->x << " " << dest->y << "\n";
        cout << "Move : " << convertPos(*now) << " -> " << convertPos(*dest) << "\n";
        board[dest->y][dest->x] = board[now->y][now->x];
        board[now->y][now->x].team = NONE;
        board[now->y][now->x].type = EMPTY;

        isWhiteTurn = isWhiteTurn ? false : true;
    }

    void returnLegalMove(set<Position>* s) {
        cout << "LEGALMOVE : ";
        for (const Position iter : *s)
            cout << iter.x << " " << iter.y << " ";
        cout << "\n";
    }

    void command() {
        cout << (isWhiteTurn ? "[White move]" : "[Black move]") << "\n";

        string now, next; 
        while (true) { // For Legal move
            int command(-1); // 0 : move, 1 : return legal move list
            while (true) { // For Correct Input
                cout << "enter the command >> "; cin >> now >> next;

                if (notationCheck(now) && next == "LEGALMOVE") {
                    command = 1;
                    break;
                }
                else if (notationCheck(now) && notationCheck(next)) {
                    command = 0;
                    break;
                }
                else cout << "move()::Command Error\n";
            }

            Position selected = convertPos(now);
            if ((isWhiteTurn && board[selected.y][selected.x].team == BLACK) ||
                (!isWhiteTurn && board[selected.y][selected.x].team == WHITE)) {
                cout << "Is not your piece\n";
                continue;
            }

            set<Position> s_legalMove;
            switch (board[selected.y][selected.x].type) {
                case NONE: cout << "There is no piece, Enter position again\n"; continue;
                case PAWN: s_legalMove = board[selected.y][selected.x].team == BLACK ?
                    move_pawn_b(selected) : move_pawn_w(selected); break;
                case KNIGHT: s_legalMove = move_knight(selected); break;
                case BISHOP: s_legalMove = move_bishop(selected); break;
                case ROOK: s_legalMove = move_rook(selected); break;
                case QUEEN: s_legalMove = move_queen(selected); break;
                case KING: s_legalMove = move_king(selected); break;
                default: cout << "I don't know, Enter position again\n"; continue;
            }

            if (command == 1) {
                returnLegalMove(&s_legalMove);
                return;
            }

            cout << "Legal move [ ";
            for (const auto iter : s_legalMove)
                cout << convertPos(iter) << " ";
            cout << "]\n";

            Position dest = convertPos(next);
            if (s_legalMove.find(dest) != s_legalMove.end()) {
                move(&selected, &dest);
                break;
            }
            else cout << "It can't move there\n";
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
                move(&Black_scotch[idx][0], &Black_scotch[idx][1]);
                cout << "MOVE : " << Black_scotch[idx][0].x << " " << Black_scotch[idx][0].y << " "
                    << Black_scotch[idx][1].x << " " << Black_scotch[idx][1].y << "\n";
                idx++;
            }
        }
    }


    // additional function
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

};

int main() {
    Game* chess = new Game();
    chess->startAI_test_scotchGame();
}
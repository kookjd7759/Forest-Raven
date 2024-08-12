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

    Position(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator<(const Position& other) const {
        if (x < other.x) return true;
        if (x > other.x) return false;
        return y < other.y;
    }

    Position operator+(const Position& other) const { return Position(x + other.x, y + other.y); }
};

class Game {
private:
    const Position dir_straight[4]{ {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
    const Position dir_diagonal[4]{ {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };

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

    bool notationCheck(const string& st) {
        if (st.size() == 2 && (st[0] >= 'a' && st[0] <= 'h') && (st[1] >= '1' && st[1] <= '8')) 
            return true;
        return false;
    }

    Position convertPos(const string& st) {
        return Position(st[0] - 'a', st[1] - '1' );
    }

    string convertPos(const Position& pos) {
        string st = "";
        st += pos.x + 'a';
        st += pos.y + '1';
        return st;
    }

    const bool boundaryCheck(const Position& pos) const {
        return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true;
    }

    const bool isEnemy(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].team != NONE && board[b.y][b.x].team != NONE) &&
            (board[a.y][a.x].team != board[b.y][b.x].team));
    }

    const bool isAlly(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].team != NONE && board[b.y][b.x].team != NONE) &&
            board[a.y][a.x].team == board[b.y][b.x].team);
    }

    const bool isEmpty(const Position& pos) const {
        return (board[pos.y][pos.x].team == NONE ? true : false);
    }

    class Square {
    public:
        Type type = EMPTY;
        Team team = NONE;
        int attack_w = 0, attack_b = 0;
    };

    Square** board = new Square *[8];
    Position prevMove[2];
    bool isWhiteTurn;

    set<Position> vec_piece_w, vec_piece_b; // Position list of pieces


    int en_passant(const Position& pos, const bool& isWhite) const {
        if (prevMove[0].x == -1) // first move
            return false;

        auto comp = [&](const Position* a, const Position* b) -> bool {
            return (a->x == b->x) && (a->y == b->y);
            };

        Position kingSide[2], queenSide[2];
        if (isWhite) {
            kingSide[0] = { pos.x + 1, pos.y + 2 };
            kingSide[1] = { pos.x + 1, pos.y };

            queenSide[0] = { pos.x - 1, pos.y + 2 };
            queenSide[1] = { pos.x - 1, pos.y };
        }
        else {
            kingSide[0] = { pos.x + 1, pos.y - 2 };
            kingSide[1] = { pos.x + 1, pos.y };

            queenSide[0] = { pos.x - 1, pos.y - 2 };
            queenSide[1] = { pos.x - 1, pos.y };
        }


        if (comp(&kingSide[0], &prevMove[0]) && comp(&kingSide[1], &prevMove[1]))
            return 1; // able to KingSide en passant
        else if (comp(&queenSide[0], &prevMove[0]) && comp(&queenSide[1], &prevMove[1]))
            return -1;
        return 0;
    }


    bool move_check(set<Position>& s, const Position& cur , const Position& next) {
        if (!boundaryCheck(next) || isAlly(cur, next)) return false;

        s.insert(next);
        if (isEmpty(next))
            return true;
        else return false;
    }

    set<Position> attack_pawn(const Position& pos, const bool& isWhite) {
        int dir = (isWhite ? +1 : -1);
        set<Position> s;
        if (boundaryCheck({ pos.x + dir, pos.y + dir }) && isEnemy(pos, { pos.x + dir, pos.y + dir }))
            s.insert(Position(pos.x + dir, pos.y + dir));

        if (boundaryCheck({ pos.x - dir, pos.y + dir }) && isEnemy(pos, { pos.x - dir, pos.y + dir }))
            s.insert(Position(pos.x - dir, pos.y + dir));

        return s;
    }

    set<Position> move_pawn(const Position& pos, const bool& isWhite) {
        int dir = (isWhite ? +1 : -1);

        cout << "pawn_" << (isWhite ? 'w' : 'b') << " !\n";

        // attack move
        set<Position> s = attack_pawn(pos, isWhite);

        // Plain move
        if (isEmpty({ pos.x, pos.y + dir })) {
            s.insert(Position(pos.x, pos.y + dir));
            // First move
            if (pos.y == (isWhite ? 1 : 6)) {
                s.insert(Position(pos.x, pos.y + dir * 2));
            }
        }

        // en_passant Check
        int en_check = en_passant(pos, false);
        if (en_check != 0) {
            s.insert(Position(pos.x + en_check, pos.y + dir));
        }

        return s;
    }

    set<Position> move_knight(const Position& pos) {
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

    set<Position> move_rook(const Position& pos) {
        cout << "rook !\n";
        set<Position> s;

        for (int i = 0; i < 4; i++) {
            while (true) {
                if (!move_check(s, pos, dir_straight[i] + pos)) break;
            }
        }

        return s;
    }

    set<Position> move_bishop(const Position& pos) {
        cout << "bishop !\n";
        set<Position> s;

        for (int i = 0; i < 4; i++) {
            while (true) {
                if (!move_check(s, pos, dir_diagonal[i] + pos)) break;
            }
        }

        return s;
    }

    set<Position> move_queen(const Position& pos) {
        cout << "queen !\n";
        set<Position> s;

        for (int i = 0; i < 4; i++) {
            while (true) {
                if (!move_check(s, pos, dir_straight[i] + pos)) break;
            }
            while (true) {
                if (!move_check(s, pos, dir_diagonal[i] + pos)) break;
            }
        }

        return s;
    }

    set<Position> move_king(const Position& pos) {
        cout << "king !\n";
        set<Position> s;

        auto insert = [&](const Position& next) -> void {
            if (!boundaryCheck(next))
                return;

            if (board[pos.y][pos.x].team == WHITE && 
                board[next.y][next.x].attack_b == 0) {
                s.insert(next);
            }
            else if (board[pos.y][pos.x].team == BLACK &&
                board[next.y][next.x].attack_w == 0) {
                s.insert(next);
            }
            };

        for (int i = 0; i < 4; i++) {
            insert(dir_straight[i] + pos);
            insert(dir_diagonal[i] + pos);
        }

        return s;
    }

    void cal_attackSquare() {
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            board[y][x].attack_w = 0, board[y][x].attack_b = 0;

        auto get = [&](const Position& pos) -> set<Position> {
            set<Position> s;
            switch (board[pos.y][pos.x].type) {
            case NONE: break;
            case PAWN: s = attack_pawn(pos, board[pos.y][pos.x].team == WHITE);
            case KNIGHT: s = move_knight(pos); break;
            case BISHOP: s = move_bishop(pos); break;
            case ROOK: s = move_rook(pos); break;
            case QUEEN: s = move_queen(pos); break;
            case KING: s = move_king(pos); break;
            }

            return s;
            };

        for (const Position pos : vec_piece_w) {
            set<Position> s = get(pos);
            for (const Position p : s) board[p.y][p.x].attack_w++;
        }

        for (const Position pos : vec_piece_b) {
            set<Position> s = get(pos);
            for (const Position p : s) board[p.y][p.x].attack_b++;
        }
    }

    bool getLegalMove(set<Position>& s, const Position& pos) {
        switch (board[pos.y][pos.x].type) {
        case NONE: cout << "There is no piece, Enter position again\n"; return false;
        case PAWN: s = board[pos.y][pos.x].team == WHITE ?
            move_pawn(pos, true) : move_pawn(pos, false); break;
        case KNIGHT: s = move_knight(pos); break;
        case BISHOP: s = move_bishop(pos); break;
        case ROOK: s = move_rook(pos); break;
        case QUEEN: s = move_queen(pos); break;
        case KING: s = move_king(pos); break;
        default: cout << "I don't know, Enter position again\n"; return false;
        }

        return true;
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
        vec_piece_w.clear(), vec_piece_b.clear();

        // piece init
        Type list[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            board[0][i].type = list[i], board[0][i].team = WHITE; vec_piece_w.insert(Position(i, 0));
            board[1][i].type = PAWN, board[1][i].team = WHITE; vec_piece_w.insert(Position(i, 1));

            board[7][i].type = list[i], board[7][i].team = BLACK; vec_piece_b.insert(Position(i, 7));
            board[6][i].type = PAWN, board[6][i].team = BLACK; vec_piece_b.insert(Position(i, 6));
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

    void move(const Position& now, const Position& dest) {
        // cout << "MOVE : " << now->x << " " << now->y << " " << dest->x << " " << dest->y << "\n";
        cout << "Move : " << convertPos(now) << " -> " << convertPos(dest) << "\n";
        board[dest.y][dest.x] = board[now.y][now.x];
        board[now.y][now.x].team = NONE;
        board[now.y][now.x].type = EMPTY;

        isWhiteTurn = isWhiteTurn ? false : true;
        prevMove[0] = now;
        prevMove[1] = dest;
    }

    void command() {
        cout << (isWhiteTurn ? "[White move]" : "[Black move]") << "\n";

        string now, next; 
        while (true) { // For Legal move
            while (true) { // For Correct Input
                cout << "enter the command >> "; cin >> now >> next;

                if (notationCheck(now) && notationCheck(next)) break;
                else cout << "move()::Command Error\n";
            }

            Position selected = convertPos(now);
            cout << "selected : " << selected.x << " " << selected.y << "\n";
            if ((isWhiteTurn && board[selected.y][selected.x].team == BLACK) ||
                (!isWhiteTurn && board[selected.y][selected.x].team == WHITE)) {
                cout << "Is not your piece\n";
                continue;
            }

            set<Position> s_legalMove;
            if (!getLegalMove(s_legalMove, selected)) continue;

            cout << "Legal move [ ";
            for (const auto iter : s_legalMove) cout << convertPos(iter) << " ";
            cout << "]\n";

            Position dest = convertPos(next);
            if (s_legalMove.find(dest) != s_legalMove.end()) {
                move(selected, dest);
                break;
            }
            else cout << "It can't move there\n";
        }
    }

    void start() {
        while (true) {
            print_board(true);
            cal_attackSquare();
            print_attackData(true);
            command();
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

        cout << "[previous Move] : " << convertPos(prevMove[0]) << " -> " << convertPos(prevMove[1]) << "\n";

    }

    void print_attackData(bool isW) {
        if (isW) {
            for (int i = 7; i >= 0; i--) {
                for (int j = 0; j < 8; j++) {
                    cout << board[i][j].attack_w << "," << board[i][j].attack_b << "   ";
                }
                cout << "\n\n";
            }
        }
        else {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    cout << board[i][j].attack_w << "," << board[i][j].attack_b << "   ";
                }
                cout << "\n\n";
            }
        }
    }

};

int main() {
    Game* chess = new Game();
    chess->start();
}
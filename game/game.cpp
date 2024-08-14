#include <iostream>
#include <windows.h>
#include <random>
#include <set>
#include "utility.h"

using namespace std;

/*
* (x, y)
* 
* y     (7, 7) 
* ^
* | WR WN ...
* (0,0) -> x 
*/

class Game {
private:
    const Position dir_straight[4]{ {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
    const Position dir_diagonal[4]{ {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };
    const Position dir_knight[8]{ {1, 2}, {-1, 2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}, {1, -2}, {-1, -2} };
    const Position dir_king[8]{ {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} };

    enum Type {
        EMPTY,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    const char typeToChar[7] = { '-', 'P', 'N', 'B', 'R', 'Q', 'K' };

    enum Team {
        NONE,
        WHITE,
        BLACK,
    };
    const char teamToChar[3] = { '-', 'w', 'b' };

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
        int attack_wb[2]{ 0, 0 };
    };

    Square** board = new Square *[8];
    Position prevMove[2];
    bool isWhiteTurn, castlingMoveCheck_wb_KQ[2][2];



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

    bool castling(const bool& isWhite, const bool& isKingSide) {
        int wb = isWhite ? 0 : 1, kq = isKingSide ? 0 : 1, rank = isWhite ? 0 : 7;
        bool ch1, ch2, ch3;
        ch1 = castlingMoveCheck_wb_KQ[wb][kq];
        ch2 = isKingSide ? board[rank][5].type == EMPTY && board[rank][6].type == EMPTY :
            board[rank][3].type == EMPTY && board[rank][2].type == EMPTY && board[rank][1].type == EMPTY;
        ch3 = isKingSide ? board[rank][5].attack_wb[wb] == 0 && board[rank][6].attack_wb[wb] == 0 :
            board[rank][3].attack_wb[wb] == 0 && board[rank][2].attack_wb[wb] == 0 && board[rank][1].attack_wb[wb] == 0;

        return ch1 && ch2 && ch3;
    }

    void promotion(const Position& pos, const bool& isWhite) {
        //cout << "Choose promotion piece\n[1] Queen\n[2] Rook\n[3] Knight\n[4] Bishop\n >> ";
        int input; cin >> input;
        switch (input) {
        case 1: board[pos.y][pos.x].type = QUEEN; break;
        case 2: board[pos.y][pos.x].type = ROOK; break;
        case 3: board[pos.y][pos.x].type = KNIGHT; break;
        case 4: board[pos.y][pos.x].type = BISHOP; break;
        default: board[pos.y][pos.x].type = QUEEN; break; // error ? auto Queen promotion
        }

        //cout << "Promotion -> " << typeToChar[board[pos.y][pos.x].type] << "\n";
    }



    bool move_check(set<Position>& s, const Position& cur , const Position& next, const bool& legalMove) {
        if (!boundaryCheck(next) || (legalMove && isAlly(cur, next))) return false;

        s.insert(next);
        if (isEmpty(next))
            return true;
        else return false;
    }

    void pawnAttack_move(set<Position>& s, const Position& now, const bool& isWhite, const bool& legalMove) {
        int dir = (isWhite ? +1 : -1);
        auto func = [&](Position next) -> void {
            if (boundaryCheck({ next.x, next.y })) {
                if (legalMove) {
                    if (isEnemy(now, next))
                        s.insert(next);
                }
                else s.insert(next);
            }
            };
        func(Position(now.x + 1, now.y + dir));
        func(Position(now.x - 1, now.y + dir));
    }

    void straight_move(set<Position>& s, const Position& pos, const bool& legalMove) {
        for (int i = 0; i < 4; i++) {
            Position next = pos;
            while (true) {
                next = next + dir_straight[i];
                if (!move_check(s, pos, next, legalMove)) break;
            }
        }
    }

    void diagonal_move(set<Position>& s, const Position& pos, const bool& legalMove) {
        for (int i = 0; i < 4; i++) {
            Position next = pos;
            while (true) {
                next = next + dir_diagonal[i];
                if (!move_check(s, pos, next, legalMove)) break;
            }
        }
    }

    void knight_move(set<Position>& s, const Position& pos, const bool& legalMove) {
        for (int i = 0; i < 8; i++) {
            Position next = pos + dir_knight[i];
            if (boundaryCheck(next)) {
                if (legalMove) {
                    if (!isAlly(pos, next))
                        s.insert(next);
                }
                else s.insert(next);
            }
        }
    }

    void king_move(set<Position>& s, const Position& pos, const bool& isWhite, const bool& legalMove) {
        for (int i = 0; i < 8; i++) {
            Position next = pos + dir_king[i];
            if (boundaryCheck(next)) {
                if (legalMove) {
                    if (board[next.y][next.x].attack_wb[isWhite ? 0 : 1 == 0] && !isAlly(pos, next))
                        s.insert(next);
                }
                else s.insert(next);
            }
        }
    }



    set<Position> get_pawn_move(const Position& pos, const bool& isWhite) {
        int dir = (isWhite ? +1 : -1);
        set<Position> s; 

        // Plain move
        if (isEmpty({ pos.x, pos.y + dir })) {
            s.insert(Position(pos.x, pos.y + dir));
            // First move
            if (isEmpty({ pos.x, pos.y + dir * 2}) && pos.y == (isWhite ? 1 : 6)) {
                s.insert(Position(pos.x, pos.y + dir * 2));
            }
        }

        // attack move
        pawnAttack_move(s, pos, isWhite, true);

        // en_passant Check
        int en_check = en_passant(pos, false);
        if (en_check != 0) {
            s.insert(Position(pos.x + en_check, pos.y + dir));
        }

        return s;
    }

    set<Position> get_knight_move(const Position& pos) {
        set<Position> s; knight_move(s, pos, true);

        return s;
    }

    set<Position> get_bishop_move(const Position& pos) {
        set<Position> s; diagonal_move(s, pos, true);

        return s;
    }

    set<Position> get_rook_move(const Position& pos) {
        set<Position> s; straight_move(s, pos, true);

        return s;
    }

    set<Position> get_queen_move(const Position& pos) {
        set<Position> s; straight_move(s, pos, true); diagonal_move(s, pos, true);

        return s;
    }

    set<Position> get_king_move(const Position& pos, const bool& isWhite) {
        set<Position> s; king_move(s, pos, isWhite, true);

        // Castling
        if (castling(isWhite, true)) // King side
            s.insert(Position(pos.x + 2, pos.y));
        if (castling(isWhite, false)) // Queen side
            s.insert(Position(pos.x - 2, pos.y));

        return s;
    }



    void cal_attackSquare() {
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            board[y][x].attack_wb[0] = 0, board[y][x].attack_wb[1] = 0;

        auto get = [&](set<Position>& s, const Position& pos, const bool& isWhite) -> void {
            switch (board[pos.y][pos.x].type) {
            case EMPTY: break;
            case PAWN: pawnAttack_move(s, pos, isWhite, false); break;
            case KNIGHT: knight_move(s, pos, false); break;
            case BISHOP: diagonal_move(s, pos, false); break;
            case ROOK: straight_move(s, pos, false); break;
            case QUEEN: diagonal_move(s, pos, false); straight_move(s, pos, false); break;
            case KING: king_move(s, pos, isWhite, false); break;
            }
            };

        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            if (board[y][x].type != EMPTY) {
                set<Position> s; get(s, Position(x, y), board[y][x].team == WHITE);
                
                for (const Position p : s)
                    board[y][x].team == WHITE ? board[p.y][p.x].attack_wb[0]++ : board[p.y][p.x].attack_wb[1]++;
            }
        }
    }

    void move(const Position& now, const Position& dest) {
        //cout << "Move : " << convertPos(now) << " -> " << convertPos(dest) << "\n";

        // update board
        board[dest.y][dest.x] = board[now.y][now.x];
        board[now.y][now.x].team = NONE;
        board[now.y][now.x].type = EMPTY;

        // change turn
        isWhiteTurn = !isWhiteTurn;

        // update previous move 
        prevMove[0] = now;
        prevMove[1] = dest;

        // Promotion check
        if (board[dest.y][dest.x].type == PAWN) {
            if (board[dest.y][dest.x].team == WHITE && dest.y == 7) // white pawn promotion
                promotion(dest, true);
            else if (board[now.y][now.x].team == BLACK && dest.y == 0) // black pawn promotion
                promotion(dest, false);
        }

        cal_attackSquare();
    }

    int get_legalMove(set<Position>& s, const Position& pos) {
        bool isWhite = board[pos.y][pos.x].team == WHITE;
        switch (board[pos.y][pos.x].type) {
        case NONE: return 1;
        case PAWN: s = get_pawn_move(pos, isWhite); break;
        case KNIGHT: s = get_knight_move(pos); break;
        case BISHOP: s = get_bishop_move(pos); break;
        case ROOK: s = get_rook_move(pos); break;
        case QUEEN: s = get_queen_move(pos); break;
        case KING: s = get_king_move(pos, isWhite); break;
        default: return -1;
        }
        return 0;
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

        cal_attackSquare();
        prevMove[0] = { -1, -1 }, prevMove[1] = { -1, -1 };
        isWhiteTurn = true;
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) 
            castlingMoveCheck_wb_KQ[i][j] = true;
    }

    void init() {
        reset();
    }

    void command() {
        //cout << "(<Input command>)\n[1] move \n[2] get legal move \n[3] print board \n[4] print attack data \n";
        int n; cin >> n;

        switch (n) {
        case 1: move_ME(); break;
        case 2: command_getLegalMove(); break;
        case 3: print_board(); break;
        case 4: print_attackData(); break;
        case 5: move_AI(); break;
        default: //cout << "wrong number !\n"; 
            break;
        }
    }

    bool move_ME() {
        string now, next; cin >> now >> next;
        if (!notationCheck(now) || !notationCheck(next)) { // notation check
            //cout << "Notation Error\n";
            return false;
        }

        Position pos = convertPos(now);
        if ((isWhiteTurn && board[pos.y][pos.x].team == BLACK) ||
            (!isWhiteTurn && board[pos.y][pos.x].team == WHITE)) { // team check
            //cout << "Is not your piece\n";
            return false;
        }

        set<Position> s_legalMove;
        int ch = get_legalMove(s_legalMove, pos);
        if (ch != 0) {
            //cout << (ch == 1 ? "There is no piece" : "I don't know") << ", Enter position again\n"; // piece check
            return false;
        }

        Position dest = convertPos(next);
        if (s_legalMove.find(dest) != s_legalMove.end()) { // move check
            move(pos, dest);
            return true;
        }
        else {
            //cout << "It can't move there\n";
            return false;
        }
    }

    void command_getLegalMove() {
        string pos; cin >> pos;
        set<Position> s; get_legalMove(s, convertPos(pos));
        for (const Position p : s)
            cout << p.x << " " << p.y << " ";
        cout << "\n";
    }

    Position AI_scotch[8][2]{
        {{4, 6}, {4, 4}},
        {{1, 7}, {2, 5}},
        {{4, 4}, {3, 3}},
        {{5, 7}, {2, 4}},
        {{3, 3}, {2, 2}},
        {{4, 7}, {5, 6}},
        {{5, 6}, {4, 7}},
        {{2, 2}, {1, 1}}
    };
    int idx = 0;
    void move_AI() {
        move(AI_scotch[idx][0], AI_scotch[idx][1]);
        for (int i = 0; i < 2; i++)
            cout << AI_scotch[idx][i].x << " " << AI_scotch[idx][i].y << " ";
        cout << "\n";
        idx++;
    }

    void start() {
        while (true) {
            command();
        }
    }

    // additional function
    void print_board() {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) cout << teamToChar[board[i][j].team] << typeToChar[board[i][j].type] << " ";
            cout << "\n";
        }
        for (int i = 0; i < 8; i++) cout << (char)('a' + i) << "  ";
        cout << "\n\n";
        cout << "[previous Move] : " << convertPos(prevMove[0]) << " -> " << convertPos(prevMove[1]) << "\n";
    }

    void print_attackData() {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) cout << board[i][j].attack_wb[0] << "," << board[i][j].attack_wb[1] << "   ";
            cout << "\n\n";
        }
    }

};

int main() {
    Game* chess = new Game();
    chess->start();
}

/*
1 e2 e4
5
1 g1 f3
5
1 d2 d4
5
1 f1 c4
5
1 c2 c3
5
1 d1 d5
5
*/
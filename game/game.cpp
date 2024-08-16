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
    enum Type {
        NOTYPE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    const char typeToChar[7] = { '-', 'P', 'N', 'B', 'R', 'Q', 'K' };

    enum Team {
        NOTEAM,
        WHITE,
        BLACK,
    };
    const char teamToChar[3] = { '-', 'w', 'b' };

    const bool boundaryCheck(const Position& pos) const {
        return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true;
    }

    const bool isEnemy(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].team != NOTEAM && board[b.y][b.x].team != NOTEAM) &&
            (board[a.y][a.x].team != board[b.y][b.x].team));
    }

    const bool isAlly(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].team != NOTEAM && board[b.y][b.x].team != NOTEAM) &&
            board[a.y][a.x].team == board[b.y][b.x].team);
    }

    const bool isEmpty(const Position& pos) const {
        return (board[pos.y][pos.x].team == NOTEAM ? true : false);
    }

    class Square {
    public:
        Type type = NOTYPE;
        Team team = NOTEAM;
        int attack_wb[2]{ 0, 0 };
    };

    Square** board = new Square *[8];
    Position prevMove[2];
    bool isWhiteTurn, castlingMoveCheck_wb_KQ[2][2];
    Position kingPos_wb[2];



    int en_passant_check(const Position& pos, const bool& isWhite) const {
        if ((prevMove[0].x == -1) || (pos.y != (isWhite ? 4 : 3))) // first move or is not rank 5
            return false;

        Position side_KQ[2][2];
        int dir = isWhite ? +2 : -2;
        for (int i = 0, add = 1; i < 2; i++, add -= 2) {
            side_KQ[i][0] = { pos.x + add, pos.y + dir };
            side_KQ[i][1] = { pos.x + add, pos.y };
        }

        if (side_KQ[0][0] == prevMove[0] && side_KQ[0][1] == prevMove[1]) return 1;
        else if (side_KQ[1][0] == prevMove[0] && side_KQ[1][1] == prevMove[1]) return -1;
        return 0;
    }

    bool castling_check(const bool& isWhite, const bool& isKingSide) {
        int wb = isWhite ? 0 : 1, kq = isKingSide ? 0 : 1, rank = isWhite ? 0 : 7;
        bool ch1, ch2, ch3;
        ch1 = castlingMoveCheck_wb_KQ[wb][kq];
        ch2 = isKingSide ? board[rank][5].type == NOTYPE && board[rank][6].type == NOTYPE :
            board[rank][3].type == NOTYPE && board[rank][2].type == NOTYPE && board[rank][1].type == NOTYPE;
        
        int op = isWhite ? 1 : 0;
        ch3 = isKingSide ? board[rank][5].attack_wb[op] == 0 && board[rank][6].attack_wb[op] == 0 :
            board[rank][3].attack_wb[op] == 0 && board[rank][2].attack_wb[op] == 0 && board[rank][1].attack_wb[op] == 0;
        
        return ch1 && ch2 && ch3;
    }

    void promotion(const Position& pos, const bool& isWhite) {
        int input; cin >> input;
        switch (input) {
        case 1: board[pos.y][pos.x].type = QUEEN; break;
        case 2: board[pos.y][pos.x].type = ROOK; break;
        case 3: board[pos.y][pos.x].type = KNIGHT; break;
        case 4: board[pos.y][pos.x].type = BISHOP; break;
        default: board[pos.y][pos.x].type = QUEEN; break;
        }
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
                    if (board[next.y][next.x].attack_wb[isWhite ? 1 : 0] == 0 && !isAlly(pos, next))
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
        int en = en_passant_check(pos, isWhite);
        if (en != 0) 
            s.insert(Position(pos.x + en, pos.y + dir));

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
        if (castling_check(isWhite, true)) // King side
            s.insert(Position(pos.x + 2, pos.y));
        if (castling_check(isWhite, false)) // Queen side
            s.insert(Position(pos.x - 2, pos.y));

        return s;
    }

    set<pair<Position, Position>> get_candidate_move(const bool& isWhite) {
        set<pair<Position, Position>> s;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++) {
                if (board[y][x].team != NOTEAM && board[y][x].team == (isWhite ? WHITE : BLACK)) {
                    set<Position> move;
                    Position pos(x, y); 
                    switch (board[y][x].type) {
                    case NOTYPE: break;
                    case PAWN: pawnAttack_move(move, pos, isWhite, true); break;
                    case KNIGHT: knight_move(move, pos, true); break;
                    case BISHOP: diagonal_move(move, pos, true); break;
                    case ROOK: straight_move(move, pos, true); break;
                    case QUEEN: diagonal_move(move, pos, true); straight_move(move, pos, true); break;
                    case KING: king_move(move, pos, isWhite, true); break;
                    }

                    for (const Position p : move)
                        s.insert(make_pair(Position(x, y), p));
                }
            }
    }



    int isCheck(const bool& isWhite) { // 0.no 1.check 2.mate
        auto isKingAttacked = [&](Position king) -> bool {
            int op = (isWhite ? 1 : 0);
            return board[king.y][king.x].attack_wb[op] != 0;
            };

        int wb = (isWhite ? 0 : 1);
        Position king = kingPos_wb[wb];
        if (!isKingAttacked(king))
            return 0;
        else {
            set<pair<Position, Position>> move_1 = get_candidate_move(isWhite);
            bool isCheck = false;
            for (const pair<Position, Position> move : move_1) {
                Position now = move.first, dest = move.second;

                Square temp = board[dest.y][dest.x];
                move_piece_boardUpdate(now, dest);
                cal_attackSquare();
                if (!isKingAttacked(king)) isCheck = true;
                move_piece_boardUpdate(dest, now);
                board[dest.y][dest.x] = temp;

                if (isCheck)
                    break;
            }
            cal_attackSquare();
            return (isCheck ? 1 : 2);
        }
    }

    void cal_attackSquare() {
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            board[y][x].attack_wb[0] = 0, board[y][x].attack_wb[1] = 0;

        auto get = [&](set<Position>& s, const Position& pos, const bool& isWhite) -> void {
            switch (board[pos.y][pos.x].type) {
            case NOTYPE: break;
            case PAWN: pawnAttack_move(s, pos, isWhite, false); break;
            case KNIGHT: knight_move(s, pos, false); break;
            case BISHOP: diagonal_move(s, pos, false); break;
            case ROOK: straight_move(s, pos, false); break;
            case QUEEN: diagonal_move(s, pos, false); straight_move(s, pos, false); break;
            case KING: king_move(s, pos, isWhite, false); break;
            }
            };

        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            if (board[y][x].type != NOTYPE) {
                set<Position> s; get(s, Position(x, y), board[y][x].team == WHITE);
                
                for (const Position p : s)
                    board[y][x].team == WHITE ? board[p.y][p.x].attack_wb[0]++ : board[p.y][p.x].attack_wb[1]++;
            }
        }
    }

    void move_piece_boardUpdate(const Position& now, const Position& dest) {
        board[dest.y][dest.x] = board[now.y][now.x];
        board[now.y][now.x].team = NOTEAM;
        board[now.y][now.x].type = NOTYPE;
    }

    void move_piece(const Position& now, const Position& dest) {
        if (board[now.y][now.x].type == KING) { // king position recording
            int wb = (board[now.y][now.x].team == WHITE ? 0 : 1);
            kingPos_wb[wb] = dest;
        }
        if (board[now.y][now.x].type == NOTYPE) {
            cout << "move_piece()::MOVE ERROR, there is no piece\n";
            return;
        }
        move_piece_boardUpdate(now, dest);
    }

    void move(const Position& now, const Position& dest) {
        bool isWhite = board[now.y][now.x].team == WHITE;
        // Castling check
        if (board[now.y][now.x].type == KING) { // If move king, never do castes
            int wb = (board[now.y][now.x].team == WHITE ? 0 : 1);
            castlingMoveCheck_wb_KQ[wb][0] = castlingMoveCheck_wb_KQ[wb][1] = false;
            if (abs(now.x - dest.x) == 2) { // Castling move
                bool kingSide = dest.x > now.x;
                int index = wb * 2 + (kingSide ? 0 : 1);
                move_piece(rookPos[index], dest + Position(dest.x + (kingSide ? -1 : +1), dest.y)); // move rook
            }
        }
        else if (board[now.y][now.x].type == ROOK) {
            for (int i = 0; i < 4; i++) {
                if (rookPos[i] == now) 
                    castlingMoveCheck_wb_KQ[i / 2][i % 2] = false;
            }
        }

        // En passant check
        if (board[now.y][now.x].type == PAWN && now.x != dest.x) { // PAWN takes something
            if (board[dest.y][dest.x].type == NOTYPE) { // En passant move
                int reverse_dir = (board[now.y][now.x].team == WHITE ? -1 : +1);
                board[dest.y + reverse_dir][dest.x].team = NOTEAM;
                board[dest.y + reverse_dir][dest.x].type = NOTYPE;
            }
        }

        // Promotion check
        if (board[now.y][now.x].type == PAWN && dest.y == (isWhite ? 7 : 0)) {
            bool isWhite = board[dest.y][dest.x].team == WHITE;
            if (isWhite && dest.y == 7) // white pawn promotion
                promotion(dest, isWhite);
            else if (!isWhite && dest.y == 0) // black pawn promotion
                promotion(dest, isWhite);
        }

        //cout << "Move : " << convertPos(now) << " -> " << convertPos(dest) << "\n";

        // update board
        move_piece(now, dest);

        // change turn
        isWhiteTurn = !isWhiteTurn;

        // update previous move 
        prevMove[0] = now; prevMove[1] = dest;

        cal_attackSquare();
    }

    int get_legalMove(set<Position>& s, const Position& pos) {
        bool isWhite = board[pos.y][pos.x].team == WHITE;
        switch (board[pos.y][pos.x].type) {
        case NOTEAM: return 1;
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
            board[i][j].type = NOTYPE, board[i][j].team = NOTEAM;
        }

        kingPos_wb[0] = { 4, 0 }, kingPos_wb[1] = { 4, 7 };
        prevMove[0] = { -1, -1 }, prevMove[1] = { -1, -1 };
        isWhiteTurn = true;
        for (int i = 0; i < 4; i++) castlingMoveCheck_wb_KQ[i / 2][i % 2] = true;

        cal_attackSquare();
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
    Position en_passant_test[3][2]{
        {{0, 6}, {0, 5}},
        {{3, 6}, {3, 4}},
        {{0, 5}, {0, 4}}
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

/*
1 e2 e4
5
1 e4 e5
5
3
2 e5
*/
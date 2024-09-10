#ifndef CHESS_H
#define CHESS_H

#include <set>
#include "utility.h"

using namespace std;

enum Type {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NOTYPE
};

enum Color {
    WHITE,
    BLACK,
    NOCOLOR
};

struct PreviousMove {
    Type type = NOTYPE;
    Position prev = Position(-1, -1), now = Position(-1, -1);

    void set(Type t, Position p, Position n) { type = t, prev = p, now = n; }
    void clear() { type = NOTYPE, prev = Position(-1, -1), now = Position(-1, -1); }
};

struct Piece {
    Type type;
    Color color;

    Piece(Type type, Color color) : type(type), color(color) {}
};

struct Square {
    Piece piece = Piece(NOTYPE, NOCOLOR);
    int attack_wb[2]{ 0, 0 };

    void set(Piece p) { piece = p; }
    bool isAttacked(Color color) const { return attack_wb[(color == WHITE ? BLACK : WHITE)] != 0 ? true : false; }
    bool empty() const { return piece.type == NOTYPE && piece.color == NOCOLOR; }
    void clear() { piece = Piece(NOTYPE, NOCOLOR); }
};

class Chess {
private:
    Position king_position_wb[2]{ Position(4, 0), Position(4, 7) };
    bool kr_moveCheck_wb_qk[2][2];
    PreviousMove prevMove;

    const bool boundaryCheck(const Position& pos);
    const bool isAlly(const Position& a, const Position& b);
    const bool isEnemy(const Position& a, const Position& b);
    const bool isCheck(const Color& color);
    const bool isThisMoveLegal(const Position& cur, const Position& dest, const Position& take = Position(-1, -1));
    void calAttackSquare();

    void append(set<Position>* set, const Position& cur, const Position& dest, const bool& legalMove, const Position& take = Position(-1, -1));
    void repeatMove(set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove);
    void oneMove(set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove);
    set<Position>* rook(const Position& pos, const bool& legalMove);
    set<Position>* bishop(const Position& pos, const bool& legalMove);
    set<Position>* knight(const Position& pos, const bool& legalMove);
    set<Position>* king(const Position& pos, const bool& legalMove);
    set<Position>* queen(const Position& pos, const bool& legalMove);
    set<Position>* pawn_move(const Position& pos);
    set<Position>* pawn_attack(const Position& pos, const bool& legalMove);
    set<Position>* get_attackList(const Position& pos);
    set<Position>* get_legalMoveList(const Position& pos);

    void move_piece(const Position& cur, const Position& dest);
    void castling_move(const Position& cur, const Position& dest);
    void en_passent_move(const Position& cur, const Position& dest);

    const bool castling_check(const Color& color, const bool& isKingside);
    const int en_passent_check(const Position& pos);

public:
    Square board[8][8];
    Color turn, myColor;

    Chess() { reset(); }
    void reset();

    set<pair<Position, Position>>* get_candidateMove(const Color& color);
    void move(const Position& cur, const Position& dest, const int& promotion = -1);

    void print_board();
};

#endif // CHESS_H
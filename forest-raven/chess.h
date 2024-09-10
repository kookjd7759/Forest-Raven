#ifndef CHESS_H
#define CHESS_H

#include <set>
#include "utility.h"

using namespace std;

struct Move {
    Position ori, dest, take;
    int promotion = -1;

    bool operator<(const Move& other) const {
        if (ori > other.ori) return true;
        else if (dest > other.dest) return false;
        return take < other.take;
    }

    Move() {}
    Move(Position ori, Position dest) : ori(ori), dest(dest) {};
    Move(Position ori, Position dest, Position take) : ori(ori), dest(dest), take(take) {};
    Move(Position ori, Position dest, int promotion) : ori(ori), dest(dest), promotion(promotion) {};
    Move(Position ori, Position dest, Position take, int promotion) :ori(ori), dest(dest), take(take), promotion(promotion) {};

    bool isTake() const { return (take.x != -1); }
};

struct PreviousMove {
    Type type;
    Move move;

    PreviousMove() { clear(); }
    
    void set(Type t, Move m) { type = t, move = m; }
    void clear() { type = NOTYPE, move = Move(NULL_POS, NULL_POS, NULL_POS, -1); }
};

struct Piece {
    Type type = NOTYPE;
    Color color = NOCOLOR;

    Piece(){}
    Piece(Type type, Color color) : type(type), color(color) {}
};

struct Square {
    Piece piece = Piece(NOTYPE, NOCOLOR);
    int attack_wb[2]{ 0, 0 };

    Square() { clear(); }

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

    const bool boundaryCheck(const Position& pos) const;
    const bool isAlly(const Position& a, const Position& b) const;
    const bool isEnemy(const Position& a, const Position& b) const;
    const bool isCheck(const Color& color) const;
    const bool isThisMoveLegal(const Move& move);
    void calAttackSquare();

    void append(set<Position>* set, const Move& move, const bool& legalMove);
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

    void move_piece(const Move& move);
    void castling_move(const Move& move);
    void en_passent_move(const Move& move);

    const bool castling_check(const Color& color, const bool& isKingside) const;
    const int en_passent_check(const Position& pos);

public:
    Square board[8][8];
    Color turn, myColor;

    Chess clone() const { return *this; }

    Chess() { reset(); }
    void reset();

    set<Move>* get_candidateMove(const Color& color);
    void move(const Move& move);
    void print_board();
};

#endif // CHESS_H
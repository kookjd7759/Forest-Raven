#include <set>
#include <string>
#include <sstream>
#include "utility.h"
#include "board.h"

namespace ForestRaven{
    class Chess {
    private:
        Board board;

        const bool castling_check() {
            if ()
        }
        const int en_passent_check(const Position& pos);

    public:
    };
}

class Chess {
private:
    Board board;

    const bool isAlly(const Position& a, const Position& b) const;
    const bool isEnemy(const Position& a, const Position& b) const;
    const bool isCheck(const Color& color) const;
    const bool isThisMoveLegal(const Move& move) const;

    void append(set<Move>* s, const Move& move);
    void repeatCheck(set<Move>* s, const Piece& piece, const Position& ori, const Position& dir);
    void oneCheck(set<Move>* s, const Piece& piece, const Position& ori, const Position& dir);
    set<Move>* queen(const Position& pos, const Piece& piece);
    set<Move>* rook(const Position& pos, const Piece& piece);
    set<Move>* bishop(const Position& pos, const Piece& piece);
    set<Move>* knight(const Position& pos, const Piece& piece);
    set<Move>* king(const Position& pos, const Piece& piece);
    set<Move>* pawn(const Position& pos, const Piece& piece);
    set<Move>* legal_moves(const Position& pos);
    int count_candidate_moves(const Color& color);

    void repeatCheck(set<Position>* s, const Position& ori, const Position& dir);
    void oneCheck(set<Position>* s, const Position& ori, const Position& dir);
    set<Position>* queen(const Position& pos);
    set<Position>* rook(const Position& pos);
    set<Position>* bishop(const Position& pos);
    set<Position>* king(const Position& pos);
    set<Position>* knight(const Position& pos);
    set<Position>* pawn(const Position& pos);
    set<Position>* get_attackList(const Position& pos);
    void calAttackSquare();

    const bool castling_check(const Color& color, const bool& isKingside) const;
    const int en_passent_check(const Position& pos);

public:
    Square board[8][8];
    Color turn, myColor, opColor;
    int pieceValue_wb[2];

    Chess clone() const { return *this; }

    Chess() { reset(); }
    void reset();
    bool isOver();

    set<Move>* get_candidateMove(const Color& color);
    void play(const Move& move);
    void print_board();
};
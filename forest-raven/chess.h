#ifndef CHESS_H
#define CHESS_H

#include <set>
#include <string>
#include <sstream>
#include "utility.h"
#include "board.h"

namespace ForestRaven{
    class Chess {

    }
}

struct Square {
    Piece piece;
    int attack_wb[2]{ 0, 0 };

    Square() { clear(); }

    void set(Piece p) { piece = p; }
    bool isAttacked(Color color) const { return (attack_wb[(color == WHITE ? BLACK : WHITE)] != 0); }
    bool empty() const { return piece.type == NOPIECE; }
    void clear() { piece = Piece(); }
};

struct Move {
    Piece piece;
    Position ori, dest, take;
    Piece_type promotion_type = NOPIECE;

    bool operator<(const Move& other) const {
        if (ori != other.ori) return ori < other.ori;
        if (dest != other.dest) return dest < other.dest;
        if (take != other.take) return take < other.take;
        if (promotion_type != other.promotion_type) return promotion_type < other.promotion_type;
        return piece < other.piece;
    }

    // NULL
    Move() {}

    // Move / Castling
    Move(Piece p, Position o, Position d) : piece(p), ori(o), dest(d) {};

    // Capture / En_passent
    Move(Piece p, Position o, Position d, Position t) : piece(p), ori(o), dest(d), take(t) {};

    // Pawn move promotion
    Move(Piece p, Position o, Position d, Piece_type pro) : piece(p), ori(o), dest(d), promotion_type(pro) {};

    // Pawn take something and promotion
    Move(Piece p, Position o, Position d, Position t, Piece_type pro) : piece(p), ori(o), dest(d), take(t), promotion_type(pro) {};

    Move_type get_move_type() const {
        if (piece.type == KING && abs(dest.x - ori.x) == 2) return CASTLING;
        else if (promotion_type != NOPIECE) return (take.x == -1 ? MOVE_PRO : CAPTURE_PRO);
        else if (take.x != -1) return CAPTURE;
        else if (take.x == -1) return MOVE;
        else return NOMOVE;
    }
    void set(const Move& other) { 
        piece = other.piece;
        ori = other.ori;
        dest = other.dest;
        take = other.take;
        promotion_type = other.promotion_type;
    }
    const string get_string() const {
        string st = "";
        auto pos_to_st = [&](const Position& pos) -> void {
            st += to_string(pos.x); st += ' '; st += to_string(pos.y); st += ' ';
            };
        pos_to_st(ori); pos_to_st(dest); pos_to_st(take);
        st += to_string(promotion_type);
        return st;
    }
    void string_init(string st) {
        stringstream ss(st); string word;
        getline(ss, word, ' '); ori.x = stoi(word);
        getline(ss, word, ' '); ori.y = stoi(word);
        getline(ss, word, ' '); dest.x = stoi(word);
        getline(ss, word, ' '); dest.y = stoi(word);
        getline(ss, word, ' '); take.x = stoi(word);
        getline(ss, word, ' '); take.y = stoi(word);
        getline(ss, word, ' '); int pro = stoi(word);
        promotion_type = (pro == -1 ? NOPIECE : promotion_list[pro]);
    }
};

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

    void move(const Move& move);
    void capture(const Move& move);
    void castling(const Move& move);
    void promotion(const Move& move);

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

#endif // CHESS_H
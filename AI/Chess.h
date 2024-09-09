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


public:
    Square board[8][8];
    Color turn, myColor;

    Chess() {
        reset();
    }

    void reset() {
        Type initList[8]{ ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            board[1][i].set(Piece(PAWN, WHITE));
            board[0][i].set(Piece(initList[i], WHITE));
            board[6][i].set(Piece(PAWN, BLACK));
            board[7][i].set(Piece(initList[i], BLACK));
        }
        turn = WHITE, myColor = WHITE;
        king_position_wb[0] = Position(4, 0), king_position_wb[1] = Position(4, 7);
        for (int i = 0; i < 4; i++) kr_moveCheck_wb_qk[i / 2][i % 2] = false;
        prevMove.clear();
        calAttackSquare();
    }

};


void move_piece(Square** board, Position* king_position_wb, const Position& cur, const Position& dest) {
    if (board[cur.y][cur.x].piece.type == KING) // king position recording
        king_position_wb[board[cur.y][cur.x].piece.color] = dest;
    board[dest.y][dest.x].piece = board[cur.y][cur.x].piece;
    board[cur.y][cur.x].clear();
}
void move(Square** board, Position* king_position_wb, bool** kr_moveCheck_wb_qk, PreviousMove* prevMove, Color* turn, const Position& cur, const Position& dest, const int& promotion = -1) {
    bool isCastling = false, isEn_passent = false, isPromotion = false;
    Color color = board[cur.y][cur.x].piece.color;
    Type type = board[cur.y][cur.x].piece.type;

    if (type == KING) {
        kr_moveCheck_wb_qk[color][0] = kr_moveCheck_wb_qk[color][1] = true;
        if (abs(dest.x - cur.x) == 2) // Castling move
            isCastling = true;
    }
    else if (type == ROOK) {
        if (cur.x == 0) kr_moveCheck_wb_qk[color][0] = true;
        else if (cur.x == 7) kr_moveCheck_wb_qk[color][1] = true;
    }
    else if (type == PAWN) {
        if (dest.y == (color == WHITE ? 7 : 0)) isPromotion = true;
        if (cur.x != dest.x) // Pawn takes something
            if (board[dest.y][dest.x].empty())
                isEn_passent = true;
    }

    if (isCastling) castling_move(board, cur, dest);
    else if (isEn_passent) en_passent_move(board, cur, dest);
    else {
        move_piece(board, king_position_wb, cur, dest);
        if (isPromotion) {
            Type promotion_type = NOTYPE;
            switch (promotion) {
            case 0: promotion_type = QUEEN; break;
            case 1: promotion_type = ROOK; break;
            case 2: promotion_type = BISHOP; break;
            case 3: promotion_type = KNIGHT; break;
            default: break;
            }
            board[dest.y][dest.x].set(Piece(promotion_type, color));
        }
    }

    calAttackSquare(board);
    prevMove->set(type, cur, dest);
    *turn = (*turn == WHITE ? BLACK : WHITE);
}
void castling_move(Square** board, const Position& cur, const Position& dest);
void en_passent_move(Square** board, const Position& cur, const Position& dest);

const bool boundaryCheck(const Position& pos) { return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true; }
const bool isAlly(Square** board, const Position& a, const Position& b);
const bool isEnemy(Square** board, const Position& a, const Position& b);
const bool isCheck(Square** board, Position king_position_wb, const Color& color);

void calAttackSquare(Square** board);
const bool isThisMoveLegal(Square** board, const Position& cur, const Position& dest, const Position& take = Position(-1, -1));
void append(Square** board, set<Position>* set, const Position& cur, const Position& dest, const bool& legalMove, const Position& take = Position(-1, -1));

void repeatMove(Square** board, set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove);
void oneMove(Square** board, set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove);
set<Position>* rook(Square** board, const Position& pos, const bool& legalMove);
set<Position>* bishop(Square** board, const Position& pos, const bool& legalMove);
set<Position>* knight(Square** board, const Position& pos, const bool& legalMove);
set<Position>* king(Square** board, const bool** kr_moveCheck_wb_qk, const Position& pos, const bool& legalMove);
set<Position>* queen(Square** board, const Position& pos, const bool& legalMove);
set<Position>* pawn_move(Square** board, const Position& pos);
set<Position>* pawn_attack(Square** board, const PreviousMove& prevMove, const Position& pos, const bool& legalMove);
set<Position>* get_attackList(Square** board, const Position& pos);
set<Position>* get_legalMoveList(Square** board, const PreviousMove& prevMove, const bool** kr_moveCheck_wb_qk, const Position& pos);
set<pair<Position, Position>>* get_candidateMove(Square** board, const Color& color);

const bool castling_check(Square** board, const bool** kr_moveCheck_wb_qk, const Color& color, const bool& isKingside) {
    int rank = (color == WHITE ? 0 : 7);
    bool condition_1 = !kr_moveCheck_wb_qk[color][isKingside ? 1 : 0];
    bool condition_2 = isKingside ? board[rank][5].empty() && board[rank][6].empty() :
        board[rank][3].empty() && board[rank][2].empty() && board[rank][1].empty();
    bool condition_3 = isKingside ? !board[rank][5].isAttacked(color) && !board[rank][6].isAttacked(color) :
        !board[rank][3].isAttacked(color) && !board[rank][2].isAttacked(color);
    return condition_1 && condition_2 && condition_3;
}
const int en_passent_check(Square** board, const PreviousMove& prevMove, const Position& pos);

void print_board(Square** board);

#endif // CHESS_H
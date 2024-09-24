#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <cassert>

using namespace std;

namespace ForestRaven {
    using Bitboard = uint64_t;

    enum Color : int {
        WHITE,
        BLACK,
        COLOR_NB,
        NOCOLOR = -1,
    };
    constexpr char colorToChar[2] = { 'w', 'b' };

    enum Move_type : int{
        NOMOVE = -1,
        MOVE,
        CAPTURE,
        MOVE_PRO,
        CAPTURE_PRO,
        CASTLING
    };
    enum Piece_type {
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        KING,
        PAWN,
        PIECE_TYPE_NB,
        NOPIECE = -1,
    };
    constexpr Piece_type init_positions[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
    constexpr char pt_char[7] = { 'Q', 'R', 'B', 'N', 'K', 'P' };
    constexpr int piece_value[7] = {  9,   5,   3,   3,   0,   1 };

    enum Direction : int {
        UP = 8,
        RIGHT = 1,
        DOWN = -UP,
        LEFT = -RIGHT,

        UP_LEFT = UP + LEFT,
        UP_RIGHT = UP + RIGHT,
        DOWN_LEFT = DOWN + LEFT,
        DOWN_RIGHT = DOWN + RIGHT
    };
    constexpr Direction dir_straight[4] = { UP, RIGHT, DOWN, LEFT };
    constexpr Direction dir_diagonal[4] = { UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };

    enum Square : uint32_t {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,
        SQUARE_NB
    };
    inline static void operator++(Square& s) { s = Square(int(s) + 1); }
    inline Square& operator+=(Square& s, Direction d) { return s = Square(s + d); }
    constexpr bool is_ok(Square s) { return s >= A1 && s <= H8; }
    constexpr Bitboard sq_bb(Square s) { assert(is_ok(s)); return Bitboard(1ULL << s); }

}

#endif // UTILITY_H
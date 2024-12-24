#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <iostream>
#include <cassert>

#define Fori(x) for (int i = 0; i < x; i++)
#define Forj(x) for (int j = 0; j < x; j++)

using namespace	std;

#if defined(USE_PEXT)
#include <immintrin.h>  // Header for _pext_u64() intrinsic
#define pext(b, m) _pext_u64(b, m)
#else
#define pext(b, m) 0
#endif


namespace ForestRaven {
#ifdef USE_PEXT
    constexpr bool HasPext = true;
#else
    constexpr bool HasPext = false;
#endif
#ifdef IS_64BIT
    constexpr bool Is64Bit = true;
#else
    constexpr bool Is64Bit = false;
#endif

    using Bitboard = uint64_t;

    enum Color : int {
        NOCOLOR = -1,
        WHITE = 0,
        BLACK = 1,
        COLOR_NB
    };
    inline Color operator~(Color& c) { return Color(c ^ BLACK); }

    enum Piece_type : int {
        NOPIECETYPE = -1,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        KING,
        PAWN,
        PIECE_TYPE_NB
    };
    constexpr Piece_type promotion_list[4] = { ROOK, KNIGHT, BISHOP, QUEEN };


    enum Piece {
        NOPIECE = -1,
        W_QUEEN = QUEEN, W_ROOK, W_BISHOP, W_KNIGHT, W_KING, W_PAWN,
        B_QUEEN = QUEEN + 8, B_ROOK, B_BISHOP, B_KNIGHT, B_KING, B_PAWN,
        PIECE_NB = 14
    };
    inline Color color_of(Piece pc) { return Color(pc >> 3); }
    inline Piece_type type_of(Piece pc) { return Piece_type(pc & 7); }

    enum Direction : int {
        U = 8,
        R = 1,
        D = -U,
        L = -R,
        UU = U + U,
        DD = D + D,
        RR = R + R,
        LL = L + L,

        UL = U + L,
        UR = U + R,
        DL = D + L,
        DR = D + R
    };
    inline Direction operator+(Direction& s, Direction d) { return Direction(int(s) + int(d)); }

    enum Square : int {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,
        SQUARE_NB,
        NOSQUARE = -1
    };
    inline Square operator+(Square& s, Direction d) { return Square(int(s) + int(d)); }
    inline static void operator++(Square& s) { s = Square(int(s) + 1); }
    inline Square& operator+=(Square& s, int d) { return s = Square(s + d); }

    constexpr bool is_ok(Square s) { return A1 <= s && s <= H8; }
    constexpr inline Bitboard sq_bb(Square s) { assert(is_ok(s)); return Bitboard(1ULL << s); }

    enum File : int {
        FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        FILE_NB
    };
    constexpr File file_of(Square s) { return File(s & 7); }
    inline static void operator++(File& f) { f = File(int(f) + 1); }

    enum Rank : int {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_NB
    };
    constexpr Rank rank_of(Square s) { return Rank(s >> 3); }

    struct Move {
        Piece piece;
        Square ori, dest, take = NOSQUARE;
        Piece_type promotion = NOPIECETYPE;
        bool isAttacked = false;

        Move(Piece pc, Square o, Square d) : piece(pc), ori(o), dest(d) {}
        Move(Piece pc, Square o, Square d, Square t) : piece(pc), ori(o), dest(d), take(t) {}
        Move(Piece pc, Square o, Square d, Square t, Piece_type pt) : piece(pc), ori(o), dest(d), take(t), promotion(pt) {}
        Move(Piece pc, Square o, Square d, Piece_type pt) : piece(pc), ori(o), dest(d), promotion(pt) {}

        Move() { piece = NOPIECE, ori = NOSQUARE, dest = NOSQUARE; }
        Move(const Move& m) : piece(m.piece), ori(m.ori), dest(m.dest), take(m.take), promotion(m.promotion) {}
    };
    inline bool move_comp(const Move& a, const Move& b) {
        if (a.take != b.take) return a.take != NOSQUARE && b.take == NOSQUARE;
        return a.isAttacked > b.isAttacked;
    }
}

#endif  // #ifndef UTILITY_H_INCLUDED
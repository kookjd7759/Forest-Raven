#include <iostream>
#include <cassert>

#define Fori(x) for (int i = 0; i < x; i++)
#define Forj(x) for (int j = 0; j < x; j++)

using namespace	std;

namespace ForestRaven {
    using Bitboard = uint64_t;

    enum Color : int {
        NOCOLOR = -1,
        WHITE = 0,
        BLACK = 1,
        COLOR_NB
    };
    inline Color operator!(Color c) { return (c == WHITE ? BLACK : WHITE); }

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
    constexpr char pt_char[7] = { 'Q', 'R', 'B', 'N', 'K', 'P' };
    Piece_type char_pt(char c) {
        return c == 'Q' ? QUEEN
            : c == 'R' ? ROOK
            : c == 'B' ? BISHOP
            : c == 'N' ? KNIGHT
            : c == 'K' ? KING
            : c == 'P' ? PAWN
            : NOPIECETYPE;
    }
    bool isPieceType(char c) { return (c == 'Q' || c == 'R' || c == 'B' || c == 'N' || c == 'K'); }
    bool isProPieceType(char c) { return (c == 'Q' || c == 'R' || c == 'B' || c == 'N'); }

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
    
    constexpr bool is_ok(Square s) { return s >= A1 && s <= H8; }
    constexpr inline Bitboard sq_bb(Square s) { assert(is_ok(s)); return Bitboard(1ULL << s); }

    string sq_notation(Square s) { return s == NOSQUARE ? "--" : string(1, "abcdefgh"[(s % 8)]) + string(1, "12345678"[(s / 8)]); }
    Square notation_sq(string st) { return !st.compare("--") ? NOSQUARE : Square(((st[1] - '1') * 8) + (st[0] - 'a')); }

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
    bool isFile(char c) { return ('a' <= c && c <= 'h'); }

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
    bool isRank(char c) { return ('1' <= c && c <= '8'); }

}
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
    inline Color operator!(Color c) { return (c == WHITE ? BLACK : WHITE); }

    enum Move_type : int{
        NOMOVE = -1,
        CASTLING_OO,
        CASTLING_OOO,
        CASTLING_TYPE_NB,
        MOVE,
        CAPTURE,
        MOVE_PRO,
        CAPTURE_PRO
    };
    const string mt_str(Move_type mt) {
        return mt == MOVE ? "MOVE"
            : mt == CAPTURE ? "CAPTURE"
            : mt == MOVE_PRO ? "MOVE_PROMOTION"
            : mt == CAPTURE_PRO ? "CAPTURE_PROMOTION"
            : mt == CASTLING_OO ? "CASTLING_OO"
            : mt == CASTLING_OOO ? "CASTLING_OOO"
            : "NOMOVE";
    }

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
    constexpr Piece_type promotion_list[8] = { ROOK, KNIGHT, BISHOP, QUEEN };
    constexpr char pt_char[7] = { 'Q', 'R', 'B', 'N', 'K', 'P' };
    constexpr int piece_value[7] = {  9,   5,   3,   3,   0,   1 };

    enum Direction : int {
        U = 8,
        R = 1,
        D = -U,
        L = -R,
        UU = U + U,
        DD = D + D,

        UL = U + L,
        UR = U + R,
        DL = D + L,
        DR = D + R,

        UUL = U + U + L,
        UUR = U + U + R,
        DDL = D + D + L,
        DDR = D + D + R,
    };
    constexpr Direction dir_straight[4] = { U, R, D, L };
    constexpr Direction dir_diagonal[4] = { UL, UR, DL, DR};
    constexpr Direction all_direction[8] = { U, R, D, L, UL, UR, DL, DR};

    enum Square : uint32_t {
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
    inline static void operator++(Square& s) { s = Square(int(s) + 1); }
    inline Square operator+(Square& s, Direction d) { return Square(int(s) + int(d)); }
    inline Square& operator+=(Square& s, Direction d) { return s = s + d; }
    constexpr bool is_ok(Square s) { return s >= A1 && s <= H8; }
    constexpr inline Bitboard sq_bb(Square s) { assert(is_ok(s)); return Bitboard(1ULL << s); }
    // constexpr inline Square bitboard_to_square(Bitboard b) { assert(b != 0); return Square(__builtin_ctzll(b)); }

    string sq_notation(Square s) { return s == NOSQUARE ? "--" : string(1, "ABCDEFGH"[(s % 8)]) + string(1, "12345678"[(s / 8)]);  }
    Square notation_sq(string st) { return st.compare("--") ? NOSQUARE : Square((('1' - st[0]) * 8) + ('A' - st[0])); }
    
    struct Move {
        Color      color = NOCOLOR;
        Move_type  type = NOMOVE;
        Piece_type piece = NOPIECE, promotion = NOPIECE;
        Square     ori = NOSQUARE, dest = NOSQUARE, take = NOSQUARE;
        Bitboard   ori_bb = 0, dest_bb = 0, take_bb = 0;

        Move() {}
        Move(Color c, Piece_type pt, Move_type mt, Square ori, Square dest, Piece_type pro_pt = NOPIECE)
            : color(c), piece(pt), type(mt), ori(ori), dest(dest), promotion(pro_pt),
            ori_bb(sq_bb(ori)), dest_bb(sq_bb(ori)) {}
        Move(Color c, Piece_type pt, Move_type mt, Square ori, Square dest, Square take, Piece_type pro_pt = NOPIECE)
            : color(c), piece(pt), type(mt), ori(ori), dest(dest), take(take), promotion(pro_pt),
            ori_bb(sq_bb(ori)), dest_bb(sq_bb(ori)), take_bb(sq_bb(take)){
        }

        void string_init() {

        }
        string get_string() {
            string st = sq_notation(ori) + ' ' + sq_notation(dest) + ' ' + sq_notation(take) + ' ';
            st += promotion == NOPIECE ? "-1" : to_string(promotion);
            return st;
        }
    };

    void print_BB(Bitboard b) {
        int idx = 0; string rank = "87654321";
        for (Square s : {A8, A7, A6, A5, A4, A3, A2, A1}) {
            cout << rank[idx++] << ' ';
            for (int i = 0; i < 8; ++i) {
                Bitboard sq = sq_bb(s) << i;
                cout << ((sq & b) ? "бс" : "бр") << ' ';
            }
            cout << "\n";
        }
        cout << "  ";
        for (char st : "ABCDEFGH")
            cout << st << ' ';
        cout << "\n";
    }
}

#endif // UTILITY_H
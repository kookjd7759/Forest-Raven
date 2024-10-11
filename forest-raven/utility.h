#include <iostream>
#include <cassert>
#include <string>

using namespace std;

namespace ForestRaven {
    using Bitboard = uint64_t;

    enum Color : int {
        NOCOLOR = -1,
        WHITE,
        BLACK,
        COLOR_NB
    };
    constexpr char colorToChar[2] = { 'w', 'b' };
    inline Color operator!(Color c) { return (c == WHITE ? BLACK : WHITE); }

    enum Move_type : int {
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
    constexpr Piece_type initPos[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
    constexpr Piece_type promotion_list[8] = { ROOK, KNIGHT, BISHOP, QUEEN };
    constexpr char pt_char[7] = { 'Q', 'R', 'B', 'N', 'K', 'P' };

    enum Piece {
        NOPIECE = -1,
        W_QUEEN = QUEEN, W_ROOK, W_BISHOP, W_KNIGHT, W_KING, W_PAWN,
        B_QUEEN = QUEEN + 8, B_ROOK, B_BISHOP, B_KNIGHT, B_KING, B_PAWN,
        PIECE_NB = 14
    };
    inline Color color_of(Piece pc) { return Color(pc >> 3); }
    inline Piece_type type_of(Piece pc) { return Piece_type(pc & 7); }
    inline Piece pt_pc(Color c, Piece_type pt) { return Piece((c << 3) | pt); }

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
    constexpr Direction dir_diagonal[4] = { UL, UR, DL, DR };
    constexpr Direction all_direction[8] = { U, R, D, L, UL, UR, DL, DR };

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
    inline static void operator++(Square& s) { s = Square(int(s) + 1); }
    inline Square operator+(Square& s, Direction d) { return Square(int(s) + int(d)); }
    inline Square& operator+=(Square& s, Direction d) { return s = s + d; }

    constexpr bool is_ok(Square s) { return s >= A1 && s <= H8; }
    constexpr inline Bitboard sq_bb(Square s) { assert(is_ok(s)); return Bitboard(1ULL << s); }

    string sq_notation(Square s) { return s == NOSQUARE ? "--" : string(1, "abcdefgh"[(s % 8)]) + string(1, "12345678"[(s / 8)]);  }
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


    // Move message format 
    // [color][pieceType][moveType][ori][dest][take][promotion]
    // [int  ][int      ][int     ][str][str ][str ]['-'/int  ]
    // [1    ][1        ][1       ][2  ][2   ][2   ][1        ]
    struct Move {
        Color      color = NOCOLOR;
        Move_type  moveType = NOMOVE;
        Piece_type pieceType = NOPIECETYPE, promotion = NOPIECETYPE;
        Square     ori = NOSQUARE, dest = NOSQUARE, take = NOSQUARE;
        Bitboard   ori_bb = 0, dest_bb = 0, take_bb = 0;

        Move() {}
        Move(Color c, Piece_type pt, Move_type mt, Square ori, Square dest, Piece_type pro_pt = NOPIECETYPE)
            : color(c), pieceType(pt), moveType(mt), ori(ori), dest(dest), promotion(pro_pt),
            ori_bb(sq_bb(ori)), dest_bb(sq_bb(dest)) {}
        Move(Color c, Piece_type pt, Move_type mt, Square ori, Square dest, Square take, Piece_type pro_pt = NOPIECETYPE)
            : color(c), pieceType(pt), moveType(mt), ori(ori), dest(dest), take(take), promotion(pro_pt),
            ori_bb(sq_bb(ori)), dest_bb(sq_bb(dest)), take_bb(sq_bb(take)) {}

        void string_init(string line) {
            color = Color(line[0] - '0');
            pieceType = Piece_type(line[1] - '0');
            moveType = Move_type(line[2] - '0');
            ori = notation_sq(line.substr(3, 2));
            dest = notation_sq(line.substr(5, 2));
            take = notation_sq(line.substr(7, 2));
            promotion = (line[9] == '-' ? NOPIECETYPE : Piece_type(line[9] - '0'));

            ori_bb = sq_bb(ori), dest_bb = sq_bb(dest);
            if (take != NOSQUARE) take_bb = sq_bb(take);
        }
        string get_string() {
            string st = to_string(color) + to_string(pieceType) + to_string(moveType) +
                sq_notation(ori) + sq_notation(dest) + sq_notation(take);
            st += promotion == NOPIECE ? "-" : to_string(promotion);
            return st;
        }
    };
    
    void print_BB(Bitboard b, string text = "") {
        if (text != "") cout << text << "\n";
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
        if (text != "") cout << "\n";
    }
}

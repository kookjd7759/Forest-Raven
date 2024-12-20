#include "utility.h"
#include <vector>

namespace ForestRaven {
    const char pt_char[7] = { 'Q', 'R', 'B', 'N', 'K', 'P' };
    Piece_type char_pt(char c) {
        return c == 'Q' ? QUEEN
            : c == 'R' ? ROOK
            : c == 'B' ? BISHOP
            : c == 'N' ? KNIGHT
            : c == 'K' ? KING
            : c == 'P' ? PAWN
            : NOPIECETYPE;
    }

    bool is_pieceType(char c)    { return (c == 'Q' || c == 'R' || c == 'B' || c == 'N' || c == 'K'); }
    bool is_proPieceType(char c) { return (c == 'Q' || c == 'R' || c == 'B' || c == 'N'); }
    bool is_file(char c)         { return ('a' <= c && c <= 'h'); }
    bool is_rank(char c)         { return ('1' <= c && c <= '8'); }

	string sq_nt(Square s) { return is_ok(s) ? string(1, "abcdefgh"[s % 8]) + "12345678"[s / 8] : "--"; }
	Square nt_sq(string st) { return !st.compare("--") ? NOSQUARE : Square(((st[1] - '1') * 8) + (st[0] - 'a')); }

    string move_nt(const vector<Move>* moves, Move move);
    Move   nt_move(const vector<Move>* moves, const string& notation);
}
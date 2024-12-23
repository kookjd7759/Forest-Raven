#ifndef NOTATION_H_INCLUDED
#define NOTATION_H_INCLUDED

#include "utility.h"
#include <vector>

namespace ForestRaven {
    extern const char pt_char[7];
    Piece_type char_pt(char c);

    inline bool is_pieceType(char c) { return (c == 'Q' || c == 'R' || c == 'B' || c == 'N' || c == 'K'); }
    inline bool is_proPieceType(char c) { return (c == 'Q' || c == 'R' || c == 'B' || c == 'N'); }
    inline bool is_file(char c) { return ('a' <= c && c <= 'h'); }
    inline bool is_rank(char c) { return ('1' <= c && c <= '8'); }

    string sq_nt(Square s);
    Square nt_sq(string st);

    string move_nt(const vector<Move>* moves, Move move);
    Move   nt_move(const vector<Move>* moves, const string& notation);
}

#endif  // #ifndef NOTATION_H_INCLUDED
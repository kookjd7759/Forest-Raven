#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

#include "notation.h"

namespace ForestRaven {
    Move READ_move(const vector<Move>& moves) {
        string notation; cin >> notation;

        Square ori(nt_sq(notation.substr(0, 2)));
        Square dest(nt_sq(notation.substr(2, 2)));
        Piece_type promotion = char_pt(notation[4]);

        for (const Move& move : moves)
            if (move.ori == ori && move.dest == dest && move.promotion == promotion)
                return move;

        return Move();
    }
    void SEND_move(const Move& move) {
        cout << "SEND ";
        cout << color_of(move.piece);
        cout << type_of(move.piece);
        cout << sq_nt(move.ori) << sq_nt(move.dest) << sq_nt(move.take);
        if (move.promotion == NOPIECETYPE) cout << '-';
        else cout << pt_char[move.promotion];
        cout << "\n";
    }
}

#endif  // #ifndef CONNECTOR_H_INCLUDED
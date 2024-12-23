#include "notation.h"

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

    string sq_nt(Square s)  { return is_ok(s) ? string(1, "abcdefgh"[s % 8]) + "12345678"[s / 8] : "--"; }
    Square nt_sq(string st) { return !st.compare("--") ? NOSQUARE : Square(((st[1] - '1') * 8) + (st[0] - 'a')); }

    string move_nt(const vector<Move>* moves, Move move) {
        Piece_type pt = type_of(move.piece);

        if (pt == KING && abs(move.ori - move.dest) == 2)
            return move.ori < move.dest ? "O-O" : "O-O-O";

        string notation("");
        if (pt != PAWN) notation += pt_char[pt];

        File file = file_of(move.ori);
        Rank rank = rank_of(move.ori);
        int file_cnt(0), rank_cnt(0);
        for (const Move& legalMove : *moves)
            if (legalMove.piece == move.piece && legalMove.dest == move.dest) {
                if (file_of(legalMove.ori) == file) ++file_cnt;
                if (rank_of(legalMove.ori) == rank) ++rank_cnt;
            }

        if (file_cnt > 1 && rank_cnt == 1) notation += (char)('a' + file);
        if (file_cnt == 1 && rank_cnt > 1) notation += (char)('1' + rank);
        if (file_cnt > 1 && rank_cnt > 1) notation += sq_nt(move.ori);

        if (move.take != NOSQUARE) {
            if (pt == PAWN) notation += (char)('a' + file_of(move.ori));
            notation += 'x';
        }

        notation += sq_nt(move.dest);

        if (move.promotion != NOPIECETYPE) notation += '=' + pt_char[move.promotion];

        if (move.check) notation += '+';

        return notation;
    }
    Move   nt_move(const vector<Move>* moves, const string& notation) {
        auto find_move = [&](Piece_type pt, Square dest, Piece_type promotion = NOPIECETYPE) -> Move {
            Move ret;
            bool ch(false);
            for (const Move& move : *moves)
                if (type_of(move.piece) == pt && move.dest == dest && move.promotion == promotion)
                    if (ch) return Move();
                    else ret = move, ch = true;

            if (!ch) {
                cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
                return Move();
            }

            return ret;
            };

        auto find_move_with_file = [&](Piece_type pt, Square dest, File file) -> Move {
            Move ret;
            bool ch(false);
            for (const Move& move : *moves)
                if (type_of(move.piece) == pt && move.dest == dest && file_of(move.ori) == file)
                    if (ch) return Move();
                    else ret = move, ch = true;

            if (!ch) {
                cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
                return Move();
            }

            return ret;
            };

        auto find_move_with_rank = [&](Piece_type pt, Square dest, Rank rank) -> Move {
            Move ret;
            bool ch(false);
            for (const Move& move : *moves)
                if (type_of(move.piece) == pt && move.dest == dest && rank_of(move.ori) == rank)
                    if (ch) return Move();
                    else ret = move, ch = true;

            if (!ch) {
                cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
                return Move();
            }

            return ret;
            };

        auto find_move_with_ori = [&](Piece_type pt, Square dest, Square ori) -> Move {
            Move ret;
            bool ch(false);
            for (const Move& move : *moves)
                if (type_of(move.piece) == pt && move.dest == dest && move.ori == ori)
                    if (ch) return Move();
                    else ret = move, ch = true;

            if (!ch) {
                cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
                return Move();
            }

            return ret;
            };


        if (moves->size() == 0) { cout << "notation ERROR::moves vector is empty\n"; return  Move(); }

        Color color = color_of(moves->at(0).piece);
        Piece_type pt;
        Square dest;
        if (notation == "O-O") { // king side castling
            dest = (color == WHITE ? G1 : G8);
            return find_move(KING, dest);
        }
        else if (notation == "O-O-O") { // queen side castling
            dest = (color == WHITE ? C1 : C8);
            return find_move(KING, dest);
        }
        if (notation.size() == 2) { // pawn, move
            if (!is_file(notation[0]) || !is_rank(notation[1])) return Move();

            dest = nt_sq(notation);
            return find_move(PAWN, dest);
        }
        else if (notation.size() == 3) { // other piece, move
            if (!is_pieceType(notation[0]) || !is_file(notation[1]) || !is_rank(notation[2])) return Move();

            pt = char_pt(notation[0]);
            dest = nt_sq(notation.substr(1, 2));
            return find_move(pt, dest);
        }
        else if (notation.size() == 4) {
            if (is_file(notation[0])) {
                if (notation[1] == 'x') { // pawn, takes
                    if (!is_file(notation[2]) || !is_rank(notation[3])) return Move();

                    dest = nt_sq(notation.substr(2, 2));
                    return find_move(PAWN, dest);
                }
                else if (is_rank(notation[1])) { // pawn, move promotion
                    if (notation[2] != '=' || !is_proPieceType(notation[3])) return Move();

                    dest = nt_sq(notation.substr(0, 2));
                    Piece_type pt_pro = char_pt(notation[3]);
                    return find_move(PAWN, dest, pt_pro);
                }
                else return Move();
            }
            else if (is_pieceType(notation[0])) {
                pt = char_pt(notation[0]);

                if (!is_file(notation[2]) || !is_rank(notation[3])) return Move();
                dest = nt_sq(notation.substr(2, 2));

                if (notation[1] == 'x') { // other piece, takes
                    return find_move(pt, dest);
                }
                else if (is_file(notation[1])) { // other piece, move but specify FILE
                    File file = (File)(notation[1] - 'a');
                    return find_move_with_file(pt, dest, file);
                }
                else if (is_rank(notation[1])) { // other piece, move but specify RANK
                    Rank rank = (Rank)(notation[1] - '1');
                    return find_move_with_rank(pt, dest, rank);
                }
                else return Move();
            }
            else return Move();
        }
        else if (notation.size() == 5) {
            if (!is_pieceType(notation[0]) || !is_file(notation[3]) || !is_rank(notation[4])) return Move();

            pt = char_pt(notation[0]);
            dest = nt_sq(notation.substr(3, 2));

            if (is_file(notation[1]) && notation[2] == 'x') { // other piece, takes but specify FILE
                File file = (File)(notation[1] - 'a');
                return find_move_with_file(pt, dest, file);
            }
            else if (is_rank(notation[1]) && notation[2] == 'x') { // other piece, takes but specify RANK
                Rank rank = (Rank)(notation[1] - '1');
                return find_move_with_rank(pt, dest, rank);
            }
            else if (is_file(notation[1]) && is_rank(notation[2])) {  // other piece, move but specify FILE and RANK
                Square ori = nt_sq(notation.substr(1, 2));
                return find_move_with_ori(pt, dest, ori);
            }
            else return Move();
        }
        else if (notation.size() == 6) {
            if (is_file(notation[0])) { // pawn, takes and promotion 
                if (notation[1] != 'x' || !is_file(notation[2]) || !is_rank(notation[3]) ||
                    notation[4] != '=' || !is_proPieceType(notation[5])) return Move();

                dest = nt_sq(notation.substr(2, 2));
                Piece_type pt_pro = char_pt(notation[5]);
                return find_move(PAWN, dest, pt_pro);
            }
            else if (is_pieceType(notation[0])) {
                if (!is_file(notation[1]) || !is_rank(notation[2]) || notation[3] != 'x' ||
                    !is_file(notation[4]) || !is_rank(notation[5])) return Move();

                pt = char_pt(notation[0]);
                Square ori = nt_sq(notation.substr(1, 2));
                dest = nt_sq(notation.substr(4, 2));
                return find_move_with_ori(pt, dest, ori);
            }
            else return Move();
        }
        else return Move();
    }
}
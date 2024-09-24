#include "board.h"

using namespace std;

namespace ForestRaven {
	Bitboard attacks[PIECE_TYPE_NB][SQUARE_NB];
	Bitboard Pawn_attacks[COLOR_NB][SQUARE_NB];

	Bitboard destination(Square s, int step) {
		Square dest = Square(s + step);
		return is_boundary(dest) ? to_board(dest) : Bitboard(0);
	}
	Bitboard sliding_attack(Piece_type pt, Square s, Bitboard occupied) {
		Bitboard attacks(0);
		const array<Direction, 4>* dir = (pt == ROOK ? &dir_straight : &dir_diagonal);
		for (Direction d : *dir) {
			while (destination(s, d)) {
				attacks |= (s += d);
				if (occupied & s) break;
			}
		}

		return attacks;
	}



	void Board::init_position() {
		for (int c = 0; c < COLOR_NB; c++) for (int p = 0; p < PIECE_TYPE_NB; p++)
			board[c][p] = 0;
		exist = 0;

		for (int i = 0; i < 8; i++) {
			create_piece<WHITE>(init_positions[i], Square(A1 + i));
			create_piece<BLACK>(init_positions[i], Square(A8 + i));
			create_piece<WHITE>(PAWN, Square(A2 + i));
			create_piece<BLACK>(PAWN, Square(A7 + i));
		}
	}
	void Board::init() {
		for (Square s1 = A1; s1 <= H8; ++s1) {
			Pawn_attacks[WHITE][s1] = pawn_attacks<WHITE>(to_board(s1));
			Pawn_attacks[BLACK][s1] = pawn_attacks<BLACK>(to_board(s1));

			for (int step : {-9, -8, -7, -1, 1, 7, 8, 9})
				attacks[KING][s1] |= destination(s1, step);

			for (int step : {-17, -15, -10, -6, 6, 10, 15, 17})
				attacks[KNIGHT][s1] |= destination(s1, step);
		}
		init_position();
	}

	void Board::print() {
		auto get_sq_info = [&](const Bitboard& b) -> string {
			for (int c = 0; c < COLOR_NB; c++) for (int p = 0; p < PIECE_TYPE_NB; p++)
				if (board[c][p] & b) {
					string st = "";
					st += (c == WHITE ? 'w' : 'b');
					st += pt_char[p];
					return st;
				}
			};
		for (Square s : {A8, A7, A6, A5, A4, A3, A2, A1}) {
			for (int i = 0; i < 8; ++i) {
				Bitboard b = to_board(Square(s + i));
				cout << (b & exist ? get_sq_info(b) : "--") << ' ';
			}
			cout << "\n";
		}
	}
}
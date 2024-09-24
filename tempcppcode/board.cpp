#include "board.h"

using namespace std;

namespace ForestRaven {
	Bitboard attacks[PIECE_TYPE_NB][SQUARE_NB];
	Bitboard Pawn_attacks[COLOR_NB][SQUARE_NB];

	Bitboard destination(Square s, int step) {
		Square dest = Square(s + step);
		return is_boundary(dest) ? to_board(dest) : Bitboard(0);
	}
	Bitboard sliding_attack(Piece_type pt, Square s, Bitboard occupied = 0) {
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

	void print_bb() {

	}

	void Board::init_position() {
		for (Square s = A1; s <= H8; ++s) board[s] = NOPIECE;
		for (int c = 0; c < COLOR_NB; c++) byColorBB[c] = 0;
		for (int p = 0; p < PIECE_TYPE_NB; p++) byTypeBB[p] = 0;

		for (int i = 0; i < 8; i++) {
			create_piece<WHITE>(init_positions[i], Square(A1 + i));
			create_piece<BLACK>(init_positions[i], Square(A8 + i));
			create_piece<WHITE>(PAWN, Square(A2 + i));
			create_piece<BLACK>(PAWN, Square(A7 + i));
		}
	}
	void Board::init() {
		for (Square s = A1; s <= H8; ++s) {
			Pawn_attacks[WHITE][s] = pawn_attacks<WHITE>(to_board(s));
			Pawn_attacks[BLACK][s] = pawn_attacks<BLACK>(to_board(s));

			for (int step : {-9, -8, -7, -1, 1, 7, 8, 9})
				attacks[KING][s] |= destination(s, step);

			for (int step : {-17, -15, -10, -6, 6, 10, 15, 17})
				attacks[KNIGHT][s] |= destination(s, step);

			attacks[BISHOP][s] = sliding_attack(BISHOP, s);
			attacks[ROOK][s] = sliding_attack(ROOK, s);
			attacks[QUEEN][s] = attacks[BISHOP][s] | attacks[ROOK][s];
		}
		init_position();
	}

	void Board::move_piece(Move move) {
		Color c = move.color(); Piece_type pt = move.piece();
		Square ori = move.ori(), dest = move.dest();
		Bitboard oir_b = to_board(ori), dest_b = to_board(dest);
		board[ori] = NOPIECE, board[dest] = Piece_type(pt);
		byColorBB[c] &= ~oir_b, byColorBB[c] |= dest_b;
		byTypeBB[pt] &= ~oir_b, byTypeBB[pt] |= dest_b;
	}
	void Board::print() {
		auto get_info = [&](Square s) -> string {
			Bitboard b = to_board(s);
			string st = "";
			st += byColorBB[WHITE] & b ? 'w' : 'b';
			st += pt_char[board[s]];
			return st;
			};
		for (Square s : {A8, A7, A6, A5, A4, A3, A2, A1}) {
			for (int i = 0; i < 8; ++i) {
				cout << (board[Square(s + i)] == NOPIECE ? "--" : get_info(Square(s + i))) << ' ';
			}
			cout << "\n";
		}
	}
}
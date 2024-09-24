#include "utility.h"

namespace ForestRaven {
	constexpr Bitboard FileA = 0x0101010101010101ULL;
	constexpr Bitboard FileH = FileA << 7;

	Bitboard attacks[PIECE_TYPE_NB - 1][SQUARE_NB];
	Bitboard Pawn_attacks[COLOR_NB][SQUARE_NB];

	Bitboard destination(Square s, int step) { return is_ok(Square(s + step)) ? sq_bb(Square(s + step)) : Bitboard(0); }
	Bitboard sliding_attacks(Piece_type pt, Square s) {
		Bitboard attacks(0);
		const Direction *dir = (pt == ROOK ? dir_straight : dir_diagonal);
		for (int i = 0; i < 4; i++) {
			while (destination(s, dir[i])) {
				attacks |= (s += dir[i]);
			}
		}

		return attacks;
	}

	template<Direction D>
	constexpr Bitboard shift(Bitboard b) {
		return D == UP ? b << 8
			: D == DOWN ? b >> 8
			: D == UP + UP ? b << 16
			: D == DOWN + DOWN ? b >> 16
			: D == RIGHT ? (b & ~FileH) << 1
			: D == LEFT ? (b & ~FileA) >> 1
			: D == UP_LEFT ? (b & ~FileA) << 7
			: D == UP_RIGHT ? (b & ~FileH) << 9
			: D == DOWN_LEFT ? (b & ~FileA) >> 9
			: D == DOWN_RIGHT ? (b & ~FileH) >> 7
			: 0;
	}

	template<Color C>
	constexpr Bitboard pawn_attacks(Bitboard b) {
		return C == WHITE ? shift<UP_LEFT>(b) | shift<UP_RIGHT>(b)
			: shift<DOWN_LEFT>(b) | shift<DOWN_RIGHT>(b);
	}

	class Board {
	private:
		Piece_type board[SQUARE_NB];
		Bitboard   byColorBB[COLOR_NB];
		Bitboard   byTypeBB[PIECE_TYPE_NB];
		Color      turn = WHITE;

		template<Color C>
		void create_piece(Piece_type p, Square s) { 
			Bitboard b = sq_bb(s);
			board[s] = p;
			byColorBB[C] |= b;
			byTypeBB[p] |= b;
		};
		void init_position() {
			for (Square s = A1; s <= H8; ++s) board[s] = NOPIECE;
			byColorBB[WHITE] = byColorBB[BLACK] = Bitboard(0);
			byTypeBB[QUEEN] = byTypeBB[ROOK] = byTypeBB[BISHOP] = byTypeBB[KNIGHT] = byTypeBB[KING] = byTypeBB[PAWN] = Bitboard(0);

			for (int i = 0; i < 8; i++) {
				create_piece<WHITE>(init_positions[i], Square(A1 + i));
				create_piece<BLACK>(init_positions[i], Square(A8 + i));
				create_piece<WHITE>(PAWN, Square(A2 + i));
				create_piece<BLACK>(PAWN, Square(A7 + i));
			}
		}
		void init() {
			for (Square s = A1; s <= H8; ++s) {
				Pawn_attacks[WHITE][s] = pawn_attacks<WHITE>(sq_bb(s));
				Pawn_attacks[BLACK][s] = pawn_attacks<BLACK>(sq_bb(s));

				for (int step : {-9, -8, -7, -1, 1, 7, 8, 9})
					attacks[KING][s] |= destination(s, step);

				for (int step : {-17, -15, -10, -6, 6, 10, 15, 17})
					attacks[KNIGHT][s] |= destination(s, step);

				attacks[BISHOP][s] = sliding_attacks(BISHOP, s);
				attacks[ROOK][s] = sliding_attacks(ROOK, s);
				attacks[QUEEN][s] = attacks[BISHOP][s] | attacks[ROOK][s];
			}
			init_position();
		}

	public:
		Board() { init(); }
		void print() {
			auto get_info = [&](Square s) -> string {
				Bitboard b = sq_bb(s);
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
	};
}
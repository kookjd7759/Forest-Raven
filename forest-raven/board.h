#include <vector>
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

	const static Move CASTLING_rook[COLOR_NB][CASTLING_TYPE_NB]{
		{Move(WHITE, ROOK, MOVE, H1,  F1), Move(WHITE, ROOK, MOVE, A1,  D1)},
		{Move(WHITE, ROOK, MOVE, H8,  F8), Move(WHITE, ROOK, MOVE, A8,  D8)}
	};

	constexpr int knight_steps[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
	constexpr int king_steps[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };

	class Chess {
	public:
		Piece_type board[SQUARE_NB];
		Bitboard   byColorBB[COLOR_NB];
		Bitboard   byAttackBB[COLOR_NB];
		Bitboard   byTypeBB[PIECE_TYPE_NB];
		Bitboard   existBB;
		Color      turn, myColor;
		Move       prevMove;
		bool       move_king[COLOR_NB],
			       move_rook[COLOR_NB][2];

		Chess(Color c) { init(c); }
		Chess clone() const { return *this; }

		void create_piece(Color c, Piece_type p, Square s) {
			Bitboard b = sq_bb(s);
			board[s] = p;
			byColorBB[c] |= b;
			byTypeBB[p] |= b;
			existBB |= b;
		};
		void init_position(Color c) {
			existBB = 0;

			for (Square s = A1; s <= H8; ++s) board[s] = NOPIECE;
			byColorBB[WHITE] = byColorBB[BLACK] = Bitboard(0);
			byTypeBB[QUEEN] = byTypeBB[ROOK] = byTypeBB[BISHOP] = byTypeBB[KNIGHT] = byTypeBB[KING] = byTypeBB[PAWN] = Bitboard(0);

			for (int i = 0; i < 8; i++) {
				create_piece(WHITE, init_positions[i], Square(A1 + i));
				create_piece(BLACK, init_positions[i], Square(A8 + i));
				create_piece(WHITE, PAWN, Square(A2 + i));
				create_piece(BLACK, PAWN, Square(A7 + i));
			}
			myColor = c;
			turn = WHITE;
			move_king[WHITE] = move_king[BLACK] = false;
			move_rook[WHITE][0] = move_rook[WHITE][1] = move_rook[BLACK][0] = move_rook[BLACK][1] = false;
			prevMove = Move();
		}
		void init(Color c) {
			for (Square s = A1; s <= H8; ++s) {
				Pawn_attacks[WHITE][s] = pawn_attacks<WHITE>(sq_bb(s));
				Pawn_attacks[BLACK][s] = pawn_attacks<BLACK>(sq_bb(s));

				for (int step : king_steps)
					attacks[KING][s] |= destination(s, step);

				for (int step : knight_steps)
					attacks[KNIGHT][s] |= destination(s, step);

				attacks[BISHOP][s] = sliding_attacks(BISHOP, s);
				attacks[ROOK][s] = sliding_attacks(ROOK, s);
				attacks[QUEEN][s] = attacks[BISHOP][s] | attacks[ROOK][s];
			}
			init_position(c);
		}

		bool isCheck(Color c) const { return ((byTypeBB[KING] & byColorBB[c] & byAttackBB[!c]) != 0); }
		bool isLegal(Move move) {
			Chess next = this->clone();
			next.play(move);
			return !next.isCheck(!turn);
		}
		void append(vector<Move>* moves, Move move) {
			if (isLegal(move)) moves->push_back(move);
		}
		void sliding_moves(vector<Move>* moves, Piece_type pt, Square ori, const Direction* dir, int size = 4) {
			Color c = (byColorBB[WHITE] & byTypeBB[pt]) ? WHITE : BLACK;
			for (int i = 0; i < size; i++) {
				while (true) {
					Square dest = ori + dir[i];
					Bitboard dest_bb = destination(ori, dir[i]);
					if (!dest_bb) break;
					if ((existBB & dest_bb)) {
						if (byColorBB[!c] & dest_bb) append(moves, Move(c, pt, CAPTURE, ori, dest, dest));
						break;
					}
					append(moves, Move(c, pt, MOVE, ori, dest));
				}
			}
		}
		void one_moves(vector<Move>* moves, Piece_type pt, Square ori, const int* step) {
			Color c = (byColorBB[WHITE] & byTypeBB[pt]) ? WHITE : BLACK;
			for (int i = 0; i < 8; ++i) {
				Square dest = Square(ori + step[i]);
				if (is_ok(dest)) {
					Bitboard dest_bb = Square(ori + step[i]);
					if (dest_bb & existBB) {
						if (byColorBB[!c] & dest_bb)
							append(moves, Move(c, pt, CAPTURE, ori, dest, dest));
					}
					else {
						append(moves, Move(c, pt, MOVE, ori, dest));
					}
				}
			}
		}
		vector<Move>* queen(Square s) {
			vector<Move>* moves = new vector<Move>;
			sliding_moves(moves, QUEEN, s, all_direction, 8);
			return moves;
		}
		vector<Move>* rook(Square s) {
			vector<Move>* moves = new vector<Move>;
			sliding_moves(moves, ROOK, s, dir_straight);
			return moves;
		}
		vector<Move>* bishop(Square s) {
			vector<Move>* moves = new vector<Move>;
			sliding_moves(moves, BISHOP, s, dir_diagonal);
			return moves;
		}
		vector<Move>* knight(Square s) {
			vector<Move>* moves = new vector<Move>;
			one_moves(moves, KNIGHT, s, knight_steps);
			return moves;
		}
		vector<Move>* king(Square s) {
			vector<Move>* moves = new vector<Move>;
			one_moves(moves, KING, s, king_steps);
			return moves;
		}
		vector<Move>* pawn(Color c, Square s) {
			vector<Move>* moves = new vector<Move>;
			Bitboard ori_bb = sq_bb(s);
			if (!(existBB & shift<UP>(ori_bb))) {
				append(moves, Move(c, PAWN, MOVE, ))
			}

			return moves;
		}

		vector<Move>* legal_moves(Color c, Square s) {

		}
		vector<Move>* candidate_moves() {

		}

		Bitboard sliding_attacks(Piece_type pt, Square s) {
			Bitboard attacks(0);
			Color c = (byColorBB[WHITE] & byTypeBB[pt]) ? WHITE : BLACK;
			const Direction* dir = (pt == ROOK ? dir_straight : dir_diagonal);
			for (int i = 0; i < 4; i++) {
				while (true) {
					Bitboard move = destination(s, dir[i]);
					if (!move) break;
					attacks |= (s += dir[i]);
					if (existBB & move) break;
				}
			}

			return attacks;
		}
		Bitboard get_attacks(Color c, Square s) {
			return board[s] == PAWN ? Pawn_attacks[c][s]
				: board[s] == KING ? attacks[KING][s]
				: board[s] == KNIGHT ? attacks[KNIGHT][s]
				: board[s] == BISHOP ? sliding_attacks(BISHOP, s)
				: board[s] == ROOK ? sliding_attacks(ROOK, s)
				: board[s] == QUEEN ? sliding_attacks(BISHOP, s) | sliding_attacks(ROOK, s)
				: 0;
		}
		void calAttackBB() {
			byAttackBB[WHITE] = byAttackBB[BLACK] = 0;
			for (Square s = A1; s <= H8; ++s) {
				if (existBB & bb[s]) {
					Color c = (byColorBB[WHITE] & existBB) ? WHITE : BLACK;
					byAttackBB[c] |= get_attacks(c, s);
				}
			}
		}

		void move(Move move) {
			if (move.type == KING) move_king[move.color] = true;
			else if (move.type == ROOK) {
				if (FileA & move.ori_bb) move_rook[move.color][1] = true;
				else if (FileH & move.ori_bb) move_rook[move.color][0] = true;
			}

			board[move.dest] = board[move.ori], board[move.ori] = NOPIECE;
			byColorBB[move.color] ^= ~move.ori_bb | move.dest_bb;
			byTypeBB[move.type] ^= ~move.ori_bb | move.dest_bb;
			existBB ^= ~move.ori_bb | move.dest_bb;

			prevMove = move;
		}
		void capture(Move move) {
			Piece_type take_pt = board[move.take];
			board[move.take] = NOPIECE;
			byColorBB[!move.color] &= ~move.take_bb;
			byTypeBB[take_pt] &= ~move.take_bb;
			existBB &= ~move.take_bb;
			Chess::move(move);
		}
		void castling(Move move) {
			Chess::move(CASTLING_rook[move.color][move.type]);
			Chess::move(move);
		}
		void promotion(Move move) {
			create_piece(move.color, move.promotion, move.dest);
		}
		void play(Move move) {
			switch (move.type) {
			case MOVE: Chess::move(move); break;
			case CAPTURE: capture(move); break;
			case CASTLING_OO || CASTLING_OOO: castling(move); break;
			case MOVE_PRO: Chess::move(move), promotion(move); break;
			case CAPTURE_PRO: capture(move), promotion(move); break;
			default: break;
			}
			turn = !turn;
			calAttackBB();
		}

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
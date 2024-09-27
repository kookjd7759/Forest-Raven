#include <vector>
#include "utility.h"

namespace ForestRaven {
	constexpr Bitboard FileA = 0x0101010101010101ULL;
	constexpr Bitboard FileB = FileA << 1;
	constexpr Bitboard FileC = FileA << 2;
	constexpr Bitboard FileD = FileA << 3;
	constexpr Bitboard FileE = FileA << 4;
	constexpr Bitboard FileF = FileA << 5;
	constexpr Bitboard FileG = FileA << 6;
	constexpr Bitboard FileH = FileA << 7;

	constexpr Bitboard Rank1 = 0xFF;
	constexpr Bitboard Rank2 = Rank1 << (8 * 1);
	constexpr Bitboard Rank3 = Rank1 << (8 * 2);
	constexpr Bitboard Rank4 = Rank1 << (8 * 3);
	constexpr Bitboard Rank5 = Rank1 << (8 * 4);
	constexpr Bitboard Rank6 = Rank1 << (8 * 5);
	constexpr Bitboard Rank7 = Rank1 << (8 * 6);
	constexpr Bitboard Rank8 = Rank1 << (8 * 7);

	uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];
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

	constexpr File file_of(Square s) { return File(s & 7); }
	constexpr Rank rank_of(Square s) { return Rank(s >> 3); }

	Bitboard attacks[PIECE_TYPE_NB - 1][SQUARE_NB];
	Bitboard Pawn_attacks[COLOR_NB][SQUARE_NB];

	template<typename T1 = Square>
	inline int distance(Square x, Square y);
	template<>
	inline int distance<File>(Square x, Square y) { return abs(file_of(x) - file_of(y)); }
	template<>
	inline int distance<Rank>(Square x, Square y) { return abs(rank_of(x) - rank_of(y)); }
	template<>
	inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }

	Bitboard destination(Square ori, int step) {
		Square dest = Square(ori + step);
		return is_ok(dest) && distance(ori, dest) <= 2 ? sq_bb(dest) : Bitboard(0);
	}
	Bitboard sliding_attacks_init(Piece_type pt, Square s) {
		Bitboard attacks(0);
		const Direction *dir = (pt == ROOK ? dir_straight : dir_diagonal);
		for (int i = 0; i < 4; i++) {
			Square sq = s;
			while (destination(sq, dir[i])) {
				attacks |= sq_bb(sq += dir[i]);
			}
		}

		return attacks;
	}

	template<Direction Dir>
	constexpr Bitboard shift(Bitboard b) {
		return Dir == U ? b << 8
			: Dir == D ? b >> 8
			: Dir == UU ? b << 16
			: Dir == DD ? b >> 16
			: Dir == R ? (b & ~FileH) << 1
			: Dir == L ? (b & ~FileA) >> 1
			: Dir == UL ? (b & ~FileA) << 7
			: Dir == UR ? (b & ~FileH) << 9
			: Dir == DL ? (b & ~FileA) >> 9
			: Dir == DR ? (b & ~FileH) >> 7
			: 0;
	}

	template<Color C>
	constexpr Bitboard pawn_attacks(Bitboard b) {
		return C == WHITE ? shift<UL>(b) | shift<UR>(b)
			: shift<DL>(b) | shift<DR>(b);
	}

	const static Move CASTLING_rook[COLOR_NB][CASTLING_TYPE_NB]{
		{Move(WHITE, ROOK, MOVE, H1,  F1), Move(WHITE, ROOK, MOVE, A1,  D1)},
		{Move(WHITE, ROOK, MOVE, H8,  F8), Move(WHITE, ROOK, MOVE, A8,  D8)}
	};

	constexpr int knight_steps[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
	constexpr int king_steps[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };
	constexpr Bitboard castlingCheck[COLOR_NB][2]{
		{{sq_bb(B1) | sq_bb(C1) | sq_bb(D1)}, {sq_bb(F1) | sq_bb(G1)}},
		{{sq_bb(B8) | sq_bb(C8) | sq_bb(D8)}, {sq_bb(F8) | sq_bb(G8)}}
	};

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
			byColorBB[WHITE] = byColorBB[BLACK] = Bitboard(0);
			byTypeBB[QUEEN] = byTypeBB[ROOK] = byTypeBB[BISHOP] = byTypeBB[KNIGHT] = byTypeBB[KING] = byTypeBB[PAWN] = Bitboard(0);
			existBB = Bitboard(0);
			turn = WHITE, myColor = c;
			prevMove = Move();
			move_king[WHITE] = move_king[BLACK] = false;
			move_rook[WHITE][0] = move_rook[WHITE][1] = move_rook[BLACK][0] = move_rook[BLACK][1] = false;

			for (Square s = A1; s <= H8; ++s) board[s] = NOPIECE;
			for (int i = 0; i < 8; i++) {
				create_piece(WHITE, init_positions[i], Square(A1 + i));
				create_piece(BLACK, init_positions[i], Square(A8 + i));
				create_piece(WHITE, PAWN, Square(A2 + i));
				create_piece(BLACK, PAWN, Square(A7 + i));
			}

			calAttackBB();
		}
		void init(Color c) {
			for (Square s1 = A1; s1 <= H8; ++s1)
				for (Square s2 = A1; s2 <= H8; ++s2)
					SquareDistance[s1][s2] = max(distance<File>(s1, s2), distance<Rank>(s1, s2));

			for (Square s = A1; s <= H8; ++s) {
				Pawn_attacks[WHITE][s] = pawn_attacks<WHITE>(sq_bb(s));
				Pawn_attacks[BLACK][s] = pawn_attacks<BLACK>(sq_bb(s));

				for (int step : king_steps)
					attacks[KING][s] |= destination(s, step);

				for (int step : knight_steps)
					attacks[KNIGHT][s] |= destination(s, step);

				attacks[BISHOP][s] = sliding_attacks_init(BISHOP, s);
				attacks[ROOK][s] = sliding_attacks_init(ROOK, s);
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
		vector<Move>* pawn(Color c, Square ori) {
			vector<Move>* moves = new vector<Move>;
			Bitboard ori_bb = sq_bb(ori);
			Bitboard dest_bb = shift<U>(ori_bb);
			Bitboard promotion_rank = (c == WHITE ? Rank8 : Rank1);

			// Normal move
			if (!(existBB & dest_bb)) {
				append(moves, Move(c, PAWN, MOVE, ori, ori + U));
				if (dest_bb & promotion_rank)
					for (Piece_type promotion : promotion_list)
						append(moves, Move(c, PAWN, MOVE_PRO, ori, ori + U, promotion));
				if (!(existBB & shift<UU>(ori_bb)))
					append(moves, Move(c, PAWN, MOVE, ori, ori + UU));
			}

			// Attack move 
			Direction dir = (c == WHITE ? U : D);
			Square king_side = ori + Direction(dir + R), queen_side = ori + Direction(dir + L);
			auto attack = [&](Square take) -> void {
				if (is_ok(take) && (sq_bb(take) & byColorBB[!c])) {
					append(moves, Move(c, PAWN, CAPTURE, ori, take, take));
					if (sq_bb(take) & promotion_rank)
						for (Piece_type promotion : promotion_list)
							append(moves, Move(c, PAWN, CAPTURE_PRO, ori, take, take, promotion));
				}
				};
			attack(king_side); attack(queen_side);

			// En_passant attack
			int EnP_dir = en_passant_check(c, ori);
			if (EnP_dir != 0)
				append(moves, Move(c, PAWN, CAPTURE, ori, Square(ori + dir + EnP_dir), Square(ori + EnP_dir)));

			return moves;
		}
		bool castling_check(Color c, int side) {
			if (move_king[c] || move_rook[side]) return false;
			if ((existBB | byAttackBB[!c]) & castlingCheck[c][side]) return false;
			return true;
		}
		int en_passant_check(Color c, Square s) {
			Bitboard en_passant_rank = (c == WHITE ? Rank5 : Rank4);
			if (prevMove.piece != PAWN || (sq_bb(s) & en_passant_rank)) return 0;

			Square k_ori = s + (c == WHITE ? UUR : DDR), k_dest = s + R, 
				q_ori = s + (c == WHITE ? UUL : DDL), q_dest = s + L;

			if (k_ori == prevMove.ori && k_dest == prevMove.dest) return +1;
			else if (q_ori == prevMove.ori && q_dest == prevMove.dest) return -1;
			return 0;
		}

		vector<Move>* legal_moves(Square s) {
			return board[s] == QUEEN ? queen(s)
				: board[s] == ROOK ? rook(s)
				: board[s] == BISHOP ? bishop(s)
				: board[s] == KNIGHT ? knight(s)
				: board[s] == KING ? king(s)
				: board[s] == PAWN ? pawn(byTypeBB[board[s]] & byColorBB[WHITE] ? WHITE : BLACK, s)
				: nullptr;
		}
		vector<Move>* candidate_moves(Color c) {
			vector<Move>* candidate_moves = new vector<Move>;
			for (Square s = A1; s <= H8; ++s) {
				if (sq_bb(s) & byColorBB[c]) {
					vector<Move>* moves = legal_moves(s);
					candidate_moves->insert(candidate_moves->end(), moves->begin(), moves->end());
				}
			}

			cout << "Candidate Moves size = " << candidate_moves->size() << "\n";
			for (int i = 0; i < candidate_moves->size(); i++) {
				cout << sq_notation(candidate_moves->at(i).ori) << " -> " << sq_notation(candidate_moves->at(i).ori)
					<< ", " << sq_notation(candidate_moves->at(i).take) << ", " << candidate_moves->at(i).promotion << "\n";
				
			}
			return candidate_moves;
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
				if (existBB & sq_bb(s)) {
					Color c = (byColorBB[WHITE] & sq_bb(s)) ? WHITE : BLACK;
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
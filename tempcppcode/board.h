#include "utility.h"

using namespace std;

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

	extern Bitboard attacks[PIECE_TYPE_NB][SQUARE_NB];
	extern Bitboard Pawn_attacks[COLOR_NB][SQUARE_NB];

	inline Bitboard& operator|=(Bitboard& b, Square s) { return b |= to_board(s); }

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

	Bitboard sliding_attack(Piece_type pt, Square s, Bitboard occupied);

	class Board {
	private:
		Bitboard board[COLOR_NB][PIECE_TYPE_NB], exist;
		Color turn = WHITE;

		template<Color C>
		void create_piece(Piece_type p, Square s) { exist |= (board[C][p] |= to_board(s)); }
		void init_position();
		void init();

	public:
		Board() { init(); }
		void print();
	};
}
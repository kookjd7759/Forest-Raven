#include <iostream>
#include <cassert>
#include <array>

using namespace std;

namespace ForestRaven {
	using Bitboard = uint64_t;
	using Bitmove = uint32_t;

	enum Color : int {
		WHITE, BLACK,
		COLOR_NB
	};

	enum Piece_type : int {
		QUEEN, ROOK, BISHOP, KNIGHT, PAWN, KING,
		PIECE_TYPE_NB
	};
	constexpr char pt_char[PIECE_TYPE_NB] = { 'Q', 'R', 'B', 'N', 'P', 'K'};
	constexpr Piece_type init_positions[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

	enum Direction : int {
		UP = 8,
		RIGHT = 1,
		DOWN = -UP,
		LEFT = -RIGHT,

		UP_LEFT = UP + LEFT,
		UP_RIGHT = UP + RIGHT,
		DOWN_LEFT = DOWN + LEFT,
		DOWN_RIGHT = DOWN + RIGHT
	};
	constexpr array<Direction, 4> dir_straight = { UP, RIGHT, DOWN, LEFT };
	constexpr array<Direction, 4> dir_diagonal = { UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };

	enum Square : int {
		A1, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8,
		SQUARE_NB
	};
	inline static void operator++(Square& s) { s = Square(int(s) + 1); }
	inline Square& operator+=(Square& s, Direction d) { return s = Square(s + d); }
	constexpr bool is_boundary(Square s) { return s >= A1 && s <= H8; }
	constexpr Bitboard to_board(Square s) { assert(is_boundary(s)); return (1ULL << s); }


}
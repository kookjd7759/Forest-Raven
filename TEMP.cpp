#include <iostream>

enum Color {
	WHITE, BLACK
};

enum Piece_type {
	QUEEN, ROOK, BISHOP, KNIGHT, PAWN, KING,
	PIECE_TYPE_NB
};

namespace ForestRaven {
	using Bitboard = uint64_t;

	class Board {
		Bitboard pieces[2][PIECE_TYPE_NB], kingPosition[2];

	};
}

int main() {

}
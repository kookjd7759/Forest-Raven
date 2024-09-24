#include "board.h"

namespace ForestRaven {
	// we represent move order in 32bit integer
	// FORMAT is next
	// 
	struct Move {
		Bitmove move;
		Move(Bitmove m) : move(m) {}
		Color get_color() {return }
	};

	class Chess {
	private:
		void move(const Move& move);
		void capture(const Move& move);
		void castling(const Move& move);
		void promotion(const Move& move);

	public:
		Board board;
		Color myColor;
		Chess() { int c; cin >> c; myColor = Color(c); }
	};
}
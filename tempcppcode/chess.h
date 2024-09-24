#include "board.h"

namespace ForestRaven {

	class Chess {
	private:

	public:
		Board board;
		Color myColor;
		Chess() { int c; cin >> c; myColor = Color(c); }
	};
}
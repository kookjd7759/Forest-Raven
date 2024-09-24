#include "utility.h"

namespace ForestRaven {
	constexpr Bitmove mask31 = (0b1 << 31);
	constexpr Bitmove mask30_28 = (0b111 << 28);
	constexpr Bitmove mask27 = (0b1 << 27);
	constexpr Bitmove mask26_24 = (0b111 << 24);
	constexpr Bitmove mask23_18 = (0b111111 << 18);
	constexpr Bitmove mask17_10 = (0b111111 << 12);
	constexpr Bitmove mask9_8 = (0b11 << 10);

	// we represent Move order in 32bit integer. FORMAT is next
	// MEAN [valid][move type][Color][piece type][ori Square][dest Square][Promotion type] .. TODO
	// BIT  [1bit ][3 bit    ][1bit ][3bit      ][6bit      ][6bit       ][2bit          ] ..
	struct Move {
		Bitmove move = 0;
		Move() {}
		Move(Move_type mt, Color c, Piece_type pt, Square ori, Square dest, Piece_type prot = QUEEN) {
			move |= mask31 | mt | (c << 27) | (pt << 24) | (ori << 18) | (dest << 12) | (prot << 10); 
		}

		Color      color() const { return ((move & mask27) ? BLACK : WHITE); }
		Piece_type piece() const { return Piece_type((move & mask26_24) >> 24); }
		Move_type  type()  const { return Move_type(move & mask30_28); }
		Square  ori()  const { return Square((move & mask23_18) >> 18); }
		Square  dest()  const { return Square((move & mask17_10) >> 12); }
	};
}

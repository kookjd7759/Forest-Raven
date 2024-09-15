#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <random>

using namespace std;

enum Color {
    NOCOLOR = -1,
    WHITE,
    BLACK
};
const char colorToChar[3] = { 'w', 'b', '-' };

enum Move_type {
    NOMOVE = -1,
    MOVE,
    CAPTURE,
    MOVE_PRO,
    CAPTURE_PRO,
    CASTLING
};

enum Piece_type {
    NOPIECE = -1,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    KING,
    PAWN,
};
const char typeToChar[7] = { 'Q', 'R', 'B', 'N', 'K', 'P', '-' };

struct Position {
    int x = -1, y = -1;

    Position() {}
    Position(int x, int y) : x(x), y(y) {}

    bool operator<(const Position& other) const { return (x != other.x ? x < other.x : y < other.y); }
    bool operator>(const Position& other) const { return (x != other.x ? x > other.x : y > other.y); }
    Position operator+(const Position& other) const { return Position(x + other.x, y + other.y); }
    Position& operator+=(const Position& other) { x += other.x, y += other.y; return *this; }
    bool operator==(const Position& other) const { return bool(x == other.x && y == other.y); }
    bool operator!=(const Position& other) const { return bool(x != other.x || y != other.y); }
};

const Position dir_straight[4]{ Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) };
const Position dir_diagonal[4]{ Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) };
const Position dir_all[8]{ Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) };
const Position dir_knight[8]{ Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2) };
const Piece_type promotion_list[4] = { QUEEN, ROOK, BISHOP, KNIGHT, };

bool notationCheck(const string& st);
Position convertPos(const string& st);
string convertPos(const Position& pos);
string to_notation(const Position& pos);
int get_random(const int& a, const int& b);

#endif // UTILITY_H
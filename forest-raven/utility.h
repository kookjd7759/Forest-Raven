#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <random>

#define NULL_POS Position(-1, -1)

using namespace std;

enum Type {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NOTYPE
};

enum Color {
    WHITE,
    BLACK,
    NOCOLOR
};

struct Position {
    int x, y;

    Position() { x = -1, y = -1; }
    Position(int x, int y) : x(x), y(y) {}

    bool operator<(const Position& other) const {
        if (x < other.x) return true;
        if (x > other.x) return false;
        return y < other.y;
    }
    bool operator>(const Position& other) const {
        if (x > other.x) return true;
        if (x < other.x) return false;
        return y > other.y;
    }
    Position operator+(const Position& other) const { return Position(x + other.x, y + other.y); }
    Position& operator+=(const Position& other) {
        x += other.x, y += other.y;
        return *this;
    }
    bool operator==(const Position& other) const { return bool(x == other.x && y == other.y); }
};

const Position dir_straight[4]{ Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) };
const Position dir_diagonal[4]{ Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) };
const Position dir_all[8]{ Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) };
const Position dir_knight[8]{ Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2) };

const char typeToChar[7] = { 'P', 'N', 'B', 'R', 'Q', 'K', '-' };
const char colorToChar[3] = { 'w', 'b', '-' };

bool notationCheck(const string& st);
Position convertPos(const string& st);
string convertPos(const Position& pos);
int get_random(const int& a, const int& b);

#endif // UTILITY_H
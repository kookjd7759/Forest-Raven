#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

using namespace std;

struct Position {
    int x, y;

    Position(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator<(const Position& other) const {
        if (x < other.x) return true;
        if (x > other.x) return false;
        return y < other.y;
    }

    Position operator+(const Position& other) const { return Position(x + other.x, y + other.y); }
    bool operator==(const Position& other) const { return bool(x == other.x && y == other.y); }
};

bool notationCheck(const string& st);
Position convertPos(const string& st);
string convertPos(const Position& pos);

#endif // UTILITY_H
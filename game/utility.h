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

const Position dir_straight[4]{ {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
const Position dir_diagonal[4]{ {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };
const Position dir_knight[8]{ {1, 2}, {-1, 2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}, {1, -2}, {-1, -2} };
const Position dir_king[8]{ {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} };
const Position rookPos[4]{ {7, 0}, {0, 0}, {7, 7}, {0, 7} }; // wk, wq, bk, bq

bool notationCheck(const string& st);
Position convertPos(const string& st);
string convertPos(const Position& pos);

#endif // UTILITY_H
#include "utility.h"

bool notationCheck(const string& st) {
    return (st.size() == 2 && (st[0] >= 'a' && st[0] <= 'h') && (st[1] >= '1' && st[1] <= '8'));
}
Position convertPos(const string& st) {
    return Position(st[0] - 'a', st[1] - '1');
}
string convertPos(const Position& pos) {
    string st = "";
    st += pos.x + 'a';
    st += pos.y + '1';
    return st;
}
string to_notation(const Position& pos) {
    string notation = "";
    notation += (pos.x + 'a');
    notation += (pos.y + '1');
    return notation;
}
int get_random(const int& a, const int& b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> rand(a, b);

    return rand(gen);
}
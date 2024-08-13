#include <iostream>
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

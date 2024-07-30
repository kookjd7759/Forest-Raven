#include <bits/stdc++.h>

using namespace std;

struct Position {
    int x, y;
};

const bool PosVecFind(vector<Position>* vec, Position pos) {
    for (const auto iter : *vec)
        if (iter.x == pos.x && iter.y == pos.y)
            return true;
    return false;
}

class Game {
private:
    enum Type {
        EMPTY,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    char typeToChar[7] = { '-', 'P', 'N', 'B', 'R', 'Q', 'K' };

    enum Team {
        NONE,
        WHITE,
        BLACK,
    };
    char teamToChar[3] = { '-', 'w', 'b' };

    const bool boundaryCheck(const Position pos) {
        return ((pos.x < 0 || pos.x > 7) || (pos.y < 0 || pos.y > 7)) ? false : true;
    }

    const bool isEnemy(const Position a, const Position b) {
        return ((board[a.y][a.x].team != NONE && board[b.y][b.x].team != NONE) &&
            board[a.y][a.x].team != board[b.y][b.x].team);
    }

    const bool isAlly(const Position a, const Position b) {
        return ((board[a.y][a.x].team != NONE && board[b.y][b.x].team != NONE) &&
            board[a.y][a.x].team == board[b.y][b.x].team);
    }

    const bool isEmpty(const Position pos) {
        return (board[pos.y][pos.x].team == NONE ? true : false);
    }

    class Square {
    public:
        Type type = EMPTY;
        Team team = NONE;
    };

    Square** board = new Square * [8];


    
    void move(const Position now, const Position next) {
        board[next.y][next.x].team = board[now.y][now.x].team;
        board[next.y][next.x].type = board[now.y][now.x].type;
        board[now.y][now.x].team = NONE, board[now.y][now.x].type = EMPTY;
    }

    vector<Position> move_pawn_b(const Position pos) {
        cout << "pawn_b !\n";
        vector<Position> vec;

        // Plain move
        if (isEmpty({ pos.x, pos.y - 1 })) {
            vec.push_back({ pos.x, pos.y - 1 });
            // First move
            if (pos.y == 6) vec.push_back({ pos.x, pos.y - 2 });
        }

        // Attack
        if (boundaryCheck({ pos.x - 1, pos.y - 1 }) && isEnemy(pos, { pos.x - 1, pos.y - 1 }))
            vec.push_back({ pos.x - 1, pos.y - 1 });

        if (boundaryCheck({ pos.x + 1, pos.y - 1 }) && isEnemy(pos, { pos.x + 1, pos.y - 1 }))
            vec.push_back({ pos.x + 1, pos.y - 1 });

        return vec;
    }

    vector<Position> move_pawn_w(const Position pos) {
        cout << "pawn_w !\n";
        vector<Position> vec;

        // Plain move
        if (isEmpty({ pos.x, pos.y + 1 })) {
            vec.push_back({ pos.x, pos.y + 1 });
            // First move
            if (pos.y == 1) vec.push_back({ pos.x, pos.y + 2 });
        }

        // Attack
        if (boundaryCheck({ pos.x + 1, pos.y + 1 }) && isEnemy(pos, { pos.x + 1, pos.y + 1 }))
            vec.push_back({ pos.x + 1, pos.y + 1 });

        if (boundaryCheck({ pos.x - 1, pos.y + 1 }) && isEnemy(pos, { pos.x - 1, pos.y + 1 }))
            vec.push_back({ pos.x - 1, pos.y + 1 });

        return vec;
    }

    vector<Position> move_knight(const Position pos) {
        cout << "knight !\n";
        vector<Position> vec;
        Position posList[8] = {
            {pos.x + 1, pos.y + 2},
            {pos.x - 1, pos.y + 2},
            {pos.x + 2, pos.y + 1},
            {pos.x - 2, pos.y + 1},
            {pos.x + 2, pos.y - 1},
            {pos.x - 2, pos.y - 1},
            {pos.x + 1, pos.y - 2},
            {pos.x - 1, pos.y - 2}
        };

        for (int i = 0; i < 8; i++){
            if (boundaryCheck(posList[i]) && !isAlly(pos, posList[i]))
                vec.push_back(posList[i]);
        }

        return vec;
    }

    vector<Position> move_rook(const Position pos) {
        cout << "rook !\n";
        vector<Position> vec;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos)) return false;

            if (isAlly(pos, nextPos))
                return false;
            else {
                vec.push_back(nextPos);
                if (isEmpty(nextPos))
                    return true;
                else return false;
            }
            };

        Position nextPos;
        cout << "R\n";
        nextPos = pos;
        while (true) {
            nextPos.x++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        return vec;
    }

    vector<Position> move_bishop(const Position pos) {
        cout << "bishop !\n";
        vector<Position> vec;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos)) return false;

            if (isAlly(pos, nextPos))
                return false;
            else {
                vec.push_back(nextPos);
                if (isEmpty(nextPos))
                    return true;
                else return false;
            }
            };

        Position nextPos;

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        return vec;
    }

    vector<Position> move_queen(const Position pos) {
        cout << "queen !\n";
        vector<Position> vec;
        auto check = [&](Position nextPos) -> bool {
            if (!boundaryCheck(nextPos)) return false;

            if (isAlly(pos, nextPos))
                return false;
            else {
                vec.push_back(nextPos);
                if (isEmpty(nextPos))
                    return true;
                else return false;
            }
            };

        Position nextPos = pos;
        nextPos = pos;
        while (true) {
            nextPos.x++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x++;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y++;
            if (!check(nextPos)) break;
        }

        nextPos = pos;
        while (true) {
            nextPos.x--;
            nextPos.y--;
            if (!check(nextPos)) break;
        }

        return vec;
    }

    vector<Position> move_king(const Position pos) {
        cout << "king !\n";
        vector<Position> vec;
        Position posList[8] = {
            {pos.x, pos.y + 1},
            {pos.x + 1, pos.y + 1},
            {pos.x + 1, pos.y},
            {pos.x + 1, pos.y - 1},
            {pos.x, pos.y - 1},
            {pos.x - 1, pos.y - 1},
            {pos.x - 1, pos.y},
            {pos.x - 1, pos.y + 1}
        };

        for (int i = 0; i < 8; i++) {
            if (boundaryCheck(posList[i]) && !isAlly(pos, posList[i]))
                vec.push_back(posList[i]);
        }

        return vec;
    }


    bool convertCheck(const string st) {
        if (st.size() == 2) {
            if ((st[0] >= 'a' && st[0] <= 'h') &&
                (st[1] >= '1' && st[1] <= '8')) {
                return true;
            }
        }
        return false;
    }

    Position convertPos(const string st) {
        Position pos{ st[0] - 'a', st[1] - '1' };
        return pos;
    }

    string convertPos(const Position pos) {
        string st = "";
        st += pos.x + 'a';
        st += pos.y + '1';
        return st;
    }


public:
    Game() : board(new Square* [8]) {
        for (int i = 0; i < 8; ++i) 
            board[i] = new Square[8];
        init();
    }

    ~Game() {
        for (int i = 0; i < 8; ++i) 
            delete[] board[i];
        delete[] board;
    }

    void reset() {
        Type list[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

        for (int i = 0; i < 8; i++) {
            board[0][i].type = list[i], board[0][i].team = WHITE;
            board[1][i].type = PAWN, board[1][i].team = WHITE;

            board[7][i].type = list[i], board[7][i].team = BLACK;
            board[6][i].type = PAWN, board[6][i].team = BLACK;
        }

        for (int i = 2; i < 6; i++) for (int j = 0; j < 8; j++) {
            board[i][j].type = EMPTY, board[i][j].team = NONE;
        }
    }

    void init() {
        reset();
    }

    void print_board(bool isW) {
        if (isW) {
            for (int i = 7; i >= 0; i--) {
                for (int j = 0; j < 8; j++) {
                    cout << teamToChar[board[i][j].team] << typeToChar[board[i][j].type] << " ";
                }
                cout << "\n";
            }
            for (int i = 0; i < 8; i++)
                cout << (char)('a' + i) << "  ";
            cout << "\n\n";
        }
        else {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    cout << teamToChar[board[i][j].team] << typeToChar[board[i][j].type] << " ";
                }
                cout << "\n";
            }
            for (int i = 7; i >= 0; i--)
                cout << (char)('a' + i) << "  ";
            cout << "\n\n";
        }
    }



    void moveCommand() {
        while (true) { // for legal move
            Position now, next;
            while (true) { // for correct Input
                cout << "select piece : ";
                string st; cin >> st;
                if (convertCheck(st)) {
                    now = convertPos(st);
                    break;
                }
                cout << "Notation Error, Enter position again\n";
            }

            vector<Position> vec;
            switch (board[now.y][now.x].type) {
            case NONE: {
                cout << "There is no piece, Enter position again\n";
                continue;
            }
            case PAWN: {
                if (board[now.y][now.x].team == BLACK)
                    vec = move_pawn_b(now);
                else
                    vec = move_pawn_w(now);
                break;
            }
            case KNIGHT: vec = move_knight(now); break;
            case BISHOP: vec = move_bishop(now); break;
            case ROOK: vec = move_rook(now); break;
            case QUEEN: vec = move_queen(now); break;
            case KING: vec = move_king(now); break;
            default: {
                cout << "I don't know, Enter position again\n";
                continue;
            }
            }

            if (vec.size() == 0) {
                cout << "There is no legal move, Enter position again\n";
                continue;
            }

            cout << "Legal move\n";
            for (const auto iter : vec) {
                cout << convertPos(iter) << " ";
            }
            cout << "\n";

            while (true) { // for correct Input
                cout << "Enter square : ";
                string st; cin >> st;
                if (convertCheck(st)) {
                    next = convertPos(st);
                    break;
                }
                cout << "Notation Error, Enter position again\n";
            }

            if (PosVecFind(&vec, next)) {
                move(now, next);
                break;
            }
            else cout << "It's not legal move, Enter position again\n";
        }
    }
};

int main() {
    Game* chess = new Game();
    while (true) {
        chess->print_board(true);
        chess->moveCommand();
    }
}
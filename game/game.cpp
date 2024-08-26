#include <set>
#include <windows.h>
#include "utility.h"

class Game {
private:
    enum Type {
        NOTYPE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    const char typeToChar[7] = { '-', 'P', 'N', 'B', 'R', 'Q', 'K' };

    enum Team {
        NOTEAM,
        WHITE,
        BLACK,
    };
    const char teamToChar[3] = { '-', 'w', 'b' };

    const bool boundaryCheck(const Position& pos) const {
        return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true;
    }

    const bool isEnemy(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].team != NOTEAM && board[b.y][b.x].team != NOTEAM) &&
            (board[a.y][a.x].team != board[b.y][b.x].team));
    }

    const bool isAlly(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].team != NOTEAM && board[b.y][b.x].team != NOTEAM) &&
            board[a.y][a.x].team == board[b.y][b.x].team);
    }

    const bool isEmpty(const Position& pos) const {
        return board[pos.y][pos.x].team == NOTEAM;
    }

    class Square {
    public:
        Type type = NOTYPE;
        Team team = NOTEAM;
        int attack_wb[2]{ 0, 0 };
    };

    Square** board = new Square *[8];
    Position prevMove[2];
    bool isWhiteTurn, isPlayerWhite, castlingMoveCheck_wb_KQ[2][2];
    Position kingPos_wb[2];
    int isCheck_wb[2]; // 0:no, 1:check, 2:mate



    int en_passant_check(const Position& pos, const bool& isWhite) const {
        if (pos.y != (isWhite ? 4 : 3)) // pawn is not in rank 5
            return false;

        Position side_KQ[2][2];
        int dir = isWhite ? +2 : -2;
        for (int i = 0, kq = 1; i < 2; i++, kq -= 2) {
            side_KQ[i][0] = { pos.x + kq, pos.y + dir };
            side_KQ[i][1] = { pos.x + kq, pos.y };
        }

        if (side_KQ[0][0] == prevMove[0] && side_KQ[0][1] == prevMove[1]) return 1;
        else if (side_KQ[1][0] == prevMove[0] && side_KQ[1][1] == prevMove[1]) return -1;
        return 0;
    }

    bool castling_check(const bool& isWhite, const bool& isKingSide) {
        int wb = isWhite ? 0 : 1, kq = isKingSide ? 0 : 1, rank = isWhite ? 0 : 7;
        bool ch1, ch2, ch3;
        ch1 = castlingMoveCheck_wb_KQ[wb][kq];
        ch2 = isKingSide ? board[rank][5].type == NOTYPE && board[rank][6].type == NOTYPE :
            board[rank][3].type == NOTYPE && board[rank][2].type == NOTYPE && board[rank][1].type == NOTYPE;
        
        int op = isWhite ? 1 : 0;
        ch3 = isKingSide ? board[rank][5].attack_wb[op] == 0 && board[rank][6].attack_wb[op] == 0 :
            board[rank][3].attack_wb[op] == 0 && board[rank][2].attack_wb[op] == 0 && board[rank][1].attack_wb[op] == 0;
        
        return ch1 && ch2 && ch3;
    }

    void promotion(const Position& pos, const Team& team) {
        board[pos.y][pos.x].team = team;
        int input; cin >> input;
        switch (input) {
        case 0: board[pos.y][pos.x].type = QUEEN; break;
        case 1: board[pos.y][pos.x].type = ROOK; break;
        case 2: board[pos.y][pos.x].type = BISHOP; break;
        case 3: board[pos.y][pos.x].type = KNIGHT; break;
        default: board[pos.y][pos.x].type = QUEEN; break;
        }
    }



    bool thisMoveCheckFree(const Position& now, const Position& dest, const bool& isWhite) {
        bool ret = false;

        Square dest_copy = board[dest.y][dest.x];
        move_piece(now, dest);
        Position king = kingPos_wb[isWhite ? 0 : 1];
        cal_attackSquare();
        if (board[king.y][king.x].attack_wb[isWhite ? 1 : 0] == 0) ret = true;
        move_piece(dest, now);
        board[dest.y][dest.x] = dest_copy;
        cal_attackSquare();
        return ret;
    }

    void insert_withCheck(set<Position>& s, const Position& pos, const Position& target, const bool& isWhite) {
        if (thisMoveCheckFree(pos, target, isWhite))
            s.insert(target);
    }

    void update_isCheck() { // 0.no 1.check 2.mate
        for (int wb = 0, op = 1; wb < 2; wb++, op--) {
            Position king = kingPos_wb[wb];
            if (board[king.y][king.x].attack_wb[op] == 0)
                isCheck_wb[wb] = 0;
            else {
                set<pair<Position, Position>> move_1 = get_candidate_move(wb == 0 ? true : false);
                bool mate = move_1.empty();
                isCheck_wb[wb] = mate ? 2 : 1;
            }
        }
    }



    bool move_check(set<Position>& s, const Position& cur , const Position& next, const bool& legalMove, const bool& isWhite) {
        if (!boundaryCheck(next) || (legalMove && isAlly(cur, next))) return false;

        if (legalMove) insert_withCheck(s, cur, next, isWhite);
        else s.insert(next);

        if (isEmpty(next))
            return true;
        else return false;
    }

    void pawnAttack_move(set<Position>& s, const Position& now, const bool& legalMove, const bool& isWhite) {
        int dir = (isWhite ? +1 : -1);
        auto func = [&](Position next) -> void {
            if (boundaryCheck({ next.x, next.y })) {
                if (legalMove) {
                    if (isEnemy(now, next))
                        insert_withCheck(s, now, next, isWhite);
                }
                else s.insert(next);
            }
            };
        func(Position(now.x + 1, now.y + dir));
        func(Position(now.x - 1, now.y + dir));
    }

    void pawn_move(set<Position>& s, const Position& pos, const bool& isWhite) {
        int dir = (isWhite ? +1 : -1);
        
        // Plain move
        Position one(pos.x, pos.y + dir);
        if (isEmpty(one)) {
            insert_withCheck(s, pos, one, isWhite);
            
            if (pos.y == (isWhite ? 1 : 6)) { // First move
                Position two(pos.x, pos.y + dir * 2);
                if (isEmpty(two))
                    insert_withCheck(s, pos, two, isWhite);
            }
        }

        // Attack move
        pawnAttack_move(s, pos, true, isWhite);

        // En_passant Check
        int en = en_passant_check(pos, isWhite);
        if (en != 0)
            insert_withCheck(s, pos, Position(pos.x + en, pos.y + dir), isWhite);
    }

    void straight_move(set<Position>& s, const Position& pos, const bool& legalMove, const bool& isWhite) {
        for (int i = 0; i < 4; i++) {
            Position next = pos;
            while (true) {
                next = next + dir_straight[i];
                if (!move_check(s, pos, next, legalMove, isWhite)) break;
            }
        }
    }

    void diagonal_move(set<Position>& s, const Position& pos, const bool& legalMove, const bool& isWhite) {
        for (int i = 0; i < 4; i++) {
            Position next = pos;
            while (true) {
                next = next + dir_diagonal[i];
                if (!move_check(s, pos, next, legalMove, isWhite)) break;
            }
        }
    }

    void knight_move(set<Position>& s, const Position& pos, const bool& legalMove, const bool& isWhite) {
        for (int i = 0; i < 8; i++) {
            Position next = pos + dir_knight[i];
            if (boundaryCheck(next)) {
                if (legalMove) {
                    if (!isAlly(pos, next))
                        insert_withCheck(s, pos, next, isWhite);
                }
                else s.insert(next);
            }
        }
    }

    void king_move(set<Position>& s, const Position& pos, const bool& legalMove, const bool& isWhite) {
        for (int i = 0; i < 8; i++) {
            Position next = pos + dir_king[i];
            if (boundaryCheck(next)) {
                if (legalMove) {
                    if (!isAlly(pos, next))
                        insert_withCheck(s, pos, next, isWhite);
                }
                else s.insert(next);
            }
        }

        // Castling
        if (isCheck_wb[isWhite ? 0 : 1] == 0) {
            if (castling_check(isWhite, true)) // King side
                s.insert(Position(pos.x + 2, pos.y));
            if (castling_check(isWhite, false)) // Queen side
                s.insert(Position(pos.x - 2, pos.y));
        }
    }

    void get_legalMove(set<Position>& s, const Position& pos) {
        bool isWhite = (board[pos.y][pos.x].team == WHITE);
        switch (board[pos.y][pos.x].type) {
        case NOTEAM: break;
        case PAWN: pawn_move(s, pos, isWhite); break;
        case KNIGHT: knight_move(s, pos, true, isWhite); break;
        case BISHOP: diagonal_move(s, pos, true, isWhite); break;
        case ROOK: straight_move(s, pos, true, isWhite); break;
        case QUEEN: straight_move(s, pos, true, isWhite); diagonal_move(s, pos, true, isWhite); break;
        case KING: king_move(s, pos, true, isWhite); break;
        default: break;
        }
    }

    set<pair<Position, Position>> get_candidate_move(const bool& isWhite) {
        set<pair<Position, Position>> s;
        Team team = isWhite ? WHITE : BLACK;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++) {
                if (board[y][x].team == team) {
                    set<Position> move; Position pos(x, y);
                    get_legalMove(move, pos);

                    for (const Position p : move)
                        s.insert(make_pair(pos, p));
                }
            }
        /*
        cout << "Candidate move [" << s.size() << "]\n";
        for (const pair<Position, Position> move : s)
            cout << convertPos(move.first) << " -> " << convertPos(move.second) << "\n";
            */
        return s;
    }



    void cal_attackSquare() {
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            board[y][x].attack_wb[0] = board[y][x].attack_wb[1] = 0;

        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            if (board[y][x].type != NOTYPE) { // there is piece
                set<Position> s; Position pos(x, y);
                bool isWhite = (board[y][x].team == WHITE);
                switch (board[pos.y][pos.x].type) {
                case NOTYPE: break;
                case PAWN: pawnAttack_move(s, pos, false, isWhite); break;
                case KNIGHT: knight_move(s, pos, false, isWhite); break;
                case BISHOP: diagonal_move(s, pos, false, isWhite); break;
                case ROOK: straight_move(s, pos, false, isWhite); break;
                case QUEEN: diagonal_move(s, pos, false, isWhite), straight_move(s, pos, false, isWhite); break;
                case KING: king_move(s, pos, false, isWhite); break;
                }

                int wb = isWhite ? 0 : 1;
                for (const Position p : s)
                    board[p.y][p.x].attack_wb[wb]++;
            }
        }
    }

    void move_piece(const Position& now, const Position& dest) {
        if (board[now.y][now.x].type == KING) // king position recording
            kingPos_wb[board[now.y][now.x].team == WHITE ? 0 : 1] = dest;

        board[dest.y][dest.x] = board[now.y][now.x];
        board[now.y][now.x].team = NOTEAM;
        board[now.y][now.x].type = NOTYPE;
    }

    void move(const Position& now, const Position& dest) {
        bool isWhite = board[now.y][now.x].team == WHITE;
        // Castling check
        if (board[now.y][now.x].type == KING) { // If move king, never do castes
            int wb = (board[now.y][now.x].team == WHITE ? 0 : 1);
            castlingMoveCheck_wb_KQ[wb][0] = castlingMoveCheck_wb_KQ[wb][1] = false;
            if (abs(now.x - dest.x) == 2) { // Castling move
                bool kingSide = dest.x > now.x;
                int index = wb * 2 + (kingSide ? 0 : 1);
                move_piece(rookPos[index], Position(dest.x + (kingSide ? -1 : +1), dest.y)); // move rook
            }
        }
        else if (board[now.y][now.x].type == ROOK) {
            for (int i = 0; i < 4; i++) {
                if (rookPos[i] == now) 
                    castlingMoveCheck_wb_KQ[i / 2][i % 2] = false;
            }
        }

        // En passant check
        if (board[now.y][now.x].type == PAWN && now.x != dest.x) { // PAWN takes something
            if (board[dest.y][dest.x].type == NOTYPE) { // En passant move
                int reverse_dir = (board[now.y][now.x].team == WHITE ? -1 : +1);
                board[dest.y + reverse_dir][dest.x].team = NOTEAM;
                board[dest.y + reverse_dir][dest.x].type = NOTYPE;
            }
        }

        // update board
        move_piece(now, dest);

        // Promotion check
        if (board[dest.y][dest.x].type == PAWN && (dest.y == 7 || dest.y == 0)) {
            promotion(dest, board[dest.y][dest.x].team);
        }

        // change turn
        isWhiteTurn = !isWhiteTurn;

        // update previous move 
        prevMove[0] = now; prevMove[1] = dest;

        cal_attackSquare();
        update_isCheck();
    }

public:
    Game() : board(new Square* [8]) {
        for (int i = 0; i < 8; i++) 
            board[i] = new Square[8];
        init();
    }

    ~Game() {
        for (int i = 0; i < 8; i++) 
            delete[] board[i];
        delete[] board;
    }

    void reset() {
        // piece init
        Type list[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            board[0][i].type = list[i], board[0][i].team = WHITE;
            board[1][i].type = PAWN, board[1][i].team = WHITE;

            board[7][i].type = list[i], board[7][i].team = BLACK;
            board[6][i].type = PAWN, board[6][i].team = BLACK;
        }
        for (int i = 2; i < 6; i++) for (int j = 0; j < 8; j++) 
            board[i][j].type = NOTYPE, board[i][j].team = NOTEAM;

        prevMove[0] = { -1, -1 }, prevMove[1] = { -1, -1 };
        isWhiteTurn = true; isPlayerWhite = true;
        for (int i = 0; i < 4; i++) castlingMoveCheck_wb_KQ[i / 2][i % 2] = true;
        kingPos_wb[0] = { 4, 0 }, kingPos_wb[1] = { 4, 7 };
        isCheck_wb[0] = 0, isCheck_wb[1] = 0;

        cal_attackSquare();
        update_isCheck();
    }
    
    void changeColor() {
        reset();
        isPlayerWhite = !isPlayerWhite;
    }

    void init() {
        reset();
    }

    char sendMateInfo() {
        // (1)White WIN (2)Black WIN
        if (isCheck_wb[1] == 2) return '1';
        else if (isCheck_wb[0] == 2) return '2';
        return '0';
    }

    void command() {
        int n; cin >> n;

        switch (n) {
        case 1: move_ME(); break;
        case 2: command_getLegalMove(); break;
        case 3: move_AI(); break;
        case 4: reset(); break;
        case 5: changeColor(); break;
        case 6: print_board(); break;
        case 7: print_attackData(); break;
        default: break;
        }
    }

    void move_ME() {
        string now, next; cin >> now >> next;
        move(convertPos(now), convertPos(next));
    }

    void command_getLegalMove() {
        string pos; cin >> pos;
        set<Position> s; get_legalMove(s, convertPos(pos));
        for (const Position p : s)
            cout << p.x << " " << p.y << " ";
        cout << "\n";
    }

    void move_AI() {
        set<pair<Position, Position>> s = get_candidate_move(!isPlayerWhite);
        if (!s.empty() && !(isCheck_wb[0] == 2 || isCheck_wb[1] == 2)) {
            auto iter = s.begin();
            int idx = get_random(0, s.size() - 1);
            iter = next(s.begin(), idx);
            move(iter->first, iter->second);
            cout << iter->first.x << " " << iter->first.y << " " << iter->second.x << " " << iter->second.y << "\n";
        }
        else cout << sendMateInfo() << " -1 -1 -1\n";
    }

    void start() {
        while (true) {
            command();
        }
    }

    // additional function
    void print_board() {
        cout << "-----------------------\n";
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) cout << teamToChar[board[i][j].team] << typeToChar[board[i][j].type] << " ";
            cout << "\n";
        }
        for (int i = 0; i < 8; i++) cout << (char)('a' + i) << "  ";
        cout << "\n-----------------------\n";
        cout << "[previous Move] : " << convertPos(prevMove[0]) << " -> " << convertPos(prevMove[1]) << "\n";
        cout << "[check White / Black] " << isCheck_wb[0] << " / " << isCheck_wb[1] << "\n";
    }

    void print_attackData() {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) cout << board[i][j].attack_wb[0] << "," << board[i][j].attack_wb[1] << "   ";
            cout << "\n\n";
        }
    }

};

int main() {
    Game* chess = new Game();
    chess->start();
}

/*
1 g2 g4
1 g4 g5
1 g5 g6
1 g6 h7
1 h7 g8 
2
6
*/
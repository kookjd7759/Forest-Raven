#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include "utility.h"

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

struct PreviousMove {
    Type type = NOTYPE;
    Position prev = Position(-1, -1), now = Position(-1, -1);

    void set(Type t, Position p, Position n) { type = t, prev = p, now = n; }
    void clear() { type = NOTYPE, prev = Position(-1, -1), now = Position(-1, -1); }
};

struct Piece {
    Type type;
    Color color;

    Piece(Type type, Color color) : type(type), color(color) {}
};

struct Square {
    Piece piece = Piece(NOTYPE, NOCOLOR);
    int attack_wb[2]{ 0, 0 };

    void set(Piece p) { piece = p; }
    bool isAttacked(Color color) const { return attack_wb[(color == WHITE ? BLACK : WHITE)] != 0 ? true : false; }
    bool empty() const { return piece.type == NOTYPE && piece.color == NOCOLOR; }
    void clear() { piece = Piece(NOTYPE, NOCOLOR); }
};

class Chess {
private:
    Position king_position_wb[2]{ Position(4, 0), Position(4, 7) };
    bool kr_moveCheck_wb_qk[2][2];
    PreviousMove prevMove;
    Square board[8][8];

    const bool boundaryCheck(const Position& pos) const { return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true; }
    const bool isAlly(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].piece.color != NOCOLOR && board[b.y][b.x].piece.color != NOCOLOR) &&
            board[a.y][a.x].piece.color == board[b.y][b.x].piece.color);
    }
    const bool isEnemy(const Position& a, const Position& b) const {
        return ((board[a.y][a.x].piece.color != NOCOLOR && board[b.y][b.x].piece.color != NOCOLOR) &&
            board[a.y][a.x].piece.color != board[b.y][b.x].piece.color);
    }
    const bool isCheck(const Color& color) const { return board[king_position_wb[color].y][king_position_wb[color].x].isAttacked(color); }

    const bool isThisMoveLegal(const Position& cur, const Position& dest, const Position& take = Position(-1, -1)) {
        Color color = board[cur.y][cur.x].piece.color;
        Piece temp_destPiece = board[dest.y][dest.x].piece, temp_takePiece = Piece(NOTYPE, NOCOLOR);
        if (take.x != -1) {
            temp_takePiece = board[take.y][take.x].piece;
            board[take.y][take.x].clear();
        }

        move_piece(cur, dest);
        calAttackSquare();
        bool ret = !isCheck(color);
        move_piece(dest, cur);
        board[dest.y][dest.x].piece = temp_destPiece;

        if (take.x != -1)
            board[take.y][take.x].piece = temp_takePiece;
        calAttackSquare();

        return ret;
    }
    void append(set<Position>* set, const Position& cur, const Position& dest, const bool& legalMove, const Position& take = Position(-1, -1)) {
        if (!legalMove || isThisMoveLegal(cur, dest)) set->insert(dest);
    }

    void repeatMove(set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove) {
        Position next = cur;
        while (true) {
            next += dir;
            if (!boundaryCheck(next)) return;

            if (board[next.y][next.x].empty())
                append(s, cur, next, legalMove);
            else if (isEnemy(cur, next)) {
                append(s, cur, next, legalMove);
                return;
            }
            else if (isAlly(cur, next)) {
                if (!legalMove) append(s, cur, next, legalMove);
                return;
            }
        }
    }
    void oneMove(set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove) {
        Position next = cur + dir;
        if (boundaryCheck(next)) {
            if (board[next.y][next.x].empty() || isEnemy(cur, next))
                append(s, cur, next, legalMove);
            else if (isAlly(cur, next) && !legalMove)
                append(s, cur, next, legalMove);
        }
    }
    set<Position>* rook(const Position& pos, const bool& legalMove) {
        set<Position>* s = new set<Position>;
        for (int i = 0; i < 4; i++)
            repeatMove(s, pos, Position(dir_straight[i].x, dir_straight[i].y), legalMove);
        return s;
    }
    set<Position>* bishop(const Position& pos, const bool& legalMove) {
        set<Position>* s = new set<Position>;
        for (int i = 0; i < 4; i++)
            repeatMove(s, pos, Position(dir_diagonal[i].x, dir_diagonal[i].y), legalMove);
        return s;
    }
    set<Position>* knight(const Position& pos, const bool& legalMove) {
        set<Position>* s = new set<Position>;
        for (int i = 0; i < 8; i++)
            oneMove(s, pos, Position(dir_knight[i].x, dir_knight[i].y), legalMove);
        return s;

    }
    set<Position>* king(const Position& pos, const bool& legalMove) {
        set<Position>* s = new set<Position>;
        for (int i = 0; i < 8; i++)
            oneMove(s, pos, Position(dir_all[i].x, dir_all[i].y), legalMove);

        if (castling_check(board[pos.y][pos.x].piece.color, true)) // Check king side castling
            s->insert(pos + Position(+2, 0)); // already guarantee the check free move
        if (castling_check(board[pos.y][pos.x].piece.color, false)) // Check queen side castling
            s->insert(pos + Position(-2, 0)); // already guarantee the check free move
        return s;
    }
    set<Position>* queen(const Position& pos, const bool& legalMove) {
        set<Position>* s = new set<Position>;
        for (int i = 0; i < 8; i++)
            repeatMove(s, pos, Position(dir_all[i].x, dir_all[i].y), legalMove);
        return s;
    }
    set<Position>* pawn_move(const Position& pos) {
        set<Position>* s = new set<Position>;
        Color color = board[pos.y][pos.x].piece.color;
        Position dir = (color == WHITE ? Position(0, +1) : Position(0, -1));
        Position next = pos + dir;
        if (board[next.y][next.x].empty()) {
            append(s, pos, next, true);
            if (pos.y == (color == WHITE ? 1 : 6)) { // first move
                next += dir;
                if (board[next.y][next.x].empty())
                    append(s, pos, next, true);
            }
        }
        return s;
    }
    set<Position>* pawn_attack(const Position& pos, const bool& legalMove) {
        set<Position>* s = new set<Position>;
        auto add = [&](const Position& next) -> void {
            if (boundaryCheck(next))
                if (!legalMove || (legalMove && isEnemy(pos, next)))
                    append(s, pos, next, legalMove);
            };
        Color color = board[pos.y][pos.x].piece.color;
        int dir = color == WHITE ? +1 : -1;
        add(pos + Position(-1, dir)); // king side
        add(pos + Position(+1, dir)); // queen side

        int en_passent_dir = en_passent_check(pos);
        if (en_passent_dir != 0) {
            int dir = color == WHITE ? +1 : -1;
            append(s, pos, Position(pos.x + en_passent_dir, pos.y + dir), legalMove, Position(pos.x + en_passent_dir, pos.y));
        }
        return s;
    }
    set<Position>* get_attackList(const Position& pos) {
        set<Position>* s = new set<Position>;
        switch (board[pos.y][pos.x].piece.type) {
        case KING: s = king(pos, false); break;
        case QUEEN: s = queen(pos, false); break;
        case ROOK: s = rook(pos, false); break;
        case KNIGHT: s = knight(pos, false); break;
        case BISHOP: s = bishop(pos, false); break;
        case PAWN: s = pawn_attack(pos, false); break;
        }
        return s;
    }
    set<Position>* get_legalMoveList(const Position& pos) {
        set<Position>* s = new set<Position>;
        switch (board[pos.y][pos.x].piece.type) {
        case KING: s = king(pos, true); break;
        case QUEEN: s = queen(pos, true); break;
        case ROOK: s = rook(pos, true); break;
        case KNIGHT: s = knight(pos, true); break;
        case BISHOP: s = bishop(pos, true); break;
        case PAWN:
            set<Position>* a = pawn_move(pos); s = pawn_attack(pos, true);
            s->insert(a->begin(), a->end()); break;
        }
        return s;
    }
    void calAttackSquare() {
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            board[y][x].attack_wb[0] = 0, board[y][x].attack_wb[1] = 0;

        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            if (!board[y][x].empty()) {
                set<Position>* s = get_attackList(Position(x, y));
                Color color = board[y][x].piece.color;
                for (const Position pos : *s)
                    board[pos.y][pos.x].attack_wb[color]++;
            }
    }

public:
    Color turn, myColor;

    Chess() {
        reset();
    }

    void reset() {
        Type initList[8]{ ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            board[1][i].set(Piece(PAWN, WHITE));
            board[0][i].set(Piece(initList[i], WHITE));
            board[6][i].set(Piece(PAWN, BLACK));
            board[7][i].set(Piece(initList[i], BLACK));
        }
        turn = WHITE, myColor = WHITE;
        king_position_wb[0] = Position(4, 0), king_position_wb[1] = Position(4, 7);
        for (int i = 0; i < 4; i++) kr_moveCheck_wb_qk[i / 2][i % 2] = false;
        prevMove.clear();
        calAttackSquare();
    }

    void move_piece(const Position& cur, const Position& dest) {
        if (board[cur.y][cur.x].piece.type == KING) // king position recording
            king_position_wb[board[cur.y][cur.x].piece.color] = dest;
        board[dest.y][dest.x].piece = board[cur.y][cur.x].piece;
        board[cur.y][cur.x].clear();
    }
    void move(const Position& cur, const Position& dest, const int& promotion = -1) {
        bool isCastling = false, isEn_passent = false, isPromotion = false;
        Color color = board[cur.y][cur.x].piece.color;
        Type type = board[cur.y][cur.x].piece.type;

        if (type == KING) {
            kr_moveCheck_wb_qk[color][0] = kr_moveCheck_wb_qk[color][1] = true;
            if (abs(dest.x - cur.x) == 2) // Castling move
                isCastling = true;
        }
        else if (type == ROOK) {
            if (cur.x == 0) kr_moveCheck_wb_qk[color][0] = true;
            else if (cur.x == 7) kr_moveCheck_wb_qk[color][1] = true;
        }
        else if (type == PAWN) {
            if (dest.y == (color == WHITE ? 7 : 0)) isPromotion = true;
            if (cur.x != dest.x) // Pawn takes something
                if (board[dest.y][dest.x].empty())
                    isEn_passent = true;
        }

        if (isCastling) castling_move(cur, dest);
        else if (isEn_passent) en_passent_move(cur, dest);
        else {
            move_piece(cur, dest);
            if (isPromotion) {
                Type promotion_type = NOTYPE;
                switch (promotion) {
                case 0: promotion_type = QUEEN; break;
                case 1: promotion_type = ROOK; break;
                case 2: promotion_type = BISHOP; break;
                case 3: promotion_type = KNIGHT; break;
                default: break;
                }
                board[dest.y][dest.x].set(Piece(promotion_type, color));
            }
        }

        calAttackSquare();
        prevMove.set(type, cur, dest);
        turn = (turn == WHITE ? BLACK : WHITE);
    }
    void castling_move(const Position& cur, const Position& dest) {
        int rank = board[cur.y][cur.x].piece.color == WHITE ? 0 : 7;
        if (dest.x > cur.x) { // king side 
            Position rook_pos = Position(7, rank);
            move_piece(cur, dest);
            move_piece(rook_pos, dest + Position(-1, 0));
        }
        else { // queen side
            Position rook_pos = Position(0, rank);
            move_piece(cur, dest);
            move_piece(rook_pos, dest + Position(+1, 0));
        }
    }
    void en_passent_move(const Position& cur, const Position& dest) {
        Position attack = dest + Position(0, (board[cur.y][cur.x].piece.color == WHITE ? -1 : +1));
        board[attack.y][attack.x].clear();
        move_piece(cur, dest);
    }

    set<pair<Position, Position>>* get_candidateMove(const Color& color) {
        set<pair<Position, Position>>* move_s = new set<pair<Position, Position>>;
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
            if (!board[y][x].empty() && board[y][x].piece.color == color) {
                set<Position>* pos_s = get_legalMoveList(Position(x, y));
                for (const Position pos : *pos_s)
                    move_s->insert(make_pair(Position(x, y), pos));
            }
        return move_s;
    }

    const bool castling_check(const Color& color, const bool& isKingside) {
        int rank = (color == WHITE ? 0 : 7);
        bool condition_1 = !kr_moveCheck_wb_qk[color][isKingside ? 1 : 0];
        bool condition_2 = isKingside ? board[rank][5].empty() && board[rank][6].empty() :
            board[rank][3].empty() && board[rank][2].empty() && board[rank][1].empty();
        bool condition_3 = isKingside ? !board[rank][5].isAttacked(color) && !board[rank][6].isAttacked(color) :
            !board[rank][3].isAttacked(color) && !board[rank][2].isAttacked(color);
        return condition_1 && condition_2 && condition_3;
    }
    const int en_passent_check(const Position& pos) {
        Color color = board[pos.y][pos.x].piece.color;
        if (pos.y != (color == WHITE ? 4 : 3) || prevMove.type != PAWN) return 0;

        int dir = color == WHITE ? +2 : -2;
        Position kingSide[2]{ Position(pos.x + 1, pos.y + dir), Position(pos.x + 1, pos.y) };
        Position queenSide[2]{ Position(pos.x - 1, pos.y + dir), Position(pos.x - 1, pos.y) };
        if (boundaryCheck(kingSide[0]) && kingSide[0] == prevMove.prev && kingSide[1] == prevMove.now)
            return 1;
        else if (boundaryCheck(queenSide[0]) && queenSide[0] == prevMove.prev && queenSide[1] == prevMove.now)
            return -1;
        return 0;
    }

    void print_board() const {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j].empty()) cout << "-- ";
                else cout << typeToChar[board[i][j].piece.type] << colorToChar[board[i][j].piece.color] << " ";
            }
            cout << "   ";
            for (int j = 0; j < 8; j++) {
                cout << board[i][j].attack_wb[0] << board[i][j].attack_wb[1] << " ";
            }
            cout << "\n";
        }
    }
};



class AI {
private:
    Chess chess;
    pair<Position, Position> find_nextMove() {
        set<pair<Position, Position>>* moveList = chess.get_candidateMove(chess.myColor);
        int idx = get_random(0, moveList->size() - 1);
        auto it = moveList->begin();
        advance(it, idx);
        return *it;
    }

public:
    AI() {
        init();
    }

    void init() {
        int color_int; cin >> color_int;
        string temp; getline(cin, temp);
        chess.myColor = (color_int == 0 ? WHITE : BLACK);
    }

    void start() {
        //board.print_board();
        while (true) {
            if (chess.turn != chess.myColor) // opponent's turn
                opponent_move();
            else
                move();
            //board.print_board();
        }
    }

    void opponent_move() {
        string line; getline(cin, line);
        stringstream ss(line); string word;
        int x, y; 
        getline(ss, word, ' '); x = stoi(word); getline(ss, word, ' '); y = stoi(word);
        Position cur = Position(x, y);
        getline(ss, word, ' '); x = stoi(word); getline(ss, word, ' '); y = stoi(word);
        Position dest = Position(x, y);
        getline(ss, word, ' '); int promotion = stoi(word);
        chess.move(cur, dest, promotion);
    }
    void move() {
        pair<Position, Position> move = find_nextMove();
        chess.move(move.first, move.second);
        send_move(move.first, move.second);
    }
    void send_move(const Position& cur_pos, const Position& dest_pos, const int& promotion = -1) {
        cout << cur_pos.x << ' ' << cur_pos.y << ' ' << dest_pos.x << ' ' << dest_pos.y << ' ' << promotion << "\n";
    }
};

int main() {
    AI* chess = new AI();
    chess->start();
}

/*
4 1 4 3 -1
3 0 5 2 -1
5 2 5 6 -1
*/
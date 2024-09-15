#include "Chess.h"

// private: 
const bool Chess::boundaryCheck(const Position& pos) const { return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true; }
const bool Chess::isAlly(const Position& a, const Position& b) const {
    return ((board[a.y][a.x].piece.color != NOCOLOR && board[b.y][b.x].piece.color != NOCOLOR) &&
        board[a.y][a.x].piece.color == board[b.y][b.x].piece.color);
}
const bool Chess::isEnemy(const Position& a, const Position& b) const {
    return ((board[a.y][a.x].piece.color != NOCOLOR && board[b.y][b.x].piece.color != NOCOLOR) &&
        board[a.y][a.x].piece.color != board[b.y][b.x].piece.color);
}
const bool Chess::isCheck(const Color& color) const { return board[king_position_wb[color].y][king_position_wb[color].x].isAttacked(color); }
const bool Chess::isThisMoveLegal(const Move& move) const {
    Chess chess_next = this->clone();
    chess_next.move(move);
    return !chess_next.isCheck(move.piece.color);
}

void Chess::append(set<Move>* s, const Move& move) {
    if (isThisMoveLegal(move))  s->insert(move);
}
void Chess::repeatCheck(set<Move>* s, const Piece& piece, const Position& ori, const Position& dir) {
    Position dest = ori;
    while (true) {
        dest += dir;
        if (!boundaryCheck(dest) || isAlly(ori, dest)) return;

        if (board[dest.y][dest.x].empty())
            append(s, Move(piece, ori, dest));
        else if (isEnemy(ori, dest)) {
            append(s, Move(piece, ori, dest, dest));
            return;
        }
    }
}
void Chess::oneCheck(set<Move>* s, const Piece& piece, const Position& ori, const Position& dir) {
    Position dest = ori + dir;
    if (boundaryCheck(dest)) {
        if (board[dest.y][dest.x].empty())
            append(s, Move(piece, ori, dest));
        else if (isEnemy(ori, dest))
            append(s, Move(piece, ori, dest, dest));
    }
}
set<Move>* Chess::queen(const Position& pos, const Piece& piece) {
    set<Move>* s = new set<Move>;
    for (int i = 0; i < 8; i++)
        repeatCheck(s, piece, pos, dir_all[i]);
    return s;
}
set<Move>* Chess::rook(const Position& pos, const Piece& piece) {
    set<Move>* s = new set<Move>;
    for (int i = 0; i < 4; i++)
        repeatCheck(s, piece, pos, dir_straight[i]);
    return s;
}
set<Move>* Chess::bishop(const Position& pos, const Piece& piece) {
    set<Move>* s = new set<Move>;
    for (int i = 0; i < 4; i++)
        repeatCheck(s, piece, pos, dir_diagonal[i]);
    return s;
}
set<Move>* Chess::knight(const Position& pos, const Piece& piece) {
    set<Move>* s = new set<Move>;
    for (int i = 0; i < 8; i++) {
        oneCheck(s, piece, pos, dir_knight[i]);
    }
    return s;

}
set<Move>* Chess::king(const Position& pos, const Piece& piece) {
    set<Move>* s = new set<Move>;
    for (int i = 0; i < 8; i++)
        oneCheck(s, piece, pos, dir_all[i]);

    if (castling_check(piece.color, true)) // Check king side castling
        s->insert(Move(piece, pos, pos + Position(+2, 0))); // already guarantee the check free move
    if (castling_check(piece.color, false)) // Check queen side castling
        s->insert(Move(piece, pos, pos + Position(-2, 0))); // already guarantee the check free move
    return s;
}
set<Move>* Chess::pawn(const Position& pos, const Piece& piece) {
    set<Move>* s = new set<Move>;
    Color color = piece.color;
    int dir = (color == WHITE ? +1 : -1);
    // MOVE
    Position dest = pos; dest.y += dir;
    if (board[dest.y][dest.x].empty()) {
        append(s, Move(piece, pos, dest));
        if (dest.y == (color == WHITE ? 7 : 0)) { // promotion move
            for (int i = 0; i < 4; i++) 
                append(s, Move(piece, pos, dest, promotion_list[i]));
        }
        if (pos.y == (color == WHITE ? 1 : 6)) { // first move
            dest.y += dir;
            if (board[dest.y][dest.x].empty()) {
                append(s, Move(piece, pos, dest));
            }
        }
    }

    // ATTACK
    // Normal attack
    auto attack = [&](const Position& dest) -> void {
        if (boundaryCheck(dest) && isEnemy(pos, dest)) {
            if (dest.y == (color == WHITE ? 7 : 0)) { // takes and promotion
                for (int i = 0; i < 4; i++)
                    append(s, Move(piece, pos, dest, dest, promotion_list[i]));
            }
            else append(s, Move(piece, pos, dest, dest));
        }
        };
    attack(pos + Position(-1, dir));
    attack(pos + Position(+1, dir));

    // en_passent attack
    int en_passent_dir = en_passent_check(pos);
    if (en_passent_dir != 0) {
        Position dest = Position(pos.x + en_passent_dir, pos.y + dir);
        Position take = Position(pos.x + en_passent_dir, pos.y);
        append(s, Move(piece, pos, dest, take));
    }
    return s;
}
set<Move>* Chess::legal_moves(const Position& pos) {
    set<Move>* s = new set<Move>;
    switch (board[pos.y][pos.x].piece.type) {
    case KING: s = king(pos, board[pos.y][pos.x].piece); break;
    case QUEEN: s = queen(pos, board[pos.y][pos.x].piece); break;
    case ROOK: s = rook(pos, board[pos.y][pos.x].piece); break;
    case KNIGHT: s = knight(pos, board[pos.y][pos.x].piece); break;
    case BISHOP: s = bishop(pos, board[pos.y][pos.x].piece); break;
    case PAWN: s = pawn(pos, board[pos.y][pos.x].piece);break;
    }
    return s;
}

void Chess::repeatCheck(set<Position>* s, const Position& ori, const Position& dir) {
    Position dest = ori;
    while (true) {
        dest += dir;
        if (!boundaryCheck(dest)) return;

        if (board[dest.y][dest.x].empty())
            s->insert(dest);
        else if (isEnemy(ori, dest) || isAlly(ori, dest)) {
            s->insert(dest);
            return;
        }
    }
}
void Chess::oneCheck(set<Position>* s, const Position& ori, const Position& dir) {
    Position dest = ori + dir;
    if (boundaryCheck(dest)) s->insert(dest);
}
set<Position>* Chess::queen(const Position& pos) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        repeatCheck(s, pos, dir_all[i]);
    return s;
}
set<Position>* Chess::rook(const Position& pos) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 4; i++)
        repeatCheck(s, pos, dir_straight[i]);
    return s;
}
set<Position>* Chess::bishop(const Position& pos) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 4; i++)
        repeatCheck(s, pos, dir_diagonal[i]);
    return s;
}
set<Position>* Chess::knight(const Position& pos) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        oneCheck(s, pos, dir_knight[i]);
    return s;

}
set<Position>* Chess::king(const Position& pos) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        oneCheck(s, pos, dir_all[i]);
    return s;
}
set<Position>* Chess::pawn(const Position& pos) {
    set<Position>* s = new set<Position>;
    Color color = board[pos.y][pos.x].piece.color;
    int dir = (color == WHITE ? +1 : -1);
    oneCheck(s, pos, Position(-1, dir));
    oneCheck(s, pos, Position(+1, dir));
    return s;
}
set<Position>* Chess::get_attackList(const Position& pos) {
    set<Position>* s = new set<Position>;
    switch (board[pos.y][pos.x].piece.type) {
    case KING: s = king(pos); break;
    case QUEEN: s = queen(pos); break;
    case ROOK: s = rook(pos); break;
    case KNIGHT: s = knight(pos); break;
    case BISHOP: s = bishop(pos); break;
    case PAWN: s = pawn(pos); break;
    }
    return s;
}
void Chess::calAttackSquare() {
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        board[y][x].attack_wb[0] = 0, board[y][x].attack_wb[1] = 0;

    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        if (!board[y][x].empty()) {
            Color color = board[y][x].piece.color;
            set<Position>* s = get_attackList(Position(x, y));
            for (const Position pos : *s)
                board[pos.y][pos.x].attack_wb[color]++;
        }
}

void Chess::move(const Move& move) {
    if (move.piece.type == KING) // king position recording
        king_position_wb[move.piece.color] = move.dest;
    board[move.dest.y][move.dest.x].piece = board[move.ori.y][move.ori.x].piece;
    board[move.ori.y][move.ori.x].clear();
}
void Chess::capture(const Move& move) {
    board[move.take.y][move.take.x].clear();
    Chess::move(move);
}
void Chess::castling(const Move& move) {
    int rank = (move.piece.color == WHITE ? 0 : 7);
    if (move.dest.x > move.ori.x) { // king side 
        Position rook_pos = Position(7, rank);
        Chess::move(move);
        Chess::move(Move(Piece(ROOK, move.piece.color), rook_pos, move.dest + Position(-1, 0)));
    }
    else { // queen side
        Position rook_pos = Position(0, rank);
        Chess::move(move);
        Chess::move(Move(Piece(ROOK, move.piece.color), rook_pos, move.dest + Position(+1, 0)));
    }
}
void Chess::promotion(const Move& move) {
    board[move.dest.y][move.dest.x].set(Piece(move.promotion_type, move.piece.color));
}

const bool Chess::castling_check(const Color& color, const bool& isKingside) const {
    int rank = (color == WHITE ? 0 : 7);
    bool condition_1 = !kr_moveCheck_wb_qk[color][isKingside ? 1 : 0];
    bool condition_2 = isKingside ? board[rank][5].empty() && board[rank][6].empty() :
        board[rank][3].empty() && board[rank][2].empty() && board[rank][1].empty();
    bool condition_3 = isKingside ? !board[rank][5].isAttacked(color) && !board[rank][6].isAttacked(color) :
        !board[rank][3].isAttacked(color) && !board[rank][2].isAttacked(color);
    return condition_1 && condition_2 && condition_3;
}
const int Chess::en_passent_check(const Position& pos) {
    Color color = board[pos.y][pos.x].piece.color;
    if (pos.y != (color == WHITE ? 4 : 3) || prevMove.piece.type != PAWN) return 0;

    int dir = (color == WHITE ? +2 : -2);
    Position KSide[2] = { Position(pos.x + 1, pos.y + dir), Position(pos.x + 1, pos.y) };
    Position QSide[2] = { Position(pos.x - 1, pos.y + dir), Position(pos.x - 1, pos.y) };
    if (boundaryCheck(KSide[0]) && KSide[0] == prevMove.ori && KSide[1] == prevMove.dest) return 1;
    if (boundaryCheck(QSide[0]) && QSide[0] == prevMove.ori && QSide[1] == prevMove.dest) return -1;
    return 0;
}

// public: 
void Chess::reset() {
    Piece_type initList[8]{ ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
    for (int i = 0; i < 8; i++) {
        board[1][i].set(Piece(PAWN, WHITE));
        board[0][i].set(Piece(initList[i], WHITE));
        board[6][i].set(Piece(PAWN, BLACK));
        board[7][i].set(Piece(initList[i], BLACK));
    }
    turn = WHITE, myColor = WHITE;
    king_position_wb[0] = Position(4, 0), king_position_wb[1] = Position(4, 7);
    for (int i = 0; i < 4; i++) kr_moveCheck_wb_qk[i / 2][i % 2] = false;
    prevMove = Move();
    calAttackSquare();
}

set<Move>* Chess::get_candidateMove(const Color& color) {
    set<Move>* moves = new set<Move>;
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        if (!board[y][x].empty() && board[y][x].piece.color == color) {
            set<Move>* s = legal_moves(Position(x, y));
            for (const Move& move : *s)
                moves->insert(move);
        }
    return moves;
}
void Chess::play(const Move& move) {
    // King, Rook move check for Castling
    if (move.piece.type == KING) kr_moveCheck_wb_qk[move.piece.color][0] = kr_moveCheck_wb_qk[move.piece.color][1] = true;
    else if (move.piece.type == ROOK) {
        if (move.ori.x == 0) kr_moveCheck_wb_qk[move.piece.color][0] = true;
        else if (move.ori.x == 7) kr_moveCheck_wb_qk[move.piece.color][1] = true;
    }

    // Play
    Move_type move_type = move.get_move_type();
    switch (move_type) {
    case MOVE: Chess::move(move); break;
    case CAPTURE: capture(move); break; // including en_passent move
    case CASTLING: castling(move); break;
    case MOVE_PRO: Chess::move(move), promotion(move); break;
    case CAPTURE_PRO: capture(move), promotion(move); break;
    default: break;
    }

    calAttackSquare();
    prevMove.set(move);
    turn = (turn == WHITE ? BLACK : WHITE);
}

void Chess::print_board() {
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
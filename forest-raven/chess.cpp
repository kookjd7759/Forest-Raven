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
const bool Chess::isThisMoveLegal(const Move& move) {
    Color color = board[move.ori.y][move.ori.x].piece.color;
    Chess chess_next = this->clone();
    chess_next.move(move);
    if (chess_next.isCheck(color))
        return false;
    return true;
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

void Chess::append(set<Position>* set, const Move& move, const bool& legalMove) {
    if (!legalMove || isThisMoveLegal(move)) set->insert(move.dest);
}
void Chess::repeatMove(set<Position>* s, const Position& ori, const Position& dir, const bool& legalMove) {
    Position dest = ori;
    while (true) {
        dest += dir;
        if (!boundaryCheck(dest)) return;

        if (board[dest.y][dest.x].empty())
            append(s, Move(ori, dest), legalMove);
        else if (isEnemy(ori, dest)) {
            append(s, Move(ori, dest, dest), legalMove);
            return;
        }
        else if (isAlly(ori, dest)) {
            if (!legalMove) append(s, Move(ori, dest), legalMove);
            return;
        }
    }
}
void Chess::oneMove(set<Position>* s, const Position& ori, const Position& dir, const bool& legalMove) {
    Position dest = ori + dir;
    if (boundaryCheck(dest)) {
        if (board[dest.y][dest.x].empty())
            append(s, Move(ori, dest), legalMove);
        else if (isEnemy(ori, dest))
            append(s, Move(ori, dest, dest), legalMove);
        else if (isAlly(ori, dest) && !legalMove)
            append(s, Move(ori, dest), legalMove);
    }
}
set<Position>* Chess::rook(const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 4; i++)
        repeatMove(s, pos, dir_straight[i], legalMove);
    return s;
}
set<Position>* Chess::bishop(const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 4; i++)
        repeatMove(s, pos, dir_diagonal[i], legalMove);
    return s;
}
set<Position>* Chess::knight(const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        oneMove(s, pos, dir_knight[i], legalMove);
    return s;

}
set<Position>* Chess::king(const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        oneMove(s, pos, dir_all[i], legalMove);

    Color color = board[pos.y][pos.x].piece.color;
    if (castling_check(color, true)) // Check king side castling
        s->insert(pos + Position(+2, 0)); // already guarantee the check free move
    if (castling_check(color, false)) // Check queen side castling
        s->insert(pos + Position(-2, 0)); // already guarantee the check free move
    return s;
}
set<Position>* Chess::queen(const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        repeatMove(s, pos, dir_all[i], legalMove);
    return s;
}
set<Position>* Chess::pawn_move(const Position& pos) {
    set<Position>* s = new set<Position>;
    Color color = board[pos.y][pos.x].piece.color;
    Position dir = (color == WHITE ? Position(0, +1) : Position(0, -1));
    Position dest = pos + dir;
    if (board[dest.y][dest.x].empty()) {
        append(s, Move(pos, dest), true);
        if (dest.y == (color == WHITE ? 7 : 0)) { // promotion move
            for (int pro_type = 0; pro_type < 4; pro_type++) 
                append(s, Move(pos, dest, pro_type), true);
        }
        if (pos.y == (color == WHITE ? 1 : 6)) { // first move
            dest += dir;
            if (board[dest.y][dest.x].empty())
                append(s, Move(pos, dest), true);
        }
    }
    return s;
}
set<Position>* Chess::pawn_attack(const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    Color color = board[pos.y][pos.x].piece.color;
    auto attack = [&](const Position& dest) -> void {
        if (boundaryCheck(dest)) {
            if (!legalMove) 
                append(s, Move(pos, dest), legalMove);
            else if (legalMove && isEnemy(pos, dest)) {
                append(s, Move(pos, dest, dest), legalMove);
                if (dest.y == (color == WHITE ? 7 : 0)) { // promotion move
                    for (int pro_type = 0; pro_type < 4; pro_type++)
                        append(s, Move(pos, dest, dest, pro_type), legalMove);
                }
            }
        }
        };
    int dir = color == WHITE ? +1 : -1;
    attack(pos + Position(-1, dir)); // king side
    attack(pos + Position(+1, dir)); // queen side

    int en_passent_dir = en_passent_check(pos);
    if (en_passent_dir != 0) {
        int dir = color == WHITE ? +1 : -1;
        Position dest = Position(pos.x + en_passent_dir, pos.y + dir);
        Position take = Position(pos.x + en_passent_dir, pos.y);
        if (dest.y == (color == WHITE ? 7 : 0)) { // takes and promotion
            for (int pro_type = 0; pro_type < 4; pro_type++)
                append(s, Move(pos, dest, take, pro_type), true);
        }
        else append(s, Move(pos, dest, take), legalMove);
    }
    return s;
}
set<Position>* Chess::get_attackList(const Position& pos) {
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
set<Position>* Chess::get_legalMoveList(const Position& pos) {
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

void Chess::move_piece(const Move& move) {
    if (board[move.ori.y][move.ori.x].piece.type == KING) // king position recording
        king_position_wb[board[move.ori.y][move.ori.x].piece.color] = move.dest;
    board[move.dest.y][move.dest.x].piece = board[move.ori.y][move.ori.x].piece;
    board[move.ori.y][move.ori.x].clear();
}
void Chess::castling_move(const Move& move) {
    int rank = (board[move.ori.y][move.ori.x].piece.color == WHITE ? 0 : 7);
    if (move.dest.x > move.ori.x) { // king side 
        Position rook_pos = Position(7, rank);
        move_piece(move);
        move_piece(Move(rook_pos, move.dest + Position(-1, 0)));
    }
    else { // queen side
        Position rook_pos = Position(0, rank);
        move_piece(move);
        move_piece(Move(rook_pos, move.dest + Position(+1, 0)));
    }
}
void Chess::en_passent_move(const Move& move) {
    board[move.take.y][move.take.x].clear();
    move_piece(move);
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
    if (pos.y != (color == WHITE ? 4 : 3) || prevMove.type != PAWN) return 0;

    int dir = (color == WHITE ? +2 : -2);
    Move kingSide(Position(pos.x + 1, pos.y + dir), Position(pos.x + 1, pos.y));
    Move queenSide(Position(pos.x - 1, pos.y + dir), Position(pos.x - 1, pos.y));
    if (boundaryCheck(kingSide.ori) && kingSide.ori == prevMove.move.ori && kingSide.dest == prevMove.move.dest)
        return 1;
    else if (boundaryCheck(queenSide.ori) && queenSide.ori == prevMove.move.ori && queenSide.dest == prevMove.move.dest)
        return -1;
    return 0;
}

// public: 
void Chess::reset() {
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

set<Move>* Chess::get_candidateMove(const Color& color) {
    set<Move>* move_s = new set<Move>;
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        if (!board[y][x].empty() && board[y][x].piece.color == color) {
            set<Position>* s = get_legalMoveList(Position(x, y));
            for (const Position pos : *s)
                move_s->insert(Move(Position(x, y), pos));
        }
    return move_s;
}
void Chess::move(const Move& move) {
    bool isCastling = false, isEn_passent = false, isPromotion = false;
    Color color = board[move.ori.y][move.ori.x].piece.color;
    Type type = board[move.ori.y][move.ori.x].piece.type;
    
    if (type == KING) {
        kr_moveCheck_wb_qk[color][0] = kr_moveCheck_wb_qk[color][1] = true;
        if (abs(move.dest.x - move.ori.x) == 2) // Castling move
            isCastling = true;
    }
    else if (type == ROOK) {
        if (move.ori.x == 0) kr_moveCheck_wb_qk[color][0] = true;
        else if (move.ori.x == 7) kr_moveCheck_wb_qk[color][1] = true;
    }
    else if (type == PAWN) {
        if (move.dest.y == (color == WHITE ? 7 : 0)) isPromotion = true;
        if (move.ori.x != move.dest.x) // Pawn takes something
            if (board[move.dest.y][move.dest.x].empty())
                isEn_passent = true;
    }

    if (isCastling) castling_move(move);
    else if (isEn_passent) en_passent_move(move);
    else {
        move_piece(move);
        if (isPromotion) {
            Type promotion_type = NOTYPE;
            switch (move.promotion) {
            case 0: promotion_type = QUEEN; break;
            case 1: promotion_type = ROOK; break;
            case 2: promotion_type = BISHOP; break;
            case 3: promotion_type = KNIGHT; break;
            default: break;
            }
            board[move.dest.y][move.dest.x].set(Piece(promotion_type, color));
        }
    }

    calAttackSquare();
    prevMove.set(type, move);
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
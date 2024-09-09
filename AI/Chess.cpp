#include "Chess.h"


void move_piece(Square** board, Position* king_position_wb, const Position& cur, const Position& dest) {
    if (board[cur.y][cur.x].piece.type == KING) // king position recording
        king_position_wb[board[cur.y][cur.x].piece.color] = dest;
    board[dest.y][dest.x].piece = board[cur.y][cur.x].piece;
    board[cur.y][cur.x].clear();
}
void move(Square** board, Position* king_position_wb, bool** kr_moveCheck_wb_qk, PreviousMove* prevMove, Color* turn, const Position& cur, const Position& dest, const int& promotion = -1) {
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

    if (isCastling) castling_move(board, cur, dest);
    else if (isEn_passent) en_passent_move(board, cur, dest);
    else {
        move_piece(board, king_position_wb, cur, dest);
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

    calAttackSquare(board);
    prevMove->set(type, cur, dest);
    *turn = (*turn == WHITE ? BLACK : WHITE);
}
void castling_move(Square** board, const Position& cur, const Position& dest) {
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
void en_passent_move(Square** board, const Position& cur, const Position& dest) {
    Position attack = dest + Position(0, (board[cur.y][cur.x].piece.color == WHITE ? -1 : +1));
    board[attack.y][attack.x].clear();
    move_piece(cur, dest);
}

const bool boundaryCheck(const Position& pos) { return (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7) ? false : true; }
const bool isAlly(Square** board, const Position& a, const Position& b) {
    return ((board[a.y][a.x].piece.color != NOCOLOR && board[b.y][b.x].piece.color != NOCOLOR) &&
        board[a.y][a.x].piece.color == board[b.y][b.x].piece.color);
}
const bool isEnemy(Square** board, const Position& a, const Position& b) {
    return ((board[a.y][a.x].piece.color != NOCOLOR && board[b.y][b.x].piece.color != NOCOLOR) &&
        board[a.y][a.x].piece.color != board[b.y][b.x].piece.color);
}
const bool isCheck(Square** board, Position king_position_wb, const Color& color) { return board[king_position_wb[color].y][king_position_wb[color].x].isAttacked(color); }

void calAttackSquare(Square** board) {
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        board[y][x].attack_wb[0] = 0, board[y][x].attack_wb[1] = 0;

    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        if (!board[y][x].empty()) {
            set<Position>* s = get_attackList(board, Position(x, y));
            Color color = board[y][x].piece.color;
            for (const Position pos : *s)
                board[pos.y][pos.x].attack_wb[color]++;
        }
}
const bool isThisMoveLegal(Square** board, const Position& cur, const Position& dest, const Position& take = Position(-1, -1)) {
    Color color = board[cur.y][cur.x].piece.color;
    Piece temp_destPiece = board[dest.y][dest.x].piece, temp_takePiece = Piece(NOTYPE, NOCOLOR);
    if (take.x != -1) {
        temp_takePiece = board[take.y][take.x].piece;
        board[take.y][take.x].clear();
    }

    move_piece(cur, dest);
    calAttackSquare(board);
    bool ret = !isCheck(color);
    move_piece(dest, cur);
    board[dest.y][dest.x].piece = temp_destPiece;

    if (take.x != -1)
        board[take.y][take.x].piece = temp_takePiece;
    calAttackSquare(board);

    return ret;
}
void append(Square** board, set<Position>* set, const Position& cur, const Position& dest, const bool& legalMove, const Position& take = Position(-1, -1)) {
    if (!legalMove || isThisMoveLegal(board, cur, dest)) set->insert(dest);
}

void repeatMove(Square** board, set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove) {
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
void oneMove(Square** board, set<Position>* s, const Position& cur, const Position& dir, const bool& legalMove) {
    Position next = cur + dir;
    if (boundaryCheck(next)) {
        if (board[next.y][next.x].empty() || isEnemy(cur, next))
            append(s, cur, next, legalMove);
        else if (isAlly(cur, next) && !legalMove)
            append(s, cur, next, legalMove);
    }
}
set<Position>* rook(Square** board, const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 4; i++)
        repeatMove(board, s, pos, Position(dir_straight[i].x, dir_straight[i].y), legalMove);
    return s;
}
set<Position>* bishop(Square** board, const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 4; i++)
        repeatMove(board, s, pos, Position(dir_diagonal[i].x, dir_diagonal[i].y), legalMove);
    return s;
}
set<Position>* knight(Square** board, const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        oneMove(board, s, pos, Position(dir_knight[i].x, dir_knight[i].y), legalMove);
    return s;

}
set<Position>* king(Square** board, const bool** kr_moveCheck_wb_qk, const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        oneMove(board, s, pos, Position(dir_all[i].x, dir_all[i].y), legalMove);

    if (castling_check(board, kr_moveCheck_wb_qk, board[pos.y][pos.x].piece.color, true)) // Check king side castling
        s->insert(pos + Position(+2, 0)); // already guarantee the check free move
    if (castling_check(board, kr_moveCheck_wb_qk, board[pos.y][pos.x].piece.color, false)) // Check queen side castling
        s->insert(pos + Position(-2, 0)); // already guarantee the check free move
    return s;
}
set<Position>* queen(Square** board, const Position& pos, const bool& legalMove) {
    set<Position>* s = new set<Position>;
    for (int i = 0; i < 8; i++)
        repeatMove(board, s, pos, Position(dir_all[i].x, dir_all[i].y), legalMove);
    return s;
}
set<Position>* pawn_move(Square** board, const Position& pos) {
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
set<Position>* pawn_attack(Square** board, const PreviousMove& prevMove, const Position& pos, const bool& legalMove) {
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

    int en_passent_dir = en_passent_check(board, prevMove, pos);
    if (en_passent_dir != 0) {
        int dir = color == WHITE ? +1 : -1;
        append(s, pos, Position(pos.x + en_passent_dir, pos.y + dir), legalMove, Position(pos.x + en_passent_dir, pos.y));
    }
    return s;
}
set<Position>* get_attackList(Square** board, const Position& pos) {
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
set<Position>* get_legalMoveList(Square** board, const PreviousMove& prevMove, const bool** kr_moveCheck_wb_qk, const Position& pos) {
    set<Position>* s = new set<Position>;
    switch (board[pos.y][pos.x].piece.type) {
    case KING: s = king(board, kr_moveCheck_wb_qk, pos, true); break;
    case QUEEN: s = queen(board, pos, true); break;
    case ROOK: s = rook(board, pos, true); break;
    case KNIGHT: s = knight(board, pos, true); break;
    case BISHOP: s = bishop(board, pos, true); break;
    case PAWN:
        set<Position>* a = pawn_move(board, pos); s = pawn_attack(board, prevMove, pos, true);
        s->insert(a->begin(), a->end()); break;
    }
    return s;
}
set<pair<Position, Position>>* get_candidateMove(Square** board, const Color& color) {
    set<pair<Position, Position>>* move_s = new set<pair<Position, Position>>;
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++)
        if (!board[y][x].empty() && board[y][x].piece.color == color) {
            set<Position>* pos_s = get_legalMoveList(board, Position(x, y));
            for (const Position pos : *pos_s)
                move_s->insert(make_pair(Position(x, y), pos));
        }
    return move_s;
}

const bool castling_check(Square** board, const bool** kr_moveCheck_wb_qk, const Color& color, const bool& isKingside) {
    int rank = (color == WHITE ? 0 : 7);
    bool condition_1 = !kr_moveCheck_wb_qk[color][isKingside ? 1 : 0];
    bool condition_2 = isKingside ? board[rank][5].empty() && board[rank][6].empty() :
        board[rank][3].empty() && board[rank][2].empty() && board[rank][1].empty();
    bool condition_3 = isKingside ? !board[rank][5].isAttacked(color) && !board[rank][6].isAttacked(color) :
        !board[rank][3].isAttacked(color) && !board[rank][2].isAttacked(color);
    return condition_1 && condition_2 && condition_3;
}
const int en_passent_check(Square** board, const PreviousMove& prevMove, const Position& pos) {
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

void print_board(Square** board) {
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
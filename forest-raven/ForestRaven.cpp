#include "Utility.h"
#include <string>
#include <cctype>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

static void setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int colorAttribute = (bgColor << 4) | textColor;
    SetConsoleTextAttribute(hConsole, colorAttribute);
}

using namespace ForestRaven;

static void print_BB(Bitboard b) {
    for (int i = 63; i >= 0; --i) {
        cout << ((b >> i) & 1) << ' ';
        if (i % 8 == 0) cout << "\n";
    } cout << "\n";
}

struct Move {
    Piece piece;
    Square ori, dest, take = NOSQUARE;
    Piece_type promotion = NOPIECETYPE;
    bool check = false;
    bool isAttacked = false;

    Move(Piece pc, Square o, Square d) : piece(pc), ori(o), dest(d) {}
    Move(Piece pc, Square o, Square d, Square t) : piece(pc), ori(o), dest(d), take(t) {}
    Move(Piece pc, Square o, Square d, Square t, Piece_type pt) : piece(pc), ori(o), dest(d), take(t), promotion(pt) {}
    Move(Piece pc, Square o, Square d, Piece_type pt) : piece(pc), ori(o), dest(d), promotion(pt) {}

    Move() { piece = NOPIECE, ori = NOSQUARE, dest = NOSQUARE; }
    Move(const Move& m) : piece(m.piece), ori(m.ori), dest(m.dest), take(m.take), promotion(m.promotion), check(m.check) {}
};
bool move_comp(const Move& a, const Move& b) {
    if (a.check != b.check) return a.check > b.check;
    if (a.take != b.take) return a.take != NOSQUARE && b.take == NOSQUARE;
    return a.isAttacked > b.isAttacked;
}
string move_notation(const vector<Move>* moves, Move move) {
    Piece_type pt = type_of(move.piece);

    if (pt == KING && abs(move.ori - move.dest) == 2)
        return (move.ori < move.dest ? "O-O" : "O-O-O");

    string notation("");
    if (pt != PAWN) notation += pt_char[pt];

    File file = file_of(move.ori);
    Rank rank = rank_of(move.ori);
    int file_cnt(0), rank_cnt(0);
    for (const Move& legalMove : *moves)
        if (legalMove.piece == move.piece && legalMove.dest == move.dest) {
            if (file_of(legalMove.ori) == file) ++file_cnt;
            if (rank_of(legalMove.ori) == rank) ++rank_cnt;
        }

    if (file_cnt > 1 && rank_cnt == 1) notation += (char)('a' + file);
    if (file_cnt == 1 && rank_cnt > 1) notation += (char)('1' + rank);
    if (file_cnt > 1 && rank_cnt > 1) notation += sq_notation(move.ori);

    if (move.take != NOSQUARE) {
        if (pt == PAWN) notation += (char)('a' + file_of(move.ori));
        notation += 'x';
    }

    notation += sq_notation(move.dest);

    if (move.promotion != NOPIECETYPE) notation += '=' + pt_char[move.promotion];

    if (move.check) notation += '+';

    return notation;
}
Move notation_move(const vector<Move>* moves, const string& notation) {
    auto find_move = [&](Piece_type pt, Square dest, Piece_type promotion = NOPIECETYPE) -> Move {
        Move ret;
        bool ch(false);
        for (const Move& move : *moves)
            if (type_of(move.piece) == pt && move.dest == dest && move.promotion == promotion)
                if (ch) return Move();
                else ret = move, ch = true;

        if (!ch) {
            cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
            return Move();
        }

        return ret;
        };

    auto find_move_with_file = [&](Piece_type pt, Square dest, File file) -> Move {
        Move ret;
        bool ch(false);
        for (const Move& move : *moves)
            if (type_of(move.piece) == pt && move.dest == dest && file_of(move.ori) == file)
                if (ch) return Move();
                else ret = move, ch = true;

        if (!ch) {
            cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
            return Move();
        }

        return ret;
        };

    auto find_move_with_rank = [&](Piece_type pt, Square dest, Rank rank) -> Move {
        Move ret;
        bool ch(false);
        for (const Move& move : *moves)
            if (type_of(move.piece) == pt && move.dest == dest && rank_of(move.ori) == rank)
                if (ch) return Move();
                else ret = move, ch = true;

        if (!ch) {
            cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
            return Move();
        }

        return ret;
        };

    auto find_move_with_ori = [&](Piece_type pt, Square dest, Square ori) -> Move {
        Move ret;
        bool ch(false);
        for (const Move& move : *moves)
            if (type_of(move.piece) == pt && move.dest == dest && move.ori == ori)
                if (ch) return Move();
                else ret = move, ch = true;

        if (!ch) {
            cout << "notation ERROR::I didn't found '" << notation << "' in legal move list \n";
            return Move();
        }

        return ret;
        };


    if (moves->size() == 0) {
        cout << "notation ERROR::moves vector is empty\n";
        return  Move();
    }

    Color color = color_of(moves->at(0).piece);

    Piece_type pt;
    Square dest;
    if (notation == "O-O") { // king side castling
        dest = (color == WHITE ? G1 : G8);
        return find_move(KING, dest);
    }
    else if (notation == "O-O-O") { // queen side castling
        dest = (color == WHITE ? C1 : C8);
        return find_move(KING, dest);
    }
    if (notation.size() == 2) { // pawn, move
        if (!isFile(notation[0]) || !isRank(notation[1])) return Move();

        dest = notation_sq(notation);
        return find_move(PAWN, dest);
    }
    else if (notation.size() == 3) { // other piece, move
        if (!isPieceType(notation[0]) || !isFile(notation[1]) || !isRank(notation[2])) return Move();

        pt = char_pt(notation[0]);
        dest = notation_sq(notation.substr(1, 2));
        return find_move(pt, dest);
    }
    else if (notation.size() == 4) {
        if (isFile(notation[0])) {
            if (notation[1] == 'x') { // pawn, takes
                if (!isFile(notation[2]) || !isRank(notation[3])) return Move();

                dest = notation_sq(notation.substr(2, 2));
                return find_move(PAWN, dest);
            }
            else if (isRank(notation[1])) { // pawn, move promotion
                if (notation[2] != '=' || !isProPieceType(notation[3])) return Move();

                dest = notation_sq(notation.substr(0, 2));
                Piece_type pt_pro = char_pt(notation[3]);
                return find_move(PAWN, dest, pt_pro);
            }
            else return Move();
        }
        else if (isPieceType(notation[0])) {
            pt = char_pt(notation[0]);
            dest = notation_sq(notation.substr(2, 2));
            if (notation[1] == 'x') { // other piece, takes
                return find_move(pt, dest);
            }
            else if (isFile(notation[1])) { // other piece, move but specify FILE
                File file = (File)(notation[1] - 'a');
                return find_move_with_file(pt, dest, file);
            }
            else if (isRank(notation[1])) { // other piece, move but specify RANK
                Rank rank = (Rank)(notation[1] - '1');
                return find_move_with_rank(pt, dest, rank);
            }
            else return Move();
        }
    }
    else if (notation.size() == 5) {
        if (!isPieceType(notation[0]) || !isFile(notation[3]) || !isRank(notation[4])) return Move();
        pt = char_pt(notation[0]);
        dest = notation_sq(notation.substr(3, 2));

        if (isFile(notation[1]) && notation[2] == 'x') { // other piece, takes but specify FILE
            File file = (File)(notation[1] - 'a');
            return find_move_with_file(pt, dest, file);
        }
        else if (isRank(notation[1]) && notation[2] == 'x') { // other piece, takes but specify RANK
            Rank rank = (Rank)(notation[1] - '1');
            return find_move_with_rank(pt, dest, rank);
        }
        else if (isFile(notation[1]) && isRank(notation[2])) {  // other piece, move but specify FILE and RANK
            Square ori = notation_sq(notation.substr(1, 2));
            return find_move_with_ori(pt, dest, ori);
        }
        else return Move();
    }
    else if (notation.size() == 6) {
        if (isFile(notation[0])) { // pawn, takes and promotion 
            if (notation[1] != 'x' || !isFile(notation[2]) || !isRank(notation[3]) ||
                notation[4] != '=' || !isProPieceType(notation[5])) return Move();

            dest = notation_sq(notation.substr(2, 2));
            Piece_type pt_pro = char_pt(notation[5]);
            return find_move(PAWN, dest, pt_pro);
        }
        else if (isPieceType(notation[0])) {
            if (!isFile(notation[1]) || !isRank(notation[2]) || notation[3] != 'x' ||
                !isFile(notation[4]) || !isRank(notation[5])) return Move();

            pt = char_pt(notation[0]);
            Square ori = notation_sq(notation.substr(1, 2));
            dest = notation_sq(notation.substr(4, 2));
            return find_move_with_ori(pt, dest, ori);
        }

    }
    else return Move();
}

constexpr Bitboard FileA_BB = 0x0101010101010101ULL;
constexpr Bitboard FileB_BB = FileA_BB << 1;
constexpr Bitboard FileC_BB = FileA_BB << 2;
constexpr Bitboard FileD_BB = FileA_BB << 3;
constexpr Bitboard FileE_BB = FileA_BB << 4;
constexpr Bitboard FileF_BB = FileA_BB << 5;
constexpr Bitboard FileG_BB = FileA_BB << 6;
constexpr Bitboard FileH_BB = FileA_BB << 7;

constexpr Bitboard Rank1_BB = 0xFF;
constexpr Bitboard Rank2_BB = Rank1_BB << (8 * 1);
constexpr Bitboard Rank3_BB = Rank1_BB << (8 * 2);
constexpr Bitboard Rank4_BB = Rank1_BB << (8 * 3);
constexpr Bitboard Rank5_BB = Rank1_BB << (8 * 4);
constexpr Bitboard Rank6_BB = Rank1_BB << (8 * 5);
constexpr Bitboard Rank7_BB = Rank1_BB << (8 * 6);
constexpr Bitboard Rank8_BB = Rank1_BB << (8 * 7);

constexpr Direction dir_straight[4] = { U, R, D, L };
constexpr Direction dir_diagonal[4] = { UL, UR, DL, DR };
constexpr Direction all_direction[8] = { U, R, D, L, UL, UR, DL, DR };

constexpr int knight_steps[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
constexpr int king_steps[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };

constexpr Piece_type promotion_list[4] = { ROOK, KNIGHT, BISHOP, QUEEN };

uint8_t  SquareDistance[SQUARE_NB][SQUARE_NB];
Bitboard attacks_pawn[COLOR_NB][SQUARE_NB];
Bitboard attacks_night[SQUARE_NB];
Bitboard attacks_king[SQUARE_NB];

template<typename T1 = Square> inline int distance(Square x, Square y);
template<> inline int distance<File>(Square x, Square y) { return abs(file_of(x) - file_of(y)); }
template<> inline int distance<Rank>(Square x, Square y) { return abs(rank_of(x) - rank_of(y)); }
template<> inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }

Bitboard destination(Square ori, int step) {
    Square dest = Square(ori + step);
    return is_ok(dest) && distance(ori, dest) <= 2 ? sq_bb(dest) : Bitboard(0);
}

constexpr Bitboard shift(Direction Dir, Bitboard b) {
    return Dir == U ? b << 8
        : Dir == D ? b >> 8
        : Dir == R ? (b & ~FileH_BB) << 1
        : Dir == L ? (b & ~FileA_BB) >> 1
        : Dir == UL ? (b & ~FileA_BB) << 7
        : Dir == UR ? (b & ~FileH_BB) << 9
        : Dir == DL ? (b & ~FileA_BB) >> 9
        : Dir == DR ? (b & ~FileH_BB) >> 7
        : 0;
}

template<Color C>
constexpr Bitboard pawn_attacks(Bitboard b) { return C == WHITE ? shift(UL, b) | shift(UR, b) : shift(DL, b) | shift(DR, b); }

static void init() {
    for (Square s1 = A1; s1 <= H8; ++s1)
        for (Square s2 = A1; s2 <= H8; ++s2)
            SquareDistance[s1][s2] = max(distance<File>(s1, s2), distance<Rank>(s1, s2));

    for (Square s = A1; s <= H8; ++s)
        attacks_pawn[WHITE][s] = pawn_attacks<WHITE>(sq_bb(s)),
        attacks_pawn[BLACK][s] = pawn_attacks<BLACK>(sq_bb(s));

    for (Square s = A1; s <= H8; ++s) {
        for (int step : king_steps) attacks_king[s] |= destination(s, step);
        for (int step : knight_steps) attacks_night[s] |= destination(s, step);
    }
}

struct Board {
    Piece board[SQUARE_NB];
    Color turn;
    bool castling_K[COLOR_NB];
    bool castling_Q[COLOR_NB];
    Square en_passant;
    int half_move, full_move;

    Bitboard byTypeBB[PIECE_TYPE_NB];
    Bitboard byColorBB[COLOR_NB];
    Bitboard byAttackBB[COLOR_NB];
    Bitboard existBB;

    Bitboard sliding_attacks(Piece_type pt, Square s, Bitboard existBB = Bitboard(0)) {
        Bitboard attacks(0);
        const Direction* dir = (pt == ROOK ? dir_straight : dir_diagonal);
        Fori(4) {
            Square sq = s;
            while (destination(sq, dir[i])) {
                Bitboard nextBB = sq_bb(sq += dir[i]);
                attacks |= nextBB;
                if (nextBB & existBB) break;
            }
        }

        return attacks;
    }
    Bitboard get_attacks(Color c, Square s) {
        Piece_type pt = type_of(board[s]);
        return pt == PAWN ? attacks_pawn[c][s]
            : pt == KING ? attacks_king[s]
            : pt == KNIGHT ? attacks_night[s]
            : pt == BISHOP ? sliding_attacks(BISHOP, s, existBB)
            : pt == ROOK ? sliding_attacks(ROOK, s, existBB)
            : pt == QUEEN ? sliding_attacks(BISHOP, s, existBB) | sliding_attacks(ROOK, s, existBB)
            : Bitboard(0);
    }
    void calAttackBB() {
        byAttackBB[WHITE] = byAttackBB[BLACK] = Bitboard(0);
        for (Square s = A1; s <= H8; ++s) {
            if (existBB & sq_bb(s)) {
                Color c = color_of(board[s]);
                byAttackBB[c] |= get_attacks(c, s);
            }
        }
    }

    bool isCheck(Color c) const { return (byTypeBB[KING] & byColorBB[c] & byAttackBB[!c]); }
    void append(vector<Move>* moves, Move m) {
        Board next; next.copy(*this);
        next.play(m);
        if (next.isCheck(turn)) return;

        m.check = next.isCheck(!turn);
        if (sq_bb(m.ori) & byAttackBB[!turn]) m.isAttacked = true;

        Move move = Move(m);
        moves->push_back(move);
    }
    void sliding_moves(vector<Move>* moves, Square ori, const Direction* dir, int size = 4) {
        Color c = (byColorBB[WHITE] & sq_bb(ori)) ? WHITE : BLACK;
        for (int i = 0; i < size; i++) {
            Square dest = ori;
            while (true) {
                Bitboard dest_bb = destination(dest, dir[i]);
                dest += dir[i];
                if (!dest_bb) break;
                if ((existBB & dest_bb)) {
                    if (byColorBB[!c] & dest_bb) append(moves, Move(board[ori], ori, dest, dest));
                    break;
                }
                append(moves, Move(board[ori], ori, dest));
            }
        }
    }
    void one_moves(vector<Move>* moves, Square ori, const int* step) {
        Color c = (byColorBB[WHITE] & sq_bb(ori)) ? WHITE : BLACK;
        for (int i = 0; i < 8; ++i) {
            Square dest = Square(ori + step[i]);
            if (destination(ori, step[i])) {
                Bitboard dest_bb = sq_bb(dest);
                if (dest_bb & existBB) {
                    if (dest_bb & byColorBB[!c])
                        append(moves, Move(board[ori], ori, dest, dest));
                }
                else {
                    append(moves, Move(board[ori], ori, dest));
                }
            }
        }
    }
    vector<Move>* queen(Square s) {
        vector<Move>* moves = new vector<Move>;
        sliding_moves(moves, s, all_direction, 8);
        return moves;
    }
    vector<Move>* rook(Square s) {
        vector<Move>* moves = new vector<Move>;
        sliding_moves(moves, s, dir_straight);
        return moves;
    }
    vector<Move>* bishop(Square s) {
        vector<Move>* moves = new vector<Move>;
        sliding_moves(moves, s, dir_diagonal);
        return moves;
    }
    vector<Move>* knight(Square s) {
        vector<Move>* moves = new vector<Move>;
        one_moves(moves, s, knight_steps);
        return moves;
    }
    vector<Move>* king(Square s) {
        vector<Move>* moves = new vector<Move>;
        one_moves(moves, s, king_steps);

        Color color = color_of(board[s]);
        Bitboard existMask_K = color == WHITE ? (sq_bb(F1) | sq_bb(G1)) : (sq_bb(F8) | sq_bb(G8));
        Bitboard attackMask_Q = color == WHITE ? (sq_bb(C1) | sq_bb(D1)) : (sq_bb(C8) | sq_bb(D8));
        Bitboard existMask_Q = color == WHITE ? (sq_bb(B1) | attackMask_Q) : (sq_bb(B8) | attackMask_Q);

        if (!isCheck(color)) {
            bool exist_K = (existMask_K & existBB) == 0;
            bool exist_Q = (existMask_Q & existBB) == 0;
            bool attack_K = (existMask_K & byAttackBB[!color]) == 0;
            bool attack_Q = (attackMask_Q & byAttackBB[!color]) == 0;
            if (castling_K[color] && exist_K && attack_K)
                moves->push_back(Move(board[s], s, s + RR));
            if (castling_Q[color] && exist_Q && attack_Q)
                moves->push_back(Move(board[s], s, s + LL));
        }

        return moves;
    }
    vector<Move>* pawn(Color c, Square ori) {
        vector<Move>* moves = new vector<Move>;
        Direction dir = (c == WHITE ? U : D);
        Bitboard ori_bb = sq_bb(ori);
        Bitboard dest_bb = shift(dir, ori_bb);
        Bitboard promotion_rank = (c == WHITE ? Rank8_BB : Rank1_BB);
        Bitboard firstMove_rank = (c == WHITE ? Rank2_BB : Rank7_BB);

        // Normal move
        if (~existBB & dest_bb) {
            if (dest_bb & promotion_rank) for (Piece_type promotion : promotion_list)
                append(moves, Move(board[ori], ori, ori + dir, promotion));
            else append(moves, Move(board[ori], ori, ori + dir));

            dest_bb = shift(dir, dest_bb);
            if ((firstMove_rank & ori_bb) && (~existBB & dest_bb))
                append(moves, Move(board[ori], ori, ori + (dir + dir)));
        }

        // Attack move 
        auto attack = [&](Direction dir) -> void {
            dest_bb = destination(ori, dir);
            if (dest_bb & byColorBB[!c]) {
                Square take_sq = ori + dir;
                if (dest_bb & promotion_rank) for (Piece_type promotion : promotion_list)
                    append(moves, Move(board[ori], ori, take_sq, take_sq, promotion));
                else
                    append(moves, Move(board[ori], ori, take_sq, take_sq));
            }
            };
        attack(Direction(dir + R)); attack(Direction(dir + L));

        // En_passant attack
        if (en_passant != NOSQUARE && sq_bb(en_passant) & attacks_pawn[c][ori])
            append(moves, Move(board[ori], ori, en_passant, en_passant + (Direction)(-dir)));

        return moves;
    }
    vector<Move>* GetLegal_moves(Square s) {
        Piece_type pt = type_of(board[s]);
        return pt == QUEEN ? queen(s)
            : pt == ROOK ? rook(s)
            : pt == BISHOP ? bishop(s)
            : pt == KNIGHT ? knight(s)
            : pt == KING ? king(s)
            : pt == PAWN ? pawn(color_of(board[s]), s)
            : new vector<Move>;
    }
    vector<Move>* legal_moves() {
        vector<Move>* legalMoves = new vector<Move>;
        for (Square s = A1; s <= H8; ++s) {
            if (sq_bb(s) & byColorBB[turn]) {
                vector<Move>* moves = GetLegal_moves(s);
                legalMoves->insert(legalMoves->end(), moves->begin(), moves->end());
            }
        }
        return legalMoves;
    }

    void create_piece(Square sq, Piece pc) {
        Bitboard b = sq_bb(sq);
        board[sq] = pc;
        byTypeBB[type_of(pc)] |= b;
        byColorBB[color_of(pc)] |= b;
        existBB |= b;
    }
    void remove_piece(Square sq) {
        Bitboard b = sq_bb(sq);
        Piece pc = board[sq];
        board[sq] = NOPIECE;
        byColorBB[color_of(pc)] &= ~b;
        byTypeBB[type_of(pc)] &= ~b;
        existBB &= ~b;
    };

    void move(const Move& move) {
        Bitboard ori_bb = sq_bb(move.ori), dest_bb = sq_bb(move.dest);
        Piece_type pt = type_of(move.piece);
        Color color = color_of(move.piece);

        board[move.dest] = board[move.ori], board[move.ori] = NOPIECE;
        byTypeBB[pt] &= ~ori_bb; byTypeBB[pt] |= dest_bb;
        byColorBB[color] &= ~ori_bb; byColorBB[color] |= dest_bb;
        existBB &= ~ori_bb; existBB |= dest_bb;
    }
    void capture(Move move) {
        remove_piece(move.take);
        Board::move(move);
    }
    void castling(Move move) {
        Square ori(A1), dest(D1);
        if (color_of(move.piece) == BLACK) ori = A8, dest = D8;
        if (move.ori < move.dest) ori += 7, dest += 2;
        Board::move(Move((Piece)((color_of(move.piece) << 3) + ROOK), ori, dest));
        Board::move(move);
    }
    void promotion(Move move) {
        move.take == NOSQUARE ? Board::move(move) : capture(move);
        remove_piece(move.dest);
        create_piece(move.dest, (Piece)(move.promotion + (color_of(move.piece) << 3)));
    }
    void play(Move move) {
        int type;
        if (type_of(move.piece) == KING && abs(move.ori - move.dest) == 2) type = 2; // castling
        else {
            if (move.take == NOPIECE && move.promotion == NOPIECETYPE) type = 0; // move
            else if (move.take != NOPIECE && move.promotion == NOPIECETYPE) type = 1; // take
            else if (move.take == NOPIECE && move.promotion != NOPIECETYPE) type = 3; // move promotion
            else if (move.take != NOPIECE && move.promotion != NOPIECETYPE) type = 3; // take promotion
            else {
                cout << "Move command ERROR\n";
                exit(0);
            }
        }

        switch (type) {
        case 0: Board::move(move); break;
        case 1: capture(move); break;
        case 2: castling(move); break;
        case 3: promotion(move); break;
        default: break;
        }

        Piece_type pt = type_of(move.piece);
        Color color = color_of(move.piece);

        if (pt == PAWN && distance(move.ori, move.dest) == 2)
            en_passant = (Square)(move.ori + (color == WHITE ? U : D));
        else en_passant = NOSQUARE;

        if (pt == KING) castling_K[color] = castling_Q[color] = false;
        else if (move.ori == A1 || move.dest == A1) castling_Q[WHITE] = false;
        else if (move.ori == A8 || move.dest == A8) castling_Q[BLACK] = false;
        else if (move.ori == H1 || move.dest == H1) castling_K[WHITE] = false;
        else if (move.ori == H8 || move.dest == H8) castling_K[BLACK] = false;

        ++half_move;
        if (color == BLACK) ++full_move;

        turn = !turn;
        calAttackBB();
    }

    bool isEnd() {
        for (Square s = A1; s <= H8; ++s) {
            if (sq_bb(s) & byColorBB[turn]) {
                vector<Move>* moves = GetLegal_moves(s);
                if (moves->size() != 0) return false;
            }
        }
        return true;
    }

    void init(string FEN) {
        stringstream ss(FEN);
        string word;
        vector<string> words;
        while (ss >> word) words.push_back(word);

        if (words.size() != 6) {
            cout << "init():: input FEN Format ERROR - FEN size is not 6\n";
            exit(0);
        }

        int idx(0);
        memset(board, NOPIECE, sizeof(board));
        memset(byTypeBB, Bitboard(0), sizeof(byTypeBB));
        memset(byColorBB, Bitboard(0), sizeof(byColorBB));
        memset(byAttackBB, Bitboard(0), sizeof(byAttackBB));
        existBB = Bitboard(0);
        for (Square rank : {A8, A7, A6, A5, A4, A3, A2, A1 }) {
            int file(0);
            while (idx < words[0].size()) {
                char c = words[0][idx++];
                if (c == '/') break;

                if (isdigit(c)) file += (c - '0');
                else {
                    int color;
                    isupper(c) ? color = 0 : color = 8, c = toupper(c);
                    Piece pc = (Piece)
                        (c == 'Q' ? color
                            : c == 'R' ? color + 1
                            : c == 'B' ? color + 2
                            : c == 'N' ? color + 3
                            : c == 'K' ? color + 4
                            : color + 5);
                    create_piece((Square)(rank + file), pc);
                    ++file;
                }
            }
        }

        if (words[1].size() != 1) {
            cout << "init():: input FEN Format ERROR - turn data size is not 1\n";
            exit(0);
        }

        turn = (words[1][0] == 'w' ? WHITE : BLACK);

        idx = 0;
        memset(castling_K, false, sizeof(castling_K));
        memset(castling_Q, false, sizeof(castling_Q));
        while (idx < words[2].size()) {
            char c = words[2][idx++];
            if (c == '-') break;

            Color color = isupper(c) ? WHITE : BLACK;
            c = toupper(c);

            c == 'K' ? castling_K[color] = true : castling_Q[color] = true;
        }

        en_passant = (words[3][0] == '-' ? NOSQUARE : notation_sq(words[3]));

        half_move = stoi(words[4]);
        full_move = stoi(words[5]);

        calAttackBB();
    }
    void copy(const Board& other) {
        memcpy(board, other.board, sizeof(board));
        turn = other.turn;
        memcpy(castling_K, other.castling_K, sizeof(castling_K));
        memcpy(castling_Q, other.castling_Q, sizeof(castling_Q));
        en_passant = other.en_passant;
        half_move = other.half_move;
        full_move = other.full_move;

        memcpy(byTypeBB, other.byTypeBB, sizeof(byTypeBB));
        memcpy(byColorBB, other.byColorBB, sizeof(byColorBB));
        memcpy(byAttackBB, other.byAttackBB, sizeof(byAttackBB));
        existBB = other.existBB;
    }

    void print(bool showLegalMoveList = false) {
        auto add_sp = [&](int n) -> string {
            return to_string(n) + (n < 10 ? "   " : n < 100 ? "  " : " ");
            };

        for (Square A : {A8, A7, A6, A5, A4, A3, A2, A1 }) {
            Rank rank = rank_of(A);
            Fori(8) {
                Square sq = (Square)(A + i);
                File file = file_of(sq);
                int color_back = (((rank & 1) ^ (file & 1)) ? 6 : 2);
                int color_text = board[sq] != NOPIECE && color_of(board[sq]) == WHITE ? 15 : 0;
                setColor(color_text, color_back);
                cout << (board[sq] == NOPIECE ? ' ' : pt_char[type_of(board[sq])]);
                cout << ' ';
            }

            setColor(0, 15);
            if (rank == RANK_8) {
                cout << " [Turn] " << (turn == WHITE ? "WHITE" : "BLACK") << "    ";
            }
            else if (rank == RANK_7) {
                cout << " [Castling] ";
                cout << (castling_K[WHITE] ? 'K' : '-');
                cout << (castling_Q[WHITE] ? 'Q' : '-');
                cout << (castling_K[BLACK] ? 'k' : '-');
                cout << (castling_Q[BLACK] ? 'q' : '-');
                cout << ' ';
            }
            else if (rank == RANK_6) {
                cout << " [En Passant] " << sq_notation(en_passant) << " ";
            }
            else if (rank == RANK_5) {
                cout << " [Half Move] " << add_sp(half_move);
            }
            else if (rank == RANK_4) {
                cout << " [Full Move] " << add_sp(full_move);
            }
            setColor(15, 0);
            cout << "\n";
        }
        cout << "\n";

        if (showLegalMoveList) {
            vector<Move>* legalMoves = legal_moves();
            cout << "[Candidate moves] (" << legalMoves->size() << ")\n";
            Fori(legalMoves->size()) {
                cout << i << ". " << sq_notation(legalMoves->at(i).ori) << " -> " << sq_notation(legalMoves->at(i).dest);
                if (legalMoves->at(i).take != NOSQUARE) cout << " Takes " << sq_notation(legalMoves->at(i).take);
                if (legalMoves->at(i).promotion != NOPIECETYPE) cout << " Promotion to " << pt_char[legalMoves->at(i).promotion];
                cout << " \n";
            }
        }
    }
};

Move READ_move(const vector<Move>* moves) {
    string notation; cin >> notation;

    Square ori(notation_sq(notation.substr(0, 2)));
    Square dest(notation_sq(notation.substr(2, 2)));
    Piece_type promotion = char_pt(notation[4]);

    for (const Move& move : *moves)
        if (move.ori == ori && move.dest == dest && move.promotion == promotion)
            return move;

    return Move();
}
void SEND_move(const Move& move) {
    cout << "SEND ";
    cout << color_of(move.piece);
    cout << type_of(move.piece);
    cout << sq_notation(move.ori) << sq_notation(move.dest) << sq_notation(move.take);
    if (move.promotion == NOPIECETYPE) cout << '-';
    else cout << pt_char[move.promotion];
    cout << "\n";
}

class Engine {
public:
    Board board;
    Color color_AI;
    const int BASIC_DEPTH = 5;
    int current_depth = BASIC_DEPTH;

    // Queen, Rook, Bishop, Knight, King, Pawn
    const int piece_value[PIECE_TYPE_NB]{ 929, 479, 320, 280, 60000, 100 };
    int PST[PIECE_TYPE_NB][COLOR_NB][SQUARE_NB]{
        // QUEEN
        {
            // WHITE
            {
               -39, -30, -31, -13, -31, -36, -34, -42,
               -36, -18,   0, -19, -15, -15, -21, -38,
               -30,  -6, -13, -11, -16, -11, -16, -27,
               -14, -15,  -2,  -5,  -1, -10, -20, -22,
                1, -16,  22,  17,  25,  20, -13,  -6,
               -2,  43,  32,  60,  72,  63,  43,   2,
               14,  32,  60, -10,  20,  76,  57,  24,
                6,   1,  -8,-104,  69,  24,  88,  26
            },
        // BLACK
        {
            6,   1,  -8,-104,  69,  24,  88,  26,
           14,  32,  60, -10,  20,  76,  57,  24,
           -2,  43,  32,  60,  72,  63,  43,   2,
            1, -16,  22,  17,  25,  20, -13,  -6,
           -14, -15,  -2,  -5,  -1, -10, -20, -22,
           -30,  -6, -13, -11, -16, -11, -16, -27,
           -36, -18,   0, -19, -15, -15, -21, -38,
           -39, -30, -31, -13, -31, -36, -34, -42
        }
    },
        // ROOK
        {
            // WHITE
            {
               -30, -24, -18,   5,  -2, -18, -31, -32,
               -53, -38, -31, -26, -29, -43, -44, -53,
               -42, -28, -42, -25, -25, -35, -26, -46,
               -28, -35, -16, -21, -13, -29, -46, -30,
                0,   5,  16,  13,  18,  -4,  -9,  -6,
               19,  35,  28,  33,  45,  27,  25,  15,
               55,  29,  56,  67,  55,  62,  34,  60,
               35,  29,  33,   4,  37,  33,  56,  50
            },
        // BLACK
        {
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0
        }
    },
        // BISHOP
        {
            // WHITE
            {
               -7,   2, -15, -12, -14, -15, -10, -10,
               19,  20,  11,   6,   7,   6,  20,  16,
               14,  25,  24,  15,   8,  25,  20,  15,
               13,  10,  17,  23,  17,  16,   0,   7,
               25,  17,  20,  34,  26,  25,  15,  10,
               -9,  39, -32,  41,  52, -10,  28, -14,
               -11,  20,  35, -42, -39,  31,   2, -22,
               -59, -78, -82, -76, -23,-107, -37, -50
            },
        // BLACK
        {
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0
        }
    },
        // KNIGHT
        {
            // WHITE
            {
               -74, -23, -26, -24, -19, -35, -22, -69,
               -23, -15,   2,   0,   2,   0, -23, -20,
               -18,  10,  13,  22,  18,  15,  11, -14,
               -1,   5,  31,  21,  22,  35,   2,   0,
               24,  24,  45,  37,  33,  41,  25,  17,
               10,  67,   1,  74,  73,  27,  62,  -2,
               -3,  -6, 100, -36,   4,  62,  -4, -14,
               -66, -53, -75, -75, -10, -55, -58, -70,
            },
            // BLACK
            {
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0
            }
        },
        // KING
        {
            // WHITE
            {
               17,  30,  -3, -14,   6,  -1,  40,  18,
               -4,   3, -14, -50, -57, -18,  13,   4,
               -47, -42, -43, -79, -64, -32, -29, -32,
               -55, -43, -52, -28, -51, -47,  -8, -50,
               -55,  50,  11,  -4, -19,  13,   0, -49,
               -62,  12, -57,  44, -67,  28,  37, -31,
               -32,  10,  55,  56,  56,  55,  10,   3,
                4,  54,  47, -99, -99,  60,  83, -62
            },
        // BLACK
        {
            4,  54,  47, -99, -99,  60,  83, -62,
           -32,  10,  55,  56,  56,  55,  10,   3,
           -62,  12, -57,  44, -67,  28,  37, -31,
           -55,  50,  11,  -4, -19,  13,   0, -49,
           -55, -43, -52, -28, -51, -47,  -8, -50,
           -47, -42, -43, -79, -64, -32, -29, -32,
           -4,   3, -14, -50, -57, -18,  13,   4,
           17,  30,  -3, -14,   6,  -1,  40,  18
        }
    },
        // PAWN
        {
            // WHITE
            {
                0,   0,   0,   0,   0,   0,   0,   0,
               -31,   8,  -7, -37, -36, -14,   3, -31,
               -22,   9,   5, -11, -10,  -2,   3, -19,
               -26,   3,  10,   9,   6,   1,   0, -23,
               -17,  16,  -2,  15,  14,   0,  15, -13,
                7,  29,  21,  44,  40,  31,  44,   7,
               78,  83,  86,  73, 102,  82,  85,  90,
                0,   0,   0,   0,   0,   0,   0,   0
            },
        // BLACK
        {
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0
        }
    }
    };
    void init_pst() {
        Fori(6) if (i != QUEEN && i != KING) for (Square sq = A1; sq < SQUARE_NB; ++sq)
            PST[i][BLACK][sq] = PST[i][WHITE][(int)((SQUARE_NB - 1) - sq)];
    }

    Engine(Color c) { // for game 
        init();
        init_pst();
        board.init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        color_AI = c;
    }
    Engine(string FEN) { // for puzzle
        init();
        init_pst();
        board.init(FEN);
        color_AI = board.turn;
        play();
    }
    int bitCount(Bitboard b) {
        int cnt(0);
        while (b) {
            b = b & (b - 1);
            ++cnt;
        }
        return cnt;
    }

    int evaluation_pieceValue() {
        int score[COLOR_NB]{ 0, 0 };
        int bishop[COLOR_NB]{ 0, 0 };
        for (Square sq = A1; sq < SQUARE_NB; ++sq) {
            if (sq_bb(sq) & board.existBB) {
                Piece_type pt = type_of(board.board[sq]);
                Color color = color_of(board.board[sq]);
                if (pt == BISHOP) ++bishop[color];
                score[color] += piece_value[pt] + PST[pt][color][sq];
            }
        }

        if (bishop[WHITE] >= 2) score[WHITE] += 55;
        if (bishop[BLACK] >= 2) score[BLACK] += 55;

        return score[WHITE] - score[BLACK];
    }
    int evaluation_castling() {
        int score[COLOR_NB]{ 0, 0 };
        for (Color color : {WHITE, BLACK}) {
            if (board.castling_K[color]) score[color] += 50;
            if (board.castling_Q[color]) score[color] += 50;
        }
        return score[WHITE] - score[BLACK];
    }
    int evaluation_pawnStructure() {
        int score[COLOR_NB]{ 0, 0 };
        int pawnCount[COLOR_NB][10]{ 
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        };
        int idx(1);
        for (Bitboard fileBB : {FileA_BB, FileB_BB, FileC_BB, FileD_BB, FileE_BB, FileF_BB, FileG_BB, FileH_BB}) {
            for (Color color : {WHITE, BLACK})
                pawnCount[color][idx] += bitCount(fileBB & board.byColorBB[color] & board.byTypeBB[PAWN]);
            ++idx;
        }

        for (int i = 1; i <= 8; ++i) {
            for (Color color : {WHITE, BLACK}) {
                // Isolation Pawn
                int isolation(0);
                if (!pawnCount[color][i - 1] && pawnCount[color][i] && !pawnCount[color][i + 1])
                    ++isolation;
                score[color] -= isolation * 20;

                // Doubled Pawn
                if (pawnCount[color][i] >= 2) score[color] -= 20;

                // Passed Pawns
                // TODO -> forwarding pass pawn calculation
                int passed(0);
                if (!pawnCount[!color][i - 1] && pawnCount[color][i] && !pawnCount[!color][i + 1])
                    ++passed;
                score[color] += passed * 30;
            }
        }

        return score[WHITE] - score[BLACK];
    }
    int evaluation() {
        int score(0);
        score += evaluation_pieceValue();
        score += evaluation_castling();
        score += evaluation_pawnStructure();
        return score;
    }

    int alpha_beta(int alpha, int beta, int depth) {
        if (board.isEnd())
            return (!board.isCheck(board.turn) ? 0 : board.turn == BLACK ? 1e9 - depth : -1e9 + depth);

        if (depth == current_depth) return evaluation();

        int resultEval;
        vector<Move>* legalMoves = board.legal_moves();
        sort(legalMoves->begin(), legalMoves->end(), move_comp);
        if (board.turn == WHITE) {
            resultEval = -2e9;
            for (const Move& move : *legalMoves) {
                int nextEval = search(move, alpha, beta, depth);
                resultEval = max(resultEval, nextEval);
                alpha = max(alpha, nextEval);
                if (beta <= alpha) break;
            }
        }
        else {
            resultEval = 2e9;
            for (const Move& move : *legalMoves) {
                int nextEval = search(move, alpha, beta, depth);
                resultEval = min(resultEval, nextEval);
                beta = min(beta, nextEval);
                if (beta <= alpha) break;
            }
        }
        return resultEval;
    }
    int search(Move move, int alpha, int beta, int depth) {
        //Fori(depth) cout << "-";
        //cout << move_notation(move) << "\n";
        Board temp; temp.copy(board);

        board.play(move);
        int score = alpha_beta(alpha, beta, depth + 1);

        board.copy(temp);
        return score;
    }
    double findTime = 0.0;
    pair<Move, string> findBestMove(int depth = 0) {

        auto start = chrono::high_resolution_clock::now();
        vector<Move>* legalMoves = board.legal_moves();
        sort(legalMoves->begin(), legalMoves->end(), move_comp);
        pair<Move, string> moveData;
        current_depth = BASIC_DEPTH;

        // opening e4 fix
        if (color_AI == WHITE && board.full_move == 1) {
            moveData.first = Move(W_PAWN, E2, E4);
            moveData.second = "e4";
            return moveData;
        }

        while (true) {
            int bestEval = color_AI == WHITE ? -2e9 : 2e9;
            int alpha(-2e9), beta(2e9);
            for (const Move& move : *legalMoves) {
                // cout << move_notation(legalMoves, move) << "\n";
                int nextEval = search(move, alpha, beta, depth);
                if ((color_AI == WHITE && bestEval < nextEval) ||
                    (color_AI == BLACK && bestEval > nextEval)) {
                    bestEval = nextEval;
                    moveData.first = move;
                    moveData.second = move_notation(legalMoves, move);
                }

                color_AI == WHITE ? alpha = max(alpha, bestEval) : beta = min(beta, bestEval);
            }
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            findTime = duration / 1000.0;
            if (findTime < 2.0) {
                ++current_depth;
                // cout << "++current_depth\n";
            }
            else break;
        }

        return moveData;
    }

    int play_test() {
        cout << fixed; cout.precision(2);
        while (true) {
            if (board.turn == color_AI) {
                board.print(false);
                cout << "Current evaluation : " << evaluation() << "\n\n";
                pair<Move, string> moveData = findBestMove();
                Move bestMove = moveData.first;
                board.play(bestMove);
                cout << "[ " << findTime << " sec / Depth : " << current_depth << " ]\n";
                cout << "Forest Raven >> " << moveData.second << "\n\n";
            }
            else {
                board.print(false);
                cout << "Current evaluation : " << evaluation() << "\n\n";
                vector<Move>* legalMoves = board.legal_moves();
                Move move;
                while (move.piece == NOPIECE) {
                    cout << "input >> ";
                    string st; cin >> st;
                    if (st == "RESTART") return 1;
                    move = notation_move(legalMoves, st);
                }

                board.play(move);
                cout << "\n";
            }

            if (board.isEnd()) {
                cout << "GAME OVER\n";
                if (!board.isCheck(board.turn)) cout << "DRAW";
                else cout << (board.turn == BLACK ? "WHITE " : "BLACK ") << "WIN\n";
                board.print(true);
                break;
            }
        }
        return 0;
    }
    int play() {
        cout << fixed; cout.precision(2);
        while (true) {
            if (board.turn == color_AI) {
                pair<Move, string> moveData = findBestMove();
                Move bestMove = moveData.first;
                SEND_move(bestMove);
                board.play(bestMove);
                cout << "[ " << findTime << " sec / Depth : " << current_depth << " ]\n";
                cout << "Forest Raven >> " << moveData.second << "\n\n";
            }
            else {
                vector<Move>* legalMoves = board.legal_moves();
                Move move = READ_move(legalMoves);
                board.play(move);
                cout << "\n";
            }

            if (board.isEnd()) {
                cout << "GAME OVER\n";
                if (!board.isCheck(board.turn)) cout << "DRAW";
                else cout << (board.turn == BLACK ? "WHITE " : "BLACK ") << "WIN\n";
                board.print(true);
                break;
            }
        }
        return 0;
    }
};

int main(void) {
    int c; cin >> c; string t; getline(cin, t);
    Engine engine((Color)c);
    engine.play();
    //Engine engine(WHITE);
    //engine.play_test();
}
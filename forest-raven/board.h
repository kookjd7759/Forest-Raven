#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "connector.h"
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

static void setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int colorAttribute = (bgColor << 4) | textColor;
    SetConsoleTextAttribute(hConsole, colorAttribute);
}

namespace ForestRaven {
    static void print_BB(Bitboard b) {
        for (int i = 63; i >= 0; --i) {
            cout << ((b >> i) & 1) << ' ';
            if (i % 8 == 0) cout << "\n";
        } cout << "\n";
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

    uint8_t  bit16cnt[1 << 16];
    uint8_t  SquareDistance[SQUARE_NB][SQUARE_NB];
    Bitboard attacks_pawn[COLOR_NB][SQUARE_NB];
    Bitboard attacks_night[SQUARE_NB];
    Bitboard attacks_king[SQUARE_NB];

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

    template<typename T1 = Square> inline int distance(Square x, Square y);
    template<> inline int distance<File>(Square x, Square y) { return abs(file_of(x) - file_of(y)); }
    template<> inline int distance<Rank>(Square x, Square y) { return abs(rank_of(x) - rank_of(y)); }
    template<> inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }

    Bitboard destination(Square ori, int step) {
        Square dest = Square(ori + step);
        return is_ok(dest) && distance(ori, dest) <= 2 ? sq_bb(dest) : Bitboard(0);
    }

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

        bool isCheck(Color c) const { return (byTypeBB[KING] & byColorBB[c] & byAttackBB[~c]); }
        void append(vector<Move>* moves, Move m) {
            Board next; next.copy(*this);
            next.play(m);
            if (next.isCheck(turn)) return;

            m.check = next.isCheck(~turn);
            if (sq_bb(m.ori) & byAttackBB[~turn]) m.isAttacked = true;

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
                        if (byColorBB[~c] & dest_bb) append(moves, Move(board[ori], ori, dest, dest));
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
                        if (dest_bb & byColorBB[~c])
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
                bool attack_K = (existMask_K & byAttackBB[~color]) == 0;
                bool attack_Q = (attackMask_Q & byAttackBB[~color]) == 0;
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
                if (dest_bb & byColorBB[~c]) {
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

            turn = ~turn;
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

            en_passant = (words[3][0] == '-' ? NOSQUARE : nt_sq(words[3]));

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
                    cout << " [En Passant] " << sq_nt(en_passant) << " ";
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
                    cout << i << ". " << sq_nt(legalMoves->at(i).ori) << " -> " << sq_nt(legalMoves->at(i).dest);
                    if (legalMoves->at(i).take != NOSQUARE) cout << " Takes " << sq_nt(legalMoves->at(i).take);
                    if (legalMoves->at(i).promotion != NOPIECETYPE) cout << " Promotion to " << pt_char[legalMoves->at(i).promotion];
                    cout << " \n";
                }
            }
        }
    };

}

#endif  // #ifndef BOARD_H_INCLUDED
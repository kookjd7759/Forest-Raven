#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "connector.h"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <bitset>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

static void setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int colorAttribute = (bgColor << 4) | textColor;
    SetConsoleTextAttribute(hConsole, colorAttribute);
}

pair<int, int> LegalMove_ct{ 0, 0 };
pair<int, int> Eval_ct{ 0, 0 };
pair<int, int> IsEnd_ct{ 0, 0 };
pair<int, int> calAttackBB_ct{ 0, 0 };

namespace ForestRaven {
    static void print_BB(Bitboard b) {
        for (Square A : {A8, A7, A6, A5, A4, A3, A2, A1 }) {
            Rank rank = rank_of(A);
            Fori(8) {
                Square sq = (Square)(A + i);
                if (sq_bb(sq) & b) cout << "1 ";
                else cout << "0 ";
            }
            cout << "\n";
        }
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

    constexpr Bitboard rank_bb(Rank r) { return Rank1_BB << (8 * r); }
    constexpr Bitboard rank_bb(Square s) { return rank_bb(rank_of(s)); }

    constexpr Bitboard file_bb(File f) { return FileA_BB << f; }
    constexpr Bitboard file_bb(Square s) { return file_bb(file_of(s)); }

    constexpr Direction dir_straight[4] = { U, R, D, L };
    constexpr Direction dir_diagonal[4] = { UL, UR, DL, DR };
    constexpr Direction all_direction[8] = { U, R, D, L, UL, UR, DL, DR };

    constexpr int knight_steps[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
    constexpr int king_steps[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };

    uint8_t  bit16cnt[1 << 16];
    uint8_t  SquareDistance[SQUARE_NB][SQUARE_NB];

    Bitboard LineBB[SQUARE_NB][SQUARE_NB];
    Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];
    Bitboard attacks_pseudo[PIECE_TYPE_NB][SQUARE_NB];
    Bitboard attacks_pawn[COLOR_NB][SQUARE_NB];

    inline int bitCount(Bitboard b) {
        union {
            Bitboard bb;
            uint16_t u[4];
        } v = { b };
        return bit16cnt[v.u[0]] + bit16cnt[v.u[1]] + bit16cnt[v.u[2]] + bit16cnt[v.u[3]];
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

    template<typename T1 = Square> inline int distance(Square x, Square y);
    template<> inline int distance<File>(Square x, Square y) { return abs(file_of(x) - file_of(y)); }
    template<> inline int distance<Rank>(Square x, Square y) { return abs(rank_of(x) - rank_of(y)); }
    template<> inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }

    Bitboard destination(Square ori, int step) {
        Square dest = Square(ori + step);
        return is_ok(dest) && distance(ori, dest) <= 2 ? sq_bb(dest) : Bitboard(0);
    }

    Bitboard RookTable[0x19000];
    Bitboard BishopTable[0x1480];

    struct Magic {
        Bitboard  mask;
        Bitboard* attacks;
#ifndef USE_PEXT
        Bitboard magic;
        unsigned shift;
#endif

        // Compute the attack's index using the 'magic bitboards' approach
        unsigned index(Bitboard occupied) const {

#ifdef USE_PEXT
            return unsigned(pext(occupied, mask));
#else
            if (Is64Bit)
                return unsigned(((occupied & mask) * magic) >> shift);

            unsigned lo = unsigned(occupied) & unsigned(mask);
            unsigned hi = unsigned(occupied >> 32) & unsigned(mask >> 32);
            return (lo * unsigned(magic) ^ hi * unsigned(magic >> 32)) >> shift;
#endif
        }

        Bitboard attacks_bb(Bitboard occupied) const { return attacks[index(occupied)]; }
    };

    alignas(64) Magic Magics[SQUARE_NB][2];

    class PRNG {
        uint64_t s;
        uint64_t rand64() {
            s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
            return s * 2685821657736338717LL;
        }

    public:
        PRNG(uint64_t seed) : s(seed) { assert(seed); }

        template<typename T>
        T rand() { return T(rand64());  }

        template<typename T>
        T sparse_rand() {  return T(rand64() & rand64() & rand64()); }
    };

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

    void init_magics(Piece_type pt, Bitboard table[], Magic magics[][2]) {
#ifndef USE_PEXT
        int seeds[][RANK_NB] = { {8977, 44560, 54343, 38998, 5731, 95205, 104912, 17020},
                                {728, 10316, 55013, 32803, 12281, 15100, 16645, 255} };

        Bitboard* occupancy = new Bitboard[4096];
        int* epoch = new int[4096] {}, cnt(0);
#endif
        Bitboard* reference = new Bitboard[4096];
        int      size = 0;

        for (Square s = A1; s <= H8; ++s) {
            Bitboard edges = ((Rank1_BB | Rank8_BB) & ~rank_bb(s)) | ((FileA_BB | FileH_BB) & ~file_bb(s));

            Magic& m = magics[s][BISHOP - pt];
            m.mask = sliding_attacks(pt, s, 0) & ~edges;
#ifndef USE_PEXT
            m.shift = (Is64Bit ? 64 : 32) - bitCount(m.mask);
#endif
            m.attacks = s == A1 ? table : magics[s - 1][BISHOP - pt].attacks + size;
            size = 0;

            Bitboard b = 0;
            do {
#ifndef USE_PEXT
                occupancy[size] = b;
#endif
                reference[size] = sliding_attacks(pt, s, b);

                if (HasPext)
                    m.attacks[pext(b, m.mask)] = reference[size];

                size++;
                b = (b - m.mask) & m.mask;
            } while (b);

#ifndef USE_PEXT
            PRNG rng(seeds[Is64Bit][rank_of(s)]);

            for (int i = 0; i < size;) {
                for (m.magic = 0; bitCount((m.magic * m.mask) >> 56) < 6;) {
                    m.magic = rng.sparse_rand<Bitboard>();
                }

                for (++cnt, i = 0; i < size; ++i) {
                    unsigned idx = m.index(occupancy[i]);

                    if (epoch[idx] < cnt) {
                        epoch[idx] = cnt;
                        m.attacks[idx] = reference[i];
                    }
                    else if (m.attacks[idx] != reference[i])
                        break;
                }
            }
#endif
        }
    }

    template<Piece_type Pt>
    inline Bitboard attacks_bb(Square s) {
        assert((Pt != PAWN) && (is_ok(s)));
        return attacks_pseudo[Pt][s];
    }
    template<Piece_type Pt>
    inline Bitboard attacks_bb(Square s, Bitboard occupied) {
        assert((Pt != PAWN) && (is_ok(s)));

        switch (Pt) {
        case BISHOP:
        case ROOK:
            return Magics[s][BISHOP - Pt].attacks_bb(occupied);
        case QUEEN:
            return attacks_bb<BISHOP>(s, occupied) | attacks_bb<ROOK>(s, occupied);
        default:
            return attacks_pseudo[Pt][s];
        }
    }
    inline Bitboard attacks_bb(Piece_type pt, Square s, Bitboard occupied) {
        assert((pt != PAWN) && (is_ok(s)));

        switch (pt) {
        case BISHOP:
            return attacks_bb<BISHOP>(s, occupied);
        case ROOK:
            return attacks_bb<ROOK>(s, occupied);
        case QUEEN:
            return attacks_bb<BISHOP>(s, occupied) | attacks_bb<ROOK>(s, occupied);
        default:
            return attacks_pseudo[pt][s];
        }
    }

    static void init() {
        for (unsigned i = 0; i < (1 << 16); ++i)
            bit16cnt[i] = uint8_t(std::bitset<16>(i).count());

        for (Square s1 = A1; s1 <= H8; ++s1)
            for (Square s2 = A1; s2 <= H8; ++s2)
                SquareDistance[s1][s2] = max(distance<File>(s1, s2), distance<Rank>(s1, s2));

        init_magics(ROOK, RookTable, Magics);
        init_magics(BISHOP, BishopTable, Magics);

        for (Square s1 = A1; s1 <= H8; ++s1) {
            attacks_pawn[BLACK][s1] = pawn_attacks<BLACK>(sq_bb(s1));
            attacks_pawn[WHITE][s1] = pawn_attacks<WHITE>(sq_bb(s1));

            for (int step : king_steps) attacks_pseudo[KING][s1] |= destination(s1, step);
            for (int step : knight_steps) attacks_pseudo[KNIGHT][s1] |= destination(s1, step);

            attacks_pseudo[QUEEN][s1] = attacks_pseudo[BISHOP][s1] = attacks_bb<BISHOP>(s1, 0);
            attacks_pseudo[QUEEN][s1] |= attacks_pseudo[ROOK][s1] = attacks_bb<ROOK>(s1, 0);
        }

        for (Square s1 = A1; s1 <= H8; ++s1) {
            for (Square s2 = A1; s2 <= H8; ++s2) {
                if (attacks_pseudo[ROOK][s1] & sq_bb(s2)) {
                    LineBB[s1][s2] = (attacks_bb(ROOK, s1, sq_bb(s2)) & attacks_bb(ROOK, s2, sq_bb(s1)));
                }
                if (attacks_pseudo[BISHOP][s1] & sq_bb(s2)) {
                    BetweenBB[s1][s2] = (attacks_bb(BISHOP, s1, sq_bb(s2)) & attacks_bb(BISHOP, s2, sq_bb(s1)));
                }
            }
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

        Bitboard pinned;

        inline Square lsb(Bitboard b) {
            unsigned long idx;
            _BitScanForward64(&idx, b);
            return Square(idx);
        }
        inline Square pop_lsb(Bitboard& b) {
            assert(b);
            const Square s = lsb(b);
            b &= b - 1;
            return s;
        }
        void calAttackBB() {
            auto start = chrono::high_resolution_clock::now();

            byAttackBB[WHITE] = byAttackBB[BLACK] = pinned = Bitboard(0);
            Bitboard temp(existBB);

            while (temp) {
                Square s = pop_lsb(temp);
                Piece_type pt = type_of(board[s]);
                Color c = color_of(board[s]);
                byAttackBB[c] |= (pt == PAWN ? attacks_pawn[c][s] : attacks_bb(pt, s, existBB));
                
                if (pt == BISHOP || pt == ROOK || pt == QUEEN) { // process to find pinned pieces 
                    Square ksq = lsb(byTypeBB[KING] & byColorBB[~c]); // opponent king's position
                    if (pt == BISHOP || pt == QUEEN) {
                        Bitboard mid = BetweenBB[s][ksq] & existBB;
                        if (bitCount(mid) == 1 && mid & byColorBB[~c]) pinned |= mid;
                    }
                    if (pt == ROOK || pt == QUEEN) {
                        Bitboard mid = LineBB[s][ksq] & existBB;
                        if (bitCount(mid) == 1 && mid & byColorBB[~c]) pinned |= mid;
                    }
                }   
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            ++calAttackBB_ct.first; calAttackBB_ct.second += duration;
        }

        bool isCheck(Color c) const { return (byTypeBB[KING] & byColorBB[c] & byAttackBB[~c]); }
        void append(vector<Move>* moves, Move m) {
            if (isCheck(turn)) {
                Board next; next.copy(*this);
                next.play(m);

                if (next.isCheck(turn)) return;
            }
            else if (sq_bb(m.ori) & pinned) {
                if (m.take == NOSQUARE) return;

                Board next; next.copy(*this);
                next.play(m);

                if (next.isCheck(turn)) return;
            }

            if (sq_bb(m.ori) & byAttackBB[~turn]) m.isAttacked = true;

            Move move = Move(m);
            moves->push_back(move);
        }
        void moves(vector<Move>* moves, Piece_type pt, Square ori) {
            assert((pt != PAWN) && (is_ok(ori)));

            Bitboard destBB(attacks_bb(pt, ori, existBB));
            destBB &= ~byColorBB[turn];

            while (destBB) {
                Square dest = pop_lsb(destBB);
                if (existBB & sq_bb(dest)) append(moves, Move(board[ori], ori, dest, dest));
                else append(moves, Move(board[ori], ori, dest));
            }
        }
        void king_moves(vector<Move>* legalMoves, Square s) {
            moves(legalMoves, KING, s);

            Color color = color_of(board[s]);
            Bitboard existMask_K(color == WHITE ? (sq_bb(F1) | sq_bb(G1)) : (sq_bb(F8) | sq_bb(G8)));
            Bitboard attackMask_Q(color == WHITE ? (sq_bb(C1) | sq_bb(D1)) : (sq_bb(C8) | sq_bb(D8)));
            Bitboard existMask_Q(color == WHITE ? (sq_bb(B1) | attackMask_Q) : (sq_bb(B8) | attackMask_Q));

            if (!isCheck(color)) {
                bool exist_K = (existMask_K & existBB) == 0;
                bool exist_Q = (existMask_Q & existBB) == 0;
                bool attack_K = (existMask_K & byAttackBB[~color]) == 0;
                bool attack_Q = (attackMask_Q & byAttackBB[~color]) == 0;
                if (castling_K[color] && exist_K && attack_K)
                    legalMoves->push_back(Move(board[s], s, s + RR));
                if (castling_Q[color] && exist_Q && attack_Q)
                    legalMoves->push_back(Move(board[s], s, s + LL));
            }
        }
        void pawn_moves(vector<Move>* legalMoves, Color c, Square ori) {
            Direction dir(c == WHITE ? U : D);
            Bitboard ori_bb = sq_bb(ori);
            Bitboard dest_bb = shift(dir, ori_bb);
            Bitboard promotion_rank_bb(c == WHITE ? Rank8_BB : Rank1_BB);
            Bitboard firstMove_rank_bb(c == WHITE ? Rank2_BB : Rank7_BB);

            // Normal move
            if (~existBB & dest_bb) {
                if (dest_bb & promotion_rank_bb) for (Piece_type promotion : promotion_list)
                    append(legalMoves, Move(board[ori], ori, ori + dir, promotion));
                else append(legalMoves, Move(board[ori], ori, ori + dir));

                if ((firstMove_rank_bb & ori_bb) && (~existBB & shift(dir, dest_bb)))
                    append(legalMoves, Move(board[ori], ori, ori + (dir + dir)));
            }

            // Attack move 
            Bitboard attackBB = attacks_pawn[c][ori] & byColorBB[~c];
            while (attackBB) {
                Square take = pop_lsb(attackBB);
                if (sq_bb(take) & promotion_rank_bb) for (Piece_type promotion : promotion_list)
                    append(legalMoves, Move(board[ori], ori, take, take, promotion));
                else
                    append(legalMoves, Move(board[ori], ori, take, take));
            }

            // En_passant attack
            if (en_passant != NOSQUARE && sq_bb(en_passant) & attacks_pawn[c][ori])
                append(legalMoves, Move(board[ori], ori, en_passant, en_passant + Direction(-dir)));
        }
        void GetLegal_moves(vector<Move>* legalMoves, Square s) {
            Piece_type pt = type_of(board[s]);
            if (pt == PAWN) pawn_moves(legalMoves, color_of(board[s]), s);
            else if (pt == KING) king_moves(legalMoves, s);
            else moves(legalMoves, pt, s);
        }
        vector<Move> legal_moves(bool test = false) {
            auto start = chrono::high_resolution_clock::now();

            vector<Move> legalMoves;
            Bitboard teamBB = byColorBB[turn];
            while (teamBB) {
                Square s = pop_lsb(teamBB);
                GetLegal_moves(&legalMoves, s);
            }
            if (!test) sort(legalMoves.begin(), legalMoves.end(), move_comp);

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            ++LegalMove_ct.first; LegalMove_ct.second += duration;

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
            auto start = chrono::high_resolution_clock::now();

            vector<Move> legalMoves;
            Bitboard teamBB = byColorBB[turn];
            while (teamBB) {
                Square s = pop_lsb(teamBB);
                GetLegal_moves(&legalMoves, s);
                if (!legalMoves.empty()) {
                    auto end = chrono::high_resolution_clock::now();
                    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
                    ++IsEnd_ct.first; IsEnd_ct.second += duration;

                    return false;
                }
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            ++IsEnd_ct.first; IsEnd_ct.second += duration;

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
            pinned = other.pinned;
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
                vector<Move> legalMoves = legal_moves();
                cout << "[Candidate moves] (" << legalMoves.size() << ")\n";
                Fori(legalMoves.size()) {
                    cout << "( " << move_nt(legalMoves, legalMoves.at(i)) << " ) ";
                }
                cout << "\n";
            }
        }
    };
}

#endif  // #ifndef BOARD_H_INCLUDED
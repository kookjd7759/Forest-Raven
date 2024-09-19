from pubsub import pub
import copy

if __name__ == '__main__':
    import connector
    from utility import *
else:
    import chess.connector as connector
    from chess.utility import *

#import connector
#from utility import * 

class Chess:
    def print_board(self):
        def tostring(piece: Piece):
            if piece == None: 
                return ' -- '
            color_to_st = {
                Color.WHITE: 'w',
                Color.BLACK: 'b'
            }
            type_to_st = {
                Piece_type.QUEEN: 'Q',
                Piece_type.KING: 'K',
                Piece_type.PAWN: 'P',
                Piece_type.KNIGHT: 'N',
                Piece_type.BISHOP: 'B',
                Piece_type.ROOK: 'R',
            }
            st = color_to_st[piece.color] + type_to_st[piece.type]
            return ' ' + st + ' '
        def turn_print():
            print(f' [TURN] {self.turn.name}')
        def color_print():
            print(f' [PLAYER] {self.player.name} [FOREST-RAVEN] {opponent(self.player).name}')
        def prevMove_print():
            print(f' [Previous Move] {to_notation(self.prev_move.ori)} -> {to_notation(self.prev_move.dest)}')
        def castling_print():
            print(f' [CASTLING] WHITE Q.{self.__castling_check(Color.WHITE, False)} K.{self.__castling_check(Color.WHITE, True)}   BLACK Q.{self.__castling_check(Color.BLACK, False)} K.{self.__castling_check(Color.BLACK, True)}')
        def candidateMove_print():
            print(f' [Candidate Move] WHITE.{self.__count_candidateMove(Color.WHITE)}   BLACK.{self.__count_candidateMove(Color.BLACK)}')
        def check_print():
            print(f' [CHECK] WHITE.{self.__isCheck(Color.WHITE)}   BLACK.{self.__isCheck(Color.BLACK)}')        
        def gameState_print():
            state_w = self.__gameOver_check(Color.WHITE)
            state_b = self.__gameOver_check(Color.BLACK)
            print(f' [State] WHITE.{state_w.name}   BLACK.{state_b.name}')
        def kingPos_print():
            king_w = self.king_position_wb[Color.WHITE.value]
            king_b = self.king_position_wb[Color.BLACK.value]
            print(f' [KING POSITION] WHITE.{to_notation(king_w)}   BLACK.{to_notation(king_b)}')
        def print_detail(y):
            if y == 7:
                turn_print()
            elif y == 6:
                color_print()
            elif y == 5:
                prevMove_print()
            elif y == 4:
                castling_print()
            elif y == 3:
                candidateMove_print()
            elif y == 2:
                check_print()
            elif y == 1:
                gameState_print()
            elif y == 0:
                kingPos_print()
            else:
                print()
        
        print('┌─────────────────────────────────┐')
        for y in range(7, -1, -1):
            print('│' + chr(ord('1') + y), end='')
            for x in range(0, 8):
                piece = self.board[y][x].piece
                print(tostring(piece), end='')
            print('│', end='')
            print_detail(y)
        print('│  ', end='')
        for i in range(0, 8):
            print(chr(ord('a') + i), end='  ')
            if i != 7:
                print(' ', end='')
        print('│')
        print('└─────────────────────────────────┘')

    def __clone(self):
        return copy.deepcopy(self)  
    def __boundaryCheck(self, pos: Position):
        return True if pos.x >= 0 and pos.x <= 7 and pos.y >= 0 and pos.y <= 7 else False
    def __isAlly(self, cur: Position, dest: Position):
        return not self.board[cur.y][cur.x].empty() and not self.board[dest.y][dest.x].empty() and \
            self.board[cur.y][cur.x].piece.color == self.board[dest.y][dest.x].piece.color
    def __isEnemy(self, cur: Position, dest: Position):
        return not self.board[cur.y][cur.x].empty() and not self.board[dest.y][dest.x].empty() and \
            self.board[cur.y][cur.x].piece.color != self.board[dest.y][dest.x].piece.color
    def __isCheck(self, color: Color):
        king = self.king_position_wb[color.value]
        return self.board[king.y][king.x].isAttacked(color)
    def __isThisMoveLegal(self, move: Move):
        chess_next = self.__clone()
        chess_next.__play(move)
        return not chess_next.__isCheck(move.piece.color)

    def __append(self, list: list, move: Move):
        if self.__isThisMoveLegal(move):
            list.append(move)
    def __repeatMove_legal(self, list: list, piece: Piece, ori: Position, dir: Position):
        dest = Position(ori.x, ori.y)
        while True:
            dest += dir
            if not self.__boundaryCheck(dest) or self.__isAlly(ori, dest):
                return
            
            if self.board[dest.y][dest.x].empty():
                self.__append(list, Move(piece, ori, dest))
            elif self.__isEnemy(ori, dest):
                self.__append(list, Move(piece, ori, dest, take=dest))
                return
    def __oneMove_legal(self, list: list, piece: Piece, ori: Position, dir: Position):
        dest = ori + dir
        if self.__boundaryCheck(dest):
            if self.board[dest.y][dest.x].empty():
                self.__append(list, Move(piece, ori, dest))
            elif self.__isEnemy(ori, dest):
                self.__append(list, Move(piece, ori, dest, take=dest))
    def __Queen_legal(self, pos: Position, piece: Piece):
        list = []
        for dir in dir_all:
            self.__repeatMove_legal(list, piece, pos, dir)
        return list
    def __rook_legal(self, pos: Position, piece: Piece):
        list = []
        for dir in dir_straight:
            self.__repeatMove_legal(list, piece, pos, dir)
        return list
    def __bishop_legal(self, pos: Position, piece: Piece):
        list = []
        for dir in dir_diagonal:
            self.__repeatMove_legal(list, piece, pos, dir)
        return list
    def __knight_legal(self, pos: Position, piece: Piece):
        list = []
        for dir in dir_knight:
            self.__oneMove_legal(list, piece, pos, dir)
        return list
    def __king_legal(self, pos: Position, piece: Piece):
        list = []
        for dir in dir_all:
            self.__oneMove_legal(list, piece, pos, dir)
        
        if self.__castling_check(piece.color, isKingside=True): # Check king side castling 
            list.append(Move(piece, pos, pos + Position(+2, 0))) # already guarantee the check free move
        if self.__castling_check(piece.color, isKingside=False): # Check queen side castling 
            list.append(Move(piece, pos, pos + Position(-2, 0))) # already guarantee the check free move

        return list
    def __pawn_legal(self, pos: Position, piece: Piece):
        list = []
        color = piece.color
        dir = (+1 if color == Color.WHITE else -1)
        # MOVE
        one = Position(pos.x, pos.y + dir)
        if self.board[one.y][one.x].empty():
            if one.y == (7 if color == Color.WHITE else 0): # promotion move
                for i in range(0, 4):
                    self.__append(list, Move(piece, pos, one, promotion_type=promotion_list[i]))
            else:
                self.__append(list, Move(piece, pos, one))
            
            if pos.y == (1 if color == Color.WHITE else 6): # first move
                two = Position(pos.x, pos.y + (dir * 2))
                if self.board[two.y][two.x].empty():
                    self.__append(list, Move(piece, pos, two))
                    
        # ATTACK
        # normal attack
        def attack(dest: Position):
            if self.__boundaryCheck(dest) and self.__isEnemy(pos, dest):
                if dest.y == (7 if color == Color.WHITE else 0): # takes and promotion
                    for i in range(0, 4):
                        self.__append(list, Move(piece, pos, dest, take=dest, promotion_type=promotion_list[i]))
                else:
                    self.__append(list, Move(piece, pos, dest, take=dest))
        attack(pos + Position(-1, dir))
        attack(pos + Position(+1, dir))

        # en_passent attack
        en_passent_dir = self.__en_passent_check(pos)
        if en_passent_dir != 0:
            dest = Position(pos.x + en_passent_dir, pos.y + dir)
            take = Position(pos.x + en_passent_dir, pos.y)
            self.__append(list, Move(piece, pos, dest, take=take))
        return list
    def __legal_moves(self, pos: Position):
        list_: list[Move] = []
        piece: Piece = self.board[pos.y][pos.x].piece
        if piece.type == Piece_type.KING:
            list_ = self.__king_legal(pos, piece)
        elif piece.type ==  Piece_type.QUEEN:
            list_ = self.__Queen_legal(pos, piece)
        elif piece.type ==  Piece_type.ROOK:
            list_ = self.__rook_legal(pos, piece)
        elif piece.type ==  Piece_type.KNIGHT:
            list_ = self.__knight_legal(pos, piece)
        elif piece.type == Piece_type.BISHOP:
            list_ = self.__bishop_legal(pos, piece)
        elif piece.type ==  Piece_type.PAWN:
            list_ = self.__pawn_legal(pos, piece)
        return list_
    def __count_candidateMove(self, color: Color):
        count = 0
        for y in range(0, 8):
            for x in range(0, 8):
                if not self.board[y][x].empty() and self.board[y][x].piece.color == color:
                    posList = self.__legal_moves(Position(x, y))
                    count += len(posList)
        return count

    def __repeatMove(self, list: list, ori: Position, dir: Position):
        dest = Position(ori.x, ori.y)
        while True:
            dest += dir
            if not self.__boundaryCheck(dest):
                return
            
            if self.board[dest.y][dest.x].empty():
                list.append(dest)
            elif self.__isEnemy(ori, dest) or self.__isAlly(ori, dest):
                list.append(dest)
                return
    def __oneMove(self, list: list, ori: Position, dir: Position):
        dest = ori + dir
        if self.__boundaryCheck(dest):
            list.append(dest)
    def __Queen(self, pos: Position):
        list = []
        for dir in dir_all:
            self.__repeatMove(list, pos, dir)
        return list
    def __rook(self, pos: Position):
        list = []
        for dir in dir_straight:
            self.__repeatMove(list, pos, dir)
        return list
    def __bishop(self, pos: Position):
        list = []
        for dir in dir_diagonal:
            self.__repeatMove(list, pos, dir)
        return list
    def __knight(self, pos: Position):
        list = []
        for dir in dir_knight:
            self.__oneMove(list, pos, dir)
        return list
    def __king(self, pos: Position):
        list = []
        for dir in dir_all:
            self.__oneMove(list, pos, dir)
        return list
    def __pawn(self, pos: Position):
        list = []
        dir = (+1 if self.board[pos.y][pos.x].piece.color == Color.WHITE else -1)
        self.__oneMove(list, pos, Position(-1, dir))
        self.__oneMove(list, pos, Position(+1, dir))
        return list
    def __get_attackList(self, pos: Position):
        list_: list[Position]  = []
        piece_type = self.board[pos.y][pos.x].piece.type
        if piece_type == Piece_type.KING:
            list_ = self.__king(pos)
        elif piece_type == Piece_type.QUEEN:
            list_ = self.__Queen(pos)
        elif piece_type == Piece_type.ROOK:
            list_ = self.__rook(pos)
        elif piece_type == Piece_type.KNIGHT:
            list_ = self.__knight(pos)
        elif piece_type == Piece_type.BISHOP:
            list_ = self.__bishop(pos)
        elif piece_type == Piece_type.PAWN:
            list_ = self.__pawn(pos)
        return list_
    def __calAttackSquare(self):
        for _ in self.board:
            for square in _:
                square.attack_wb = [0, 0]

        for y in range(0, 8):
            for x in range(0, 8):
                if not self.board[y][x].empty():
                    posList = self.__get_attackList(Position(x, y))
                    color = self.board[y][x].piece.color
                    for pos in posList:
                        self.board[pos.y][pos.x].attack_wb[color.value] += 1

    def __move(self, move: Move):
        if move.piece.type == Piece_type.KING:
            self.king_position_wb[move.piece.color.value] = move.dest
        self.board[move.dest.y][move.dest.x].piece = self.board[move.ori.y][move.ori.x].piece
        self.board[move.ori.y][move.ori.x].piece = None
    def __capture(self, move: Move):
        self.board[move.take.y][move.take.x].piece = None
        self.__move(move)
    def __castling(self, move: Move):
        rank = (0 if move.piece.color == Color.WHITE else 7)
        if move.dest.x > move.ori.x: # king side 
            rook_pos = Position(7, rank)
            self.__move(move)
            self.__move(Move(Piece(Piece_type.ROOK, move.piece.color), rook_pos, move.dest + Position(-1, 0)))
        else: # queen side
            rook_pos = Position(0, rank)
            self.__move(move)
            self.__move(Move(Piece(Piece_type.ROOK, move.piece.color), rook_pos, move.dest + Position(+1, 0)))
    def __promotion(self, move: Move):
        self.board[move.dest.y][move.dest.x].set(Piece(move.promotion_type, move.piece.color))
    def __play(self, move: Move):
        # King, Rook move check for Castling
        if move.piece.type == Piece_type.KING:
            self.kr_moveCheck_wb_qk[move.piece.color.value][0] = self.kr_moveCheck_wb_qk[move.piece.color.value][1] = True
        elif move.piece.type == Piece_type.ROOK:
            if move.ori.x == 0:
                self.kr_moveCheck_wb_qk[move.piece.color.value][0] = True
            elif move.ori.x == 7:
                self.kr_moveCheck_wb_qk[move.piece.color.value][1] = True

        # Play
        move_type = move.get_move_type()
        if move_type == Move_type.MOVE:
            self.__move(move)
        elif move_type == Move_type.CAPTURE:
            self.__capture(move)
        elif move_type == Move_type.CASTLING:
            self.__castling(move)
        elif move_type == Move_type.MOVE_PRO:
            self.__move(move)
            self.__promotion(move)
        elif move_type == Move_type.CAPTURE_PRO:
            self.__capture(move)
            self.__promotion(move)
        
        self.__calAttackSquare()
        self.prev_move = move
        self.turn = opponent(self.turn)
    def __gameOver_check(self, color):
        if self.__count_candidateMove(color) != 0:
            return Gameover_type.NOGAMEOVER
        
        if self.__isCheck(color):
            return Gameover_type.CHECKMATE_WHITE if color == Color.WHITE else Gameover_type.CHECKMATE_BLACK
        else:
            return Gameover_type.STALEMATE

    def __castling_check(self, color: Color, isKingside: bool):
        rank = (0 if color == Color.WHITE else 7)
        condition_1 = not self.kr_moveCheck_wb_qk[color.value][1 if isKingside else 0]
        condition_2 = self.board[rank][5].empty() and self.board[rank][6].empty() if isKingside else \
            self.board[rank][3].empty() and self.board[rank][2].empty() and self.board[rank][1].empty()
        condition_3 = not self.board[rank][5].isAttacked(color) and not self.board[rank][6].isAttacked(color) if isKingside else \
            not self.board[rank][3].isAttacked(color) and not self.board[rank][2].isAttacked(color)
        return condition_1 and condition_2 and condition_3
    def __en_passent_check(self, pos: Position):
        piece_color = self.board[pos.y][pos.x].piece.color
        if pos.y != (4 if piece_color == Color.WHITE else 3) or self.prev_move.piece.type != Piece_type.PAWN:
            return 0
        
        dir = (+2 if piece_color == Color.WHITE else -2)
        kingSide = (Position(pos.x + 1, pos.y + dir), Position(pos.x + 1, pos.y))
        queenSide = (Position(pos.x - 1, pos.y + dir), Position(pos.x - 1, pos.y))

        if self.__boundaryCheck(kingSide[0]) and kingSide[0].isEqual(self.prev_move.ori) and kingSide[1].isEqual(self.prev_move.dest):
            return 1
        elif self.__boundaryCheck(queenSide[0]) and queenSide[0].isEqual(self.prev_move.ori) and queenSide[1].isEqual(self.prev_move.dest):
            return -1
        else:
            return 0

    def __firstPlay(self):
        if self.player == Color.BLACK:
            self.AI()


    def __init__(self):
        self.king_position_wb = list[Position]
        self.kr_moveCheck_wb_qk = list[list[bool]]
        self.prev_move = Move()
        self.turn = Color.WHITE
        self.init_value()
        self.player = Color.WHITE
        
        self.board = [[Square() for x in range(8)] for y in range(8)]
        self.init_board()
        connector.START(opponent(self.player))
        self.__firstPlay()

    def init_board(self):
        for _ in self.board:
            for square in _:
                square.piece = None

        for i in range(8): 
            self.board[1][i].set(Piece(Piece_type.PAWN, Color.WHITE)) # White's pawns
            self.board[0][i].set(Piece(initPos_type[i], Color.WHITE)) # White's backrank pieces
            self.board[6][i].set(Piece(Piece_type.PAWN, Color.BLACK)) # Black's pawns
            self.board[7][i].set(Piece(initPos_type[i], Color.BLACK)) # Black's backrank pieces
            
        self.__calAttackSquare()
    def init_value(self):
        self.king_position_wb = [Position(4, 0), Position(4, 7)]
        self.kr_moveCheck_wb_qk = [[False, False],[False, False]]
        self.prev_move = Move()
        self.turn = Color.WHITE

### get function (for UI)
    def get_legalMove(self, pos: Position):
        return self.__legal_moves(pos)

### PLAY
    def PLAYER(self, move: Move):
        print(f'PLAYER_PLAY [{to_notation(move.ori)} -> {to_notation(move.dest)}] {move.get_move_type().name}')
        self.__play(move)
        self.print_board()
        connector.send_move(self.prev_move)
        if not self.GAMEOVER_CHECK(self.turn):
            self.AI()
    def AI(self):
        move = connector.get_move()
        move.piece = Piece(self.board[move.ori.y][move.ori.x].piece.type, Color.BLACK)
        print(f'AI_PLAY [{to_notation(move.ori)} -> {to_notation(move.dest)}] {move.get_move_type().name}')
        self.__play(move)
        self.print_board()
        pub.sendMessage('AI', move=move)
        self.GAMEOVER_CHECK(self.turn)
    def GAMEOVER_CHECK(self, color: Color):
        gameOver = self.__gameOver_check(color)
        if gameOver != Gameover_type.NOGAMEOVER:
            pub.sendMessage('GAMEOVER', ret=gameOver)
            return True
        return False
    def RESTART(self, color: Color):
        self.player = color
        self.init_value()
        self.init_board()
        connector.RESTART(opponent(color))
        self.print_board()
        self.__firstPlay()
    def CHANGE(self):
        self.RESTART(opponent(self.player))
        self.__firstPlay()

if __name__ == '__main__':
    chess = Chess()
    chess.CHANGE()


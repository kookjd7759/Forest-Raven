from typing import Literal
from pubsub import pub
from enum import Enum
import copy
import connector


class Color(Enum):
    NOCOLOR = -1
    WHITE = 0
    BLACK = 1
def opponent(color: Color):
    return Color.WHITE if color == Color.BLACK else Color.WHITE

class Move_type(Enum):
    NOMOVE = -1,
    MOVE = 0
    CAPTURE = 1
    MOVE_PRO = 2
    CAPTURE_PRO = 3
    CASTLING = 4

class Piece_type(Enum):
    NOPIECE = -1,
    QUEEN = 0
    ROOK = 1
    BISHOP = 2
    KNIGHT = 3
    KING = 4
    PAWN = 5

class Position:
    def __init__(self, x: int = -1, y: int = -1):
        self.x = x
        self.y = y
        
    def __add__(self, other):
        if isinstance(other, Position):
            return Position(self.x + other.x, self.y + other.y)

    def isEqual(self, other):
        if isinstance(other, Position):
            return self.x == other.x and self.y == other.y

class Piece:
    def __init__(self, type: Piece_type = Piece_type.NOPIECE, color: Color = Color.NOCOLOR):
        self.type = type
        self.color = color

class Move:
    def __init__(self):
        self.piece = Piece()
        self.ori = Position()
        self.dest = Position()
        self.take = Position()
        self.promotion = Piece_type.NOPIECE
    
    def set(self, piece: Piece, ori: Position, dest: Position):
        self.piece = piece
        self.ori = ori
        self.dest = dest
    def set(self, piece: Piece, ori: Position, dest: Position, take: Position):
        self.piece = piece
        self.ori = ori
        self.dest = dest
        self.take = take
    def set(self, piece: Piece, ori: Position, dest: Position, promotion: Piece_type):
        self.piece = piece
        self.ori = ori
        self.dest = dest
        self.promotion = promotion
    def set(self, piece: Piece, ori: Position, dest: Position, take: Position, promotion: Piece_type):
        self.piece = piece
        self.ori = ori
        self.dest = dest
        self.take = take
        self.promotion = promotion

class Square:
    def __init__(self, piece: Piece = None):
        self.piece = piece
        self.attack_wb = [0, 0]

    def set(self, piece: Piece):
        self.piece = piece
    
    def isAttacked(self, color: Color):
        return True if self.attack_wb[opponent(color)] != 0 else False
        
    def empty(self):
        return True if self.piece == None else False

def to_position(notation: str):
    return Position(ord(notation[0]) - ord('a'), ord(notation[1]) - ord('1'))
    
def to_notation(position: Position):
    return chr(position.x + ord('a')) + chr(position.y + ord('1')) if position.x != -1 else 'NULL'

Color_to_string = { # 'White : 0, 'Black' : 1
    0: 'w',
    1: 'b'
}
Type_to_string = { # 'King' : 4, 'Queen' : 5, 'Rook' : 6, 'Knight' : 7, 'Bishop' : 8, 'Pawn' : 9
    4 : 'K', 
    5 : 'Q', 
    6 : 'R', 
    7 : 'N', 
    8 : 'B', 
    9 : 'P'
}
initList = (Piece_type.ROOK, Piece_type.KNIGHT, Piece_type.BISHOP, Piece_type.QUEEN, Piece_type.KING, Piece_type.BISHOP, Piece_type.KNIGHT, Piece_type.ROOK)
promotion_list = (Piece_type.QUEEN, Piece_type.ROOK, Piece_type.BISHOP, Piece_type.KNIGHT)
dir_straight = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) }
dir_diagonal = { Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
dir_all = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
dir_knight = { Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2)}

class Chess:
    class Board:
        def __boundaryCheck(self, pos: Position):
            return True if pos.x >= 0 and pos.x <= 7 and pos.y >= 0 and pos.y <= 7 else False
        def __isAlly(self, cur: Position, dest: Position):
            return not self.__board[cur.y][cur.x].empty() and not self.__board[dest.y][dest.x].empty() and \
                self.__board[cur.y][cur.x].piece.color == self.__board[dest.y][dest.x].piece.color
        def __isEnemy(self, cur: Position, dest: Position):
            return not self.__board[cur.y][cur.x].empty() and not self.__board[dest.y][dest.x].empty() and \
                self.__board[cur.y][cur.x].piece.color != self.__board[dest.y][dest.x].piece.color
        def __isCheck(self, color: Color):
            king = self.king_position_wb[color]
            return self.__board[king.y][king.x].isAttacked(color)
        def __count_candidateMove(self, color: Color):
            count = 0
            for y in range(0, 8):
                for x in range(0, 8):
                    if not self.__board[y][x].empty() and self.__board[y][x].piece.color == color:
                        posList = self.__legal_moves(Position(x, y))
                        count += len(posList)
            return count

        def __isThisMoveLegal(self, move: Move):
            return True # TODO
        
        def __append(self, list: list, move: Move):
            if self.__isThisMoveLegal(move):
                list.append(move)
        def __repeatMove(self, list: list, piece: Piece, ori: Position, dir: Position):
            dest = ori
            while True:
                dest += dir
                if not self.__boundaryCheck(dest) or self.__isAlly(ori, dest):
                    return
                
                if self.__board[dest.y][dest.x].empty():
                    self.__append(list, Move(piece, ori, dest))
                elif self.__isEnemy(ori, dest):
                    self.__append(list, Move(piece, ori, dest))
                    return
        def __oneMove(self, list: list, piece: Piece, ori: Position, dir: Position):
            dest = ori + dir
            if self.__boundaryCheck(dest):
                if self.__board[dest.y][dest.x].empty():
                    self.__append(list, Move(piece, ori, dest))
                elif self.__isEnemy(ori, dest):
                    self.__append(list, Move(piece, ori, dest, dest))
        def __Queen(self, pos: Position, piece: Piece):
            list = []
            for dir in Chess.dir_all:
                self.__repeatMove(list, piece, pos, dir)
            return list
        def __rook(self, pos: Position, piece: Piece):
            list = []
            for dir in Chess.dir_straight:
                self.__repeatMove(list, piece, pos, dir)
            return list
        def __bishop(self, pos: Position, piece: Piece):
            list = []
            for dir in Chess.dir_diagonal:
                self.__repeatMove(list, piece, pos, dir)
            return list
        def __knight(self, pos: Position, piece: Piece):
            list = []
            for dir in Chess.dir_knight:
                self.__oneMove(list, piece, pos, dir)
            return list
        def __king(self, pos: Position, piece: Piece):
            list = []
            for dir in Chess.dir_all:
                self.__oneMove(list, piece, pos, dir)
            
            if self.castling_check(piece.color, isKingside=True): # Check king side castling 
                list.append(Move(piece, pos, pos + Position(+2, 0))) # already guarantee the check free move
            if self.castling_check(piece.color, isKingside=False): # Check queen side castling 
                list.append(Move(piece, pos, pos + Position(-2, 0))) # already guarantee the check free move

            return list
        def __pawn(self, pos: Position, piece: Piece):
            list = []
            color = piece.color
            dir = (+1 if color == Color.WHITE else -1)
            # MOVE
            dest = pos
            dest.y += dir
            if self.__board[dest.y][dest.x].empty():
                self.__append(list, Move(piece, pos, dest))
                if dest.y == (7 if color == Color.WHITE else 0): # promotion move
                    for promotion in promotion_list:
                        self.__append(list, Move(piece, pos, dest, promotion))
                if pos.y == (1 if color == Color.WHITE else 6): # first move
                    dest.y += dir
                    if self.__board[dest.y][dest.x].empty():
                        self.__append(list, Move(piece, pos, dest))
            
            # ATTACK
            # normal attack
            def attack(dest: Position):
                if self.__boundaryCheck(dest) and self.__isEnemy(pos, dest):
                    if dest.y == (7 if color == Color.WHITE else 0): # takes and promotion
                        for promotion in promotion_list:
                            self.__append(list, Move(piece, pos, dest, dest, promotion))
                    else:
                        self.__append(list, Move(piece, pos, dest, dest))
            attack(pos + Position(-1, dir))
            attack(pos + Position(+1, dir))

            # en_passent attack
            en_passent_dir = self.en_passent_check(pos)
            if en_passent_dir != 0:
                dest = Position(pos.x + en_passent_dir, pos.y + dir)
                take = Position(pos.x + en_passent_dir, pos.y)
                self.__append(list, Move(piece, pos, dest, take))
            return list
        def __legal_moves(self, pos: Position):
            list = []
            piece_type = self.__board[pos.y][pos.x].piece.type
            if piece_type == Piece_type.KING:
                list = self.__king(pos, self.__board[pos.y][pos.x].piece)
            elif piece_type ==  Piece_type.QUEEN:
                list = self.__Queen(pos, self.__board[pos.y][pos.x].piece)
            elif piece_type ==  Piece_type.ROOK:
                list = self.__rook(pos, self.__board[pos.y][pos.x].piece)
            elif piece_type ==  Piece_type.KNIGHT:
                list = self.__knight(pos, self.__board[pos.y][pos.x].piece)
            elif piece_type == Piece_type.BISHOP:
                list = self.__bishop(pos, self.__board[pos.y][pos.x].piece)
            elif piece_type ==  Piece_type.PAWN:
                list = self.__pawn(pos, self.__board[pos.y][pos.x].piece)
            return list

        def __repeatMove(self, list: list, ori: Position, dir: Position):
            dest = ori
            while True:
                dest += dir
                if not self.__boundaryCheck(dest):
                    return
                
                if self.__board[dest.y][dest.x].empty():
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
            for dir in Chess.dir_all:
                self.__repeatMove(list, pos, dir)
            return list
        def __rook(self, pos: Position):
            list = []
            for dir in Chess.dir_straight:
                self.__repeatMove(list, pos, dir)
            return list
        def __bishop(self, pos: Position):
            list = []
            for dir in Chess.dir_diagonal:
                self.__repeatMove(list, pos, dir)
            return list
        def __knight(self, pos: Position):
            list = []
            for dir in Chess.dir_knight:
                self.__oneMove(list, pos, dir)
            return list
        def __king(self, pos: Position):
            list = []
            for dir in Chess.dir_all:
                self.__oneMove(list, pos, dir)
            return list
        def __pawn(self, pos: Position):
            list = []
            dir = (+1 if self.__board[pos.y][pos.x].piece.color == Color.WHITE else -1)
            self.__oneMove(list, pos, Position(-1, dir))
            self.__oneMove(list, pos, Position(+1, dir))
            return list
        def __get_attackList(self, pos: Position):
            list = []
            piece_type = self.__board[pos.y][pos.x].piece.type
            if piece_type == Piece_type.KING:
                list = self.__king(pos)
            elif piece_type == Piece_type.QUEEN:
                list = self.__Queen(pos)
            elif piece_type == Piece_type.ROOK:
                list = self.__rook(pos)
            elif piece_type == Piece_type.KNIGHT:
                list = self.__knight(pos)
            elif piece_type == Piece_type.BISHOP:
                list = self.__bishop(pos)
            elif piece_type == Piece_type.PAWN:
                list = self.__pawn(pos)
            return list
        def __calAttackSquare(self):
            for _ in self.__board:
                for square in _:
                    square.attack_wb[Color.WHITE] = 0
                    square.attack_wb[Color.BLACK] = 0

            for y in range(0, 8):
                for x in range(0, 8):
                    if not self.__board[y][x].empty():
                        posList = self.__get_attackList(Position(x, y))
                        color = self.__board[y][x].piece.color
                        for pos in posList:
                            self.__board[pos.y][pos.x].attack_wb[color] += 1

        def __init__(self):
            self.king_position_wb = list[Position]
            self.kr_moveCheck_wb_qk = list[list[bool]]
            self.prev_move = Chess.PreviousMove()
            self.reset()
        
        def reset(self):
            self.__board = [[Square() for x in range(8)] for y in range(8)]
            for i in range(8): 
                self.__board[1][i].set(Piece(Piece_type.PAWN, Color.WHITE)) # White's pawns
                self.__board[0][i].set(Piece(initList[i], Color.WHITE)) # White's backrank pieces
                self.__board[6][i].set(Piece(Piece_type.PAWN, Color.BLACK)) # Black's pawns
                self.__board[7][i].set(Piece(initList[i], Color.BLACK)) # Black's backrank pieces
                
            self.king_position_wb = [Position(4, 0), Position(4, 7)]
            self.kr_moveCheck_wb_qk = [[False, False],[False, False]]
            self.prev_move = Move()
            self.__calAttackSquare()

        # game function
        def gameEnd_check(self, color: Literal[0, 1]): # (0) None (1) CheckMate (2) StaleMate
            if self.__count_candidateMove(color) != 0:
                return 0
            
            if self.__isCheck(color):
                return 1
            else:
                return 2
        
        # Move function
        def move():
        def move_piece(self, cur: Position, dest: Position):
            if self.__board[cur.y][cur.x].piece.type == Chess.Type['King']:
                color = self.__board[cur.y][cur.x].piece.color
                self.king_position_wb[color] = dest

            self.__board[dest.y][dest.x].piece = self.__board[cur.y][cur.x].piece
            self.__board[cur.y][cur.x].piece = None

        def move(self, cur: Position, dest: Position, promotion: Literal[0, 1, 2, 3] = None): 
            isCastling = False
            isEn_passent = False
            isPromotion = False
            ### king, rook moving check
            piece_color = self.__board[cur.y][cur.x].piece.color
            piece_type = self.__board[cur.y][cur.x].piece.type
            if piece_type == Chess.Type['King']:
                self.kr_moveCheck_wb_qk[piece_color][0] = True
                self.kr_moveCheck_wb_qk[piece_color][1] = True
                if abs(dest.x - cur.x) == 2: # castling move
                    isCastling = True
            elif piece_type == Chess.Type['Rook']:
                if cur.x == 0: # Queen side rook 
                    self.kr_moveCheck_wb_qk[piece_color][0] = True
                elif cur.x == 7: # King side rook
                    self.kr_moveCheck_wb_qk[piece_color][1] = True
            elif piece_type == Chess.Type['Pawn']:
                if dest.y == (7 if piece_color == Chess.Color['White'] else 0):
                    isPromotion = True
                if cur.x != dest.x: # Pawn takes something
                    if self.__board[dest.y][dest.x].empty(): # en_passent move
                        isEn_passent = True

            if isCastling:
                self.castling_move(cur, dest)
            elif isEn_passent:
                self.en_passent_move(cur, dest)
            else:
                self.move_piece(cur, dest)
                print(f'promotion -> {promotion}')
                if isPromotion:
                    promotion_type = ''
                    if promotion == 0:
                        promotion_type = 'Queen'
                    elif promotion == 1:
                        promotion_type = 'Rook'
                    elif promotion == 2:
                        promotion_type = 'Bishop'
                    elif promotion == 3:
                        promotion_type = 'Knight'
                    self.__board[dest.y][dest.x].set(Chess.Piece(Chess.Type[promotion_type], piece_color))

            self.__calAttackSquare()
            self.prev_move.set(piece_type, cur, dest)

        def castling_move(self, cur: Position, dest: Position):
            rank = (0 if self.__board[cur.y][cur.x].piece.color == Chess.Color['White'] else 7)
            if dest.x > cur.x: # King Side Castling
                rook_pos = Position(7, rank)
                self.move_piece(cur, dest)
                self.move_piece(rook_pos, dest + Position(-1, 0))
            else: # Queen Side Castling
                rook_pos = Position(0, rank)
                self.move_piece(cur, dest)
                self.move_piece(rook_pos, dest + Position(+1, 0))

        def en_passent_move(self, cur: Position, dest: Position):
            attack = dest + Position(0, (-1 if self.__board[cur.y][cur.x].piece.color == Chess.Color['White'] else +1))
            self.__board[attack.y][attack.x].piece = None
            self.move_piece(cur, dest)

        # Get function
        def get_candidateMove(self, color: Literal[0, 1]):
            list = []
            for y in range(0, 8):
                for x in range(0, 8):
                    if not self.__board[y][x].empty() and self.__board[y][x].piece.color == color:
                        posList = self.get_legalMoveList(Position(x, y))
                        for pos in posList:
                            list.append((Position(x, y), pos))

            return list

        def get_square(self, pos: Position):
            return self.__board[pos.y][pos.x]
        
        # Check function
        def castling_check(self, color: Color, isKingside: bool):
            rank = (0 if color == Color.WHITE else 7)
            condition_1 = not self.kr_moveCheck_wb_qk[color][1 if isKingside else 0]
            condition_2 = self.__board[rank][5].empty() and self.__board[rank][6].empty() if isKingside else \
                self.__board[rank][3].empty() and self.__board[rank][2].empty() and self.__board[rank][1].empty()
            condition_3 = not self.__board[rank][5].isAttacked(color) and not self.__board[rank][6].isAttacked(color) if isKingside else \
                not self.__board[rank][3].isAttacked(color) and not self.__board[rank][2].isAttacked(color)
            return condition_1 and condition_2 and condition_3
        def en_passent_check(self, pos: Position):
            piece_color = self.__board[pos.y][pos.x].piece.color
            if pos.y != (4 if piece_color == Color.WHITE else 3) or self.prev_move.type != Piece_type.PAWN:
                return 0
            
            dir = (+2 if piece_color == Color.WHITE else -2)
            kingSide = (Position(pos.x + 1, pos.y + dir), Position(pos.x + 1, pos.y))
            queenSide = (Position(pos.x - 1, pos.y + dir), Position(pos.x - 1, pos.y))

            if self.__boundaryCheck(kingSide[0]) and kingSide[0].isEqual(self.prev_move.prev) and kingSide[1].isEqual(self.prev_move.now):
                return 1
            elif self.__boundaryCheck(queenSide[0]) and queenSide[0].isEqual(self.prev_move.prev) and queenSide[1].isEqual(self.prev_move.now):
                return -1
            else:
                return 0
        
        def clone(self):
            return copy.deepcopy(self)
        
        # Additional function
        def print_board(self):
            # white criteria
            for i in range(7, -1, -1):
                for j in range(8):
                    square = self.__board[i][j]
                    if not square.empty():
                        print(f'{Chess.Color_to_string[square.piece.color]}{Chess.Type_to_string[square.piece.type]} ', end='')
                    else:
                        print('-- ', end='')
                print('  ', end='')
                for j in range(8):
                    square = self.__board[i][j]
                    print(f'{square.attack_wb[0]}{square.attack_wb[1]} ', end='')
                print()
            print(f'Castling : [White].Q (={self.castling_check(0, False)}) [White].K (={self.castling_check(0, True)})')
            print(f'           [Black].Q (={self.castling_check(1, False)}) [Black].K (={self.castling_check(1, True)})')


    def __init__(self):
        self.board = self.Board()
        self.turn = self.Color['White']
        self.player = self.Color['White']
        pub.subscribe(self.AI_move, 'Player_move')
        connector.set_color(self.Color['Black'] if self.player == self.Color['White'] else self.Color['White'])

    def restart(self):
        self.board.reset()
        self.turn = self.Color['White']
        self.player = self.Color['White']

    def get_legalMove(self, pos: Position):
        square = self.board.get_square(pos)

        ### piece existence check
        if square.empty():
            print('get_legalMove()::there is no piece')
        
        list = self.board.get_legalMoveList(pos)
        return list

    def move(self, cur: Position, dest: Position, take: Position, promotion: Literal[0, 1, 2, 3] = None): # (-1) can't move (0) None (1) CheckMate (2) StaleMate
        print('chess.move function')
        square = self.board.get_square(cur)

        ### piece existence check
        if square.empty():
            print('move()::there is no piece')
            return -1
        
        ### piece color check
        if self.turn != square.piece.color:
            print('move()::it\'s not their\'s turn')
            return -1
        
        ### move check
        isLegal = False
        legalMoveList = self.board.get_legalMoveList(cur)
        for move in legalMoveList:
            if move.isEqual(dest):
                isLegal = True
        
        if not isLegal:
            print('move()::it\'s illegal move')
            return -1
        
        if square.piece.type == self.Type['Pawn'] and dest.y == (7 if square.piece.color == Chess.Color['White'] else 0) and promotion == None: # promotion check
            print('promotion signal !!')
            return 3
        
        ### Move
        self.board.move(cur, dest, promotion)
        self.board.print_board()
        self.turn = self.Color['White'] if self.turn == self.Color['Black'] else self.Color['Black']
        print(f'[turn] : {"WHITE" if self.turn == 0 else "BLACK"}')

        ### game end check
        return self.board.gameEnd_check(self.turn)

    def AI_move(self, promotion):
        connector.send_move(self.board.prev_move.prev.x, self.board.prev_move.prev.y, self.board.prev_move.now.x, self.board.prev_move.now.y, promotion)
        now_x, now_y, next_x, next_y, take_x, take_y, promotion = connector.get_move()
        cur = Position(now_x, now_y)
        dest = Position(next_x, next_y)
        take = Position(take_x, take_y)
        print(f'AI MOVE COMMAND {to_notation(cur)} -> {to_notation(dest)} / take {to_notation(take)}')
        self.move(cur, dest, take, promotion)
        pub.sendMessage('AI_move', cur=cur, dest=dest, take=take, smooth=True)


if __name__ == '__main__':
    chess = Chess()

from typing import Literal

class Position:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y
        
    def __add__(self, other):
        if isinstance(other, Position):
            return Position(self.x + other.x, self.y + other.y)

    def isEqual(self, other):
        if isinstance(other, Position):
            return self.x == other.x and self.y == other.y



def to_position(notation: str):
    return Position(ord(notation[0]) - ord('a'), ord(notation[1]) - ord('1'))
    
def to_notation(position: Position):
    return chr(position.x + ord('a')) + chr(position.y + ord('1'))



class Chess:
    Color = { # 'White' : 0, 'Black' : 1
        'White' : 0,
        'Black' : 1
    }
    Side = { # 'KingSide' : 2, 'QueenSide' : 3
        'KingSide' : 2,
        'QueenSide' : 3,
    }
    Type = { # 'King' : 4, 'Queen' : 5, 'Rook' : 6, 'Knight' : 7, 'Bishop' : 8, 'Pawn' : 9
        'King' : 4, 
        'Queen' : 5, 
        'Rook' : 6, 
        'Knight' : 7, 
        'Bishop' : 8, 
        'Pawn' : 9
    }
    initList = ('Rook', 'Knight', 'Bishop', 'Queen', 'King', 'Bishop', 'Knight', 'Rook')

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

    class PreviousMove:
        def __init__(self, type: Literal[4, 5, 6, 7, 8, 9] = None, prev: Position = None, now: Position = None):
            self.type = type
            self.prev = prev
            self.now = now
        
        def set(self, type: Literal[4, 5, 6, 7, 8, 9], prev: Position, now: Position):
            self.type = type
            self.prev = prev
            self.now = now

    class Piece:
        def __init__(self, type: Literal[4, 5, 6, 7, 8, 9], color: Literal[0, 1]):
            self.type = type
            self.color = color

    class Square:
        def __init__(self, piece: 'Chess.Piece' = None):
            self.piece = piece
            self.attack_wb = [0, 0]

        def set(self, piece: 'Chess.Piece'):
            self.piece = piece
        
        def isAttacked(self, color: Literal[0, 1]):
            return True if self.attack_wb[Chess.Color['White'] if color == Chess.Color['Black'] else Chess.Color['Black']] != 0 else False
            
        def empty(self):
            return True if self.piece == None else False

    dir_straight = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) }
    dir_diagonal = { Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
    dir_all = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
    dir_knight = { Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2)}
    class Board:
        def __boundaryCheck(self, pos: Position):
            return True if pos.x >= 0 and pos.x <= 7 and pos.y >= 0 and pos.y <= 7 else False

        def __isAlly(self, cur: Position, dest: Position):
            return not self.__board[cur.y][cur.x].empty() and not self.__board[dest.y][dest.x].empty() and \
                self.__board[cur.y][cur.x].piece.color == self.__board[dest.y][dest.x].piece.color

        def __isEnemy(self, cur: Position, dest: Position):
            return not self.__board[cur.y][cur.x].empty() and not self.__board[dest.y][dest.x].empty() and \
                self.__board[cur.y][cur.x].piece.color != self.__board[dest.y][dest.x].piece.color

        def __isCheck(self, color: Literal[0, 1]):
            king = self.king_position_wb[color]
            return self.__board[king.y][king.x].isAttacked(color)

        def __count_candidateMove(self, color: Literal[0, 1]):
            count = 0
            for y in range(0, 8):
                for x in range(0, 8):
                    if not self.__board[y][x].empty() and self.__board[y][x].piece.color == color:
                        posList = self.get_legalMoveList(Position(x, y))
                        count += len(posList)
            return count



        def __isThisMoveLegal(self, cur: Position, dest: Position, take: Position = None):
            color = self.__board[cur.y][cur.x].piece.color
            temp_destPiece = self.__board[dest.y][dest.x].piece
            temp_takePiece = None
            if take != None:
                temp_takePiece = self.__board[take.y][take.x].piece
                self.__board[take.y][take.x].piece = None

            self.move_piece(cur, dest)
            self.__calAttackSquare()

            ret = not self.__isCheck(color)

            self.move_piece(dest, cur)
            self.__board[dest.y][dest.x].piece = temp_destPiece

            if take != None:
                self.__board[take.y][take.x].piece = temp_takePiece
            self.__calAttackSquare()

            return ret
        
        def __append(self, list: list, cur: Position, dest: Position, legalMove: bool, take: Position = None):
            if not legalMove or self.__isThisMoveLegal(cur, dest, take):
                list.append(dest)



        def __repeatMove(self, list: list, cur: Position, dir: Position, legalMove: bool):
            next = cur
            while True:
                next += dir
                if not self.__boundaryCheck(next):
                    return
                
                if self.__board[next.y][next.x].empty():
                    self.__append(list, cur, next, legalMove)
                elif self.__isEnemy(cur, next):
                    self.__append(list, cur, next, legalMove)
                    return
                elif self.__isAlly(cur, next):
                    if not legalMove:
                        self.__append(list, cur, next, legalMove)
                    return
                else:
                    print('ERROR, __repeatMove()::It\'s weird error check this out')
                    exit(0)

        def __oneMove(self, list: list, cur: Position, dir: Position, legalMove: bool):
            next = cur + dir
            if self.__boundaryCheck(next):
                if self.__board[next.y][next.x].empty() or self.__isEnemy(cur, next):
                    self.__append(list, cur, next, legalMove)
                elif self.__isAlly(cur, next) and not legalMove:
                    self.__append(list, cur, next, legalMove)

        def __rook(self, pos: Position, legalMove: bool):
            list = []
            for dir in Chess.dir_straight:
                self.__repeatMove(list, pos, dir, legalMove)
            return list

        def __bishop(self, pos: Position, legalMove: bool):
            list = []
            for dir in Chess.dir_diagonal:
                self.__repeatMove(list, pos, dir, legalMove)
            return list

        def __knight(self, pos: Position, legalMove: bool):
            list = []
            for dir in Chess.dir_knight:
                self.__oneMove(list, pos, dir, legalMove)
            return list

        def __king(self, pos: Position, legalMove: bool):
            list = []
            for dir in Chess.dir_all:
                self.__oneMove(list, pos, dir, legalMove)
            
            if self.castling_check(self.__board[pos.y][pos.x].piece.color, isKingside=True): # Check king side castling 
                list.append(pos + Position(+2, 0)) # already guarantee the check free move
            if self.castling_check(self.__board[pos.y][pos.x].piece.color, isKingside=False): # Check queen side castling 
                list.append(pos + Position(-2, 0)) # already guarantee the check free move

            return list

        def __Queen(self, pos: Position, legalMove: bool):
            list = []
            for dir in Chess.dir_all:
                self.__repeatMove(list, pos, dir, legalMove)
            return list

        def __pawn_move(self, pos: Position):
            list = []
            color = self.__board[pos.y][pos.x].piece.color
            dir = Position(0, +1) if color == Chess.Color['White'] else Position(0, -1)

            next = pos + dir
            if self.__board[next.y][next.x].empty():
                self.__append(list, pos, next, True)
                if pos.y == (1 if color == Chess.Color['White'] else 6): # first move
                    next += dir
                    if self.__board[next.y][next.x].empty():
                        self.__append(list, pos, next, True)
            return list

        def __pawn_attack(self, pos: Position, legalMove: bool):
            list = []
            def add(next: Position):
                if self.__boundaryCheck(next):
                    if (not legalMove) or (legalMove and self.__isEnemy(pos, next)):
                        self.__append(list, pos, next, legalMove)

            color = self.__board[pos.y][pos.x].piece.color
            dir = +1 if color == Chess.Color['White'] else -1
            add(pos + Position(-1, dir)) # king side
            add(pos + Position(+1, dir)) # queen side

            en_passent_dir = self.en_passent_check(pos)
            if en_passent_dir != 0:
                dir = (+1 if color == Chess.Color['White'] else -1)
                self.__append(list, pos, Position(pos.x + en_passent_dir, pos.y + dir), legalMove, Position(pos.x + en_passent_dir, pos.y))
            return list

        def __get_attackList(self, pos: Position):
            list = []
            piece_type = self.__board[pos.y][pos.x].piece.type
            if piece_type == Chess.Type['King']:
                list = self.__king(pos, False)
            elif piece_type == Chess.Type['Queen']:
                list = self.__Queen(pos, False)
            elif piece_type == Chess.Type['Rook']:
                list = self.__rook(pos, False)
            elif piece_type == Chess.Type['Knight']:
                list = self.__knight(pos, False)
            elif piece_type == Chess.Type['Bishop']:
                list = self.__bishop(pos, False)
            elif piece_type == Chess.Type['Pawn']:
                list = self.__pawn_attack(pos, False)
            return list

        def __calAttackSquare(self):
            for _ in self.__board:
                for square in _:
                    square.attack_wb[Chess.Color['White']] = 0
                    square.attack_wb[Chess.Color['Black']] = 0
            
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
            self.__board = [[Chess.Square() for x in range(8)] for y in range(8)]
            for i in range(8): 
                self.__board[1][i].set(Chess.Piece(Chess.Type['Pawn'], Chess.Color['White'])) # White's pawns
                self.__board[0][i].set(Chess.Piece(Chess.Type[Chess.initList[i]], Chess.Color['White'])) # White's backrank pieces
                self.__board[6][i].set(Chess.Piece(Chess.Type['Pawn'], Chess.Color['Black'])) # Black's pawns
                self.__board[7][i].set(Chess.Piece(Chess.Type[Chess.initList[i]], Chess.Color['Black'])) # Black's backrank pieces
                
            self.king_position_wb = [Position(4, 0), Position(4, 7)]
            self.kr_moveCheck_wb_qk = [[False, False],[False, False]]
            self.prev_move = Chess.PreviousMove()
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
        def get_legalMoveList(self, pos: Position):
            list = []
            piece_type = self.__board[pos.y][pos.x].piece.type
            if piece_type == Chess.Type['King']:
                list = self.__king(pos, True)
            elif piece_type == Chess.Type['Queen']:
                list = self.__Queen(pos, True)
            elif piece_type == Chess.Type['Rook']:
                list = self.__rook(pos, True)
            elif piece_type == Chess.Type['Knight']:
                list = self.__knight(pos, True)
            elif piece_type == Chess.Type['Bishop']:
                list = self.__bishop(pos, True)
            elif piece_type == Chess.Type['Pawn']:
                list = self.__pawn_move(pos) + self.__pawn_attack(pos, True)
            return list

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
        def castling_check(self, color: Literal[0, 1], isKingside: bool):
            rank = (0 if color == Chess.Color['White'] else 7)
            condition_1 = not self.kr_moveCheck_wb_qk[color][1 if isKingside else 0]
            condition_2 = self.__board[rank][5].empty() and self.__board[rank][6].empty() if isKingside else \
                self.__board[rank][3].empty() and self.__board[rank][2].empty() and self.__board[rank][1].empty()
            condition_3 = not self.__board[rank][5].isAttacked(color) and not self.__board[rank][6].isAttacked(color) if isKingside else \
                not self.__board[rank][3].isAttacked(color) and not self.__board[rank][2].isAttacked(color)
            return condition_1 and condition_2 and condition_3

        def en_passent_check(self, pos: Position):
            piece_color = self.__board[pos.y][pos.x].piece.color
            if pos.y != (4 if piece_color == Chess.Color['White'] else 3) or self.prev_move.type != Chess.Type['Pawn']:
                return 0
            
            dir = (+2 if piece_color == Chess.Color['White'] else -2)
            kingSide = (Position(pos.x + 1, pos.y + dir), Position(pos.x + 1, pos.y))
            queenSide = (Position(pos.x - 1, pos.y + dir), Position(pos.x - 1, pos.y))

            if self.__boundaryCheck(kingSide[0]) and kingSide[0].isEqual(self.prev_move.prev) and kingSide[1].isEqual(self.prev_move.now):
                return 1
            elif self.__boundaryCheck(queenSide[0]) and queenSide[0].isEqual(self.prev_move.prev) and queenSide[1].isEqual(self.prev_move.now):
                return -1
            else:
                return 0

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

    def restart(self):
        self.board.reset()
        self.turn = self.Color['White']
        self.player = self.Color['White']

    def move(self, cur: Position, dest: Position, promotion: Literal[0, 1, 2, 3] = None): # (-1) can't move (0) None (1) CheckMate (2) StaleMate
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
        self.turn = self.Color['White'] if self.turn == self.Color['Black'] else self.Color['Black']
        self.board.print_board()

        ### game end check
        return self.board.gameEnd_check(self.turn)

    def get_legalMove(self, pos: Position):
        square = self.board.get_square(pos)

        ### piece existence check
        if square.empty():
            print('get_legalMove()::there is no piece')
        
        list = self.board.get_legalMoveList(pos)
        print(f'size : {len(list)}')
        for next in list:
            print(f'{to_notation(next)} ', end='')
        print()
        return list

    def get_candidateMove(self):
        list = self.board.get_candidateMove(self.turn)
        print(f'size : {len(list)}')
        for move in list:
            print(f'{to_notation(move[0])} -> {to_notation(move[1])} ')
        print()



    def start(self):
        while True:
            self.board.print_board()
            color = 'WHITE' if self.turn == self.Color['White'] else 'BLACK'
            print(f'{color}\'s turn')
            while True:
                command = input('[1] move [2] get legal move [3] get candidate move [4] get board data\n -> ').split()
                print(command)
                if command[0] == '1':
                    cur, dest = command[1], command[2]
                    if not self.move(to_position(cur), to_position(dest)):
                        continue
                elif command[0] == '2':
                    pos = command[1]
                    self.get_legalMove(to_position(pos))
                    continue
                elif command[0] == '3':
                    self.get_candidateMove()

                break



if __name__ == '__main__':
    chess = Chess()
    chess.start()
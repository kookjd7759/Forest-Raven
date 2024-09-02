from typing import Literal

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
    Movement_type = { # 'Move': 10, 'Capture': 11, 'Castling': 12, 'En_passent': 13, 'Pomotion': 14
        'Move': 10,
        'Capture': 11,
        'Castling': 12,
        'En_passent': 13,
        'Pomotion': 14,
    }

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

    class Position:
        def __init__(self, x: int, y: int):
            self.x = x
            self.y = y
        
        def __add__(self, other):
            if isinstance(other, Chess.Position):
                return Chess.Position(self.x + other.x, self.y + other.y)

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
        
        def isattacked(self, color: Literal[0, 1]):
            return True if self.attack_wb[0 if color == 1 else 1] != 0 else False
            
        def empty(self):
            return True if self.piece == None else False

    dir_straight = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) }
    dir_diagonal = { Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
    dir_all = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
    dir_knight = { Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2)}
    class Board:
        def __boundaryCheck(self, pos: 'Chess.Position'):
            return True if pos.x >= 0 and pos.x <= 7 and pos.y >= 0 and pos.y <= 7 else False

        def __isAlly(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            return not self.__board[cur.y][cur.x].empty() and not self.__board[dest.y][dest.x].empty() and \
                self.__board[cur.y][cur.x].piece.color == self.__board[cur.y][cur.x].piece.color

        def __isEnemy(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            return not self.__board[cur.y][cur.x].empty() and not self.__board[dest.y][dest.x].empty() and \
                self.__board[cur.y][cur.x].piece.color != self.__board[cur.y][cur.x].piece.color

        def __isCheck(self, color: Literal[0, 1]):
            king = self.king_position_wb[color]
            return True if self.__board[king.y][king.x].isattacked(color) else False

        def __isThisMoveLegal(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            color = self.__board[cur.y][cur.x].piece.color
            temp_dest = self.__board[dest.y][dest.x]

            self.move(cur, dest)

            ret = not self.__isCheck(color)

            self.move(dest, cur)
            self.__board[dest.y][dest.x] = temp_dest

            return ret
        
        def __appand(self, list: list, cur: 'Chess.Position', dest: 'Chess.Position', legalMove: bool):
            if not legalMove:
                list.append(dest)
                return
            
            if self.__isThisMoveLegal(cur, dest):
                list.append(dest)



        def __repeatMove(self, list: list, cur: 'Chess.Position', dir: 'Chess.Position', legalMove: bool):
            next = cur
            while True:
                next += dir
                if not self.__boundaryCheck(next):
                    return
                
                if self.__board[next.y][next.x].empty():
                    self.__appand(list, cur, next, legalMove)
                elif self.__isEnemy(cur, next):
                    self.__appand(list, cur, next, legalMove)
                    return
                elif self.__isAlly(cur, next):
                    if not legalMove:
                        self.__appand(list, cur, next, legalMove)
                    return
                else:
                    print('ERROR, __repeatMove()::It\'s weird error check this out')
                    exit(0)

        def __oneMove(self, list: list, cur: 'Chess.Position', dir: 'Chess.Position', legalMove: bool):
            next = cur + dir
            if self.__boundaryCheck(next):
                if self.__board[next.y][next.x].empty() or self.__isEnemy(cur, next):
                    self.__appand(list, cur, next, legalMove)
                elif self.__isAlly(cur, next):
                    if not legalMove:
                        self.__appand(list, cur, next, legalMove)
                    return
            
            # TODO: legalMove function

        def __rook(self, pos: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_straight:
                self.__repeatMove(list, pos, dir, legalMove)
            return list

        def __bishop(self, pos: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_diagonal:
                self.__repeatMove(list, pos, dir, legalMove)
            return list

        def __knight(self, pos: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_knight:
                self.__oneMove(list, pos, dir, legalMove)
            return list

        def __king(self, pos: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_all:
                self.__oneMove(list, pos, dir, legalMove)
            return list

        def __Queen(self, pos: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_all:
                self.__repeatMove(list, pos, dir, legalMove)
            return list

        def __pawn_move(self, pos: 'Chess.Position'):
            list = []
            color = self.__board[pos.y][pos.x].piece.color
            dir = Chess.Position(0, +1) if color == Chess.Color['White'] else Chess.Position(0, -1)

            next = pos + dir
            if self.__board[next.y][next.x].empty():
                self.__appand(list, pos, next, True)
                if pos.y == (1 if color == Chess.Color['White'] else 6): # first move
                    next += dir
                    if self.__board[next.y][next.x].empty():
                        self.__appand(list, pos, next, True)
            return list

        def __pawn_attack(self, pos: 'Chess.Position', legalMove: bool):
            list = []
            def add(next: 'Chess.Position'):
                if self.__boundaryCheck(next):
                    if (not legalMove) or (legalMove and self.__isEnemy(pos, next)):
                        self.__appand(list, pos, next, legalMove)

            color = self.__board[pos.y][pos.x].piece.color
            dir = +1 if color == Chess.Color['White'] else -1
            add(pos + Chess.Position(-1, dir)) # king side 
            add(pos + Chess.Position(+1, dir)) # queen side   
            return list

            # TODO: en passant

        def __get_attackList(self, pos: 'Chess.Position'):
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
                        posList = self.__get_attackList(Chess.Position(x, y))
                        color = self.__board[y][x].piece.color
                        for pos in posList:
                            self.__board[pos.y][pos.x].attack_wb[color] += 1



        def __init__(self):
            self.king_position_wb = []
            self.reset()
        
        def reset(self):
            initList = ['Rook', 'Knight', 'Bishop', 'Queen', 'King', 'Bishop', 'Knight', 'Rook']
            self.__board = [[Chess.Square() for x in range(8)] for y in range(8)]
            for i in range(8): 
                self.__board[1][i].set(Chess.Piece(Chess.Type['Pawn'], Chess.Color['White'])) # White's pawns
                self.__board[0][i].set(Chess.Piece(Chess.Type[initList[i]], Chess.Color['White'])) # White's backrank pieces
                self.__board[6][i].set(Chess.Piece(Chess.Type['Pawn'], Chess.Color['Black'])) # Black's pawns
                self.__board[7][i].set(Chess.Piece(Chess.Type[initList[i]], Chess.Color['Black'])) # Black's backrank pieces
                
            self.king_position_wb = [Chess.Position(4, 0), Chess.Position(4, 7)]
            self.__calAttackSquare()

        def move(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            if self.__board[cur.y][cur.x].piece.type == Chess.Type['King']:
                color = self.__board[cur.y][cur.x].piece.color
                self.king_position_wb[color] = dest
            
            self.__board[dest.y][dest.x].piece = self.__board[cur.y][cur.x].piece
            self.__board[cur.y][cur.x].piece = None

            self.__calAttackSquare()

        def castling(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            rank = (0 if self.__board[cur.y][cur.x].piece.color == Chess.Color['White'] else 7)
            if dest.x > cur.x: # King Side Castling
                rook_pos = Chess.Position(7, rank)
                self.move(cur, dest)
                self.move(rook_pos, dest + Chess.Position(-1, 0))
            else: # Queen Side Castling
                rook_pos = Chess.Position(0, rank)
                self.move(cur, dest)
                self.move(rook_pos, dest + Chess.Position(+1, 0))

        def en_passent(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            self.move(cur, dest)
            dir = Chess.Position(0, (-1 if self.__board[cur.y][cur.x].piece.color == Chess.Color['White'] else +1))
            attack = dest + dir
            self.__board[attack.y][attack.x] = None

        def get_legalMoveList(self, pos: 'Chess.Position'):
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
                        posList = self.get_legalMoveList(Chess.Position(x, y))
                        for pos in posList:
                            list.append((Chess.Position(x, y), pos))

            return list

        def get_square(self, pos: 'Chess.Position'):
            return self.__board[pos.y][pos.x]

        def print_board(self):
            # white criteria
            for i in range(7, -1, -1):
                for j in range(8):
                    square = self.__board[i][j]
                    if not square.empty():
                        print(f'{Chess.Color_to_string[square.piece.color]}{Chess.Type_to_string[square.piece.type]} ', end='')
                    else:
                        print('--', end=' ')
                print()

            for i in range(7, -1, -1):
                for j in range(8):
                    square = self.__board[i][j]
                    print(f'({square.attack_wb[0]}, {square.attack_wb[1]})  ', end='')
                print()


    def to_position(self, notation: str):
        return Chess.Position(ord(notation[0]) - ord('a'), ord(notation[1]) - ord('1'))
    
    def to_notation(self, position: 'Chess.Position'):
        return chr(position.x + ord('a')) + chr(position.y + ord('1'))



    def castling_Check(self, color: Literal[0, 1]):
        return True
        # TODO: Castling 조건 구현

    def castling(self, color: Literal[0, 1], side: Literal[2, 3]):
        if self.castling_Check(self): 
            print('castling')

    def en_passent(self):
        print('en_passent')

    def promotion(self):
        print('promotion')



    def __init__(self):
        self.board = self.Board()
        self.turn = self.Color['White']
        self.player = self.Color['White']

    def move(self, cur: str, dest: str):
        cur = self.to_position(cur)
        dest = self.to_position(dest)
        square = self.board.get_square(cur)

        ### piece existence check
        if square.empty():
            print('move()::there is no piece')
            return False
        
        ### piece color check
        if self.turn != square.piece.color:
            print('move()::it\'s not their\'s turn')
            return False
        
        ### move check
        isLegal = False
        legalMoveList = self.board.get_legalMoveList(cur)
        for move in legalMoveList:
            if move.x == dest.x and move.y == dest.y:
                isLegal = True
        
        if not isLegal:
            print('move()::it\'s illegal move')
            return False

        ### move
        print(f'{cur} -> {dest}')
        self.board.move(cur, dest)
        self.turn = self.Color['White'] if self.turn == self.Color['Black'] else self.Color['Black']

        return True

    def get_legalMove(self, pos: str):
        pos = self.to_position(pos)
        square = self.board.get_square(pos)

        ### piece existence check
        if square.empty():
            print('get_legalMove()::there is no piece')
            return False
        
        list = self.board.get_legalMoveList(pos)
        print(f'size : {len(list)}')
        for next in list:
            print(f'{self.to_notation(next)} ', end='')
        print()

    def get_candidateMove(self):
        list = self.board.get_candidateMove(self.turn)
        print(f'size : {len(list)}')
        for move in list:
            print(f'{self.to_notation(move[0])} -> {self.to_notation(move[1])} ')
        print()

    def start(self):
        while True:
            self.board.print_board()
            color = 'WHITE' if self.turn == self.Color['White'] else 'BLACK'
            print(f'{color}\'s turn')
            while True:
                command = int(input('[1] move [2] get legal move [3] get candidate move \n -> '))
                if command == 1:
                    cur, dest = input().split()
                    if not self.move(cur, dest):
                        continue
                elif command == 2:
                    pos = input()
                    self.get_legalMove(pos)
                    continue
                elif command == 3:
                    self.get_candidateMove()

                break



if __name__ == '__main__':
    chess = Chess()
    chess.start()
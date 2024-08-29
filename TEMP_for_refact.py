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
        def __init__(self, piece: 'Chess.Piece'):
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
        def __boundaryCheck(self, position: 'Chess.Position'):
            return True if position.x >= 0 and position.x <= 7 and position.y >= 0 and position.y <= 7 else False

        def __isAlly(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            return self.__board[cur.y][cur.x].empty() == False and self.__board[dest.y][dest.x].empty() == False and \
                self.__board[cur.y][cur.x].piece.color == self.__board[cur.y][cur.x].piece.color

        def __isEnemy(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            return self.__board[cur.y][cur.x].empty() == False and self.__board[dest.y][dest.x].empty() == False and \
                self.__board[cur.y][cur.x].piece.color != self.__board[cur.y][cur.x].piece.color

        def __isCheck(self, color: Literal[0, 1]):
            king = self.king_position_wb[color]
            return True if self.__board[king.y][king.x].isattacked(color) else False


        def __repeatMove(self, list: list, cur: 'Chess.Position', dir: 'Chess.Position'):
            next = cur
            while True:
                next += dir
                if not self.__boundaryCheck(next) or self.__isAlly(cur, next):
                    return
                
                if self.__board[next.y][next.x].empty():
                    list.append(next)
                elif self.__isEnemy(cur, next):
                    list.append(next)
                    return
                
                # TODO: legalMove function

        def __oneMove(self, list: list, cur: 'Chess.Position', dir: 'Chess.Position'):
            next = cur + dir
            if self.__boundaryCheck(next) and (self.__board[next.y][next.x].empty() or self.__isEnemy(cur, next)):
                list.append(next)

        def __rook(self, position: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_straight:
                self.__repeatMove(list, position, dir)
            return list

        def __bishop(self, position: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_diagonal:
                self.__repeatMove(list, position, dir)
            return list

        def __knight(self, position: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_knight:
                self.__oneMove(list, position, dir)
            return list

        def __king(self, position: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_all:
                self.__oneMove(list, position, dir)
            return list

        def __Queen(self, position: 'Chess.Position', legalMove: bool):
            list = []
            for dir in Chess.dir_all:
                self.__repeatMove(list, position, dir)
            return list

        def __pawn_move():
            print('__pawn_move()')
            # TODO: implement pawn move

        def __pawn_attack():
            print('__pawn_attack()')
            # TODO: implement pawn attck

        def __getMoveList(self, position: 'Chess.Position', legalMove: bool):
            list = []
            print(type(self.__board[position.y][position.x].piece.type))
            piece_type = self.__board[position.y][position.x].piece.type
            if piece_type == Chess.Type['King']:
                list = self.__king(position, legalMove)
            elif piece_type == Chess.Type['Queen']:
                list = self.__Queen(position, legalMove)
            elif piece_type == Chess.Type['Rook']:
                list = self.__rook(position, legalMove)
            elif piece_type == Chess.Type['Knight']:
                list = self.__knight(position, legalMove)
            elif piece_type == Chess.Type['Bishop']:
                list = self.__bishop(position, legalMove)
            elif piece_type == Chess.Type['Pawn']:
                print('Pawn move') # TODO

            return list

        def __calAttackSquare(self):
            for _ in self.__board:
                for square in _:
                    square.attack_wb[0] = 0
                    square.attack_wb[1] = 0
                    
            for x in range(0, 8):
                for y in range(0, 8):
                    if not self.__board[y][x].empty():
                        posList = self.__getMoveList(Chess.Position(x, y), legalMove=False)
                        for pos in posList:
                            color = self.__board[pos.y][pos.x].piece.color
                            self.__board[pos.y][pos.x].attack_wb[color] += 1



        def __init__(self):
            self.king_position_wb = []
            self.reset()
        
        def reset(self):
            initList = ['Rook', 'Knight', 'Bishop', 'Queen', 'King', 'Bishop', 'Knight', 'Rook']
            self.__board = [[Chess.Square(Chess.Position(x, y)) for x in range(8)] for y in range(8)]
            for i in range(8): 
                self.__board[1][i].set(Chess.Piece(Chess.Type['Pawn'], Chess.Color['White'])) # White's pawns
                self.__board[0][i].set(Chess.Piece(Chess.Type[initList[i]], Chess.Color['White'])) # White's backrank pieces
                self.__board[6][i].set(Chess.Piece(Chess.Type['Pawn'], Chess.Color['Black'])) # Black's pawns
                self.__board[7][i].set(Chess.Piece(Chess.Type[initList[i]], Chess.Color['Black'])) # Black's backrank pieces
                
            self.king_position_wb = [Chess.Position(0, 4), Chess.Position(7, 4)]
            self.__calAttackSquare()

        def move(self, cur: 'Chess.Position', dest: 'Chess.Position'):
            self.__board[dest.y][dest.x] = self.__board[cur.y][cur.x]
            self.__board[cur.y][cur.x] = None

        def capture(self, cur: 'Chess.Position', dest: 'Chess.Position', attack: 'Chess.Position'):
            self.__board[attack.y][attack.x] = None
            self.move(cur, dest)



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

    def start(self):
        print('game start')


if __name__ == '__main__':
    chess = Chess()
from typing import Literal
from pubsub import pub
import copy
import connector
from utility import *

class Chess:
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
        king = self.king_position_wb[color]
        return self.board[king.y][king.x].isAttacked(color)
    def __isThisMoveLegal(self, move: Move):
        chess_next = self.__clone()
        chess_next.__move(move)
        return not chess_next.__isCheck(move.piece.color)

    def __append(self, list: list, move: Move):
        if self.__isThisMoveLegal(move):
            list.append(move)
    def __repeatMove(self, list: list, piece: Piece, ori: Position, dir: Position):
        dest = ori
        while True:
            dest += dir
            if not self.__boundaryCheck(dest) or self.__isAlly(ori, dest):
                return
            
            if self.board[dest.y][dest.x].empty():
                self.__append(list, Move(piece, ori, dest))
            elif self.__isEnemy(ori, dest):
                self.__append(list, Move(piece, ori, dest, take=dest))
                return
    def __oneMove(self, list: list, piece: Piece, ori: Position, dir: Position):
        dest = ori + dir
        if self.__boundaryCheck(dest):
            if self.board[dest.y][dest.x].empty():
                self.__append(list, Move(piece, ori, dest))
            elif self.__isEnemy(ori, dest):
                self.__append(list, Move(piece, ori, dest, take=dest))
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
        
        if self.__castling_check(piece.color, isKingside=True): # Check king side castling 
            list.append(Move(piece, pos, pos + Position(+2, 0))) # already guarantee the check free move
        if self.__castling_check(piece.color, isKingside=False): # Check queen side castling 
            list.append(Move(piece, pos, pos + Position(-2, 0))) # already guarantee the check free move

        return list
    def __pawn(self, pos: Position, piece: Piece):
        list = []
        color = piece.color
        dir = (+1 if color == Color.WHITE else -1)
        # MOVE
        dest = pos
        dest.y += dir
        if self.board[dest.y][dest.x].empty():
            self.__append(list, Move(piece, pos, dest))
            if dest.y == (7 if color == Color.WHITE else 0): # promotion move
                for promotion in promotion_list:
                    self.__append(list, Move(piece, pos, dest, promotion_type=promotion))
            if pos.y == (1 if color == Color.WHITE else 6): # first move
                dest.y += dir
                if self.board[dest.y][dest.x].empty():
                    self.__append(list, Move(piece, pos, dest))
        
        # ATTACK
        # normal attack
        def attack(dest: Position):
            if self.__boundaryCheck(dest) and self.__isEnemy(pos, dest):
                if dest.y == (7 if color == Color.WHITE else 0): # takes and promotion
                    for promotion in promotion_list:
                        self.__append(list, Move(piece, pos, dest, take=dest, promotion_type=promotion))
                else:
                    self.__append(list, Move(piece, pos, dest, dest))
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
        piece = self.board[pos.y][pos.x].piece
        if piece.type == Piece_type.KING:
            list_ = self.__king(pos, piece)
        elif piece.type ==  Piece_type.QUEEN:
            list_ = self.__Queen(pos, piece)
        elif piece.type ==  Piece_type.ROOK:
            list_ = self.__rook(pos, piece)
        elif piece.type ==  Piece_type.KNIGHT:
            list_ = self.__knight(pos, piece)
        elif piece.type == Piece_type.BISHOP:
            list_ = self.__bishop(pos, piece)
        elif piece.type ==  Piece_type.PAWN:
            list_ = self.__pawn(pos, piece)
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
        dest = ori
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
        dir = (+1 if self.board[pos.y][pos.x].piece.color == Color.WHITE else -1)
        self.__oneMove(list, pos, Position(-1, dir))
        self.__oneMove(list, pos, Position(+1, dir))
        return list
    def __get_attackList(self, pos: Position):
        list = []
        piece_type = self.board[pos.y][pos.x].piece.type
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
        for _ in self.board:
            for square in _:
                square.attack_wb = [0, 0]

        for y in range(0, 8):
            for x in range(0, 8):
                if not self.board[y][x].empty():
                    posList = self.__get_attackList(Position(x, y))
                    color = self.board[y][x].piece.color
                    for pos in posList:
                        self.board[pos.y][pos.x].attack_wb[color] += 1

    def __move(self, move: Move):
        if move.piece.type == Piece_type.KING:
            self.king_position_wb[move.piece.color] = move.dest
        self.board[move.dest.y][move.dest.x].piece = self.board[move.ori.y][move.ori.x].piece
        self.board[move.ori.y][move.ori.x].piece = None
    def __capture(self, move: Move):
        self.board[move.take.y][move.take.x].piece = None
        self.move(move)
    def __castling(self, move: Move):
        rank = (0 if move.piece.color == Color.WHITE else 7)
        if move.dest.x > move.ori.x: # king side 
            rook_pos = Position(7, rank)
            self.move(move)
            self.move(Move(Piece(Piece_type.ROOK, move.piece.color), rook_pos, move.dest + Position(-1, 0)))
        else: # queen side
            rook_pos = Position(0, rank)
            self.move(move)
            self.move(Move(Piece(Piece_type.ROOK, move.piece.color), rook_pos, move.dest + Position(+1, 0)))
    def __promotion(self, move: Move):
        self.board[move.dest.y][move.dest.x].set(Piece(move.promotion_type, move.piece.color))
    def __play(self, move: Move):
        # King, Rook move check for Castling
        if move.piece.type == Piece_type.KING:
            self.kr_moveCheck_wb_qk[move.piece.color][0] = self.kr_moveCheck_wb_qk[move.piece.color][1] = True
        elif move.piece.type == Piece_type.ROOK:
            if move.ori.x == 0:
                self.kr_moveCheck_wb_qk[move.piece.color][0] = True
            elif move.ori.x == 7:
                self.kr_moveCheck_wb_qk[move.piece.color][1] = True

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
        self.prevMove = move
        self.turn = opponent(self.turn)

    def __castling_check(self, color: Color, isKingside: bool):
        rank = (0 if color == Color.WHITE else 7)
        condition_1 = not self.kr_moveCheck_wb_qk[color][1 if isKingside else 0]
        condition_2 = self.board[rank][5].empty() and self.board[rank][6].empty() if isKingside else \
            self.board[rank][3].empty() and self.board[rank][2].empty() and self.board[rank][1].empty()
        condition_3 = not self.board[rank][5].isAttacked(color) and not self.board[rank][6].isAttacked(color) if isKingside else \
            not self.board[rank][3].isAttacked(color) and not self.board[rank][2].isAttacked(color)
        return condition_1 and condition_2 and condition_3
    def __en_passent_check(self, pos: Position):
        piece_color = self.board[pos.y][pos.x].piece.color
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



    def __init__(self):
        self.king_position_wb = list[Position]
        self.kr_moveCheck_wb_qk = list[list[bool]]
        self.prev_move = Chess.PreviousMove()
        self.turn = Color.WHITE
        self.player = Color.WHITE
        self.init_value()

        self.board = [[Square() for x in range(8)] for y in range(8)]
        self.init_board()
        
        connector.set_color(opponent(self.player))
        pub.subscribe(self.AI_move, 'Player_move')
    
    def init_board(self):
        for _ in self.board:
            for square in _:
                square.piece = None

        for i in range(8): 
            self.board[1][i].set(Piece(Piece_type.PAWN, Color.WHITE)) # White's pawns
            self.board[0][i].set(Piece(initList[i], Color.WHITE)) # White's backrank pieces
            self.board[6][i].set(Piece(Piece_type.PAWN, Color.BLACK)) # Black's pawns
            self.board[7][i].set(Piece(initList[i], Color.BLACK)) # Black's backrank pieces
            
        self.__calAttackSquare()
    
    def init_value(self):
        self.king_position_wb = [Position(4, 0), Position(4, 7)]
        self.kr_moveCheck_wb_qk = [[False, False],[False, False]]
        self.prev_move = Move()

    def gameEnd_check(self, color: Color): # (0) None (1) CheckMate (2) StaleMate
        if self.__count_candidateMove(color) != 0:
            return 0
        
        if self.__isCheck(color):
            return 1
        else:
            return 2
    
### get function (for UI)
    def get_legalMove(self, pos: Position):
        return self.__legal_moves(pos)

    def play(self, move: Move): # (-1) can't move (0) None (1) CheckMate (2) StaleMate
        print('chess.move function')
        square = self.board[move.ori.y][move.ori.x]

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
        legalMoveList = self.__legal_moves(move.ori)
        for legalMove in legalMoveList:
            if move.isEqual(legalMove):
                isLegal = True
                break
        
        if not isLegal:
            print('move()::it\'s illegal move')
            return -1
        
        if square.piece.type == Piece_type.PAWN and dest.y == (7 if square.piece.color == Chess.Color['White'] else 0) and promotion == None: # promotion check
            print('promotion signal !!')
            return 3
        
        ### Move
        self.board.move(cur, dest, promotion)
        self.board.print_board()
        self.turn = self.Color['White'] if self.turn == self.Color['Black'] else self.Color['Black']
        print(f'[turn] : {"WHITE" if self.turn == 0 else "BLACK"}')

        ### game end check
        return self.board.gameEnd_check(self.turn)

    def AI_Play(self, promotion):
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
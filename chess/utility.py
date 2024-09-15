from enum import Enum

class Color(Enum):
    NOCOLOR = -1
    WHITE = 0
    BLACK = 1
def opponent(color: Color):
    return Color.BLACK if color == Color.WHITE else Color.WHITE

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

class Gameover_type(Enum):
    NOGAMEOVER = -1
    CHECKMATE_BLACK = 0
    CHECKMATE_WHITE = 1
    STALEMATE = 2

class Position:
    def __init__(self, x: int = -1, y: int = -1):
        self.x = x
        self.y = y
        
    def __add__(self, other):
        return Position(self.x + other.x, self.y + other.y)
    def isEqual(self, other):
        if isinstance(other, Position):
            return self.x == other.x and self.y == other.y

class Piece:
    def __init__(self, type: Piece_type = Piece_type.NOPIECE, color: Color = Color.NOCOLOR):
        self.type = type
        self.color = color

class Move:
    def __init__(self, piece: Piece = Piece(), ori: Position = Position(), dest: Position = Position(), take: Position = None, promotion_type: Piece_type = None):
        self.piece = piece
        self.ori = ori
        self.dest = dest
        self.take = take if take != None else Position()
        self.promotion_type  = promotion_type if promotion_type != None else Piece_type.NOPIECE
    
    def get_move_type(self):
        if self.piece.type == Piece_type.KING and abs(self.dest.x - self.ori.x) == 2:
            return Move_type.CASTLING
        elif self.promotion_type != Piece_type.NOPIECE:
            return (Move_type.MOVE_PRO if self.take.x == -1 else Move_type.CAPTURE_PRO)
        elif self.take.x != -1:
            return Move_type.CAPTURE
        elif self.take.x == -1:
            return Move_type.MOVE
        else: 
            return Move_type.NOMOVE
    def get_string(self):
        st = f'{str(self.ori.x)} {str(self.ori.y)} {str(self.dest.x)} {str(self.dest.y)} {str(self.take.x)} {str(self.take.y)} '
        if self.promotion_type == Piece_type.NOPIECE:
            st += '-1'
        else:
            st += f'{self.promotion_type.value}'
        return st
    def string_init(self, text: str):
        st = text.split()
        self.ori = Position(int(st[0]), int(st[1]))
        self.dest = Position(int(st[2]),int(st[3]))
        self.take = Position(int(st[4]), int(st[5]))
        self.promotion_type = promotion_list[int(st[6])]
    def isEqual(self, other):
        if isinstance(other, Move):
            return self.piece.type == other.piece.type and self.piece.color == other.piece.color and \
                self.ori.isEqual(other.ori) and self.dest.isEqual(other.dest) and self.take.isEqual(other.take) and self.promotion_type == other.promotion_type

class Square:
    def __init__(self, piece: Piece = None):
        self.piece = piece
        self.attack_wb = [0, 0]

    def set(self, piece: Piece):
        self.piece = piece 
    def isAttacked(self, color: Color):
        return True if self.attack_wb[opponent(color).value] != 0 else False
    def empty(self):
        return True if self.piece == None else False

def to_position(notation: str):
    return Position(ord(notation[0]) - ord('a'), ord(notation[1]) - ord('1'))
    
def to_notation(position: Position):
    return chr(position.x + ord('a')) + chr(position.y + ord('1')) if position.x != -1 else 'NULL'

dir_straight = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) }
dir_diagonal = { Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
dir_all = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
dir_knight = { Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2)}

initPos_type = (Piece_type.ROOK, Piece_type.KNIGHT, Piece_type.BISHOP, Piece_type.QUEEN, Piece_type.KING, Piece_type.BISHOP, Piece_type.KNIGHT, Piece_type.ROOK)
promotion_list = (Piece_type.QUEEN, Piece_type.ROOK, Piece_type.BISHOP, Piece_type.KNIGHT, Piece_type.NOPIECE)
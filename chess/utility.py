from enum import Enum

class Color(Enum):
    NOCOLOR = -1
    WHITE = 0
    BLACK = 1
def opponent(color: Color):
    return Color.BLACK if color == Color.WHITE else Color.WHITE
def Getcolor(value: int):
    return Color.WHITE if value == Color.WHITE.value else Color.BLACK

class Move_type(Enum):
    NOMOVE = -1,
    CASTLING_OO = 0
    CASTLING_OOO = 1
    CASTLING_TYPE_NB = 2
    MOVE = 3
    CAPTURE = 4
    MOVE_PRO = 5
    CAPTURE_PRO = 6
class Piece_type(Enum):
    NOPIECE = -1,
    QUEEN = 0
    ROOK = 1
    BISHOP = 2
    KNIGHT = 3
    KING = 4
    PAWN = 5
def GetpieceType(value: int):
    if value == Piece_type.QUEEN.value:
        return Piece_type.QUEEN
    elif value == Piece_type.ROOK.value:
        return Piece_type.ROOK
    elif value == Piece_type.BISHOP.value:
        return Piece_type.BISHOP
    elif value == Piece_type.KNIGHT.value:
        return Piece_type.KNIGHT
    elif value == Piece_type.KING.value:
        return Piece_type.KING
    elif value == Piece_type.PAWN.value:
        return Piece_type.PAWN

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

# Move message format 
# [color][pieceType][moveType][ori][dest][take][promotion]
# [int  ][int      ][int     ][str][str ][str ]['-'/int  ]
# [1    ][1        ][1       ][2  ][2   ][2   ][1        ]
class Move:
    def __init__(self, piece: Piece = Piece(), ori: Position = Position(), dest: Position = Position(), take: Position = None, promotion_type: Piece_type = None):
        self.piece = piece
        self.ori = ori
        self.dest = dest
        self.take = take if take != None else Position()
        self.promotion_type  = promotion_type if promotion_type != None else Piece_type.NOPIECE
    
    def get_move_type(self):
        if self.piece.type == Piece_type.KING and abs(self.dest.x - self.ori.x) == 2:
            return (Move_type.CASTLING_OO if self.dest.x - self.ori.x > 0 else Move_type.CASTLING_OOO)
        elif self.promotion_type != Piece_type.NOPIECE:
            return (Move_type.MOVE_PRO if self.take.x == -1 else Move_type.CAPTURE_PRO)
        elif self.take.x != -1:
            return Move_type.CAPTURE
        elif self.take.x == -1:
            return Move_type.MOVE
        else: 
            return Move_type.NOMOVE
    def get_string(self):
        st = f'{to_notation(self.ori)}{to_notation(self.dest)}'
        if self.promotion_type == Piece_type.NOPIECE:
            st += '-'
        else:
            st += f'{self.promotion_type.value}'
        return st
    def string_init(self, st: str):
        self.piece = Piece(GetpieceType(int(st[1])), Getcolor(int(st[0])))
        self.ori = to_position(st[2:4])
        self.dest = to_position(st[4:6])
        self.take = to_position(st[6:8])
        if st[8] == '-':
            self.promotion_type = Piece_type.NOPIECE
        else:
            self.promotion_type = promotion_list[int(st[8])]
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
    return Position(ord(notation[0]) - ord('a'), ord(notation[1]) - ord('1')) if notation != '--' else Position()
def to_notation(position: Position):
    return chr(position.x + ord('a')) + chr(position.y + ord('1')) if position.x != -1 else '--'

dir_straight = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0) }
dir_diagonal = { Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
dir_all = { Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0), Position(1, 1), Position(1, -1), Position(-1, -1), Position(-1, 1) }
dir_knight = { Position(1, 2), Position(-1, 2), Position(2, 1), Position(-2, 1), Position(2, -1), Position(-2, -1), Position(1, -2), Position(-1, -2)}

initPos_type = (Piece_type.ROOK, Piece_type.KNIGHT, Piece_type.BISHOP, Piece_type.QUEEN, Piece_type.KING, Piece_type.BISHOP, Piece_type.KNIGHT, Piece_type.ROOK)
promotion_list = (Piece_type.QUEEN, Piece_type.ROOK, Piece_type.BISHOP, Piece_type.KNIGHT, Piece_type.NOPIECE)

if __name__ == '__main__':
    move = Move()
    move.string_init('153g7g5---')
    print(move.get_string())
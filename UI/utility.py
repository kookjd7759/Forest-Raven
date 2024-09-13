from chess.utility import *
import path
CELL_SIZE = 60
BOARD_SIZE = 480

PRO_WINDOW_SIZE = (CELL_SIZE + 15) * 2
WINDOW_POINT = {
    'LU': (BOARD_SIZE // 2 - PRO_WINDOW_SIZE // 2, BOARD_SIZE // 2 - PRO_WINDOW_SIZE // 2),
    'RD': (BOARD_SIZE // 2 + PRO_WINDOW_SIZE // 2, BOARD_SIZE // 2 + PRO_WINDOW_SIZE // 2)
}

def piece_img_path(piece: Piece):
    return path.getImgFolder() + ('w' if piece.Color == Color.WHITE else 'b') + (type_to_symbol[piece.type]) + '.png'

type_to_symbol = {
    Piece_type.QUEEN: 'q',
    Piece_type.ROOK: 'r',
    Piece_type.BISHOP: 'b',
    Piece_type.KNIGHT: 'n',
    Piece_type.KING: 'k',
    Piece_type.PAWN: 'p',
}
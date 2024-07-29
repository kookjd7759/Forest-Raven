import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import Qt, QPoint

import Path

class ChessPiece(QLabel):
    def posToNotation(self, x, y):
        rank = ['1', '2', '3', '4', '5', '6', '7', '8']
        file = ['h', 'g', 'f', 'e', 'd', 'c', 'b', 'a']
        return file[x] + rank[y]

    def __init__(self, image_path, parent, x, y):
        super().__init__(parent)
        self.pos_x = x
        self.pos_y = y

        self.default_size = 60
        self.setPixmap(QPixmap(image_path))
        self.setFixedSize(self.default_size, self.default_size)
        self.setScaledContents(True)
        self.setMouseTracking(True)
        self.moving = False
        self.offset = QPoint()

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.moving = True
            self.raise_()  # Bring the piece to the top
            
            center_offset = QPoint(self.width() // 2, self.height() // 2)
            new_pos = self.mapToParent(event.pos() - center_offset)
            self.move(new_pos)
            self.offset = event.pos() - center_offset

            self.boundaryCheck()

    def mouseMoveEvent(self, event):
        if self.moving:
            center_offset = QPoint(self.width() // 2, self.height() // 2)
            new_pos = self.mapToParent(event.pos() - center_offset)
            self.move(new_pos)
            self.offset = event.pos() - center_offset
            
            self.boundaryCheck()

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.moving = False
            self.snap_to_grid(self.parent().mapFromGlobal(self.mapToGlobal(event.pos())))

    def boundaryCheck(self):
        parent = self.parent()
        
        # 기물의 현재 위치를 부모 위젯의 좌표계로 변환한 직사각형 영역을 가져옵니다
        piece_rect = self.rect().translated(self.mapToParent(QPoint(0, 0)))
        parent_rect = parent.rect()

        if piece_rect.left() < -30:
            self.move(-30, self.y()) 
        if piece_rect.top() < -30:
            self.move(self.x(), -30) 
        
        if piece_rect.right() > parent_rect.width() + 30:
            self.move(parent_rect.width() - self.width() + 30, self.y()) 
        
        if piece_rect.bottom() > parent_rect.height() + 30:
            self.move(self.x(), parent_rect.height() - self.height() + 30) 

    def snap_to_grid(self, mousePos):
        grid_size = 60  
        new_x = mousePos.x() // grid_size
        new_y = mousePos.y() // grid_size
        new_x = max(0, min(new_x, 7))
        new_y = max(0, min(new_y, 7))
        self.move(new_x * grid_size, new_y * grid_size)

        print(f'Move {self.posToNotation(self.pos_x, self.pos_y)} -> {self.posToNotation(new_x, new_y)}')
        
        self.pos_x = new_x
        self.pos_y = new_y



class ChessBoard(QMainWindow):

    def load_img(self):
        self.img_board = Path.getImgFolder() + 'Ground.png'
        self.img_bq = Path.getImgFolder() + 'bq.png'
        self.img_bk = Path.getImgFolder() + 'bk.png'
        self.img_bb = Path.getImgFolder() + 'bb.png'
        self.img_bn = Path.getImgFolder() + 'bn.png'
        self.img_bp = Path.getImgFolder() + 'bp.png'
        self.img_br = Path.getImgFolder() + 'br.png'
        
        self.img_wq = Path.getImgFolder() + 'wq.png'
        self.img_wk = Path.getImgFolder() + 'wk.png'
        self.img_wb = Path.getImgFolder() + 'wb.png'
        self.img_wn = Path.getImgFolder() + 'wn.png'
        self.img_wp = Path.getImgFolder() + 'wp.png'
        self.img_wr = Path.getImgFolder() + 'wr.png'

        self.piece_images = {
            'bb': self.img_bb,
            'bk': self.img_bk,
            'bn': self.img_bn,
            'bq': self.img_bq,
            'br': self.img_br,
            'bp': self.img_bp,
            
            'wb': self.img_wb,
            'wk': self.img_wk,
            'wn': self.img_wn,
            'wq': self.img_wq,
            'wr': self.img_wr,
            'wp': self.img_wp,
        }

    def __init__(self):
        super().__init__()
        self.load_img()
        self.setFixedSize(480, 480)  # Set size of the chessboard

        self.lbl_board = QLabel(self)
        self.lbl_board.setPixmap(QPixmap(self.img_board))
        self.lbl_board.setFixedSize(480, 480)
        self.lbl_board.setScaledContents(True)

        # Initialize pieces
        self.pieces = [[None for _ in range(8)] for _ in range(8)]  # 2D list to keep track of pieces
        self.init_pieces()

    def create_piece(self, st, x, y):
        lbl_piece = ChessPiece(self.piece_images[st], self, x, y)
        lbl_piece.move(x * 60, y * 60)
        self.pieces[y][x] = lbl_piece  # Update the 2D list with the new piece
        return lbl_piece
    
    def init_pieces(self):
        white_initPos = [ 'wr', 'wn', 'wb', 'wk', 'wq', 'wb', 'wn', 'wr']
        black_initPos = [ 'br', 'bn', 'bb', 'bk', 'bq', 'bb', 'bn', 'br']

        for col in range(8):
            self.create_piece('wp', col, 1)  # White pawns
            self.create_piece(white_initPos[col], col, 0)  # White other pieces
            self.create_piece('bp', col, 6)  # Black pawns
            self.create_piece(black_initPos[col], col, 7)  # Black other pieces



if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = ChessBoard()
    mainWindow.show()
    sys.exit(app.exec_())

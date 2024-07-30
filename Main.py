import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

import Path

def posToNotation(x, y):
    if x == -1 and y == -1:
        return 'None'
    rank = ['1', '2', '3', '4', '5', '6', '7', '8']
    file = ['h', 'g', 'f', 'e', 'd', 'c', 'b', 'a']
    return file[x] + rank[y]



class ChessPiece(QLabel):
    def __init__(self, image_path, parent, x, y, Type):
        super().__init__(parent)
        self.pos_x = x
        self.pos_y = y
        self.type = Type

        self.default_size = 60
        self.setPixmap(QPixmap(image_path))
        self.setFixedSize(self.default_size, self.default_size)
        self.setScaledContents(True)
        self.setMouseTracking(True)
        self.moving = False

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.moving = True
            self.raise_()  # Bring the piece to the top
            
            center_offset = QPoint(self.width() // 2, self.height() // 2)
            new_pos = self.mapToParent(event.pos() - center_offset)
            self.move(new_pos)

            self.boundaryCheck()

    def mouseMoveEvent(self, event):
        if self.moving:
            center_offset = QPoint(self.width() // 2, self.height() // 2)
            new_pos = self.mapToParent(event.pos() - center_offset)
            self.move(new_pos)

            self.boundaryCheck()

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.moving = False
            self.snap_to_grid(self.parent().mapFromGlobal(self.mapToGlobal(event.pos())))

    def boundaryCheck(self):
        parent = self.parent()
        
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
        parent = self.parent()

        grid_size = 60  
        next_x = max(0, min(mousePos.x() // grid_size, 7))
        next_y = max(0, min(mousePos.y() // grid_size, 7))

        print(f'({self.pos_x},{self.pos_y}) -> ({next_x},{next_y})')

        selected = parent.isclick(self.pos_x, self.pos_y, next_x, next_y)
        if selected:
            print('click !')
            self.move(next_x * grid_size, next_y * grid_size)
        else:
            print('move !')
            parent.move(self.pos_x, self.pos_y, next_x, next_y)

        self.pos_x = next_x
        self.pos_y = next_y

    def deleteSelf(self):
        self.deleteLater()


class ChessBoard(QWidget):

    def update_selectedPiece(self, x, y):
        self.line_selected.setText(posToNotation(x, y))
        templist = list(self.selected_piece)
        templist[0] = x
        templist[1] = y
        self.selected_piece = tuple(templist)



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

    def load_findChildren(self):
        self.line_selected = self.findChildren(QLineEdit)[0]

    def __init__(self):
        super().__init__()

        self.load_img()
        self.selected_piece = (-1, -1)

        self.UIinit()
        self.load_findChildren()

        # Initialize pieces
        self.pieces = [[None for _ in range(8)] for _ in range(8)]  # 2D list to keep track of pieces
        self.init_pieces()


    def UIinit(self):
        self.setFixedSize(480, 500)  # Set size of the chessboard
        vbox = QVBoxLayout()

        lbl_board = QLabel(self)
        lbl_board.setPixmap(QPixmap(self.img_board))
        lbl_board.setFixedSize(480, 480)
        lbl_board.setScaledContents(True)

        hbox = QHBoxLayout()
        lbl_selected = QLabel('<b>[Selected Piece]</b>', self)
        line_selected = QLineEdit('None', self)
        line_selected.setReadOnly(True)
        hbox.addWidget(lbl_selected)
        hbox.addWidget(line_selected)

        vbox.addWidget(lbl_board)
        vbox.addLayout(hbox)
        vbox.setContentsMargins(0,0,0,0)

        self.setLayout(vbox)

    def create_piece(self, st, x, y, type):
        lbl_piece = ChessPiece(self.piece_images[st], self, x, y, type)
        lbl_piece.move(x * 60, y * 60)
        self.pieces[y][x] = lbl_piece  # Update the 2D list with the new piece
        return lbl_piece
    
    def init_pieces(self):
        white_initPos = [ 'wr', 'wn', 'wb', 'wk', 'wq', 'wb', 'wn', 'wr']
        black_initPos = [ 'br', 'bn', 'bb', 'bk', 'bq', 'bb', 'bn', 'br']
        typeList = [ 'R', 'N', 'B', 'K', 'Q', 'B', 'N', 'R']

        for i in range(8):
            self.create_piece('wp', i, 1, 'P')  # White pawns
            self.create_piece(white_initPos[i], i, 0, typeList[i])  # White other pieces
            self.create_piece('bp', i, 6, 'P')  # Black pawns
            self.create_piece(black_initPos[i], i, 7, typeList[i])  # Black other pieces

    def isclick(self, now_x, now_y, next_x, next_y):
        if now_x == next_x and now_y == next_y: # click command
            if (self.selected_piece[0] == -1 and self.selected_piece[1] == -1): # No selected 
                self.update_selectedPiece(now_x, now_y)
            else: # Already selected 
                self.update_selectedPiece(-1, -1)
            return True
        else: # Move command
            self.update_selectedPiece(-1, -1)
            return False

    def move(self, now_x, now_y, next_x, next_y):
        if self.pieces[next_y][next_x] != None:
            self.pieces[next_y][next_x].deleteSelf()

        print(f'Move {posToNotation(now_x, now_y)} -> {posToNotation(next_x, next_y)}')
        self.pieces[now_y][now_x].pos_x = next_x
        self.pieces[now_y][now_x].pos_y = next_y
        self.pieces[next_y][next_x] = self.pieces[now_y][now_x]
        self.pieces[now_y][now_x].move(next_x * 60, next_y * 60)
        self.pieces[now_y][now_x] = None

    def print2DInfo(self):
        for i in range(8):
            for j in range(8):
                if self.pieces[i][j] != None:
                    print(self.pieces[i][j].type, end=' ')
                else:
                    print('-', end=' ')
            print()


    def mousePressEvent(self, event):
        grid_size = 60  
        if event.button() == Qt.LeftButton:
            mousePos = self.mapFromGlobal(self.mapToGlobal(event.pos()))
            x = mousePos.x() // grid_size
            y = mousePos.y() // grid_size
            print(f'{posToNotation(x, y)} clicked !')
            if self.selected_piece[0] != -1 or self.selected_piece[1] != -1:
                self.move(self.selected_piece[0], self.selected_piece[1], x, y)
                self.update_selectedPiece(-1, -1)

        self.print2DInfo()



if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = ChessBoard()
    mainWindow.show()
    sys.exit(app.exec_())

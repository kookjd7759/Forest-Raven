import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

import Path

def boardPosToNotation(x, y): # Get Board Position
    if x == -1 and y == -1:
        return 'None'
    rank = ['1', '2', '3', '4', '5', '6', '7', '8']
    file = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
    return file[x] + rank[y]

def UI_Board_positionConverter(x, y, isPlayerWhite): # UI to Board / Board to UI Converting
    if isPlayerWhite:
        return x, 7 - y
    else:
        return 7 - x, y
    
def mousePosToBoardPos(x, y, isPlayerWhite): # low Mouse Position to Board position
    grid_size = 60
    x, y = UI_Board_positionConverter(x // grid_size, y // grid_size, isPlayerWhite)
    return x, y




class ChessPiece(QLabel):

    def __init__(self, image_path, parent, UIx, UIy, Type): # Get UI Position
        print(f'Create Piece[{Type}] at ({UIx}, {UIy}) with UI position')
        print(image_path)
        super().__init__(parent)
        self.UIx = UIx # UI Position
        self.UIy = UIy # UI Position
        self.move(self.UIx * 60, self.UIy * 60)
        self.type = Type

        self.default_size = 60
        self.setPixmap(QPixmap(image_path))
        self.setFixedSize(self.default_size, self.default_size)
        self.setScaledContents(True)
        self.setMouseTracking(True)
        self.moving = False
        self.raise_() 
        self.show() 

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

        next_UIx = max(0, min(mousePos.x() // grid_size, 7)) # UI Position
        next_UIy = max(0, min(mousePos.y() // grid_size, 7)) # UI Position

        selected = parent.isclick(self.UIx, self.UIy, next_UIx, next_UIy)
        if selected:
            x, y = UI_Board_positionConverter(next_UIx, next_UIy, parent.isPlayerWhite) # UI to Board Position Convert
            print(f'{boardPosToNotation(x, y)} click !')
            self.move(next_UIx * grid_size, next_UIy * grid_size)
        else:
            now_x, now_y = UI_Board_positionConverter(self.UIx, self.UIy, parent.isPlayerWhite)
            next_x, next_y = UI_Board_positionConverter(next_UIx, next_UIy, parent.isPlayerWhite)
            parent.move_piece(now_x, now_y, next_x, next_y)

        self.UIx = next_UIx
        self.UIy = next_UIy

    def die(self):
        self.deleteLater()



class HighLightSquare(QLabel):
    def __init__(self, parent):
        super().__init__(parent)
        self.setFixedSize(60, 60)
        self.setStyleSheet("""
            background-color: rgba(255, 255, 130, 255);
            border: 2px solid rgba(240, 240, 240, 130);
        """)
        self.hide()

    def highlight(self, UIx, UIy): # Get UI Position
        self.move(UIx * 60, UIy * 60)
        self.raise_()
        x, y = UI_Board_positionConverter(UIx, UIy, self.parent().isPlayerWhite) # UI to Board Position Convert
        if (self.parent().pieces[y][x] != None):    
            self.parent().pieces[y][x].raise_()

        self.show()

    def clear(self):
        self.hide()



class ChessBoard(QWidget):
    
    def isExistSelectedPiece(self):
        if self.selected_piece[0] == -1 or self.selected_piece[1] == -1:
            return False
        return True

    def update_selectedPiece(self, x, y): # Get Board Position
        self.line_selected.setText(boardPosToNotation(x, y))

        if x == -1 or y == -1:
            self.square_highlight.clear()
        else:
            UIx, UIy = UI_Board_positionConverter(x, y, self.isPlayerWhite) # Board to UI Position Convert
            self.square_highlight.highlight(UIx, UIy)

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
        self.line_player = self.findChildren(QLineEdit)[0]
        self.line_selected = self.findChildren(QLineEdit)[1]
        self.lbl_board = self.findChildren(QLabel)[0]



    def __init__(self):
        super().__init__()
        self.selected_piece = (-1, -1)
        self.isPlayerWhite = True
        self.load_img()

        self.UIinit()
        self.load_findChildren()

        # Initialize pieces
        self.pieces = [[None for _ in range(8)] for _ in range(8)]
        self.init_pieces()

    def UIinit(self):
        self.square_highlight = HighLightSquare(self)  # 강조 표시용 객체 생성
        
        self.setFixedSize(480, 530) # size of the window
        vbox = QVBoxLayout()

        lbl_board = QLabel(self)
        lbl_board.setPixmap(QPixmap(self.img_board))
        lbl_board.setFixedSize(480, 480) # size of the chessboard
        lbl_board.setScaledContents(True)

        # player
        hbox_player = QHBoxLayout()
        lbl_player = QLabel('<b>[Player]</b>', self)
        line_player = QLineEdit('White', self)
        line_player.setReadOnly(True)
        btn_changePlayer = QPushButton('Change my color', self)
        btn_changePlayer.clicked.connect(self.btn_changePlayer_function)
        btn_restart = QPushButton('Game Restart', self)
        btn_restart.clicked.connect(self.btn_gameRestart_function)
        hbox_player.addWidget(lbl_player)
        hbox_player.addWidget(line_player)
        hbox_player.addWidget(btn_changePlayer)
        hbox_player.addWidget(btn_restart)

        # selected
        hbox_selected = QHBoxLayout()
        lbl_selected = QLabel('<b>[Selected Piece]</b>', self)
        line_selected = QLineEdit('None', self)
        line_selected.setReadOnly(True)
        hbox_selected.addWidget(lbl_selected)
        hbox_selected.addWidget(line_selected)

        vbox.addWidget(lbl_board)
        vbox.addLayout(hbox_player)
        vbox.addLayout(hbox_selected)
        vbox.setContentsMargins(0,0,0,0)

        self.setLayout(vbox)

    def create_piece(self, st, x, y, type): # Get Board Position
        UIx, UIy = UI_Board_positionConverter(x, y, self.isPlayerWhite) # Board to UI Position Convert
        lbl_piece = ChessPiece(self.piece_images[st], self, UIx, UIy, type)
        self.pieces[y][x] = lbl_piece # Update the 2D list with the new piece
    
        
    def init_pieces(self):
        # reset
        for i in range(8): 
            for j in range(8): 
                if self.pieces[i][j] != None:
                    self.pieces[i][j].die()  # 체스말 삭제
        self.pieces = [[None for _ in range(8)] for _ in range(8)] # 체스말 리스트 초기화

        white_initPos = [ 'wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']
        black_initPos = [ 'br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br']
        typeList = [ 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']

        for i in range(8): 
            self.create_piece('wp', i, 1, 'P')  # White pawns
            self.create_piece(white_initPos[i], i, 0, typeList[i]) # White other pieces
            self.create_piece('bp', i, 6, 'P')  # Black pawns
            self.create_piece(black_initPos[i], i, 7, typeList[i]) # Black other pieces

    def isclick(self, now_x, now_y, next_x, next_y): # Get UI Position
        if now_x == next_x and now_y == next_y: # click command
            x, y = UI_Board_positionConverter(now_x, now_y, self.isPlayerWhite) # UI to Board Position Convert
            if (self.selected_piece[0] != x or self.selected_piece[1] != y): # No selected / Already selected But click other piece 
                self.update_selectedPiece(x, y)
            else: # Same piece two click 
                self.update_selectedPiece(-1, -1)
            return True
        else: # Move command
            self.update_selectedPiece(-1, -1)
            return False

    def move_piece(self, now_x, now_y, next_x, next_y): # Get Board Position
        if self.pieces[next_y][next_x] != None:
            self.pieces[next_y][next_x].die()

        print(f'Move {boardPosToNotation(now_x, now_y)} -> {boardPosToNotation(next_x, next_y)}')
        UIx, UIy = UI_Board_positionConverter(next_x, next_y, self.isPlayerWhite)
        self.pieces[now_y][now_x].UIx = UIx
        self.pieces[now_y][now_x].UIy = UIy
        self.pieces[next_y][next_x] = self.pieces[now_y][now_x]
        self.pieces[now_y][now_x].move(UIx * 60, UIy * 60)
        self.pieces[now_y][now_x] = None

    def print2DInfo(self):
        if self.isPlayerWhite:
            for i in range(7, -1, -1):
                for j in range(8):
                    if self.pieces[i][j] != None:
                        print(self.pieces[i][j].type, end=' ')
                    else:
                        print('-', end=' ')
                print()
        else:
            for i in range(8):
                for j in range(7, -1, -1):
                    if self.pieces[i][j] != None:
                        print(self.pieces[i][j].type, end=' ')
                    else:
                        print('-', end=' ')
                print()



    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            mousePos = self.mapFromGlobal(self.mapToGlobal(event.pos()))
            x, y = mousePosToBoardPos(mousePos.x(), mousePos.y(), self.isPlayerWhite)
            print(f'{boardPosToNotation(x, y)} clicked !')
            if self.isExistSelectedPiece():
                self.move_piece(self.selected_piece[0], self.selected_piece[1], x, y)
                self.update_selectedPiece(-1, -1)

        self.print2DInfo()



    def btn_changePlayer_function(self):
        if self.isPlayerWhite:
            self.isPlayerWhite = False
            self.line_player.setText('Black')
        else:
            self.isPlayerWhite = True
            self.line_player.setText('White')
            
        self.init_pieces()

    def btn_gameRestart_function(self):
        self.init_pieces()



if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = ChessBoard()
    mainWindow.show()
    sys.exit(app.exec_())

import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from typing import Literal

import Path
import chess

CELL_SIZE = 60
BOARD_SIZE = 480

class HighLightSquare(QLabel):

    def __init__(self, parent, UIpos: chess.Position):
        super().__init__(parent)
        self.UIpos = UIpos
        self.move(UIpos.x * CELL_SIZE, UIpos.y * CELL_SIZE)
        self.setFixedSize(CELL_SIZE, CELL_SIZE)
        self.hide()

    def on(self, isSelectLight):
        self.raise_()
        if isSelectLight:
            self.setStyleSheet('background-color: rgba(210, 210, 0, 170); border: 2px solid rgba(240, 240, 240, 120);')
        else:
            self.setStyleSheet('background-color: rgba(250, 150, 150, 180);')
        self.show()

    def off(self):
        self.hide()


class ChessPiece(QLabel):

    def die(self):
        self.deleteLater()

    def move_direct(self, UIpos: chess.Position):
        self.move(UIpos.x * CELL_SIZE, UIpos.y * CELL_SIZE)
        self.UIpos = UIpos

    def __init__(self, parent, piece_type: Literal['King', 'Queen', 'Rook', 'Knight', 'Bishop', 'Pawn'], 
                image_path: str, UIpos: chess.Position, color: Literal[0, 1], callback_press): # Get UI Position
        super().__init__(parent)
        self.UIpos = UIpos
        self.color = color
        self.piece_type = piece_type
        self.move(UIpos.x * CELL_SIZE, UIpos.y * CELL_SIZE)
        self.callback_press = callback_press

        self.setPixmap(QPixmap(image_path))
        self.setFixedSize(CELL_SIZE, CELL_SIZE)
        self.setScaledContents(True)
        self.setMouseTracking(True)

        self.show()
        self.raise_()

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.callback_press(self.UIpos)


class Window(QWidget):
### Utility
    def convert_position(self, pos: chess.Position): # UI to Board / Board to UI position converting
        return chess.Position(pos.x, 7 - pos.y) if self.chess.player == self.chess.Color['White'] else chess.Position(7 - pos.x, pos.y)

### load
    def load_img(self):
        self.img = {
            'board': Path.getImgFolder() + 'Ground.png',
            
            'bb': Path.getImgFolder() + 'bb.png',
            'bk': Path.getImgFolder() + 'bk.png',
            'bn': Path.getImgFolder() + 'bn.png',
            'bq': Path.getImgFolder() + 'bq.png',
            'br': Path.getImgFolder() + 'br.png',
            'bp': Path.getImgFolder() + 'bp.png',
            
            'wb': Path.getImgFolder() + 'wb.png',
            'wk': Path.getImgFolder() + 'wk.png',
            'wn': Path.getImgFolder() + 'wn.png',
            'wq': Path.getImgFolder() + 'wq.png',
            'wr': Path.getImgFolder() + 'wr.png',
            'wp': Path.getImgFolder() + 'wp.png'
        }

### legal move
    def set_legalMove(self):
        self.legalMove = self.chess.get_legalMove(self.selected)
        self.on_legalMove_light()

    def del_legalMove(self):
        self.off_legalMove_light()
        self.legalMove = []

### selected piece
    def isSelected(self):
        return self.selected.x != -1 and self.selected.y != -1

    def setSelect(self, pos: chess.Position): # Get Board Position
        if self.isSelected():
            self.del_legalMove()
            self.off_light(self.selected)
        self.line_selected.setText(chess.to_notation(pos))
        self.selected = pos
        self.on_light(self.selected, isSelectedLight=True)
        self.set_legalMove() 

    def delSelect(self):
        self.line_selected.setText('None')
        if self.isSelected():
            self.off_light(self.selected)
            self.del_legalMove()
        self.selected = chess.Position(-1, -1)

### Highlight
    def off_light(self, pos: chess.Position):
        UIpos = self.convert_position(pos)
        self.highlight[UIpos.y][UIpos.x].off()

    def on_light(self, pos: chess.Position, isSelectedLight):
        UIpos = self.convert_position(pos)
        self.highlight[UIpos.y][UIpos.x].on(isSelectedLight)
        if self.board[pos.y][pos.x] != None:
            self.board[pos.y][pos.x].raise_()

    def on_legalMove_light(self):
        for pos in self.legalMove:
            self.on_light(chess.Position(pos.x, pos.y), isSelectedLight=False)

    def off_legalMove_light(self):
        for pos in self.legalMove:
            self.off_light(chess.Position(pos.x, pos.y))

### Window initalize
    def __init__(self):
        super().__init__()
        self.load_img()
        self.UIinit()

        self.selected = chess.Position(-1, -1)
        self.chess = chess.Chess()
        self.legalMove: list[chess.Position] = []
        
        # Initialize pieces
        self.board: list[list[ChessPiece]] = [[None for _ in range(8)] for _ in range(8)]
        self.init_pieces()

        # Initialize Highlight sheet
        self.highlight = [[None for _ in range(8)] for _ in range(8)]
        for UIx in range(8):
            for UIy in range(8):
                self.highlight[UIy][UIx] = HighLightSquare(self, chess.Position(UIx, UIy))

    def UIinit(self):
            self.setFixedSize(BOARD_SIZE, 585) # size of the windows
            self.setWindowTitle('Chess')
            vbox = QVBoxLayout()

            lbl_board = QLabel(self)
            lbl_board.setPixmap(QPixmap(self.img['board']))
            lbl_board.setFixedSize(BOARD_SIZE, BOARD_SIZE) # size of the chessboard
            lbl_board.setScaledContents(True)

            # Player
            hbox_player = QHBoxLayout()
            lbl_player = QLabel('<b>[Player]</b>', self)
            line_player = QLineEdit('White', self)
            line_player.setReadOnly(True)
            btn_changePlayer = QPushButton('Change my color', self)
            btn_changePlayer.clicked.connect(self.btn_test_function)
            btn_restart = QPushButton('Game Restart', self)
            btn_restart.clicked.connect(self.btn_test_function)
            hbox_player.addWidget(lbl_player)
            hbox_player.addWidget(line_player)
            hbox_player.addWidget(btn_changePlayer)
            hbox_player.addWidget(btn_restart)

            # Selected
            hbox_selected = QHBoxLayout()
            lbl_selected = QLabel('<b>[Selected Piece]</b>', self)
            self.line_selected = QLineEdit('None', self)
            self.line_selected.setReadOnly(True)
            hbox_selected.addWidget(lbl_selected)
            hbox_selected.addWidget(self.line_selected)

            # Turn
            hbox_turn = QHBoxLayout()
            lbl_turn = QLabel('<b>[Turn]</b>', self)
            line_turn = QLineEdit('White', self)
            line_turn.setReadOnly(True)
            hbox_turn.addWidget(lbl_turn)
            hbox_turn.addWidget(line_turn)

            # TestButton
            hbox_test = QHBoxLayout()
            lbl_test = QLabel('<b>[test]</b>', self)
            btn_test= QPushButton('test btn', self)
            btn_test.clicked.connect(self.btn_test_function)
            hbox_test.addWidget(lbl_test)
            hbox_test.addWidget(btn_test)

            vbox.addWidget(lbl_board)
            vbox.addLayout(hbox_player)
            vbox.addLayout(hbox_selected)
            vbox.addLayout(hbox_turn)
            vbox.addLayout(hbox_test)
            vbox.setContentsMargins(0,0,0,0)

            self.setLayout(vbox)

### piece
    def create_piece(self, piece_type, img_key, pos: chess.Position, color: Literal[0, 1]): # Get Board Position
        UIpos = self.convert_position(pos) # Board to UI Position
        self.board[pos.y][pos.x] = ChessPiece(self, piece_type, self.img[img_key], UIpos, color, self.piece_callback_press)

    def init_pieces(self):
        for i in range(8): 
            for j in range(8): 
                if self.board[i][j] != None:
                    self.board[i][j].die() # delete Piece
                    self.board[i][j] = None
        
        initPos = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r']
        for i in range(8): 
            self.create_piece('Pawn', 'wp', chess.Position(i, 1), self.chess.Color['White'])  # White pawns
            self.create_piece(self.chess.initList[i],'w' + initPos[i], chess.Position(i, 0), self.chess.Color['White']) # White other pieces
            self.create_piece('Pawn', 'bp', chess.Position(i, 6), self.chess.Color['Black'])  # Black pawns
            self.create_piece(self.chess.initList[i], 'b' + initPos[i], chess.Position(i, 7), self.chess.Color['Black']) # Black other pieces

### move
    def move_piece(self, cur: chess.Position, dest: chess.Position):
        if self.board[cur.y][cur.x] == None:
            print('ERROR::move_piece(), there is no piece')
            exit(0)
        
        if self.board[dest.y][dest.x] != None:
            self.board[dest.y][dest.x].die()

        UIpos = self.convert_position(dest)
        self.board[cur.y][cur.x].move_direct(UIpos)

        if self.board[cur.y][cur.x].piece_type == 'King' and abs(dest.x - cur.x) == 2: # Castling
            rank = (0 if self.board[cur.y][cur.x].color == self.chess.Color['White'] else 7)
            if dest.x > cur.x: # King Side Castling
                print('# King Side Castling')
                self.move_piece(chess.Position(7, rank), dest + chess.Position(-1, 0))
            else: # Queen Side Castling
                print('# Queen Side Castling')
                self.move_piece(chess.Position(0, rank), dest + chess.Position(+1, 0))
        elif self.board[cur.y][cur.x].piece_type == 'Pawn': # En passent
            if cur.x != dest.x: # Pawn takes somthing
                if self.board[dest.y][dest.x] == None: # en_passent move
                    dir = chess.Position(0, (-1 if self.board[cur.y][cur.x].color == self.chess.Color['White'] else +1))
                    attack = dest + dir
                    self.board[attack.y][attack.x].die()
                    self.board[attack.y][attack.x].piece = None

        self.board[dest.y][dest.x] = self.board[cur.y][cur.x]
        self.board[cur.y][cur.x] = None
        

    def play_move(self, dest: chess.Position):
        if self.chess.move(self.selected, dest):
            self.move_piece(self.selected, dest)
        self.delSelect()

### Mouse event
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            mousePos = self.mapFromGlobal(self.mapToGlobal(event.pos()))
            pos = self.convert_position(chess.Position(mousePos.x() // CELL_SIZE, mousePos.y() // CELL_SIZE))
            print(f'{chess.to_notation(pos)} clicked !')
            if self.isSelected():
                self.play_move(pos)
        elif event.button() == Qt.RightButton:
            self.delSelect()

### callback 
    def piece_callback_press(self, UIpos: chess.Position):
        pos = self.convert_position(UIpos)
        #if self.board[pos.y][pos.x].color != self.chess.player and not self.isSelected():
        #    return
        if self.board[pos.y][pos.x].color != self.chess.turn and not self.isSelected():
            return
        elif self.board[pos.y][pos.x].color != self.chess.turn and self.isSelected():
            self.play_move(pos)
        else:
            self.setSelect(pos)

### Button function 
    def btn_test_function(self):
        print('test Button')


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from typing import Literal

import Path
import chess

CELL_SIZE = 60
BOARD_SIZE = 480


class GameEndWindow(QDialog): 
    # loser = 0.win-white 1.win-black 2.Draw-staleMate 3.Draw-By Repetition 4.Draw-piece shortage 
    def __init__(self, parent, callback, ret:Literal[0, 1, 2, 3]): 
        super().__init__(parent)
        self.ret = ret
        self.callback = callback
        self.initUI()

    def initUI(self):
        self.setFixedSize(170, 100)
        label = QLabel('test', self)
        label.setAlignment(Qt.AlignCenter)
        if self.ret != 0 and self.ret != 1:
            self.setWindowTitle('Draw')
            text = 'Draw\n'
            if self.ret == 2:
                text += 'StaleMate'
            elif self.ret == 3:
                text += 'Piece Shortage'
            label.setText(text)
        else:
            self.setWindowTitle('CheckMate')
            print(f'result : {self.ret}')
            label.setText(f'CheckMate\n {"White" if self.ret == 0 else "Black"} WIN !')

        btn = QPushButton('Restart', self)
        btn.clicked.connect(self.btn_function)
        vbox = QVBoxLayout()
        vbox.addWidget(label)
        vbox.addWidget(btn)
        self.setLayout(vbox)
        self.show()
    
    def btn_function(self):
        self.close()
        self.callback()


class PromotionWindow(QLabel):
    window_size = (CELL_SIZE + 15) * 2
    window_point = {
        'LU': (BOARD_SIZE // 2 - window_size // 2, BOARD_SIZE // 2 - window_size // 2),
        'RD': (BOARD_SIZE // 2 + window_size // 2, BOARD_SIZE // 2 + window_size // 2)
    }

    def loadImages(self):
        parent = self.parent()
        team = 'w' if self.player == 0 else 'b'
        self.images = {
            'q': parent.img[team + 'q'],
            'n': parent.img[team + 'n'],
            'r': parent.img[team + 'r'],
            'b': parent.img[team + 'b']
        }
    
    finished = pyqtSignal()
    def __init__(self, parent, player: Literal[0, 1], callback):
        super().__init__(parent)
        self.player = player
        self.callback = callback
        self.loadImages()
        self.UIinit()
        self.off()

    def UIinit(self):
        self.resize(BOARD_SIZE, BOARD_SIZE)
        
        background_label = QLabel(self)
        background_label.setStyleSheet('background-color: rgba(0, 0, 0, 60);')
        background_label.setFixedSize(BOARD_SIZE, BOARD_SIZE)

        
        window = QLabel(self)
        window.resize(self.window_size, self.window_size)
        window.move(self.window_point['LU'][0], self.window_point['LU'][1])

        window_background = QLabel(self)
        window_background.setStyleSheet('background-color: rgba(255, 255, 255, 230); border-radius: 4px;')
        window_background.setFixedSize(self.window_size, self.window_size)
        window_background.move(BOARD_SIZE // 2 - self.window_size // 2, BOARD_SIZE // 2 - self.window_size // 2)
        window_background.raise_()

        piece_positions = [(0, 0), (1, 0), (0, 1), (1, 1)]
        pieces = ['q', 'b', 'r', 'n']
        
        layout = QGridLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        for pos, piece in zip(piece_positions, pieces):
            label = QLabel(self)
            label.setPixmap(QPixmap(self.images[piece]))
            label.setFixedSize(CELL_SIZE, CELL_SIZE)
            label.setScaledContents(True)
            layout.addWidget(label, pos[0], pos[1])
        window.setLayout(layout)
        window.raise_()
    
    def on(self):
        self.setEnabled(True)
        self.raise_()
        self.show()

    def off(self):
        self.setEnabled(False)
        self.hide()

    # (0)Q (1)R (2)B (3)N
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            mousePos = self.parent().mapFromGlobal(self.mapToGlobal(event.pos()))
            x, y = mousePos.x(), mousePos.y()
            if x >= self.window_point['LU'][0] and y >= self.window_point['LU'][1] and x <= self.window_point['RD'][0] and y <= self.window_point['RD'][1]:
                idx = (0 if x < BOARD_SIZE // 2 else 1) + (0 if y < BOARD_SIZE // 2 else 2)
                self.callback(idx)
            else:
                self.callback(-1)
        self.finished.emit()
        self.off()


class HighLightSquare(QLabel):

    def __init__(self, parent, UIpos: chess.Position):
        super().__init__(parent)
        self.UIpos = UIpos
        self.move(UIpos.x * CELL_SIZE, UIpos.y * CELL_SIZE)
        self.setFixedSize(CELL_SIZE, CELL_SIZE)
        self.hide()

    def on(self, isSelectLight: bool):
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
        self.hide()
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

    def off_all_light(self):
        for _ in self.highlight:
            for light in _:
                light.off()

### Window initalize
    def __init__(self):
        super().__init__()
        self.load_img()
        self.UIinit()

        self.selected = chess.Position(-1, -1)
        self.chess = chess.Chess()
        self.legalMove: list[chess.Position] = []
        self.playing: bool = True
        self.promotion_num: int = -1
        
        # Initialize pieces
        self.board: list[list[ChessPiece]] = [[None for _ in range(8)] for _ in range(8)]
        self.init_pieces()

        # Initialize Highlight sheet
        self.highlight: list[list[HighLightSquare]] = [[None for _ in range(8)] for _ in range(8)]
        for UIx in range(8):
            for UIy in range(8):
                self.highlight[UIy][UIx] = HighLightSquare(self, chess.Position(UIx, UIy))
                
        self.promotion_window = PromotionWindow(self, self.chess.player, self.promotion_callback)

    def reset(self):
        self.selected = chess.Position(-1, -1)
        self.legalMove: list[chess.Position] = []
        self.playing = True
        self.chess.restart()
        self.init_pieces()
        self.off_all_light()

    def UIinit(self):
            self.setFixedSize(BOARD_SIZE, 560) # size of the windows
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
            btn_restart.clicked.connect(self.btn_restart_function)
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
            vbox.addLayout(hbox_test)
            vbox.setContentsMargins(0,0,0,0)

            self.setLayout(vbox)
    
    def gameEnd(self, ret:Literal[0, 1, 2, 3] = None):
        self.setEnabled(False)
        gameEnd_window = GameEndWindow(self, self.btn_restart_function, ret)
        gameEnd_window.exec_() 
        self.setEnabled(True)

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
    def capture(self, pos: chess.Position):
        if self.board[pos.y][pos.x] != None:
            self.board[pos.y][pos.x].die()
        self.board[pos.y][pos.x] = None

    def move_piece(self, cur: chess.Position, dest: chess.Position):
        if self.board[cur.y][cur.x] == None:
            print('ERROR::move_piece(), there is no piece')
            exit(0)
        
        UIpos = self.convert_position(dest)
        self.board[cur.y][cur.x].move_direct(UIpos)
        isPromotion = False
        if self.board[cur.y][cur.x].piece_type == 'King' and abs(dest.x - cur.x) == 2: # Castling
            rank = (0 if self.board[cur.y][cur.x].color == self.chess.Color['White'] else 7)
            if dest.x > cur.x: # King Side Castling
                print('# King Side Castling')
                self.move_piece(chess.Position(7, rank), dest + chess.Position(-1, 0))
            else: # Queen Side Castling
                print('# Queen Side Castling')
                self.move_piece(chess.Position(0, rank), dest + chess.Position(+1, 0))
        elif self.board[cur.y][cur.x].piece_type == 'Pawn':
            if dest.y == (7 if self.board[cur.y][cur.x].color == self.chess.Color['White'] else 0): # promotion
                isPromotion = True
            if cur.x != dest.x: # Pawn takes somthing
                if self.board[dest.y][dest.x] == None: # en_passent move
                    dir = chess.Position(0, (-1 if self.board[cur.y][cur.x].color == self.chess.Color['White'] else +1))
                    attack = dest + dir
                    self.capture(attack)

        self.capture(dest)
        self.board[dest.y][dest.x] = self.board[cur.y][cur.x]
        self.board[cur.y][cur.x] = None

        if isPromotion: # Promotion
            # (0)Queen (1)Rook (2)Bishop (3)Knight
            self.board[dest.y][dest.x].die() # delete Piece
            self.board[dest.y][dest.x] = None
            type_text = ''
            img_key_text = 'w' if self.chess.player == self.chess.Color['White'] else 'b'
            if self.promotion_num == 0: # Queen promotion
                type_text = 'Queen'
                img_key_text += 'q'
            elif self.promotion_num == 1:
                type_text = 'Rook'
                img_key_text += 'r'
            elif self.promotion_num == 2:
                type_text = 'Bishop'
                img_key_text += 'b'
            elif self.promotion_num == 3:
                type_text = 'Knight'
                img_key_text += 'n'
            self.create_piece(type_text, img_key_text, dest, self.chess.player)

    def play_move(self, dest: chess.Position):
        check = self.chess.move(self.selected, dest)
        self.delSelect()
        # (-1) can't move (0) None (1) CheckMate (2) StaleMate (3) Promotion (4) By Repetition (5) Piece Shortage -> TODO
        if check != -1:
            if check == 3:
                self.promotion()
                if self.promotion_num != -1: # promotion
                    self.move_piece(self.selected, dest)
                    self.chess.move(self.selected, dest, self.promotion_num)
            else:
                self.move_piece(self.selected, dest)
                if check == 1:
                    self.gameEnd(0 if self.chess.turn == 1 else 1) # White <-> Black Change
                elif check == 2:
                    self.gameEnd(2)
        self.selected = chess.Position(-1, -1)

    def promotion(self):
        self.promotion_window.finished.connect(self.promotion_finished)
        self.promotion_window.on()
        self.event_loop = QEventLoop()
        self.event_loop.exec_()

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
    
    def promotion_callback(self, piece: int):
        self.promotion_num = piece

    def promotion_finished(self):
        self.event_loop.quit()

### Button function 
    def btn_test_function(self):
        print('test function')

    def btn_restart_function(self):
        self.reset()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())
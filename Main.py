import sys
import threading
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

import Path
import connector

CELL_SIZE = 60
BOARD_SIZE = 480

def boardPosToNotation(x, y): # Get Board Position
    rank = ['1', '2', '3', '4', '5', '6', '7', '8']
    file = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
    return file[x] + rank[y]

def UI_Board_PosConv(x, y, isPlayerWhite): # UI to Board / Board to UI Converting
    if isPlayerWhite:
        return x, 7 - y
    else:
        return 7 - x, y



class HighLightSquare(QLabel):

    def __init__(self, parent, UIx, UIy):
        super().__init__(parent)
        self.UIx = UIx
        self.UIy = UIy
        self.move(UIx * CELL_SIZE, UIy * CELL_SIZE)
        self.setFixedSize(CELL_SIZE, CELL_SIZE)
        self.hide()

    def on(self, isSelectedLight):
        self.raise_()
        x, y = UI_Board_PosConv(self.UIx, self.UIy, self.parent().isPlayerWhite) # UI to Board Position Convert
        if self.parent().pieces[y][x] != None:
            self.parent().pieces[y][x].raise_()
        if isSelectedLight:
            self.setStyleSheet('background-color: rgba(220, 220, 0, 170); border: 2px solid rgba(240, 240, 240, 130);')
        else:
            self.setStyleSheet('background-color: rgba(250, 130, 130, 170);')
        self.show()

    def off(self):
        self.hide()



class ChessPiece(QLabel):

    def boundaryCheck(self):
        piece_rect = self.rect().translated(self.mapToParent(QPoint(0, 0)))

        if piece_rect.left() < -30:
            self.move(-30, self.y()) 
        if piece_rect.top() < -30:
            self.move(self.x(), -30) 
        if piece_rect.right() > BOARD_SIZE + 30:
            self.move(BOARD_SIZE - 30, self.y()) 
        if piece_rect.bottom() > BOARD_SIZE + 30:
            self.move(self.x(), BOARD_SIZE - 30)
     
    def followMouse(self, mousePos):
        self.raise_()
            
        pos = self.mapToParent(mousePos - QPoint(self.width() // 2, self.height() // 2))
        self.move(pos.x(), pos.y())
        self.boundaryCheck()
   
    def die(self):
        self.deleteLater()

    def move_smooth(self, next_UIx, next_UIy):
        self.animation.setStartValue(QPoint(self.UIx * CELL_SIZE, self.UIy * CELL_SIZE))
        self.animation.setEndValue(QPoint(next_UIx * CELL_SIZE, next_UIy * CELL_SIZE))
        
        self.loop = QEventLoop()
        self.animation.finished.connect(self.loop.quit)
        
        self.animation.start()
        self.loop.exec_()

        self.UIx = next_UIx
        self.UIy = next_UIy

    def move_direct(self, next_UIx, next_UIy):
        self.move(next_UIx * CELL_SIZE, next_UIy * CELL_SIZE)
        self.UIx = next_UIx
        self.UIy = next_UIy

    def move_return(self):
        self.move(self.UIx * CELL_SIZE, self.UIy * CELL_SIZE)



    def __init__(self, parent, image_path, UIx, UIy, Type, IsTeamWhite, callback_press, callback_land): # Get UI Position
        super().__init__(parent)
        self.UIx = UIx # UI Position
        self.UIy = UIy # UI Position
        self.move(self.UIx * CELL_SIZE, self.UIy * CELL_SIZE)
        self.type = Type
        self.isTeamWhite = IsTeamWhite
        self.moving = False
        self.callback_press = callback_press
        self.callback_land = callback_land

        self.setPixmap(QPixmap(image_path))
        self.setFixedSize(CELL_SIZE, CELL_SIZE)
        self.setScaledContents(True)
        self.setMouseTracking(True)

        # smooth move animation
        self.animation = QPropertyAnimation(self, b"pos")
        self.animation.setDuration(150) # animation duration (ms)
        self.animation.setEasingCurve(QEasingCurve.InOutQuad) 

        self.show()
        self.raise_()

    def mousePressEvent(self, event):
        if self.parent().isPlayerWhite != self.isTeamWhite: # NOT player piece
            self.callback_land(self.UIx, self.UIy, True)
            return
        
        # player piece
        self.moving = True
        self.callback_press(self.UIx, self.UIy)
        if event.button() == Qt.LeftButton:
            self.followMouse(event.pos())

    def mouseMoveEvent(self, event):
        if self.moving:
            self.followMouse(event.pos())

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.moving = False
            mousePos = self.parent().mapFromGlobal(self.mapToGlobal(event.pos()))
            land_UIx = max(0, min(mousePos.x() // CELL_SIZE, 7)) # UI Position
            land_UIy = max(0, min(mousePos.y() // CELL_SIZE, 7)) # UI Position
            self.callback_land(land_UIx, land_UIy, False)



class Chess(QWidget):
### Load
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
        self.line_turn = self.findChildren(QLineEdit)[2]
        self.lbl_board = self.findChildren(QLabel)[0]

### Highlight
    def off_light(self, x, y):
        UIx, UIy = UI_Board_PosConv(x, y, self.isPlayerWhite)
        self.highlight[UIy][UIx].off()

    def on_light(self, x, y, isSelectedLight):
        UIx, UIy = UI_Board_PosConv(x, y, self.isPlayerWhite)
        self.highlight[UIy][UIx].on(isSelectedLight)

    def clear_light(self):
        for x in range(8):
            for y in range(8):
                self.highlight[y][x].off()

    def on_legalMove_light(self):
        it = iter(self.legalMove)
        for x, y in zip(it, it):
            self.on_light(x, y, isSelectedLight=False)
    
    def off_legalMove_light(self):
        it = iter(self.legalMove)
        for x, y in zip(it, it):
            self.off_light(x, y)

### Select Piece
    def isSelected(self):
        return self.selected_piece[0] != -1 or self.selected_piece[1] != -1

    def setSelect(self, x, y): # Get Board Position
        notation = boardPosToNotation(x, y)
        # turn off prev light
        if self.isSelected():
            self.off_light(self.selected_piece[0], self.selected_piece[1])

        # turn on HighLight
        self.on_light(x, y, True)

        # change UI
        self.line_selected.setText(notation)

        # Change Data
        templist = list(self.selected_piece)
        templist[0] = x
        templist[1] = y
        self.selected_piece = tuple(templist)

        # set legal move list
        self.off_legalMove_light()
        self.legalMove = connector.get_legalMove(notation)
        self.on_legalMove_light()

    def delSelect(self):
        self.line_selected.setText('None') # change UI
        if self.isSelected():
            self.off_light(self.selected_piece[0], self.selected_piece[1])
            self.off_legalMove_light()
        self.selected_piece = (-1, -1) # Change Data

### Piece
    def create_piece(self, st, x, y, type, isTeamWhite): # Get Board Position
        UIx, UIy = UI_Board_PosConv(x, y, self.isPlayerWhite) # Board to UI Position Convert
        lbl_piece = ChessPiece(self, self.piece_images[st], UIx, UIy, type, isTeamWhite, self.piece_callback_press, self.piece_callback_land)
        self.pieces[y][x] = lbl_piece # Update the 2D list with the new piece
    
    def init_pieces(self):
        # reset
        for i in range(8): 
            for j in range(8): 
                if self.pieces[i][j] != None:
                    self.pieces[i][j].die() # delete Piece
                    self.pieces[i][j] = None
        
        initPos = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r']
        typeList = ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']

        for i in range(8): 
            self.create_piece('wp', i, 1, 'P', True)  # White pawns
            self.create_piece('w' + initPos[i], i, 0, typeList[i], True) # White other pieces
            self.create_piece('bp', i, 6, 'P', False)  # Black pawns
            self.create_piece('b' + initPos[i], i, 7, typeList[i], False) # Black other pieces

    def piece_callback_press(self, UIx, UIy):
        x, y = UI_Board_PosConv(UIx, UIy, self.isPlayerWhite)
        self.setSelect(x, y)

    def piece_callback_land(self, UIx, UIy, smooth):
        if self.isSelected() == False:
            return 
        
        x, y = UI_Board_PosConv(UIx, UIy, self.isPlayerWhite)
        if self.selected_piece[0] != x or self.selected_piece[1] != y: # move
            self.move_ME(x, y, smooth)
        else: # click
            self.pieces[y][x].move_return()

### Window initalize
    def __init__(self):
        super().__init__()
        self.selected_piece = (-1, -1)
        self.prev_move = (-1, -1, -1, -1)
        self.isPlayerWhite = True
        self.isTurnWhite = True
        self.legalMove = []

        self.load_img()
        self.UIinit()
        self.load_findChildren()

        # Initialize pieces
        self.pieces = [[None for _ in range(8)] for _ in range(8)]
        self.init_pieces()
        
        # Initialize Highlight sheet
        self.highlight = [[None for _ in range(8)] for _ in range(8)]
        for UIx in range(8):
            for UIy in range(8):
                self.highlight[UIy][UIx] = HighLightSquare(self, UIx, UIy)

    def UIinit(self):
        
        self.setFixedSize(BOARD_SIZE, 555) # size of the windows
        self.setWindowTitle('Chess')
        vbox = QVBoxLayout()

        lbl_board = QLabel(self)
        lbl_board.setPixmap(QPixmap(self.img_board))
        lbl_board.setFixedSize(BOARD_SIZE, BOARD_SIZE) # size of the chessboard
        lbl_board.setScaledContents(True)

        # Player
        hbox_player = QHBoxLayout()
        lbl_player = QLabel('<b>[Player]</b>', self)
        line_player = QLineEdit('White', self)
        line_player.setReadOnly(True)
        btn_changePlayer = QPushButton('Change my color', self)
        btn_changePlayer.clicked.connect(self.btn_changeColor_function)
        btn_restart = QPushButton('Game Restart', self)
        btn_restart.clicked.connect(self.btn_gameRestart_function)
        hbox_player.addWidget(lbl_player)
        hbox_player.addWidget(line_player)
        hbox_player.addWidget(btn_changePlayer)
        hbox_player.addWidget(btn_restart)

        # Selected
        hbox_selected = QHBoxLayout()
        lbl_selected = QLabel('<b>[Selected Piece]</b>', self)
        line_selected = QLineEdit('None', self)
        line_selected.setReadOnly(True)
        hbox_selected.addWidget(lbl_selected)
        hbox_selected.addWidget(line_selected)

        # Turn
        hbox_turn = QHBoxLayout()
        lbl_turn = QLabel('<b>[Turn]</b>', self)
        line_turn = QLineEdit('White', self)
        line_turn.setReadOnly(True)
        hbox_turn.addWidget(lbl_turn)
        hbox_turn.addWidget(line_turn)


        vbox.addWidget(lbl_board)
        vbox.addLayout(hbox_player)
        vbox.addLayout(hbox_selected)
        vbox.addLayout(hbox_turn)
        vbox.setContentsMargins(0,0,0,0)

        self.setLayout(vbox)

### Move Logic
    def isLegalMove(self, next_x, next_y):
        it = iter(self.legalMove)
        for x, y in zip(it, it):
            print(f'{x}, {y} is legal')
            if x == next_x and y == next_y:
                return True
        return False
    
    def move_ME(self, next_x, next_y, smooth):
        if self.isPlayerWhite != self.isTurnWhite or self.isLegalMove(next_x, next_y) == False: # Opponent Turn & illegal move
            self.pieces[self.selected_piece[1]][self.selected_piece[0]].move_return()
            self.delSelect()
            return 
        
        self.move_piece(self.selected_piece[0], self.selected_piece[1], next_x, next_y, smooth)
        self.off_legalMove_light()
        self.delSelect()

    def move_AI_callback(self, now_x, now_y, next_x, next_y):
        self.move_piece(now_x, now_y, next_x, next_y, True)

    def move_piece(self, now_x, now_y, next_x, next_y, smooth): # Get Board Position
        print(f'Move {boardPosToNotation(now_x, now_y)} -> {boardPosToNotation(next_x, next_y)}', end='')

        if self.pieces[next_y][next_x] != None:
            self.pieces[next_y][next_x].die()
        self.pieces[next_y][next_x] = self.pieces[now_y][now_x]

        UIx, UIy = UI_Board_PosConv(next_x, next_y, self.isPlayerWhite)
        if smooth:
            self.pieces[now_y][now_x].move_smooth(UIx, UIy)
        else:
            self.pieces[now_y][now_x].move_direct(UIx, UIy)

        self.pieces[now_y][now_x] = None

        self.prev_move = (now_x, now_y, next_x, next_y)
        self.changeTurn()

    def changeTurn(self):
        if self.isTurnWhite:
            self.isTurnWhite = False
            self.line_turn.setText('Black')
        else:
            self.isTurnWhite = True
            self.line_turn.setText('White')
        
        if self.isPlayerWhite != self.isTurnWhite: # AI turn
            nowNotation = boardPosToNotation(self.prev_move[0], self.prev_move[1])
            nextNotation = boardPosToNotation(self.prev_move[2], self.prev_move[3])
            getAImove = threading.Thread(target=connector.getAI_move, args=(self.move_AI_callback, nowNotation, nextNotation, ))
            getAImove.start()

### Mouse event
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            mousePos = self.mapFromGlobal(self.mapToGlobal(event.pos()))
            x, y = UI_Board_PosConv(mousePos.x() // CELL_SIZE, mousePos.y() // CELL_SIZE, self.isPlayerWhite)
            print(f'{boardPosToNotation(x, y)} clicked !')
            if self.isSelected():
                self.move_ME(x, y, True)
        elif event.button() == Qt.RightButton:
            self.delSelect()

### button function
    def btn_gameRestart_function(self):
        self.init_pieces()
        self.isTurnWhite = True
        self.delSelect()
        self.prev_move = (-1, -1, -1, -1)

    def btn_changeColor_function(self):
        if self.isPlayerWhite:
            self.isPlayerWhite = False
            self.line_player.setText('Black')
        else:
            self.isPlayerWhite = True
            self.line_player.setText('White')
            
        self.btn_gameRestart_function()

### Additional function
    def print2DInfo(self):
        def team(isTeamWhite):
            if isTeamWhite:
                return 'w'
            else:
                return 'b'
            
        if self.isPlayerWhite:
            for i in range(7, -1, -1):
                print(i + 1, end='  ')
                for j in range(8):
                    if self.pieces[i][j] != None:
                        print(team(self.pieces[i][j].isTeamWhite) + self.pieces[i][j].type, end=' ')
                    else:
                        print('--', end=' ')
                print()
            
            print('   ', end='')
            for i in range(ord('a'), ord('h') + 1):
                print(chr(i), end='  ')
            print()
        else:
            for i in range(8):
                print(i + 1, end='  ')
                for j in range(7, -1, -1):
                    if self.pieces[i][j] != None:
                        print(team(self.pieces[i][j].isTeamWhite) + self.pieces[i][j].type, end=' ')
                    else:
                        print('--', end=' ')
                print()

            print('   ', end='')
            for i in range(ord('h'), ord('a') - 1, -1):
                print(chr(i), end='  ')
            print()



if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = Chess()
    mainWindow.show()
    sys.exit(app.exec_())

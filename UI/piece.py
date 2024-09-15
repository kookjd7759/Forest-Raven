from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *

from utility import * 

class ChessPiece(QLabel):
    def __init__(self, parent, piece: Piece, UIpos: Position, callback_press, callback_land): # Get UI Position
        super().__init__(parent)
        self.UIpos = UIpos
        self.piece = piece
        self.moving = False
        self.move(UIpos.x * CELL_SIZE, UIpos.y * CELL_SIZE)
        self.callback_press = callback_press
        self.callback_land = callback_land
        self.setPixmap(QPixmap(piece_img_path(piece)))
        self.setFixedSize(CELL_SIZE, CELL_SIZE)
        self.setScaledContents(True)
        self.setMouseTracking(True)

        # smooth move animation
        self.animation = QPropertyAnimation(self, b"pos")
        self.animation.setDuration(150) # animation duration (ms)
        self.animation.setEasingCurve(QEasingCurve.InOutQuad) 

        self.show()
        self.raise_()

    def __boundaryCheck(self):
        piece_rect = self.rect().translated(self.mapToParent(QPoint(0, 0)))
        if piece_rect.left() < -30:
            self.move(-30, self.y()) 
        if piece_rect.top() < -30:
            self.move(self.x(), -30) 
        if piece_rect.right() > BOARD_SIZE + 30:
            self.move(BOARD_SIZE - 30, self.y()) 
        if piece_rect.bottom() > BOARD_SIZE + 30:
            self.move(self.x(), BOARD_SIZE - 30)
    def __followMouse(self, mousePos):
        self.raise_()
        pos = self.mapToParent(mousePos - QPoint(self.width() // 2, self.height() // 2))
        self.move(pos.x(), pos.y())
        self.__boundaryCheck()

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.callback_press(self.UIpos)
            if self.parent().chess.player == self.piece.color:
                self.moving = True
                self.__followMouse(event.pos())
        if event.button() == Qt.RightButton and self.moving:
            self.moving = False
            self.callback_land(self.UIpos)
    def mouseMoveEvent(self, event):
        if self.moving:
            self.__followMouse(event.pos())
    def mouseReleaseEvent(self, event):
        if self.moving == False:
            return
        
        if event.button() == Qt.LeftButton:
            self.moving = False
            mousePos = self.parent().mapFromGlobal(self.mapToGlobal(event.pos()))
            land_UIpos = Position(max(0, min(mousePos.x() // CELL_SIZE, 7)), max(0, min(mousePos.y() // CELL_SIZE, 7))) # UI Position
            self.callback_land(land_UIpos)

    def move_piece(self, next_UIpos: Position, smooth):
        x, y = next_UIpos.x, next_UIpos.y
        if smooth:
            self.animation.setStartValue(QPoint(self.UIpos.x * CELL_SIZE, self.UIpos.y * CELL_SIZE))
            self.animation.setEndValue(QPoint(x * CELL_SIZE, y * CELL_SIZE))
            
            self.loop = QEventLoop()
            self.animation.finished.connect(self.loop.quit)
            
            self.animation.start()
            self.loop.exec_()
        self.move(x * CELL_SIZE, y * CELL_SIZE)
        self.UIpos = next_UIpos
    def move_return(self):
        self.move(self.UIpos.x * CELL_SIZE, self.UIpos.y * CELL_SIZE)
    def die(self):
        self.hide()
        self.deleteLater()

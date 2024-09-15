from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from typing import Literal

from chess.utility import *
from utility import *

class PromotionWindow(QLabel):
    finished = pyqtSignal()
    def __init__(self, parent, color: Color, callback):
        super().__init__(parent)
        self.color = color
        self.callback = callback
        self.UIinit()
        self.off()

    def UIinit(self):
        self.resize(BOARD_SIZE, BOARD_SIZE)
        
        background_label = QLabel(self)
        background_label.setStyleSheet('background-color: rgba(0, 0, 0, 60);')
        background_label.setFixedSize(BOARD_SIZE, BOARD_SIZE)

        window = QLabel(self)
        window.resize(PRO_WINDOW_SIZE, PRO_WINDOW_SIZE)
        window.move(WINDOW_POINT['LU'][0], WINDOW_POINT['LU'][1])

        window_background = QLabel(self)
        window_background.setStyleSheet('background-color: rgba(255, 255, 255, 230); border-radius: 4px;')
        window_background.setFixedSize(PRO_WINDOW_SIZE, PRO_WINDOW_SIZE)
        window_background.move(BOARD_SIZE // 2 - PRO_WINDOW_SIZE // 2, BOARD_SIZE // 2 - PRO_WINDOW_SIZE // 2)
        window_background.raise_()
        
        layout = QGridLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        posList = [(0, 0), (0, 1), (1, 0), (1, 1)]
        for pos, piece_type in zip(posList, promotion_list):
            piece = Piece(piece_type, self.color)
            label = QLabel(self)
            label.setPixmap(QPixmap(piece_img_path(piece)))
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

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            mousePos = self.parent().mapFromGlobal(self.mapToGlobal(event.pos()))
            x, y = mousePos.x(), mousePos.y()
            idx = -1
            if x >= WINDOW_POINT['LU'][0] and y >= WINDOW_POINT['LU'][1] and x <= WINDOW_POINT['RD'][0] and y <= WINDOW_POINT['RD'][1]:
                idx = (0 if x < BOARD_SIZE // 2 else 1) + (0 if y < BOARD_SIZE // 2 else 2)
            self.callback(promotion_list[idx])
            self.finished.emit()
            self.off()

class GameEndWindow(QDialog): 
    def __init__(self, parent, callback, gameOver:Gameover_type):
        super().__init__(parent)
        self.gameOver = gameOver
        self.callback = callback
        self.initUI()

    def initUI(self):
        self.setFixedSize(170, 100)
        label = QLabel('test', self)
        label.setAlignment(Qt.AlignCenter)
        if self.gameOver == Gameover_type.CHECKMATE_BLACK:
            self.setWindowTitle('CHECKMATE')
            label.setText('CheckMate\n White WIN !')
        elif self.gameOver == Gameover_type.CHECKMATE_WHITE:
            self.setWindowTitle('CHECKMATE')
            label.setText('CheckMate\n Black WIN !')
        elif self.gameOver == Gameover_type.STALEMATE:
            self.setWindowTitle('DRAW')
            label.setText('Stalemate')

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

from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from setting import *

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

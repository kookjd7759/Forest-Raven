from PyQt5.QtWidgets import *
from utility import * 

class HighLightSquare(QLabel):
    def __init__(self, parent, UIpos: Position):
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
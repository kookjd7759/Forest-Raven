import sys

from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *


class MyWindow(QWidget):
    
    def mousePressEvent(self, e):
        click = ''
        if e.button() == Qt.LeftButton:
            click = 'Left Mouse'
        if e.button() == Qt.MidButton:
            click = 'Middle Mouse'
        if e.button() == Qt.RightButton:
            click = 'Right Mouse'
        print(click)

    def mouseMoveEvent(self, event):
        txt = "Mouse 위치 ; x={0},y={1}, global={2},{3}".format(event.x(), event.y(), event.globalX(), event.globalY())
        self.label.setText(txt)

    def center(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def __init__(self):
        super().__init__()
        self.UIinit()

    def UIinit(self):
        self.setWindowTitle('WaterMarking Simulator')
        self.setMaximumSize(700, 450)
        self.setMinimumSize(700, 450)
        self.center()

        self.label = QLabel('None')
        vbox = QVBoxLayout()
        vbox.addWidget(self.label)

        self.setLayout(vbox)



if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MyWindow()
    window.show()
    sys.exit(app.exec_())
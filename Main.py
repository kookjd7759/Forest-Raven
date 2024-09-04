import sys
import UI
from PyQt5.QtWidgets import QApplication

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = UI.Window()
    window.show()
    sys.exit(app.exec_())
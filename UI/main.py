import sys
from pubsub import pub
from PyQt5.QtCore import *

from utility import *
from windows import *
from highLight import *
from piece import *

from chess.chess import *

class Window(QWidget):
### Utility
    def convert_position(self, pos: Position): # UI to Board / Board to UI position converting
        return Position(pos.x, 7 - pos.y) if self.chess.player == Color.WHITE else Position(7 - pos.x, pos.y)

### Selected piece
    def isSelected(self):
        return self.selected.x != -1 and self.selected.y != -1
    def setSelect(self, pos: Position):
        if self.isSelected():
            self.del_legalMove()
            self.off_light(self.selected)
        self.selected = pos
        self.line_selected.setText(to_notation(pos))
        self.on_light(self.selected, isSelectedLight=True)
        self.set_legalMove() 
    def delSelect(self):
        if self.isSelected():
            self.off_light(self.selected)
            self.del_legalMove()
        self.line_selected.setText('None')
        self.selected = Position()

### Highlight
    def off_light(self, pos: Position):
        UIpos = self.convert_position(pos)
        self.highlight[UIpos.y][UIpos.x].off()
    def on_light(self, pos: Position, isSelectedLight):
        UIpos = self.convert_position(pos)
        self.highlight[UIpos.y][UIpos.x].on(isSelectedLight)
        if self.board[pos.y][pos.x] != None:
            self.board[pos.y][pos.x].raise_()
    def on_legalMove_light(self):
        for move in self.legalMoves:
            self.on_light(move.dest, isSelectedLight=False)
    def off_legalMove_light(self):
        for move in self.legalMoves:
            self.off_light(move.dest)
    def off_all_light(self):
        for _ in self.highlight:
            for light in _:
                light.off()

### Legal move
    def set_legalMove(self):
        self.legalMoves = self.chess.get_legalMove(self.selected)
        self.on_legalMove_light()
    def del_legalMove(self):
        self.off_legalMove_light()
        self.legalMoves = []
       
### piece
    def create_piece(self, piece: Piece, pos: Position): # Get Board Position
        UIpos = self.convert_position(pos) # Board to UI Position
        self.board[pos.y][pos.x] = ChessPiece(self, piece, UIpos, self.piece_callback_press, self.piece_callback_land)
    def init_pieces(self):
        for i in range(8):
            for j in range(8):
                if self.board[i][j] != None:
                    self.board[i][j].die() # delete Piece
                    self.board[i][j] = None
        
        for i in range(8):
            self.create_piece(Piece(Piece_type.PAWN, Color.WHITE), Position(i, 1)) # White pawns
            self.create_piece(Piece(initPos_type[i], Color.WHITE), Position(i, 0)) # White other pieces
            self.create_piece(Piece(Piece_type.PAWN, Color.BLACK), Position(i, 6)) # Black pawns
            self.create_piece(Piece(initPos_type[i], Color.BLACK), Position(i, 7)) # Black other pieces
 
### play
    def __move(self, move: Move, smooth: bool):
        UIpos = self.convert_position(move.dest)
        self.board[move.ori.y][move.ori.x].move_piece(UIpos, smooth)
        self.board[move.dest.y][move.dest.x] = self.board[move.ori.y][move.ori.x]
        self.board[move.ori.y][move.ori.x] = None
    def __capture(self, move: Move, smooth: bool):
        self.board[move.take.y][move.take.x].die()
        self.board[move.take.y][move.take.x] = None
        self.__move(move, smooth)
    def __castling(self, move: Move, smooth: bool):
        rank = (0 if move.piece.color == Color.WHITE else 7)
        if move.dest.x > move.ori.x: # king side 
            rook_ori = Position(7, rank)
            rook_dest = move.dest + Position(-1, 0)
            rookMove = Move(Piece(Piece_type.ROOK, move.piece.color), rook_ori, rook_dest)
            self.__move(move, smooth)
            self.__move(rookMove, smooth=True)
        else: # queen side
            rook_ori = Position(0, rank)
            rook_dest = move.dest + Position(+1, 0)
            rookMove = Move(Piece(Piece_type.ROOK, move.piece.color), rook_ori, rook_dest)
            self.__move(move, smooth)
            self.__move(rookMove, smooth=True)
    def __promotion(self, move: Move):
        self.board[move.dest.y][move.dest.x].die()
        self.create_piece(Piece(move.promotion_type, move.piece.color), move.dest)
    def __get_promotion(self, move: Move):
        self.promotion_window.finished.connect(self.__promotion_finished)
        self.promotion_window.on()
        self.event_loop = QEventLoop()
        self.event_loop.exec_()
        move.promotion_type = self.promotion_type
        if move.promotion_type == Piece_type.NOPIECE:
            return False
        return True
    def __promotion_finished(self):
        self.event_loop.quit()
    def __play(self, move: Move, smooth: bool):
        move_type = move.get_move_type()
        if move_type == Move_type.MOVE:
            self.__move(move, smooth)
        elif move_type == Move_type.CAPTURE:
            self.__capture(move, smooth)
        elif move_type == Move_type.CASTLING:
            self.__castling(move, smooth)
        elif move_type == Move_type.MOVE_PRO:
            if not self.__get_promotion(move):
                return False
            self.__move(move, smooth)
            self.__promotion(move)
        elif move_type == Move_type.CAPTURE_PRO:
            if not self.__get_promotion(move):
                return False
            self.__capture(move, smooth)
            self.__promotion(move)
        return True
    def __isLegal(self, dest: Position):
        for move in self.legalMoves:
            if move.ori.isEqual(self.selected) and move.dest.isEqual(dest):
                return move
        return None
    def PLAYER_PLAY(self, dest: Position, smooth: bool):
        if self.chess.turn != self.chess.player:
            return 
        move = self.__isLegal(dest)
        if move != None:
            if not self.__play(move, smooth):
                self.board[move.ori.y][move.ori.x].move_return()
                return
            self.delSelect()
            self.chess.PLAYER(move)
    def AI_PLAY(self, move: Move):
        self.__play(move, smooth=True)
        self.del_legalMove()
        self.set_legalMove()
    def GAMEOVER(self, ret:Gameover_type):
        self.setEnabled(False)
        gameEnd_window = GameEndWindow(self, self.btn_restart_function, ret)
        gameEnd_window.exec_() 
        self.setEnabled(True)

### Window initalize
    def __init__(self):
        super().__init__()
        self.UIinit()

        self.selected = Position()
        self.chess = Chess()
        self.legalMoves: list[Move] = []
        self.promotion_type: Piece_type = Piece_type.NOPIECE
        
        # Initialize pieces
        self.board: list[list[ChessPiece]] = [[None for _ in range(8)] for _ in range(8)]
        self.init_pieces()

        # Initialize Highlight sheet
        self.highlight: list[list[HighLightSquare]] = [[None for _ in range(8)] for _ in range(8)]
        for UIx in range(8):
            for UIy in range(8):
                self.highlight[UIy][UIx] = HighLightSquare(self, Position(UIx, UIy))
                
        self.promotion_window = PromotionWindow(self, self.chess.player, self.promotion_callback)
        pub.subscribe(self.AI_PLAY, 'AI')
        pub.subscribe(self.GAMEOVER, 'GAMEOVER')

    def reset(self):
        self.selected = Position()
        self.legalMoves = []
        self.chess.restart()
        self.init_pieces()
        self.off_all_light()

    def UIinit(self):
            self.setFixedSize(BOARD_SIZE, 560) # size of the windows
            self.setWindowTitle('Chess')
            vbox = QVBoxLayout()

            lbl_board = QLabel(self)
            lbl_board.setPixmap(QPixmap(getImgFolder() + 'Ground.png'))
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
    



### Mouse event
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            mousePos = self.mapFromGlobal(self.mapToGlobal(event.pos()))
            pos = self.convert_position(Position(mousePos.x() // CELL_SIZE, mousePos.y() // CELL_SIZE))
            print(f'CLICK, {to_notation(pos)} square')
            if self.isSelected():
                self.PLAYER_PLAY(pos, smooth=True)
        elif event.button() == Qt.RightButton:
            self.delSelect()

### callback 
    def piece_callback_press(self, UIpos: Position):
        pos = self.convert_position(UIpos)
        print(f'PRESS, {to_notation(pos)} piece')
        if self.board[pos.y][pos.x].piece.color != self.chess.player and not self.isSelected():
            return
        elif self.board[pos.y][pos.x].piece.color != self.chess.player and self.isSelected(): # capture
            self.PLAYER_PLAY(pos, smooth=True)
        else:
            self.setSelect(pos)
    
    def piece_callback_land(self, UIpos):
        pos = self.convert_position(UIpos)
        print(f'LAND, {to_notation(pos)} piece')
        if self.chess.turn != self.board[self.selected.y][self.selected.x].piece.color:
            self.board[self.selected.y][self.selected.x].move_return()

        if self.selected.x != pos.x or self.selected.y != pos.y: # move
            self.PLAYER_PLAY(pos, smooth=False)
        else: # click
            self.board[pos.y][pos.x].move_return()
        
    def promotion_callback(self, piece_type: Piece_type):
        self.promotion_type = piece_type

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
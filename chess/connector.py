import subprocess
import os

from utility import *

EXEPATH = os.getcwd() + '\\Forest-Raven\\forest-raven\\build\\Debug\\Forest_Raven.exe'
forest_raven = None

def START(color: Color):
    global forest_raven
    if forest_raven is None or forest_raven.poll() is not None:
        forest_raven = subprocess.Popen(
            [EXEPATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
    send(f'{color.value}')
def STOP():
    global forest_raven
    if forest_raven is not None:
        forest_raven.terminate()
        forest_raven = None
def RESTART(color: Color):
    STOP()
    START(color)
def send(st):
    print(f'connector.SEND {st}')
    forest_raven.stdin.write(st + '\n')
    forest_raven.stdin.flush()
def read():
    output = forest_raven.stdout.readline()
    while output == '':
        output = forest_raven.stdout.readline()
    print(f'connector.READ {output}')
    return output.strip()

def send_move(move: Move):
    send(move.get_string())
def get_move():
    text = read()
    move = Move()
    move.string_init(text)
    return move

if __name__ == '__main__':
    START()
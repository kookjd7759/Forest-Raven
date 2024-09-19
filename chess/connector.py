import subprocess
import os

from utility import *

exePath = os.getcwd() + '\\Forest-Raven\\forest-raven\\build\\Debug\\Forest_Raven.exe'

process = subprocess.Popen(
    [exePath],
    stdin = subprocess.PIPE,
    stdout = subprocess.PIPE, 
    stderr = subprocess.PIPE,
    text = True
)

def send(st):
    print(f'connector.SEND {st}')
    process.stdin.write(st + '\n')
    process.stdin.flush()
def read():
    output = process.stdout.readline()
    while output == '':
        output = process.stdout.readline()
    print(f'connector.READ {output}')
    return output.strip()

def send_move(move: Move):
    send(move.get_string())

def get_move():
    text = read()
    move = Move()
    move.string_init(text)
    return move

def set_color(color: Color):
    send(f'{color.value}')

if __name__ == '__main__':
    set_color(0)
    read()
import subprocess
import path
from typing import Literal
from utility import *

process = subprocess.Popen(
    [path.getGameexeFile()],
    stdin = subprocess.PIPE,
    stdout = subprocess.PIPE, 
    stderr = subprocess.PIPE,
    text = True
)

def send(st):
    print(f'connector.send {st}')
    process.stdin.write(st + '\n')
    process.stdin.flush()

def read():
    output = process.stdout.readline()
    while output == '':
        output = process.stdout.readline()
        print(f'connector.read {output}')
    return output.strip()

def send_move(move: Move):
    send(move.get_string())

def get_move():
    text = read()
    move = Move()
    move.string_init(text)
    return move

def set_color(color: Color):
    print('set_color')
    send(f'{color}')

if __name__ == '__main__':
    set_color(0)
    read()
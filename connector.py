import subprocess
import path
import time
from typing import Literal

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

def send_move(cur_x: int, cur_y: int, dest_x: int, dest_y: int, promotion):
    send(f'{cur_x} {cur_y} {dest_x} {dest_y} {promotion}')

def get_move():
    print('get_move')
    output = read()
    now_x, now_y, next_x, next_y, promotion = map(int, output.split())
    return now_x, now_y, next_x, next_y, promotion

def set_color(color: Literal[0, 1]):
    print('set_color')
    send(f'{color}')

if __name__ == '__main__':
    set_color(0)
    read()
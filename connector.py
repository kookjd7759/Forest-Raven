import subprocess
import Path

process = subprocess.Popen(
    [Path.getGameexeFile()],
    stdin = subprocess.PIPE,
    stdout = subprocess.PIPE, 
    stderr = subprocess.PIPE,
    text = True
)



def send(st):
    process.stdin.write(st + '\n')
    process.stdin.flush()

def read():
    output = process.stdout.readline()
    print(f'game.cpp :: {output}')
    return output.strip()

def sendMY_move(now, next, promotion = -1):
    send(f'1 {now} {next} {promotion}')

def getAI_move(callback):
    send('3')
    output = read()
    now_x, now_y, next_x, next_y = map(int, output.split())
    callback(now_x, now_y, next_x, next_y)

def restart():
    send('4')

def changeColor():
    send('5')

if __name__ == '__main__':
    print('')
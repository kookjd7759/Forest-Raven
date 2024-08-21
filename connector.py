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

def sendMY_move(now, next):
    send(f'1 {now} {next}')

def sendMY_move_promotion(now, next, num):
    send(f'1 {now} {next} {num}')

def getAI_move(callback):
    send('3')
    output = read()
    now_x, now_y, next_x, next_y = map(int, output.split())
    callback(now_x, now_y, next_x, next_y)

def get_legalMove(notation):
    send(f'2 {notation}')
    output = read()
    result = list(map(int, output.split()))
    return result

def restart():
    send('4')


if __name__ == '__main__':
    while True:
        st = input()
        print(st)
        send(st)
        output = read()
        print(output)
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
    return output.strip()

def sendMY_move(now, next):
    send(f'1 {now} {next}')

def getAI_move(callback):
    print('getAI_move()')
    send(f'5')
    output = read()
    now_x, now_y, next_x, next_y = map(int, output.split())
    callback(now_x, now_y, next_x, next_y)

def get_legalMove(notation):
    print('get_legalMove()')
    send(f'2 {notation}')
    output = read()
    print(f'send : 2 {notation}')
    print(f'recieve : {output}')
    result = list(map(int, output.split()))
    return result

if __name__ == '__main__':
    while True:
        st = input()
        print(st)
        send(st)
        output = read()
        print(output)
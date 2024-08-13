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

def getAI_move(callback, nowNotation, nextNotation):
    send(f'{nowNotation} {nextNotation}')
    output = read()
    print(output)
    now_x, now_y, next_x, next_y = map(int, output.split())
    callback(now_x, now_y, next_x, next_y)

def get_legalMove(notation):
    send(f'2 {notation}')
    output = read()
    result = list(map(int, output.split()))
    return result

if __name__ == '__main__':
    result = get_legalMove('b1')
    it = iter(result)
    for x, y in zip(it, it):
        print(f'{x}, {y}')
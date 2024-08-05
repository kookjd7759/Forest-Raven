import subprocess
import Path

process = subprocess.Popen(
    [Path.getGameexeFile()],
    stdin = subprocess.PIPE,
    stdout = subprocess.PIPE, 
    stderr = subprocess.PIPE,
    text = True
)

def send_input_to_process(st):
    process.stdin.write(st + '\n')
    process.stdin.flush()

def read_output_from_process():
    output = process.stdout.readline()
    return output.strip()

def getAI_move(callback, nowNotation, nextNotation):
    send_input_to_process(f'{nowNotation} {nextNotation}')
    while True:
        output = read_output_from_process()
        if output.find('MOVE') != -1:
            output = output[7:]
            break
        print('process.exe :: ' + output)
    print(output)
    now_x, now_y, next_x, next_y = map(int, output.split())
    callback(now_x, now_y, next_x, next_y)

def get_legalMove(notation):
    send_input_to_process(f'{notation} LEGALMOVE')
    while True:
        output = read_output_from_process()
        if output.find('LEGALMOVE') != -1:
            output = output[12:]
            break
        print('process.exe :: ' + output)
    
    result = list(map(int, output.split()))
    return result

def send_restart_event():
    send_input_to_process('-1 -1 -1 -1')

if __name__ == '__main__':
    legalMoveList = get_legalMove('a1')
    print(legalMoveList)
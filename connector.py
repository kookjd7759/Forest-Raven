import subprocess
import Path

process = subprocess.Popen(
    [Path.getAIexeFile()],
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

def getAI_move(callback, prev_move):
    send_input_to_process(f'{prev_move[0]} {prev_move[1]} {prev_move[2]} {prev_move[3]}')
    output = read_output_from_process()
    now_x, now_y, next_x, next_y = map(int, output.split())
    callback(now_x, now_y, next_x, next_y)

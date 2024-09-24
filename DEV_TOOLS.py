import os
import shutil
import subprocess


src = 'C:\\Users\\kookj\\source\\repos\\Chess\\Chess'
dest = 'C:\\WorkSpace\\Visual Studio Code Workspace\\Forest-Raven\\forest-raven'

def cpp_pull():
    for filename in os.listdir(src):
        src_file = os.path.join(src, filename)
        
        if os.path.isfile(src_file):
            if filename.endswith(('.cpp', '.h')):
                dest_file = os.path.join(dest, filename)
                
                shutil.copy2(src_file, dest_file)
                print(f"Copied {src_file} to {dest_file}")

def cpp_debugging():
    commands = [
    'cd C:\\WorkSpace\\Visual Studio Code Workspace\\Forest-Raven\\forest-raven\\build',
    'cmake ..',
    'cmake --build ./'
    ]
    cmd = subprocess.Popen('cmd.exe', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    for command in commands:
        cmd.stdin.write(command + '\n')
    output, _ = cmd.communicate()
    print(output)

def commit(comment):
    commands = [
    'cd C:\\WorkSpace\\Visual Studio Code Workspace\\Forest-Raven',
    'git config --global core.autocrlf input',
    'git add .',
    f'git commit -m "{comment}"',
    'git push origin main',
    ]
    cmd = subprocess.Popen('cmd.exe', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    for command in commands:
        cmd.stdin.write(command + '\n')
    output, _ = cmd.communicate()
    print(output)


if __name__ == '__main__':
    while True:
        command = input('[1] pull [2] debug [3] commit [4] pull temp cpp code [5] exit \n >> ')
        if command == '1':
            cpp_pull()
        elif command == '2':
            cpp_debugging()
        elif command == '3':
            comment = input('Enter commit comment -> ')
            commit(comment)
        elif command == '4':
            src = 'C:\\Users\\kookj\\source\\repos\\Chess_new(Bitboard version)\\Chess_new(Bitboard version)'
            dest = 'C:\\WorkSpace\\Visual Studio Code Workspace\\Forest-Raven\\tempcppcode'
            cpp_pull()
        else:
            break
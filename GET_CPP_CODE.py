import os
import shutil
import subprocess


def copy_files(src_dir, dest_dir):
    for filename in os.listdir(src_dir):
        src_file = os.path.join(src_dir, filename)
        
        if os.path.isfile(src_file):
            if filename.endswith(('.cpp', '.h')):
                dest_file = os.path.join(dest_dir, filename)
                
                shutil.copy2(src_file, dest_file)
                print(f"Copied {src_file} to {dest_file}")

def debugging():
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

src_directory = 'C:\\Users\\kookj\\source\\repos\\Chess\\Chess'
dest_directory = 'C:\\WorkSpace\\Visual Studio Code Workspace\\Forest-Raven\\forest-raven'

copy_files(src_directory, dest_directory)
debugging()
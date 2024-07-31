import time


def getAI_move(callback):
    print('start getAI_move')
    time.sleep(1)
    callback(4, 6, 4, 4)

if __name__ == '__main__':
    getAI_move()
import threading
import time
import numpy as np
from threading import Thread


time_of_sitting = 12
speedThinking = 0.02
speedEating=0.05
counts_eating = np.zeros(5)
printL = threading.Lock();
threads = []
forks = [threading.Lock() for i in range(5)]


def philosophers(num, name, leftF, rightF):
    start = time.time()
    with printL:
        print(f'{name} sat at the table')
    time.sleep(1)
    while (time.time() - start < time_of_sitting):
        hungry=1
        with printL:
            print(f'...{name} is thinking...')

        time.sleep(np.random.random() * speedThinking)

        while(hungry!=0):
            leftF.acquire()
            if(rightF.locked()):
                 leftF.release()
                 time.sleep(0)
            else:
                rightF.acquire()
                hungry = 0
                printL.acquire()
                print(f'+ {name} is eating')
                printL.release()
                time.sleep(np.random.random() * speedEating)
                rightF.release()
                leftF.release()

        with printL:
            print(f'- {name} finished to eat')

        counts_eating[num] += 1


start = time.time()

for i in range(4):
    t = Thread(target=philosophers, name=f'Phil{i}', args=(i, f'Phil{i}', forks[i], forks[i + 1]))
    threads.append(t)
    t.start()
t = Thread(target=philosophers, name=f'Phil{4}', args=(4, f'Phil{4}', forks[0], forks[4]))
threads.append(t)
t.start()

for i in threads:
    i.join()
end = time.time()

print('time is {}'.format(end - start))
print(counts_eating)

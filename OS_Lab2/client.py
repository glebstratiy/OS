import sys
import asyncio
import socket
import argparse
from util import serialize, deserialize


parser = argparse.ArgumentParser('client')
parser.add_argument('--timeout', default=10, help='timeout for computed tasks')

async def sendArg(loop, hostname, port, arg):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setblocking(False)
    await loop.sock_connect(sock, (hostname, port))

    try:
        await loop.sock_sendall(sock, serialize(arg))

        data = await loop.sock_recv(sock, 1024)
        
        return deserialize(data)
    finally:
        sock.close()


async def main(argv):

    args = parser.parse_args(argv)

    loop = asyncio.get_event_loop()

    loop.set_debug(True)

    network = (
        ('127.0.0.1', 7000), # remote f
        ('127.0.0.1', 7001)  # remote g
    )
    finished = False

    while not finished:
        try:
            argX = int(input('Enter argument X: '))
        except ValueError:
            continue

        tasks = asyncio.gather(*[
            sendArg(loop, hostname, port, argX) for hostname, port in network
        ])

        results = None

        while True:
            try:
                results = await asyncio.wait_for(asyncio.shield(tasks), timeout=args.timeout)
            except asyncio.TimeoutError:
                keep_waiting = input('Computation takes too long. Would you like to continue waiting? [y|n]: ')

                if keep_waiting == 'y':
                    continue
            
            break
        
        if results is None:
            continue

        print(f'f-computed: {results[0]}, g-computed: {results[1]}, f && g: {results[0] and results[1]}')



if __name__ == '__main__':
    try:
        asyncio.run(main(sys.argv[1:]))
    except KeyboardInterrupt:
        pass
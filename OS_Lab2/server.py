import asyncio
import atexit
import logging

from random import uniform
import socket
import sys
import argparse

from util import serialize, deserialize



parser = argparse.ArgumentParser('server')
parser.add_argument('--executor', required=True, choices=('f', 'g'))

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


def functionF(arg):
    return arg * 2

def functionG(arg):
    return arg * 4


network = {
    'f': ('localhost', 7000, functionF),
    'g': ('localhost', 7001, functionG)
}

def _onexit():
    pass

atexit.register(_onexit)

class Server:

    @classmethod
    async def create(cls, *args, **kwargs):
        self = cls()

        try:
            await self.__ainit__(*args, **kwargs)
        except Exception as e:
            logger.exception(f'server create err: %s', e)
        
        return self


    async def __ainit__(self, hostname, port, executor):
        self.loop = asyncio.get_running_loop()
        self.hostname = hostname
        self.port = port
        self.executor = executor
        self._looptask = None
        self._finishevent = asyncio.Event()
        self._tasks = []
    
    async def start(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print(self.hostname, self.port)
        self.socket.bind((self.hostname, self.port))
        self.socket.listen(8)
        self.socket.setblocking(False)

        self._looptask = asyncio.create_task(self._loop())

        logger.debug(f'Listening on {self.hostname}:{self.port}')
    

    async def _loop(self):
        while not self._finishevent.is_set():
            try:
                conn, address = await self.loop.sock_accept(self.socket)
            except KeyboardInterrupt:
                self._finishevent.set()
                break

            logger.debug(f'Connection from {address}')

            task = asyncio.create_task(self._handle_req(conn, address))
            self._tasks.append(task)

            def _task_done(task):
                self._tasks.remove(task)

                try:
                    if not task.done():
                        task.result()
                except asyncio.CancelledError:
                    pass
                except Exception as e:
                    logger.exception('handler task %s raised exception: %s', task, e)

            task.add_done_callback(_task_done)

    async def _handle_req(self, conn, address):
        logger.debug(f'Handling connection from {address}')

        try:
            while True:
                data = await self.loop.sock_recv(conn, 1024)

                if data == b'':
                    logger.debug('Socket closed remotely')
                    break

                logger.debug('Received data %r', data)
                
                await asyncio.sleep(uniform(0, 20))
                result = self.executor(
                    deserialize(data)
                )

                await self.loop.sock_sendall(conn, serialize(result))
                logger.debug('Sent data %r', serialize(result))

        except Exception as e:
            logger.exception('handle req error: %s', e)
        finally:
            logger.debug('Closing socket')
            conn.close()

    async def waitfinish(self):
        await self._finishevent.wait()

async def main(argv):
    args = parser.parse_args(argv)

    hostname, port, executor = network.get(args.executor)

    server = await Server.create(hostname, port, executor)

    await server.start()

    await server.waitfinish()
    

if __name__ == '__main__':
    asyncio.run(main(sys.argv[1:]))
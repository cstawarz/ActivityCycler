import serial
import sys
import time


class ACController(object):

    def __init__(self, serial_port):
        self._conn = serial.Serial(serial_port)

    def _send(self, cmd):
        self._conn.write(cmd + '\n')

    def _recv(self):
        return self._conn.readline()

    def _exec(self, cmd):
        self._send(cmd)
        sys.stdout.write(self._recv())

    def _run(self, cmd, intervals):
        cmd += ' ' + ', '.join(str(int(i)) for i in intervals)
        self._exec(cmd)

    def status(self):
        self._exec('STATUS')

    def run(self, *intervals):
        self._run('RUN', intervals)

    def stop(self):
        self._exec('STOP')

    def debug(self, *intervals):
        self._run('DEBUG', intervals)
        
        t_start = time.time()
        last_t = t_start
        
        while True:
            msg = self._recv().strip()
            t = time.time()
            sys.stdout.write('%-9s(t = %.3f, delta_t = %.3f)\n' %
                             (msg, t-t_start, t-last_t))
            if msg == 'DONE':
                break
            last_t = t
            
        self.status()


if __name__ == '__main__':
    import sys
    acc = ACController(sys.argv[1])

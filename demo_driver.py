#!/usr/bin/env python3
"""Drive ./cash through a PTY with human-paced input so asciinema can record it."""
import os, pty, select, sys, time

STEPS = [
    (1.0, b"help\r"),
    (1.5, b"ls\r"), 
    (1.2, b"cd ..\r"),
    (1.2, b"ls\r"),
    (1.5, b"cd shell\r"),
    (1.2, b"ls\r"),
    (1.2, b"ls;help\r"),
    (1.5, b"exit\r"),
]

def main():
    pid, fd = pty.fork()
    if pid == 0:
        os.execv("./cash", ["./cash"])
        os._exit(1)

    def pump(duration):
        end = time.time() + duration
        while time.time() < end:
            r, _, _ = select.select([fd], [], [], 0.05)
            if r:
                try:
                    data = os.read(fd, 4096)
                except OSError:
                    return False
                if not data:
                    return False
                os.write(sys.stdout.fileno(), data)
        return True

    for delay, keys in STEPS:
        if not pump(delay):
            break
        for b in keys:
            os.write(fd, bytes([b]))
            pump(0.06)
    pump(0.8)

if __name__ == "__main__":
    main()

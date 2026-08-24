import multiprocessing
import pathlib
import sys


WORK = 4_000_000


def work() -> None:
    x = 0
    for i in range(WORK):
        x += i * i


def main() -> int:
    procs = []
    for _ in range(2):
        proc = multiprocessing.Process(target=work)
        proc.start()
        procs.append(proc)

    for proc in procs:
        proc.join()
        if proc.exitcode != 0:
            return proc.exitcode

    pathlib.Path(sys.argv[1]).touch()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

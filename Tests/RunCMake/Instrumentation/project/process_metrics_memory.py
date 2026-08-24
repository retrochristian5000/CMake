import pathlib
import sys


def main() -> int:
    data = bytearray(64 * 1024 * 1024)
    data[::4096] = b"x" * ((len(data) + 4095) // 4096)
    pathlib.Path(sys.argv[1]).touch()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

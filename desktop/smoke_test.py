#!/usr/bin/env python3
"""Run a basic USB serial smoke test against the Pico firmware."""

import argparse
import sys
import time

import serial
from serial.tools import list_ports


TEST_CASES = (
    ("PING", "PONG"),
    ("INFO", "INFO name=gameboy-printer version=0.2.0 board=pico2_w"),
    ("LED ON", "OK LED ON"),
    ("LED OFF", "OK LED OFF"),
    ("BOGUS", "ERROR unknown command"),
)


def find_pico_port() -> str:
    candidates = [
        port.device
        for port in list_ports.comports()
        if port.vid == 0x2E8A or "usbmodem" in port.device.lower()
    ]

    if not candidates:
        raise RuntimeError("no Pico USB serial port found; use --port")
    if len(candidates) > 1:
        ports = ", ".join(candidates)
        raise RuntimeError(f"multiple possible Pico ports found: {ports}; use --port")

    return candidates[0]


def run_test(port: str) -> None:
    with serial.Serial(port, baudrate=115200, timeout=1) as connection:
        time.sleep(0.1)
        connection.reset_input_buffer()

        for command, expected in TEST_CASES:
            connection.write(f"{command}\r\n".encode("ascii"))
            response = connection.readline().decode("ascii").strip()

            if response != expected:
                raise RuntimeError(
                    f"{command!r}: expected {expected!r}, received {response!r}"
                )

            print(f"PASS  {command:<8} -> {response}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", help="serial device; auto-detected when omitted")
    args = parser.parse_args()

    try:
        port = args.port or find_pico_port()
        print(f"Testing {port}")
        run_test(port)
    except (RuntimeError, serial.SerialException, UnicodeDecodeError) as error:
        print(f"FAIL  {error}", file=sys.stderr)
        return 1

    print("All USB serial smoke tests passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

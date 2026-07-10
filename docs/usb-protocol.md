# USB serial protocol

The Pico firmware exposes a line-oriented command interface over USB CDC
serial. It is currently intended for development and diagnostics; a binary
printing protocol will be added separately.

## Transport

- USB CDC serial
- UTF-8/ASCII text
- One command per line
- `CR`, `LF`, and `CRLF` line endings are accepted
- Commands are uppercase and case-sensitive
- Maximum command length: 63 bytes, excluding the line ending

The firmware sends this message when it starts:

```text
READY gameboy-printer 0.2.0
```

## Commands

| Command | Response | Effect |
| --- | --- | --- |
| `PING` | `PONG` | Confirms two-way communication |
| `INFO` | `INFO name=gameboy-printer version=0.2.0 board=pico2_w` | Reports firmware identity |
| `LED ON` | `OK LED ON` | Turns the onboard LED on |
| `LED OFF` | `OK LED OFF` | Turns the onboard LED off |

An unrecognized command returns:

```text
ERROR unknown command
```

A command longer than 63 bytes returns:

```text
ERROR command too long
```

## Smoke test

With the Pico connected and running the firmware:

```sh
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r desktop/requirements-dev.txt
python3 desktop/smoke_test.py
```

The script automatically locates a Pico USB serial port. Pass
`--port DEVICE` to select one explicitly.

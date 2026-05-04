# ns_pg

Baseline Hello World project with TrustZone disabled.  This is the starting point for [TrustZone Demystified: Part 1](https://evanstoddard.com/posts/trustzone-demystified_part_1/) and exists to establish a working UART output before TrustZone is introduced.

## Hardware

- **Board:** NUCLEO-H563ZI
- **MCU:** STM32H563 (Cortex-M33)
- UART output is on USART3, which is wired to the onboard ST-LINK virtual COM port

## Building

```bash
cmake --preset Debug -B build
cmake --build build
```

## Flashing

Flashing requires **ST's fork of OpenOCD**, which ships with STM32CubeIDE and is also available from ST's GitHub.  The standard upstream OpenOCD release does not include support for STM32H5 targets.

Once ST's OpenOCD is on your `PATH`:

```bash
./flash.sh
```

The script programs the binary and resets the board.  Note that it also writes the option bytes to disable TrustZone on the target, so this project can be flashed cleanly on a board that was previously running a TrustZone-enabled image.

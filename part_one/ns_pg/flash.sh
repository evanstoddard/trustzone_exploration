#!/usr/bin/env bash
set -euo pipefail

ELF="build/Debug/ns_pg.elf"
BOARD_CFG="board/st_nucleo_h5.cfg"

if [[ ! -f "$ELF" ]]; then
    echo "error: $ELF not found — build first" >&2
    exit 1
fi

openocd \
    -f "$BOARD_CFG" \
    -c "init" \
    -c "reset halt" \
    -c "stm32h5x option_write 0 0x74 0xC3000000 0xFF000000" \
    -c "stm32h5x option_load 0" \
    -c "reset halt" \
    -c "program $ELF verify reset exit"

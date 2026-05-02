#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."


SECURE_ELF="${ROOT_DIR}/Secure/build/tz_pg_S.elf"
NONSECURE_ELF="${ROOT_DIR}/NonSecure/build/tz_pg_NS.elf"

if [[ ! -f "$SECURE_ELF" ]]; then
    echo "Error: secure image not found: $SECURE_ELF" >&2
    exit 1
fi
if [[ ! -f "$NONSECURE_ELF" ]]; then
    echo "Error: nonsecure image not found: $NONSECURE_ELF" >&2
    exit 1
fi

echo "Flashing secure:    $SECURE_ELF"
echo "Flashing nonsecure: $NONSECURE_ELF"

openocd \
    -f board/st_nucleo_h5.cfg \
    -c "init" \
    -c "reset halt" \
    -c "flash write_image erase \"${SECURE_ELF}\"" \
    -c "flash write_image erase \"${NONSECURE_ELF}\"" \
    -c "reset run" \
    -c "shutdown"

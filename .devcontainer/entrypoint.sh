#!/bin/sh
set -e

# If no arguments are provided, print usage and exit
if [ $# -eq 0 ]; then
    echo "Usage: koraos-dev <make targets> [make options]"
    echo "Example: koraos-dev bcmrpi_defconfig"
    exit 1
fi

# Build flags: output dir, architecture, and any toolchain overrides
MAKEFLAGS="O=/build ARCH=${ARCH}"
[ -n "${CROSS_COMPILE}" ] && MAKEFLAGS="${MAKEFLAGS} CROSS_COMPILE=${CROSS_COMPILE}"
[ -n "${CC}" ] && MAKEFLAGS="${MAKEFLAGS} CC=${CC}"
[ -n "${LD}" ] && MAKEFLAGS="${MAKEFLAGS} LD=${LD}"

exec make ${MAKEFLAGS} "$@"

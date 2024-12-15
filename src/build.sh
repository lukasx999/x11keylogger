#!/usr/bin/env bash
set -euxo pipefail

CC=gcc
CFLAGS="-Wall -Wextra -ggdb -lX11 -lXi -std=c99 -pedantic"

$CC $CFLAGS main.c -o out

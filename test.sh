#!/bin/bash -

gcc -lcriterion -o test ./unit/test.c ./src/fast_crc32.c
./test

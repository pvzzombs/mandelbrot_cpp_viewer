#!/bin/bash
./runner.sh -t 10 -i 1 -d 3 ./build/Mandelbrot
wait
./runner.sh -t 10 -i 1 -d 3 ./build/Test
wait
echo "Done..."
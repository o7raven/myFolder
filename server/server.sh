#!/bin/bash

if [[ $1 == "comp" ]]; then
  echo "compiling..."
  gcc ../src/*.c ../src/structures.h ../src/network/*.c ../src/misc/*.c -o server.out -Wall -Wextra
fi

./server.out -d testingdirectory -t server -a localhost -p 3562
exit=$?
echo "Exit status: $exit"


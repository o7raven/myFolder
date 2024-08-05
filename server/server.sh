#!/bin/bash

if [[ $1 == "comp" ]]; then
  echo "compiling..."
  gcc ../src/main.c -o server.out -Wall -Wextra
fi

./server.out -d null -t server -a localhost -p 3562
exit=$?
echo "Exit status: $exit"


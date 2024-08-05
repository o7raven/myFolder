#!/bin/bash

if [[ $1 == "comp" ]]; then
  echo "compiling..."
  gcc ../src/main.c -o client.out -Wall -Wextra
fi

./client.out -d null -t client -a localhost -p 3562
exit=$?
echo "Exit status: $exit"


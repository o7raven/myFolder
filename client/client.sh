#!/bin/bash

if [[ $1 == "comp" ]]; then
  echo "compiling..."
  gcc main.c 
fi

./a.out -d null -t client -a localhost -p 3562
exit=$?
echo "Exit status: $exit"


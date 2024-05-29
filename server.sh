#!/bin/bash
gcc main.c
./a.out -p 3521 -d null -t server
exit=$?
echo "Exit status: $exit"

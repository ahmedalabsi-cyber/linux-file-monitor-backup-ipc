#!/bin/bash

EXECUTABLE="./monitor"
INPUT_DIR="."
LOG_FILE="./logs/report.txt"

mkdir -p logs

if [ ! -f Makefile ]; then
    echo "Error: Makefile not found."
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: executable not found."
    exit 1
fi

$EXECUTABLE "$INPUT_DIR" > "$LOG_FILE" 2>&1

if [ $? -eq 0 ]; then
    echo "System executed successfully. Output saved to $LOG_FILE"
else
    echo "Program execution failed."
fi

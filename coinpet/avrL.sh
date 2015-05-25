#!/bin/bash

echo "write target device's name"
read devName

echo "write target file's name"
read fileName

avrdude -c avrisp2 -p m324p -P /dev/tty.$devName -U flash:w:$fileName.hex

echo "Done:avrdude -c avrisp2 -p m324p -P /dev/tty.$devName -U flash:w:$fileName.hex"

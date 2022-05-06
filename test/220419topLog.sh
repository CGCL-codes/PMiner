#!/bin/sh
echo “  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND” >> 220419topLog.txt
top -b -n 6 -d 10 | grep main >>220419topLog.txt

#!/bin/bash
cd /proc/

processes=$( find . -maxdepth 1 -type d | grep '[0-9]' )

for process in $processes; do
  if [ -d "/proc/$process/" ]; then
    cat $process/status | grep -e Name -e Pid -e State
    number=$(echo $process | cut -d '/' -f 2)
    echo $number
    echo "files opened: $(lsof -p $number -w | wc -l)"
    echo ""
  fi



done

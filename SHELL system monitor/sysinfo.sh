#!/bin/bash
progress=( " " "\xE2\x96\x81" "\xE2\x96\x82" "\xE2\x96\x83" "\xE2\x96\x84" "\xE2\x96\x85" "\xE2\x96\x86" "\xE2\x96\x87" "\xE2\x96\x88")

function draw_graph {
  x_pos=$1
  block_size=$2
  shift 2
  a=("$@")
  #echo ${a[@]}

    for row in {1..16}; do
      tput cup $(( 22 - $row)) $x_pos

      for column in {1..10}; do
        value=${a[$(( $column - 1 ))]}

        if (( value <= block_size * $((row - 1)) )); then
          printf "${progress[0]}"
        elif (( value >= block_size * $row)); then
          printf "${progress[8]}"
        else
          block=$(( $((value % block_size)) / $(( block_size / 8)) ))
          if [ $block = 0 ]; then
            printf "${progress[1]}"
          else
            printf "${progress[$block]}"
          fi
        fi
      done
    done
}

 function draw_axis {
   tput cup 21 $1
   printf "$2|"

   tput cup 22 $1
   printf "$3"
   for column in {1..11}; do
      printf "\xE2\x80\xBE"
   done
   for row in {15..1}; do
     tput cup $(($row + 5)) $1
     printf "$3|"
   done
 }

 function format_IOoutput {
   tput cup 0 $1
   if [ $2 -le 4096 ]; then
     printf "$3: $2 B/s        "
   elif [ $2 -le 4194304 ]; then
     printf "$3: $(( $2 / 1024 )) kB/s       "
   else
     printf "$3: $(( $2 / 1048576 )) mB/s      "
   fi
 }

clear
DISK=$1

declare -a writes
writes=( 0 0 0 0 0 0 0 0 0 0 )

declare -a reads
reads=( 0 0 0 0 0 0 0 0 0 0 )

declare -a usages
usages=( 0 0 0 0 0 0 0 0 0 0 )

draw_axis 0 "32kB" "    "
draw_axis 19 "128kB" "     "
draw_axis 39 "0.062" "     "

while true; do

  disk_info=$( cat /proc/diskstats | grep "$DISK " | sed -e 's/  */ /g' | cut -f 5 );

  sectors_written_old=$(echo $disk_info | cut -f 10 -d " ");
  sectos_read_old=$(echo $disk_info | cut -f 6 -d " ")

  sleep 1;

  load_avg=$(cat /proc/loadavg | cut -f 1 -d " ")
  disk_info=$( cat /proc/diskstats | grep "$DISK " | sed -e 's/  */ /g' | cut -f 5 );

  sectors_written=$(echo $disk_info | cut -f 10 -d " ");
  sectors_read=$(echo $disk_info | cut -f 6 -d " ")

  #echo $load_avg
  #echo $writes_merged
  #echo $writes_merged_old

  writes=(${writes[@]:1})
  reads=(${reads[@]:1})
  usages=(${usages[@]:1})

  write_result=$((($sectors_written - $sectors_written_old) * 512));
  read_result=$((($sectors_read - $sectos_read_old) * 512))

  writes[9]=$write_result
  reads[9]=$read_result
  usages[9]=$(echo "$load_avg * 100" | bc -l | cut -f 1 -d ".")
  #echo ${usages[@]}

  format_IOoutput 0 $write_result "w"
  format_IOoutput 20 $read_result "r"
  tput cup 0 40
  printf "usage: $load_avg    "
  #echo "reading: $read_result b/s         "
  #echo "usage: $load_avg "
  #echo ${writes[@]}
  #echo "$load_avg * 100" | bc -l


  draw_graph 5 32768 "${writes[@]}"
  draw_graph 25 131072 "${reads[@]}"
  draw_graph  45  8 "${usages[@]}"
  tput cup 21 61
  #reads_merged_old=$reads_merged;
  #spent_read_old=$spent_read;
  #tput cup 5 $(( $column ))
  #printf ${progress[(( ($column) % 8 ))]}

done

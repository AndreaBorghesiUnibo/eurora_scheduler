#!/bin/bash

mod=1

log=`cat $1 | grep -E "Starting Scheduling Cycle|Leaving Scheduling Cycle" | awk -F';' '{print $1}'| tr ' ' '_'`
original=`echo $1 | awk -F/ '{print $NF}'`
#echo $original
for data in $log
do
	if test $mod -eq 1 ; then
		d=`echo $data | tr '_' ' '`
		date1=$(date --date="$d" +%s)
		#echo "starting "$date1
		mod=2
	else if test $mod -eq 2; then
		d=`echo $data | tr '_' ' '`
		date2=$(date --date="$d" +%s)
		#echo "ending "$date2
		diff=$(( $date2 - $date1 ))
		echo "Tempi_PBS;"$diff >>"OverheadPBS_"$original".log"
		mod=1
	fi
	fi
done 

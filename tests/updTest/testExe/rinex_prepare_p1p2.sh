#!/bin/bash

   # read station name from igs_list.txt
cat reference.p1p2.list | while read line 
do 

   station_name=`echo $line | tr A-Z a-z`

   echo "processing ${station_name}"

   teqc ./rinex/${station_name}2820.11o ./rinex/${station_name}2830.11o ./rinex/${station_name}2840.11o \
        ./rinex/${station_name}2850.11o ./rinex/${station_name}2860.11o ./rinex/${station_name}2870.11o > ./rinex/${station_name}16577.11o 

done

#!/bin/bash

   # read station.ion name from igs_list.txt
cat reference.p1p2.list | while read line 
do 

   STATION=`echo $line | tr a-z A-Z`

       # find station name in 'OCEAN_GTOP00.txt'
   INDEX=`awk -v sta=$STATION 'BEGIN{ i=0; } { if(sta==$1) { i=i+1; } } END  {print i;}' OCEAN-GOT00.list`

   if [ $INDEX -eq 0 ]
   then
      echo $STATION 
   fi

done  

   # read station.ion name from igs_list.txt
cat reference.c1only.list | while read line 
do 

   STATION=`echo $line | tr a-z A-Z`

       # find station name in 'OCEAN_GTOP00.txt'
   INDEX=`awk -v sta=$STATION 'BEGIN{ i=0; } { if(sta==$1) { i=i+1; } } END  {print i;}' OCEAN-GOT00.list`

   if [ $INDEX -eq 0 ]
   then
      echo $STATION 
   fi

done 

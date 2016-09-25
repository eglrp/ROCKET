#!/bin/bash

:>rover.p1p2.llh

   # read station.ion name from igs_list.txt
cat rover.p1p2.list | while read line 
do 

   STATION=`echo $line | tr a-z A-Z`
   awk -v STA=$STATION '{if($7==STA) print $0}' station.p1p2.llh >> rover.p1p2.llh

done  


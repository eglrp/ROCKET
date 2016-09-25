#!/bin/bash

:>reference.c1only.llh

   # read station.ion name from igs_list.txt
cat reference.c1only.list | while read line 
do 

   STATION=`echo $line | tr a-z A-Z`
   awk -v STA=$STATION '{if($7==STA) print $0}' station.c1only.llh >> reference.c1only.llh

done  


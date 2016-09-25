#!/bin/bash

rm reference.p1p2.90.llh

   # read station.ion name from igs_list.txt
cat reference.p1p2.90.list | while read line 
do 

   STATION=`echo $line | tr a-z A-Z`
   awk -v STA=$STATION '{if($7==STA) print $0}' station.p1p2.llh >> reference.p1p2.90.llh

done  


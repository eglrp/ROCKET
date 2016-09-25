#!/bin/bash

ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/PRN_GPS PRN_GPS
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/igs08.atx igs08.atx
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/igs08_1648.atx igs08_1648.atx
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/p1c1bias.hist p1c1bias.hist

ls /Users/shjzhang/Documents/Data/IGS/products/*1656* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./products/$file
done

ls /Users/shjzhang/Documents/Data/IGS/products/*1657* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./products/$file
done

ls /Users/shjzhang/Documents/Data/IGS/rinex/*16577.11o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done

ls /Users/shjzhang/Documents/Data/IGS/rinex/*2820.11o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done

#ls /Users/shjzhang/Documents/Data/IGS/products/*.11i | while read line
#do
#   echo $line
#   file=`echo $line | awk -F / '{print $8}'`
#   ln -s $line ./products/$file
#done

#!/bin/bash

##############################
# get the rinex file list
##############################
> clk.rnxlist
get_rnx.sh -b "2011 10 1 0 0 0" -e "2011 10 7 0 0 0" -i 6 -a "IGS" -u "../../tables/url.list" -s "clk.stalist" -l "clk" -p "/Users/apple/Documents/data/IGS/data" > get_rnx.log 

##############################
# get the ephemeris file list
##############################
> clk.sp3list
> clk.clklist
> clk.erplist
get_eph.sh -b "2011 10 1 0 0 0" -e "2011 10 7 0 0 0" -i 6 -a "IGS" -u "../../tables/url.list" -t "type.list" -l "clk" -p "/Users/apple/Documents/data/IGS/data" > get_eph.log 

##############################
# get the msc file list
##############################
# convert ssc2msc
cat clk.ssclist | while read line
do
   ssc2msc -s $line
done

> msc.txt
# now, let's merge all the msc files together for pppar positioning
ls *.msc | while read line
do
   cat $line >> msc.txt
done

##############################
# get the output file list
##############################
> clk.outlist
cat clk.rnxlist | while read line
do
   echo "$line".out >> clk.outlist
done

# now, Let's perform the ppp positioning
clk -r clk.rnxlist -s clk.sp3list -e clk.erplist -m msc.txt -D clk.p1c1list -i clk.ionlist

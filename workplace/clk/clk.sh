#!/bin/bash

##############################
# get the rinex file list
##############################
> clk.rnxlist
get_rnx.sh -b "2011 10 1 0 0 0" -e "2011 10 2 0 0 0" -i 24 -m  -a "IGS" -u "../../tables/url.list" -s "clk.60.reflist" -l "clk" -p "/Users/shjzhang/Documents/data/IGS/data" > get_rnx.log 

##############################
# get the ephemeris file list
##############################
> clk.ephList
> clk.clklist
> clk.erplist
> clk.ssclist
get_eph.sh -b "2011 09 30 0 0 0" -e "2011 10 2 0 0 0" -i 24 -u "../../tables/url.list" -t "type.list" -l "clk" -p "/Users/shjzhang/Documents/data/IGS/data" > get_eph.log 

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
ls /Users/shjzhang/Documents/data/IGS/data/*.msc | while read line
do
  echo $line;
  cat $line >> msc.txt
done

# now, Let's compute the satellite clock products
# warning, please pay attention to the option CASE.
clk -m msc.txt -r clk.rnxlist -s clk.ephlist -e clk.erplist -D clk.dcb_p1c1list -O whu2740_2790.clk

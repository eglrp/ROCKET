#!/bin/bash

# Download files needed.
ln -f -s /Users/zhaolei/ROCKET/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/zhaolei/ROCKET/tables/URL_LIST.txt url.list
ln -f -s /Users/zhaolei/ROCKET/tables/PRN_GPS PRN_GPS
ln -f -s /Users/zhaolei/ROCKET/tables/igs08.atx igs08.atx
ln -f -s /Users/zhaolei/ROCKET/tables/type.list ppp_type.list

proj="updTest"
dir=/Users/zhaolei/Documents/Data/IGS/
##get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -s "$proj.stalist.test" -l "$proj" -p "./rinex/" 
#get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -s "reference.p1p2.40.list.order" -l "$proj" -p "$dir" 
get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -s "tmp.sta" -l "$proj" -p "$dir" 

get_eph.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a IGS -u "url.list" -t "ppp_type.list" -l "$proj" -p "$dir"

# convert ssc2msc
cat $proj.ssclist | while read line
do
	ssc2msc -s $line
done

# merge all *.msc files
> msc.txt 
ls *.msc | while read line
do
	echo $line
	cat $line >> msc.txt
done

date

upd4 -m msc.txt -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist -O whu2820.3p1p2.11upd_upd4_again 

date



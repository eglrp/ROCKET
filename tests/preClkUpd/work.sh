#!/bin/bash

# Download files needed.
ln -f -s /Users/leizhao/pppbox/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/leizhao/pppbox/tables/URL_LIST.txt url.list
ln -f -s /Users/leizhao/pppbox/tables/PRN_GPS PRN_GPS
ln -f -s /Users/leizhao/pppbox/tables/igs08.atx igs08.atx
ln -f -s /Users/leizhao/pppbox/tables/type.list ppp_type.list
ln -f -s /Users/leizhao/pppbox/tables/receiver_bernese.lis recType.list



proj="clkupd"
dir=/Users/leizhao/Data/IGS/
#get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -s "$proj.stalist.test" -l "$proj" -p "./rinex/" 
#get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -s "reference.p1p2.40.list.order" -l "$proj" -p "$dir" 
get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -s "tmp.sta" -l "$proj" -p "$dir" 

get_eph.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a IGS -u "url.list" -t "type.list" -l "$proj" -p "$dir"

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

#upd -m msc.txt -r $proj.rnxlist -s $proj.ephlist -e $proj.erplist	-O whu2820.50.upd.new 

date

preClkUpd -m msc.txt -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist	-O jsonFileNames.list  

date 



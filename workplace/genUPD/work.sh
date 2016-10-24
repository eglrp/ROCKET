#!/bin/bash

# Download files needed.
ln -f -s /Users/zhaolei/ROCKET/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/zhaolei/ROCKET/tables/URL_LIST.txt url.list
ln -f -s /Users/zhaolei/ROCKET/tables/PRN_GPS PRN_GPS
ln -f -s /Users/zhaolei/ROCKET/tables/igs08.atx igs08.atx
ln -f -s /Users/zhaolei/ROCKET/tables/type.list ppp_type.list

date

genUPD -i inputJson.list -O whu282.3p1p2.11upd.test1 

date



#!/bin/bash

rm cnes.wl.diff sgg.wl.diff
awk '{for(i=1;i<=NF;i++) {if(i!=1) print $i-$1} }' cnes.wl.txt >> cnes.wl.diff
awk '{for(i=1;i<=NF;i++) {if(i!=1) print $i-$1} }' sgg.wl.txt >> sgg.wl.diff
paste sgg.wl.diff cnes.wl.diff | awk '{print $1-$2 - int($1-$2+0.5)}' > sgg_cnes.diff

#!/bin/bash
#
awk '{ if( $1=="AS")print $0}' whu16577.p1p2.upd > whu.upd
#
awk '{if($2=="G01") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G01
awk '{if($2=="G02") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G02
awk '{if($2=="G03") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G03
awk '{if($2=="G04") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G04
awk '{if($2=="G05") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G05
awk '{if($2=="G06") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G06
awk '{if($2=="G07") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G07
awk '{if($2=="G08") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G08
awk '{if($2=="G09") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G09
awk '{if($2=="G10") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G10
awk '{if($2=="G11") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G11
awk '{if($2=="G12") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G12
awk '{if($2=="G13") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G13
awk '{if($2=="G14") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G14
awk '{if($2=="G15") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G15
awk '{if($2=="G16") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G16
awk '{if($2=="G17") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G17
awk '{if($2=="G18") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G18
awk '{if($2=="G19") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G19
awk '{if($2=="G20") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G20
awk '{if($2=="G21") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G21
awk '{if($2=="G22") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G22
awk '{if($2=="G23") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G23
awk '{if($2=="G25") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G25
awk '{if($2=="G26") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G26
awk '{if($2=="G27") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G27
awk '{if($2=="G28") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G28
awk '{if($2=="G29") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G29
awk '{if($2=="G30") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G30
awk '{if($2=="G31") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G31
awk '{if($2=="G32") { print $5+($6+$7/60 )/24 , $10}}' whu.upd > bias.nl.G32

gnuplot << EOF
set terminal png
set output 'whu16577.lc.png'
set xlabel 'time(day)'
set ylabel 'narrow-lane upd(Cycles)'
set size ratio 1
set key outside
set key font ",10"
set key box
set yr[-1:1]
set xr[9:10]
set grid
plot "bias.nl.G01" using 1:2  with lines title "PRN 01", \
     "bias.nl.G02" using 1:2  with lines title "PRN 02", \
     "bias.nl.G03" using 1:2  with lines title "PRN 03", \
     "bias.nl.G04" using 1:2  with lines title "PRN 04", \
     "bias.nl.G05" using 1:2  with lines title "PRN 05", \
     "bias.nl.G06" using 1:2  with lines title "PRN 06", \
     "bias.nl.G07" using 1:2  with lines title "PRN 07", \
     "bias.nl.G08" using 1:2  with lines title "PRN 08", \
     "bias.nl.G09" using 1:2  with lines title "PRN 09", \
     "bias.nl.G10" using 1:2  with lines title "PRN 10", \
     "bias.nl.G11" using 1:2  with lines title "PRN 11", \
     "bias.nl.G12" using 1:2  with lines title "PRN 12", \
     "bias.nl.G13" using 1:2  with lines title "PRN 13", \
     "bias.nl.G14" using 1:2  with lines title "PRN 14", \
     "bias.nl.G15" using 1:2  with lines title "PRN 15"   
EOF

#     "bias.nl.G16" using 1:2  with lines title "PRN 16", \
#     "bias.nl.G17" using 1:2  with lines title "PRN 17", \
#     "bias.nl.G18" using 1:2  with lines title "PRN 18", \
#     "bias.nl.G19" using 1:2  with lines title "PRN 19", \
#     "bias.nl.G20" using 1:2  with lines title "PRN 20", \
#     "bias.nl.G21" using 1:2  with lines title "PRN 21", \
#     "bias.nl.G22" using 1:2  with lines title "PRN 22", \
#     "bias.nl.G23" using 1:2  with lines title "PRN 23", \
#     "bias.nl.G25" using 1:2  with lines title "PRN 25", \
#     "bias.nl.G28" using 1:2  with lines title "PRN 28", \
#     "bias.nl.G29" using 1:2  with lines title "PRN 29", \
#     "bias.nl.G31" using 1:2  with lines title "PRN 31", \
#     "bias.nl.G24" using 1:2  with lines title "PRN 24", \
#     "bias.nl.G26" using 1:2  with lines title "PRN 26", \
#     "bias.nl.G27" using 1:2  with lines title "PRN 27", \
#     "bias.nl.G30" using 1:2  with lines title "PRN 30", \
#     "bias.nl.G32" using 1:2  with lines title "PRN 32"

##################
##################
##################

#
awk '{if($2=="G01") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G01
awk '{if($2=="G02") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G02
awk '{if($2=="G03") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G03
awk '{if($2=="G04") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G04
awk '{if($2=="G05") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G05
awk '{if($2=="G06") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G06
awk '{if($2=="G07") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G07
awk '{if($2=="G08") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G08
awk '{if($2=="G09") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G09
awk '{if($2=="G10") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G10
awk '{if($2=="G11") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G11
awk '{if($2=="G12") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G12
awk '{if($2=="G13") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G13
awk '{if($2=="G14") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G14
awk '{if($2=="G15") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G15
awk '{if($2=="G16") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G16
awk '{if($2=="G17") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G17
awk '{if($2=="G18") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G18
awk '{if($2=="G19") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G19
awk '{if($2=="G20") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G20
awk '{if($2=="G21") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G21
awk '{if($2=="G22") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G22
awk '{if($2=="G23") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G23
awk '{if($2=="G25") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G25
awk '{if($2=="G26") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G26
awk '{if($2=="G27") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G27
awk '{if($2=="G28") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G28
awk '{if($2=="G29") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G29
awk '{if($2=="G30") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G30
awk '{if($2=="G31") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G31
awk '{if($2=="G32") { print $5+($6+$7/60 )/24, $11}}' whu.upd > bias.wl.G32


gnuplot << EOF
set terminal png
set output 'whu16577.wl.png'
set xlabel 'time(day)'
set ylabel 'widelane upd(Cycles)'
set size ratio 1
set grid
set xr[9:10]
set key outside
set key font ",10"
set key box
plot "bias.wl.G01" using 1:2  with lines title "PRN 01", \
     "bias.wl.G02" using 1:2  with lines title "PRN 02", \
     "bias.wl.G03" using 1:2  with lines title "PRN 03", \
     "bias.wl.G04" using 1:2  with lines title "PRN 04", \
     "bias.wl.G05" using 1:2  with lines title "PRN 05", \
     "bias.wl.G06" using 1:2  with lines title "PRN 06", \
     "bias.wl.G07" using 1:2  with lines title "PRN 07", \
     "bias.wl.G08" using 1:2  with lines title "PRN 08", \
     "bias.wl.G09" using 1:2  with lines title "PRN 09", \
     "bias.wl.G11" using 1:2  with lines title "PRN 11", \
     "bias.wl.G12" using 1:2  with lines title "PRN 12", \
     "bias.wl.G13" using 1:2  with lines title "PRN 13", \
     "bias.wl.G14" using 1:2  with lines title "PRN 14", \
     "bias.wl.G15" using 1:2  with lines title "PRN 15", \
     "bias.wl.G10" using 1:2  with lines title "PRN 10"

EOF

#     "bias.wl.G11" using 1:2  with lines title "PRN 11", \
#     "bias.wl.G12" using 1:2  with lines title "PRN 12", \
#     "bias.wl.G13" using 1:2  with lines title "PRN 13", \
#     "bias.wl.G14" using 1:2  with lines title "PRN 14", \
#     "bias.wl.G15" using 1:2  with lines title "PRN 15", \


rstats --col 2 --prec 2  bias.wl.G01 --brief|awk '{if($4=="Ave") print "G01",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G02 --brief|awk '{if($4=="Ave") print "G02",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G03 --brief|awk '{if($4=="Ave") print "G03",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G04 --brief|awk '{if($4=="Ave") print "G04",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G05 --brief|awk '{if($4=="Ave") print "G05",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G06 --brief|awk '{if($4=="Ave") print "G06",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G07 --brief|awk '{if($4=="Ave") print "G07",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G08 --brief|awk '{if($4=="Ave") print "G08",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G09 --brief|awk '{if($4=="Ave") print "G09",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G10 --brief|awk '{if($4=="Ave") print "G10",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G11 --brief|awk '{if($4=="Ave") print "G11",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G12 --brief|awk '{if($4=="Ave") print "G12",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G13 --brief|awk '{if($4=="Ave") print "G13",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G14 --brief|awk '{if($4=="Ave") print "G14",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G15 --brief|awk '{if($4=="Ave") print "G15",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G16 --brief|awk '{if($4=="Ave") print "G16",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G17 --brief|awk '{if($4=="Ave") print "G17",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G18 --brief|awk '{if($4=="Ave") print "G18",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G19 --brief|awk '{if($4=="Ave") print "G19",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G20 --brief|awk '{if($4=="Ave") print "G20",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G21 --brief|awk '{if($4=="Ave") print "G21",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G22 --brief|awk '{if($4=="Ave") print "G22",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G23 --brief|awk '{if($4=="Ave") print "G23",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G25 --brief|awk '{if($4=="Ave") print "G25",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G26 --brief|awk '{if($4=="Ave") print "G26",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G27 --brief|awk '{if($4=="Ave") print "G27",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G28 --brief|awk '{if($4=="Ave") print "G28",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G29 --brief|awk '{if($4=="Ave") print "G29",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G30 --brief|awk '{if($4=="Ave") print "G30",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G31 --brief|awk '{if($4=="Ave") print "G31",$0}'>>bias.wl.ave
rstats --col 2 --prec 2  bias.wl.G32 --brief|awk '{if($4=="Ave") print "G32",$0}'>>bias.wl.ave

rm bias.wl.G*
rm bias.nl.G*

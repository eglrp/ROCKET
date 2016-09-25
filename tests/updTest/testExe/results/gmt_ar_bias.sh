#!/bin/bash
#
awk '{ if($1=="AR")print $0}' updx.ar  > whu.updx
#
awk '{if($2=="ABPO") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G01
awk '{if($2=="AMC2") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G02
awk '{if($2=="ARTU") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G03
awk '{if($2=="BOGT") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G04
awk '{if($2=="CHAT") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G05
awk '{if($2=="CRO1") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G06
awk '{if($2=="GUAM") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G07
awk '{if($2=="HARB") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G08
awk '{if($2=="ISPA") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G09
awk '{if($2=="KERG") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G10
awk '{if($2=="KOKB") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G11
awk '{if($2=="MDO1") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G12
awk '{if($2=="MKEA") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G13
awk '{if($2=="NRIL") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G14
awk '{if($2=="OPMT") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G15
awk '{if($2=="PERT") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G16
awk '{if($2=="QAQ1") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G17
awk '{if($2=="QUIN") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G18
awk '{if($2=="RABT") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G19
awk '{if($2=="RIO2") { print ($5+($6+$7/60.0)/24.0), $10}}' whu.updx > bias.G20

gnuplot << EOF
set terminal png
set output 'updx.wl.ar.png'
set xlabel 'time(day)'
set ylabel 'updx(m)'
set size 1.0, 1.0
set grid
set xr[9:15]
set yr[-2:2]
plot "bias.G01" using 1:2  with lines title "ABPO", \
     "bias.G02" using 1:2  with lines title "AMC2", \
     "bias.G03" using 1:2  with lines title "ARTU", \
     "bias.G04" using 1:2  with lines title "BOGT", \
     "bias.G05" using 1:2  with lines title "CHAT", \
     "bias.G06" using 1:2  with lines title "CRO1", \
     "bias.G07" using 1:2  with lines title "GUAM", \
     "bias.G08" using 1:2  with lines title "HARB", \
     "bias.G09" using 1:2  with lines title "ISPA", \
     "bias.G10" using 1:2  with lines title "KERG", \
     "bias.G11" using 1:2  with lines title "KOKB", \
     "bias.G12" using 1:2  with lines title "MDO1", \
     "bias.G13" using 1:2  with lines title "MKEA", \
     "bias.G14" using 1:2  with lines title "NRIL", \
     "bias.G15" using 1:2  with lines title "OPMT", \
     "bias.G16" using 1:2  with lines title "PERT", \
     "bias.G17" using 1:2  with lines title "QAQ1", \
     "bias.G18" using 1:2  with lines title "QUIN", \
     "bias.G19" using 1:2  with lines title "RABT", \
     "bias.G20" using 1:2  with lines title "RIO2"   
EOF
#
#    IIA satellite
#

#    "bias.G03" using 1:2  with lines title "PRN 03", \
#    "bias.G04" using 1:2  with lines title "PRN 04", \
#    "bias.G08" using 1:2  with lines title "PRN 08", \
#    "bias.G09" using 1:2  with lines title "PRN 09", \


##################
##################
##################

#
awk '{if($2=="ABPO") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G01
awk '{if($2=="AMC2") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G02
awk '{if($2=="ARTU") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G03
awk '{if($2=="BOGT") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G04
awk '{if($2=="CHAT") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G05
awk '{if($2=="CRO1") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G06
awk '{if($2=="GUAM") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G07
awk '{if($2=="HARB") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G08
awk '{if($2=="ISPA") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G09
awk '{if($2=="KERG") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G10
awk '{if($2=="KOKB") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G11
awk '{if($2=="MDO1") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G12
awk '{if($2=="MKEA") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G13
awk '{if($2=="NRIL") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G14
awk '{if($2=="OPMT") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G15
awk '{if($2=="PERT") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G16
awk '{if($2=="QAQ1") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G17
awk '{if($2=="QUIN") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G18
awk '{if($2=="RABT") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G19
awk '{if($2=="RIO2") { print ($5+($6+$7/60.0)/24.0), $11}}' whu.updx > bias2.G20

gnuplot << EOF
set terminal png
set output 'updx.lc.ar.png'
set xlabel 'time(day)'
set ylabel 'updx(m)'
set size 1.0, 1.0
set grid
set xr[9:15]
plot "bias2.G01" using 1:2  with lines title "ABPO", \
     "bias2.G02" using 1:2  with lines title "AMC2", \
     "bias2.G03" using 1:2  with lines title "ARTU", \
     "bias2.G04" using 1:2  with lines title "BOGT", \
     "bias2.G05" using 1:2  with lines title "CHAT", \
     "bias2.G06" using 1:2  with lines title "CRO1", \
     "bias2.G07" using 1:2  with lines title "GUAM", \
     "bias2.G08" using 1:2  with lines title "HARB", \
     "bias2.G09" using 1:2  with lines title "ISPA", \
     "bias2.G10" using 1:2  with lines title "KERG", \
     "bias2.G11" using 1:2  with lines title "KOKB", \
     "bias2.G12" using 1:2  with lines title "MDO1", \
     "bias2.G13" using 1:2  with lines title "MKEA", \
     "bias2.G14" using 1:2  with lines title "NRIL", \
     "bias2.G15" using 1:2  with lines title "OPMT", \
     "bias2.G16" using 1:2  with lines title "PERT", \
     "bias2.G17" using 1:2  with lines title "QAQ1", \
     "bias2.G18" using 1:2  with lines title "QUIN", \
     "bias2.G19" using 1:2  with lines title "RABT", \
     "bias2.G20" using 1:2  with lines title "RIO2"
EOF

rm  bias.G[0-9][0-9]
rm  bias2.G[0-9][0-9]
rm  bias_MW.G[0-9][0-9]

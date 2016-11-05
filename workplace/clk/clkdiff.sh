#!/bin/bash
#clkfile1="/home/shjzhang/data/IGS/data/cod16555.clk"
#clkfile2="/home/shjzhang/data/IGS/data/cod16556.clk"
#clkfile3="/home/shjzhang/data/IGS/data/cod16560.clk"
#clkfile4="/home/shjzhang/data/IGS/data/cod16561.clk"
#clkfile5="/home/shjzhang/data/IGS/data/cod16562.clk"
#
#mergeRinClk -i $clkfile1 -i $clkfile2 -i $clkfile3 -i $clkfile4 -i $clkfile5 -o cod.clk
#
#clkdiff -v -r 02 whu20112740_2770.clk cod.clk > clkdiff.log 
#
#awk '{if($1 != "#" && NF>10) print $0}' clkdiff.log > clkdiff.txt
#
#gnuplot << EOF
#set terminal png
#set output 'clkdiff2011.png'
#set xlabel 'day'
#set ylabel 'diff(m)'
#set size 1.0, 1.0
#set grid
#set yrange[-3:3]
#plot "clkdiff.txt" using 2:3   with lines title "01", \
#     "clkdiff.txt" using 2:4   with lines title "02", \
#     "clkdiff.txt" using 2:5   with lines title "03", \
#     "clkdiff.txt" using 2:6   with lines title "04", \
#     "clkdiff.txt" using 2:7   with lines title "05", \
#     "clkdiff.txt" using 2:8   with lines title "06", \
#     "clkdiff.txt" using 2:9   with lines title "07", \
#     "clkdiff.txt" using 2:10  with lines title "08", \
#     "clkdiff.txt" using 2:11  with lines title "09", \
#     "clkdiff.txt" using 2:12  with lines title "10", \
#     "clkdiff.txt" using 2:13  with lines title "11", \
#     "clkdiff.txt" using 2:14  with lines title "12", \
#     "clkdiff.txt" using 2:15  with lines title "13", \
#     "clkdiff.txt" using 2:16  with lines title "14", \
#     "clkdiff.txt" using 2:17  with lines title "15", \
#     "clkdiff.txt" using 2:18  with lines title "16", \
#     "clkdiff.txt" using 2:19  with lines title "17", \
#     "clkdiff.txt" using 2:20  with lines title "18", \
#     "clkdiff.txt" using 2:21  with lines title "19"   
#EOF

#clkfile1="/home/shjzhang/data/IGS/data/cod18122.clk"
#clkfile2="/home/shjzhang/data/IGS/data/cod18123.clk"
#clkfile3="/home/shjzhang/data/IGS/data/cod18124.clk"
#clkfile4="/home/shjzhang/data/IGS/data/cod18125.clk"
#clkfile5="/home/shjzhang/data/IGS/data/cod18126.clk"
#
#mergeRinClk -i $clkfile1 -i $clkfile2 -i $clkfile3 -i $clkfile4 -i $clkfile5 -o cod.clk
#
#clkdiff -v -r 02 whu20142740_2770.clk cod.clk > clkdiff.log 
#
#awk '{if($1 != "#" && NF>10) print $0}' clkdiff.log > clkdiff.txt
#
#gnuplot << EOF
#set terminal png
#set output 'clkdiff2014.png'
#set xlabel 'day'
#set ylabel 'diff(m)'
#set size 1.0, 1.0
#set grid
#set yrange[-3:3]
#plot "clkdiff.txt" using 2:3   with lines title "01", \
#     "clkdiff.txt" using 2:4   with lines title "02", \
#     "clkdiff.txt" using 2:5   with lines title "03", \
#     "clkdiff.txt" using 2:6   with lines title "04", \
#     "clkdiff.txt" using 2:7   with lines title "05", \
#     "clkdiff.txt" using 2:8   with lines title "06", \
#     "clkdiff.txt" using 2:9   with lines title "07", \
#     "clkdiff.txt" using 2:10  with lines title "08", \
#     "clkdiff.txt" using 2:11  with lines title "09", \
#     "clkdiff.txt" using 2:12  with lines title "10", \
#     "clkdiff.txt" using 2:13  with lines title "11", \
#     "clkdiff.txt" using 2:14  with lines title "12", \
#     "clkdiff.txt" using 2:15  with lines title "13", \
#     "clkdiff.txt" using 2:16  with lines title "14", \
#     "clkdiff.txt" using 2:17  with lines title "15", \
#     "clkdiff.txt" using 2:18  with lines title "16", \
#     "clkdiff.txt" using 2:19  with lines title "17", \
#     "clkdiff.txt" using 2:20  with lines title "18", \
#     "clkdiff.txt" using 2:21  with lines title "19"   
#EOF

clkfile1="/home/shjzhang/data/IGS/data/cod18643.clk"
clkfile2="/home/shjzhang/data/IGS/data/cod18644.clk"
clkfile3="/home/shjzhang/data/IGS/data/cod18645.clk"
clkfile4="/home/shjzhang/data/IGS/data/cod18646.clk"
clkfile5="/home/shjzhang/data/IGS/data/cod18650.clk"

mergeRinClk -i $clkfile1 -i $clkfile2 -i $clkfile3 -i $clkfile4 -i $clkfile5 -o cod.clk

clkdiff -v -r 02 whu20152740_2770.clk cod.clk > clkdiff.log 

awk '{if($1 != "#" && NF>10) print $0}' clkdiff.log > clkdiff.txt

gnuplot << EOF
set terminal png
set output 'clkdiff2015.png'
set xlabel 'day'
set ylabel 'diff(m)'
set size 1.0, 1.0
set grid
set yrange[-3:3]
plot "clkdiff.txt" using 2:3   with lines title "01", \
     "clkdiff.txt" using 2:4   with lines title "02", \
     "clkdiff.txt" using 2:5   with lines title "03", \
     "clkdiff.txt" using 2:6   with lines title "04", \
     "clkdiff.txt" using 2:7   with lines title "05", \
     "clkdiff.txt" using 2:8   with lines title "06", \
     "clkdiff.txt" using 2:9   with lines title "07", \
     "clkdiff.txt" using 2:10  with lines title "08", \
     "clkdiff.txt" using 2:11  with lines title "09", \
     "clkdiff.txt" using 2:12  with lines title "10", \
     "clkdiff.txt" using 2:13  with lines title "11", \
     "clkdiff.txt" using 2:14  with lines title "12", \
     "clkdiff.txt" using 2:15  with lines title "13", \
     "clkdiff.txt" using 2:16  with lines title "14", \
     "clkdiff.txt" using 2:17  with lines title "15", \
     "clkdiff.txt" using 2:18  with lines title "16", \
     "clkdiff.txt" using 2:19  with lines title "17", \
     "clkdiff.txt" using 2:20  with lines title "18", \
     "clkdiff.txt" using 2:21  with lines title "19"   
EOF

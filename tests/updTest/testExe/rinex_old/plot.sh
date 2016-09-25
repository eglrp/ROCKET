#!/bin/bash
#
ls *.out| while read line
do

   awk '{print $2+$3/86400.0, $4, $5, $6}' $line > tempfile

   gnuplot << EOF
   set terminal png
   set output '${line}.png'
   set xlabel 'day'
   set ylabel 'diff(m)'
   set size 1.0, 1.0
   set grid
   plot "tempfile" using 1:2  with lines title "dn", \
        "tempfile" using 1:3  with lines title "de", \
        "tempfile" using 1:4  with lines title "du"
EOF

rm tempfile

done

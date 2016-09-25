#!/bin/bash
#
# Input/output file setting
#

gmtset ANNOT_FONT Times-Roman ANNOT_FONT_SIZE 8


# plot the stations using GMT command
pscoast -Rg -JQ0/18c -B60/30 -Dc -A5000 -Gwhite -Scyan  -X0.55i -Y0.55i -K -P > station.c1only.ps
psxy -R  -JQ -St0.2c -B  -Gred -K  -O  station.c1only.llh >> station.c1only.ps
pstext  -R  -JQ  -B  -Dj0.1c/0.1c -Gblack  -O station.c1only.llh >> station.c1only.ps
ps2raster station.c1only.ps -A -Tg

exit 0

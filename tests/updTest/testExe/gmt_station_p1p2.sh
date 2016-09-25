#!/bin/bash
#
# Input/output file setting
#

gmt gmtset ANNOT_FONT Times-Roman ANNOT_FONT_SIZE 8
gmt gmtset GRID_CROSS_SIZE 0

# plot the stations using GMT command
gmt pscoast -Rg -JQ0/18c -B60/30 -Dc -A5000 -Gwhite -Scyan  -X0.55i -Y0.55i -K -P > station.p1p2.ps
gmt psxy -R  -JQ -St0.2c -B  -Gred -K  -O  station.p1p2.llh >> station.p1p2.ps
gmt pstext  -R  -JQ  -B  -Dj0.02c/0.02c -G -O station.p1p2.llh >> station.p1p2.ps
gmt ps2raster station.p1p2.ps -A -Tg

## plot the stations using GMT command
#pscoast -Rg -JQ180/18c -B60/30 -Dc -A5000 -Gwhite -Scyan  -X0.55i -Y0.55i -K -P > station.p1p2.180.ps
#psxy -R  -JQ -St0.2c -B  -Gred -K  -O  station.p1p2.llh >> station.p1p2.180.ps
#pstext  -R  -JQ  -B  -Dj0.1c/0.1c -Gblack  -O station.p1p2.llh >> station.p1p2.180.ps
#ps2raster station.p1p2.180.ps -A -Tg

#gmtset PLOT_DEGREE_FORMAT ddd:mm:ssF GRID_CROSS_SIZE 0.05i
#gmtset GRID_CROSS_SIZE 0

## plot the stations using GMT command
#pscoast -R-15/25/30/60 -Jm0.05i -Ba10f05/a10f5 -Di -A50 -Gwhite -Scyan -K  -P > station.p1p2.ps
#psxy    -R -Jm -B -St0.2c -Gred -K  -O  station.p1p2.llh >> station.p1p2.ps
#pstext  -R -Jm -B -Dj0.1c/0.1c -Gblack  -O station.p1p2.llh >> station.p1p2.ps
#ps2raster station.p1p2.ps -A -Tg
#
## plot the stations using GMT command
#pscoast -R239/244/32/36 -Jm2c -Ba2f1/a2f1 -Di -A10 -Gwhite -Scyan -K  -P > station.p1p2_us.ps
#psxy    -R -Jm -B -St0.4c -Gred -K  -O  station.p1p2.llh >> station.p1p2_us.ps
#pstext  -R -Jm -B -Dj0.1c/0.1c -Gblack  -O station.p1p2.llh >> station.p1p2_us.ps
#ps2raster station.p1p2_us.ps -A -Tg

exit 0

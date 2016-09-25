#!/bin/bash
#
# Input/output file setting
#

gmt gmtset FONT_ANNOT_PRIMARY 8p,Helvetica,black
gmt gmtset MAP_FRAME_WIDTH 2p

# plot the stations using GMT command
gmt pscoast -Rg -JQ0/14c -B60/30 -Dl -A0/0/4 -N1/0.25p -G200 -X0.55i -Y0.55i -K -P > rover.p1p2.ps
gmt psxy -R  -JQ -Sc0.1c -B  -Gred -W -O  rover.p1p2.llh >> rover.p1p2.ps
gmt ps2raster rover.p1p2.ps -A -Tg


exit 0

#!/bin/bash

#       # Convert the time from (year)/(day of year) to (gps week)/(day of week)
#gpsweek=`timeconvert -y "2005 224 0.0" -F "%F %w" | awk '{print $1}'`
#dow=`timeconvert -y "2005 224 0.0" -F "%F %w" | awk '{print $2}'`
#
#      # download ephemeris data
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.clk_05s.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.clk.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.eph.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}7.erp.Z
#
#
#   # download ephemeris data of igs 
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/igs${gpsweek}${dow}.*.Z 
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/igs${gpsweek}7.erp.Z
#
#       # Convert the time from (year)/(day of year) to (gps week)/(day of week)
#gpsweek=`timeconvert -y "2004 237 0.0" -F "%F %w" | awk '{print $1}'`
#dow=`timeconvert -y "2004 237 0.0" -F "%F %w" | awk '{print $2}'`
#
#      # download ephemeris data
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.clk_05s.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.clk.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.eph.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}7.erp.Z
#
#
#   # download ephemeris data of igs 
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/igs${gpsweek}${dow}.*.Z 
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/igs${gpsweek}7.erp.Z


       # Convert the time from (year)/(day of year) to (gps week)/(day of week)
gpsweek=`timeconvert -y "2014 7 0.0" -F "%F %w" | awk '{print $1}'`
dow=`timeconvert -y "2014 7 0.0" -F "%F %w" | awk '{print $2}'`

      # download ephemeris data
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.clk_05s.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.clk.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}${dow}.eph.Z
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/cod${gpsweek}7.erp.Z


   # download ephemeris data of igs 
#wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/igs${gpsweek}${dow}.*.Z 
wget -c -r -k -nd -nc -np -P ./ ftp://cddis.gsfc.nasa.gov/gps/products/$gpsweek/igs${gpsweek}7.erp.Z

#   # download igs antenna file
#wget --tries=5 -r -c -k -nd -np ftp://igs.ensg.ign.fr/pub/igs/igscb/station/general/igs08.atx
#
## p1c1bias data
#wget ftp://dgn6.esoc.esa.int/CC2NONCC/p1c1bias.hist
#
## download GPS satellite constellation file
#wget -r -c -k -nd -nc -np ftp://sideshow.jpl.nasa.gov/pub/gipsy_products/gipsy_params/PRN_GPS.gz




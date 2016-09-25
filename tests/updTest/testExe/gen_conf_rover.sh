#!/bin/bash
# 
# According to the format and function of configure file, generate a configure
# file for clock/bias estimation using global GPS stations' observation data.
#
###################################################################################

#
# Input/output file setting
#
:>rover.sec

  # file to be generated
OUTFILE=rover.sec        

  # snx file, which include the station information, e.g. antenna type, offset, and position
SNXFILE=./products/igs11P1657.ssc

#
# Now, list all the stations according to the rinex data file
#
#
# Generate the gps receiver "SECTION" according to the stations list file and
# the IGS Sinex solution file "cod16567.ssc"
#
   # solution
awk '/\+SOLUTION\/ESTIMATE/,/\-SOLUTION\/ESTIMATE/' $SNXFILE > solution.txt
   # antenna type
awk '/\+SITE\/ANTENNA/,/\-SITE\/ANTENNA/'           $SNXFILE > antenna.txt
   # offsetARP
awk '/\+SITE\/ECCENTRICITY/,/\-SITE\/ECCENTRICITY/' $SNXFILE > offset.txt

yy=11

dir="./rinex/"

   # read station name from igs_list.txt
cat rover.p1p2.list | while read line 
do 
   station=`echo $line | tr A-Z a-z `

     # STATION Name, all the character should be capitalized
   STATION=`echo $line | tr a-z A-Z `

      # search coordinate in the igs SINEX file
   COORDX=`awk -v STA=$STATION ' BEGIN{ i=0; COODX="" } 
                                 { if( ($2=="STAX") && ($3==STA)) { i=i+1; COORDX=$9;} }
                                 END { if(i==1) {printf("%12.3f\n",COORDX) } else { print "N"} }' solution.txt`
   COORDY=`awk -v STA=$STATION ' BEGIN{ i=0; COODY="" } 
                                 { if( ($2=="STAY") && ($3==STA)) { i=i+1; COORDY=$9;} }
                                 END { if(i==1) {printf("%12.3f\n",COORDY) } else { print "N"}}' solution.txt`
   COORDZ=`awk -v STA=$STATION ' BEGIN{ i=0; COODZ="" } 
                                 { if( ($2=="STAZ") && ($3==STA)) { i=i+1; COORDZ=$9;} }
                                 END{ if(i==1) {printf("%12.3f\n",COORDZ) } else { print "N"}}' solution.txt`

      # search Accentricity values in the igs SINEX file
#  awk -v STA=$STATION '{ if( ($1==STA)) printf("%8.4f\n",$8  ) }' offset.txt | wc
   dU=`awk -v STA=$STATION 'BEGIN{ i=0; dU=""}
                            { if( ($1==STA)) {i=i+1; dU=$8 }}
                            END{ if(i==1) { printf("%8.4f\n",dU) } else { print "N" } }' offset.txt`
   dN=`awk -v STA=$STATION 'BEGIN{ i=0; dN=""}
                            { if( ($1==STA)) {i=i+1; dN=$9 }}
                            END{ if(i==1) { printf("%8.4f\n",dN) } else { print "N" } }' offset.txt`
   dE=`awk -v STA=$STATION 'BEGIN{ i=0; dE=""}
                            { if( ($1==STA)) {i=i+1; dE=$10}}
                            END{ if(i==1) { printf("%8.4f\n",dE) } else { print "N" } }' offset.txt`

     # antenna model
   ANT=`awk -v STA=$STATION 'BEGIN{ i=0; ant1=""; ant2=""} 
                             { if( ($1==STA)) {i=i+1; ant1=$7; ant2=$8 } }
                             END{ if(i==1) { printf("%-16s%s\n", ant1, ant2 )} else {print "N"} }' antenna.txt`
   
   if [ "$COORDX" != "N" ] && [ "$COORDY" != "N" ] && [ "$COORDZ" != "N" ] &&
      [ "$dU" != "N" ] && [ "$dN" != "N" ] && [ "$dE" != "N" ] && 
      [ "$ANT" != "N" ] 
   then
         # print the section name with brackets surrounding the station name
   cat << EOF >> $OUTFILE
[$STATION]
  
# Rinex observation file 
rinexObsFile, Rinex observation file = $dir${station}16577.11o
# nominal position extracted from the IGS weekly solutions 
nominalPosition, ECEF-GPSTk = $COORDX $COORDY $COORDZ, meters            
# offset of ARP 
offsetARP, dH dLon dLat (UEN) = $dU $dE $dN, meters  
# antenna model 
antennaModel = $ANT # Antenna model 
printModel = TRUE 
modelFile = $dir${station}16577.model 
outputFile = $dir${station}16577.out

EOF
   
    ## Set additional variables for the MASTER station
    if [ $STATION == "ONSA" ] 
    then
    cat << EOF >> $OUTFILE

masterStation = TRUE
refStation = FALSE

EOF
    fi # End of 'if [ $STATION == "ONSA" ] '
    else
        echo "error:"
        echo $station
    fi # End of 'if [ $COORDX = ... ]'


done

exit 0

rm solution.txt antenna.txt offset.txt

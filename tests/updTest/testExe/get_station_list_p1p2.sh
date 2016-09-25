#!/bin/bash
#
#  Get the stations with y-codeless receivers, which report p1/p2 type code observables, 
#  given the p1c1bias.hist 
#
############ ############ ############ ############

   #
   # Step 1:  Get the cc and noncc receiver type list
   #

   # cc and noncc receiver types
awk '/\+cc2noncc\/rcvr/,/\-cc2noncc\/rcvr/' p1c1bias.hist > cc_noncc.info
 
   # create new file
touch c1p2.txt c1only.txt cc_and_noncc.txt

while IFS='' read -r line
do 
       # flag indicating the receiver type
    flag1=`echo $line | awk '{print $1}'`

    if   [ "$flag1" == "cc2noncc-type:C1P2" ]; then
       rcv_type="cc2noncc-type:C1P2"
       continue
    elif [ "$flag1" == "cc2noncc-type:C1"   ]; then
       rcv_type="cc2noncc-type:C1"
       continue
    fi

       # flag indicating the receiver type name
    flag2=`echo "$line" | awk '{print(substr($0,1,1))}'`

    if   [ "$rcv_type" == "cc2noncc-type:C1P2" ]  && [ "$flag2" == " "  ]; then
      echo "$line" | awk '{print(substr($0,1))}'|sed 's/^[ \t]*//g'| sed 's/[ \t]*$//g'>> c1p2.txt
      echo "$line" | awk '{print(substr($0,1))}'|sed 's/^[ \t]*//g'| sed 's/[ \t]*$//g'>> cc_and_noncc.txt
    elif [ "$rcv_type" == "cc2noncc-type:C1"   ]  && [ "$flag2" == " "  ]; then
      echo "$line" | awk '{print(substr($0,1))}'|sed 's/^[ \t]*//g'| sed 's/[ \t]*$//g'>> c1only.txt
      echo "$line" | awk '{print(substr($0,1))}'|sed 's/^[ \t]*//g'| sed 's/[ \t]*$//g'>> cc_and_noncc.txt
    fi

done < cc_noncc.info

   #
   # Step 2: get the stations with receiver reporting p1/p2 code 
   #

   # Get site list according to 'igs11P1656.ssc' or 'igs11P1656.ssc'
awk '/\+SITE\/ID/,/\-SITE\/ID/' ./products/igs11P1656.ssc > site.txt

   # Get station name list
ls ./rinex/*2820.11o > rinex.list

   # read station name from igs_list.txt
:>station.p1p2.llh
:>station.p1p2.list
while read rinexfile
do 
    declare -x rec_type

    while IFS="" read -r line
    do
          # RINEX Header flag
       flag=`echo "$line" | awk '{print(substr($0,61,19))}'`
          
          # Receiver type
       if [ "$flag" == "REC # / TYPE / VERS" ]; then
          rec_type=`echo "$line" | awk '{print(substr($0,21,20))}'|sed 's/^[ \t]*//g'| sed 's/[ \t]*$//g'`
          break 
       fi

    done < $rinexfile

#   echo "$rec_type"

       # find receiver type in 'cc_and_noncc.txt'
    found=`awk -v rec="$rec_type" 'BEGIN{ i=0; } { if(rec==substr($0,1)) { i=i+1; } } END  {print i;}' cc_and_noncc.txt`

       # If not fond
    if [[ $found -eq 0 ]];then

         # get station name with large character
       STATION=`echo "$rinexfile" | awk '{print substr($0,9,4)}' | tr a-z A-Z`

         # find this station name in 'site.txt'
       IS=`awk -v STA=$STATION 'BEGIN{ i=0; }
                                { if( ($1==STA)) {i=i+1;}}
                                END{ if(i==1) { print "Y" } else { print "N" } }' site.txt`

         # If found                                
       if [[ "$IS" == "Y" ]]; then

          LonDeg=`awk -v STA=$STATION 'NF>1 {if($1==STA)print(substr($0,45))}' site.txt | 
          awk '{printf("%f", $1)}'`
          LonMin=`awk -v STA=$STATION 'NF>1 {if($1==STA)print(substr($0,45))}' site.txt |  
          awk '{printf("%f", $2/60.0)}'`
          LonSec=`awk -v STA=$STATION 'NF>1 {if($1==STA)print(substr($0,45))}' site.txt |  
          awk '{printf("%f", $3/3600.0)}'`
   
          Lon=$(echo "$LonDeg+$LonMin"|bc)
   
          LatDeg=`awk -v STA=$STATION 'NF>1 {if($1==STA)print(substr($0,45))}' site.txt | 
          awk '{printf("%f",$4)}'`
          LatMin=`awk -v STA=$STATION 'NF>1 {if($1==STA)print(substr($0,45))}' site.txt | 
          awk '{printf("%f", $5/60.0)}'`
          LatSec=`awk -v STA=$STATION 'NF>1 {if($1==STA)print(substr($0,45))}' site.txt | 
          awk '{printf("%f", $6/3600.0)}'`
   
          Lat=$(echo "$LatDeg+$LatMin+$LatSec"|bc)
   
          echo $Lon $Lat 12 0 4 CB $STATION >> station.p1p2.llh
          echo $STATION >> station.p1p2.list
       else
          echo "$STATION is not in igs11P1656.ssc"
       fi
    fi

done < rinex.list

 # remove the files
rm c1p2.txt
rm c1only.txt
rm cc_and_noncc.txt
rm site.txt
rm cc2noncc.info

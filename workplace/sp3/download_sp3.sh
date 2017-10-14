week1=1949
dow1=0
week2=1949
dow2=6

dir_hdr=ftp://igs.gnsswhu.cn/pub/gps/products/

for((i=week1; i<=week2; i++))
do

    for((j=0; j<=6; j++))
    do
        if [[ $i -eq week1 ]] && [[ $j -lt dow1 ]]
        then
            continue
        fi

        if [[ $i -eq week2 ]] && [[ $j -gt dow2 ]]
        then
            continue
        fi

        downdir=$dir_hdr$i"/""igu"$i$j"_00.sp3.Z"
        wget -c $downdir

        downdir=$dir_hdr$i"/""igr"$i$j".sp3.Z"
        wget -c $downdir

        downdir=$dir_hdr$i"/""igs"$i$j".sp3.Z"
        wget -c $downdir

    done


done

#uncompress *Z

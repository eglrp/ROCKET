
#include "SSCStream.hpp"
#include "SSCHeader.hpp"
#include "SSCData.hpp"

#include "MSCStream.hpp"
#include "MSCHeader.hpp"
#include "MSCData.hpp"
#include "MSCStore.hpp"

#include "DataStructures.hpp"


using namespace std;
using namespace gpstk;


int main(int argc, char* argv[])
{
    string filename(argv[1]);

    int i = 0;

    SSCStream ss(filename.c_str());

    SSCHeader sh;
    SSCData sd;

    ss >> sh;

    int releaseYear = sh.releaseTime.year;
    int releaseDOY = sh.releaseTime.doy;

    CommonTime commonReleTime(sh.releaseTime.convertToCommonTime());

    YDSTime RefEpoch, EarliestEffect;

    typeValueMap tvMap, tvMap2, tvMap3;

    map<string, string> stationAntypeMap;

    map<string, typeValueMap> stationTypeValueMap;
    map<string, typeValueMap> stationTypeValueMap2;
    map<string, typeValueMap> stationTypeValueMap3;

    while (ss >> sd)
    {
        if (sd.antennaTypeFlag)
        {
            if ( sd.station.length() != 0 && sd.antennaType.length() != 0)
            {
                stationAntypeMap[sd.station] = sd.antennaType;
            }
        }

        if (sd.antennaOffsetFlag)
        {
            if ( sd.station.length() != 0)
            {
                tvMap[TypeID::AntOffU] = sd.antennaOffset[0];
                tvMap[TypeID::AntOffN] = sd.antennaOffset[1];
                tvMap[TypeID::AntOffE] = sd.antennaOffset[2];
                stationTypeValueMap[sd.station] = tvMap;
            }
        }

        if (sd.stationCoorFlag)
        {
            RefEpoch = sd.RefEpoch;
            EarliestEffect = sd.RefEpoch;
            if (RefEpoch.year >= 80 && RefEpoch.year <= 99)
            {
                RefEpoch.year = 1900 + RefEpoch.year;
                EarliestEffect.year = 1900 + EarliestEffect.year;
            }
            else if ( RefEpoch >= 0 && RefEpoch.year <= 20)
            {
                RefEpoch.year = 2000 + RefEpoch.year;
                EarliestEffect.year = 2000 + EarliestEffect.year;
            }
            else
            {
               cerr << "Invalid RefEpoch!" << endl;
            }

            if (sd.coordinates[0] != 0.0 &&
                sd.coordinates[1] != 0.0 &&
                sd.coordinates[2] != 0.0)
            {
                tvMap2[TypeID::staX] = sd.coordinates[0];
                tvMap2[TypeID::staY] = sd.coordinates[1];
                tvMap2[TypeID::staZ] = sd.coordinates[2];
                stationTypeValueMap2[sd.station] = tvMap2;
            }

            if (sd.containVelFlag)
            {
                tvMap3[TypeID::staVX] = sd.vel[0];
                tvMap3[TypeID::staVY] = sd.vel[1];
                tvMap3[TypeID::staVZ] = sd.vel[2];
                stationTypeValueMap3[sd.station] = tvMap3;
            }
        }

    } // end of 'while'

    ss.close();

    cout << filename << endl;

    string outfilename;

    int pos = filename.size() - 4;

    outfilename = filename.substr(0,pos) + ".msc";

    cout << outfilename << endl;


    ofstream ms;

    ms.open(outfilename.c_str(), ios::out);

    map<string, string>::iterator ss_it;

    for(ss_it = stationAntypeMap.begin();
        ss_it != stationAntypeMap.end();
        ++ss_it)
    {
        string currStation = (*ss_it).first;

        double STAX, STAY, STAZ,
               STAVX, STAVY, STAVZ,
               AntOffU, AntOffN, AntOffE;

        map<string, typeValueMap>::iterator stv_it;
        stv_it = stationTypeValueMap.find(currStation);

        if ( stv_it != stationTypeValueMap.end() )
        {
            AntOffU = (*stv_it).second.getValue(TypeID::AntOffU);
            AntOffN = (*stv_it).second.getValue(TypeID::AntOffN);
            AntOffE = (*stv_it).second.getValue(TypeID::AntOffE);
        }

        map<string, typeValueMap>::iterator stv_it2;
        stv_it2 = stationTypeValueMap2.find(currStation);

        if ( stv_it2 != stationTypeValueMap2.end() )
        {
            STAX = (*stv_it2).second.getValue(TypeID::staX);
            STAY = (*stv_it2).second.getValue(TypeID::staY);
            STAZ = (*stv_it2).second.getValue(TypeID::staZ);
        }

        if (sd.containVelFlag)
        {
            map<string, typeValueMap>::iterator stv_it3;
            stv_it3 = stationTypeValueMap3.find(currStation);

            if ( stv_it3 != stationTypeValueMap3.end() )
            {
                STAVX = (*stv_it3).second.getValue(TypeID::staVX);
                STAVY = (*stv_it3).second.getValue(TypeID::staVY);
                STAVZ = (*stv_it3).second.getValue(TypeID::staVZ);
            }
        }
        else
        {
            STAVX = 0.0;
            STAVY = 0.0;
            STAVZ = 0.0;
        }

        i++;

        if (ms.is_open())
        {
            ms << setw(4) << releaseYear
               << setw(3) << releaseDOY << " "

               << setfill('0') << setw(4) << i
               << setw(4) << (*ss_it).first << "   "

               << setw(4) << RefEpoch.year << " "
               << setw(3) << RefEpoch.doy << " "
               << setfill('0') << setw(5) << int(RefEpoch.sod)

               << setw(4) << EarliestEffect.year << " "
               << setw(3) << EarliestEffect.doy << " "
               << setfill('0') << setw(5) << int(EarliestEffect.sod)

               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(12) << STAX
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(12) << STAY
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(12) << STAZ

               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(7) << STAVX
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(7) << STAVY
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(7) << STAVZ

               << setfill(' ') << setprecision(4) << setw(8) << AntOffU
               << setfill(' ') << setprecision(4) << setw(8) << AntOffN
               << setfill(' ') << setprecision(4) << setw(8) << AntOffE

               << setw(20) << (*ss_it).second
               << endl;
        }

    } // end of 'for...'

    ms.close();

    return 0;
}

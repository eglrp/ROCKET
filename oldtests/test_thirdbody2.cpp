/*
 * Test for ThirdBody Gravitation.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ThirdBody.hpp"

#include "SP3EphemerisStore.hpp"
#include "SatDataReader.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


int main(void)
{
    // Conf File
    ConfDataReader confReader;

    try
    {
        confReader.open("../../ROCKET/oldtests/test.conf");
    }
    catch(...)
    {
        cerr << "Conf File Open Error." << endl;

        return 1;
    }

    // EOP File
    EOPDataStore2 eopDataStore;
    string eopFile;

    try
    {
        eopFile = confReader.getValue("IERSEOPFILE", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get EOP File Directory Error." << endl;

        return 1;
    }

    try
    {
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "EOP File Load Error." << endl;

        return 1;
    }

    // LeapSec File
    LeapSecStore leapSecStore;
    string lsFile;

    try
    {
        lsFile = confReader.getValue("IERSLSFILE", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get LeapSec File Directory Error." << endl;

        return 1;
    }

    try
    {
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "LeapSec File Load Error." << endl;

        return 1;
    }


    // Reference System
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);

    // Solar System
    SolarSystem solSys;
    string ephFile;

    try
    {
        ephFile = confReader.getValue("JPLEPHFILE", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get EPH File Directory Error." << endl;

        return 1;
    }

    try
    {
        solSys.initializeWithBinaryFile(ephFile);
    }
    catch(...)
    {
        cerr << "SolarSystem Initialize Error." << endl;

        return 1;
    }

    // Sat ID
    int prn;

    try
    {
        prn = confReader.getValueAsInt("SATPRN", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Satellite PRN Error." << endl;

        return 1;
    }

    SatID sat(prn, SatID::systemGPS);

    // SP3 File
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    string sp3File;
    while(true)
    {
        try
        {
            sp3File = confReader.fetchListValue("IGSSP3LIST", "DEFAULT");
        }
        catch(...)
        {
            cerr << "Get SP3 File Directory Error." << endl;

            return 1;
        }

        if(sp3File == "") break;

        try
        {
            sp3Eph.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "IGS SP3 File Load Error." << endl;

            return 1;
        }
    }

    // Earth Body
    EarthBody eb;

    // SatData File
    SatDataReader satData;
    string satDataFile = confReader.getValue("SATDATAFILE", "DEFAULT");

    try
    {
        satData.open(satDataFile);
    }
    catch(...)
    {
        cerr << "SatData File Open Error." << endl;

        return 1;
    }

    // Spacecraft
    Spacecraft sc;
    sc.setNumOfParam(0);
    sc.setSatID(sat);
    sc.setSatData(satData);

    // ThirdBody
    ThirdBody thd;
    thd.setSolarSystem(solSys);
    thd.setReferenceSystem(refSys);
    thd.enableMoon();


    // Position
    ifstream fin("coor.log");

    CivilTime ct(2014,2,1,0,0,0.0, TimeSystem::GPS);
    CommonTime gps0(ct.convertToCommonTime());
    CommonTime utc0(refSys.GPS2UTC(gps0));

    Vector<double> r_icrs(3,0.0), v_icrs(3,0.0);
    Vector<double> a_thd(3,0.0);

    cout << fixed;

    string line;
    while(!fin.eof() && fin.good())
    {
        getline(fin,line);

        if( fin.eof() ) break;

        double t = asDouble( line.substr( 0,10) );
        CommonTime gps = gps0 + t;
        CommonTime utc = refSys.GPS2UTC(gps);

        r_icrs(0) = asDouble( line.substr(10,15) );
        r_icrs(1) = asDouble( line.substr(25,15) );
        r_icrs(2) = asDouble( line.substr(40,15) );
        v_icrs(0) = asDouble( line.substr(55,15) );
        v_icrs(1) = asDouble( line.substr(70,15) );
        v_icrs(2) = asDouble( line.substr(85,15) );

        sc.setCurrentTime(utc);
        sc.setCurrentPos(r_icrs);
        sc.setCurrentVel(v_icrs);

        thd.doCompute(utc, eb, sc);

        a_thd = thd.getAcceleration();
    }

    fin.close();

    return 0;
}

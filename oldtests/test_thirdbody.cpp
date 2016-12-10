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

    // Initial Time
    int year = confReader.getValueAsInt("YEAR", "DEFAULT");
    int mon  = confReader.getValueAsInt("MON", "DEFAULT");
    int day  = confReader.getValueAsInt("DAY", "DEFAULT");
    int hour = confReader.getValueAsInt("HOUR", "DEFAULT");
    int min  = confReader.getValueAsInt("MIN", "DEFAULT");
    double sec = confReader.getValueAsDouble("SEC", "DEFAULT");

    CivilTime CT(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime GPS0( CT.convertToCommonTime() );
    CommonTime UTC0( refSys.GPS2UTC(GPS0) );

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
    ThirdBody third;
    third.setSolarSystem(solSys);
    third.setReferenceSystem(refSys);
//    third.enableSun();
    third.enableMoon();

    // Length
    double length;

    try
    {
        length = confReader.getValueAsDouble("ARCLENGTH", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Time Length Error." << endl;

        return 1;
    }

    int i = 0;

    while(true)
    {
        // Current Time
        CommonTime GPS( GPS0 + i*900.0 );
        CommonTime UTC( refSys.GPS2UTC(GPS) );

        // Current Position and Velocity in ITRS
        Vector<double> r_itrs, v_itrs;

        try
        {
            r_itrs = sp3Eph.getXvt(sat, GPS).x.toVector();
            v_itrs = sp3Eph.getXvt(sat, GPS).v.toVector();
        }
        catch(...)
        {
            cerr << "Get Position and Velocity from SP3 File Error." << endl;

            return 1;
        }

        // Current Transform Matrix
        Matrix<double> c2t ( refSys.C2TMatrix(UTC) );
        Matrix<double> t2c ( transpose(c2t) );

        // Current Transform Matrix Time Dot
        Matrix<double> dc2t( refSys.dC2TMatrix(UTC) );
        Matrix<double> dt2c( transpose(dc2t) );

        // Currrent Position and Velocity in ICRS
        Vector<double> r_icrs = t2c * r_itrs;
        Vector<double> v_icrs = t2c * v_itrs + dt2c * r_itrs;

        // Current Spacecraft
        sc.setCurrentPos(r_icrs);
        sc.setCurrentVel(v_icrs);
        sc.setCurrentTime(UTC);

        // Current Acceleration
        third.doCompute(UTC, eb, sc);

        Vector<double> a_third = third.getAcceleration();
        Matrix<double> p_third = third.dA_dR();

//        cout << CivilTime(GPS);
//        cout << setw(20) << a_third(0)
//             << setw(20) << a_third(1)
//             << setw(20) << a_third(2)
//             << endl;

        JulianDate JD(GPS);
        long int IJD(JD.jd + 0.5);
        double FJD( (JD.jd+0.5-IJD)*DAY_TO_SEC );

        cout << fixed;

        cout << setprecision(3);
        cout << setw(12) << IJD
             << setw(12) << FJD;

        cout << setprecision(20);
        cout << setw(30) << p_third(0,0)
             << setw(30) << p_third(0,1)
             << setw(30) << p_third(0,2)
             << setw(30) << p_third(1,0)
             << setw(30) << p_third(1,1)
             << setw(30) << p_third(1,2)
             << setw(30) << p_third(2,0)
             << setw(30) << p_third(2,1)
             << setw(30) << p_third(2,2)
             << endl;

        i++;

        if(i >= length*3600/900) break;
    }

    return 0;
}

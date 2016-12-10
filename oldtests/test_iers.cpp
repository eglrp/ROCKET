/*
 * Test for the transformation between ITRS and ICRS.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{
    // Configuration file
    ConfDataReader confReader;
    try
    {
        confReader.open("../../ROCKET/oldtests/test.conf");
    }
    catch(...)
    {
        cerr << "Configuration file open error." << endl;

        return 1;
    }

    // EOP Data Store
    EOPDataStore2 eopDataStore;
//    eopDataStore.setUseBulletinB(true);

    // IERS EOP file
    string eopFile = confReader.getValue("STKEOPFILE", "DEFAULT");
    try
    {
        eopDataStore.loadSTKFile(eopFile);
    }
    catch(...)
    {
        cerr << "EOP File Load Error." << endl;

        return 1;
    }

    eopDataStore.setInterpPoints(8);
    eopDataStore.setRegularization(true);
    eopDataStore.setOceanTides(true);
    eopDataStore.setLibration(true);

    // Leap Sec Store
    LeapSecStore leapSecStore;

    // IERS LeapSecond file
    string lsFile  = confReader.getValue("IERSLSFILE", "DEFAULT");
    try
    {
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS Leap Second File Load Error." << endl;

        return 1;
    }


    // Reference System
    ReferenceSystem rs;
    rs.setEOPDataStore(eopDataStore);
    rs.setLeapSecStore(leapSecStore);

    // GPS
    int year, mon, day, hour, min;
    double sec;

    try
    {
        year = confReader.getValueAsInt("YEAR", "DEFAULT");
        mon  = confReader.getValueAsInt("MON", "DEFAULT");
        day  = confReader.getValueAsInt("DAY", "DEFAULT");
        hour = confReader.getValueAsInt("HOUR", "DEFAULT");
        min  = confReader.getValueAsInt("MIN", "DEFAULT");
        sec  = confReader.getValueAsDouble("SEC", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Initial Time Error." << endl;

        return 1;
    }

    CivilTime CT(2014,12,31,20,30,0.0, TimeSystem::GPS);
//    CommonTime UTC0( CT.convertToCommonTime() );
//    CommonTime UTC( UTC0 );
//    CommonTime UTC0( rs.GPS2UTC(GPS0) );

    CommonTime GPS0( CT.convertToCommonTime() );
    CommonTime UTC0( rs.GPS2UTC(GPS0) );

    cout << CivilTime(UTC0) << endl;

    // Loop
    for(int i=0; i<124; ++i)
    {
        CommonTime GPS = GPS0 + i*900.0;
        CommonTime UTC = rs.GPS2UTC(GPS);

        Matrix<double> T2C( rs.T2CMatrix(UTC) );

        JulianDate JD(UTC);
        long int IJD(JD.jd+0.5);
        double FJD( (JD.jd+0.5-IJD)*DAY_TO_SEC );
//        cout << CivilTime(UTC);

        cout << fixed << setprecision(3);
        cout << setw(10) << IJD;
        cout << setw(10) << FJD;

        cout << fixed << setprecision(10);

        cout << setw(15) << T2C(0,0) << " "
             << setw(15) << T2C(0,1) << " "
             << setw(15) << T2C(0,2) << " "
             << setw(15) << T2C(1,0) << " "
             << setw(15) << T2C(1,1) << " "
             << setw(15) << T2C(1,2) << " "
             << setw(15) << T2C(2,0) << " "
             << setw(15) << T2C(2,1) << " "
             << setw(15) << T2C(2,2) << endl;

        UTC.addDays(1);
    }

    return 0;
}

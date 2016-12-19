/*
 * Test for the EOP time series.
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
    eopDataStore.setUseBulletinB(false);

    // IERS EOP file
    string eopFile = confReader.getValue("IERSEOPFILE", "DEFAULT");
    try
    {
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "EOP File Load Error." << endl;

        return 1;
    }

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
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);


    // Time
    CivilTime ct0(2010,1,31,9,35,59.0, TimeSystem::UTC);
    CommonTime utc0( ct0.convertToCommonTime() );

    CommonTime utcx( utc0 );

    // Variables
    int np = 1;

    EOPDataStore2::EOPData eopx;

    Vector<double> xp(np,0.0),yp(np,0.0);
    Vector<double> UT1mUTC(np,0.0);
    Vector<double> dX(np,0.0),dY(np,0.0);


    eopDataStore.setInterpPoints(4);
    eopDataStore.setRegularization(true);
    eopDataStore.setOceanTides(true);
    eopDataStore.setLibration(true);

/*
    Vector<double> dpm = eopDataStore.PM_LIBR(utcx);
    Vector<double> dut = eopDataStore.UT_LIBR(utcx);
    Vector<double> dpmut = eopDataStore.RG_ZONT2(utcx);

    cout << setprecision(12);
    cout << dut << endl;
*/

    cout << fixed << setprecision(8);

    for(int i=0; i<np; ++i)
    {
        // Comparison of EOP time series

        eopx = eopDataStore.getEOPData(utcx);

        cout << setw(5) << YDSTime(utcx).doy;
//        cout << setw(5) << i;
        cout << setw(20) << eopx.xp
             << setw(20) << eopx.yp
             << setw(20) << eopx.UT1mUTC
             << setw(20) << eopx.dX
             << setw(20) << eopx.dY
             << endl;

        utcx.addDays(1);

    }

    return 0;
}

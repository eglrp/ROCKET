/*
 * Test for the transformation between ITRS and ICRS.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"

#include "SP3EphemerisStore.hpp"


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

    // IERS EOP file
    string eopFile = confReader.getValue("STKEOPFILE", "DEFAULT");
    try
    {
        eopDataStore.loadSTKFile(eopFile);
    }
    catch(...)
    {
        cerr << "STK Earth Orientation Parameters File Load Error." << endl;

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
    ReferenceSystem rs;
    rs.setEOPDataStore(eopDataStore);
    rs.setLeapSecStore(leapSecStore);

//    cout << "sizeof(eopDataStore): " << sizeof(eopDataStore) << endl;
//    cout << "sizeof(leapSecStore): " << sizeof(leapSecStore) << endl;
//    cout << "sizeof(rs): " << sizeof(rs) << endl;

    // IGS SP3 file
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    string sp3File;
    while( (sp3File = confReader.fetchListValue("IGSSP3LIST", "DEFAULT")) != "" )
    {
        try
        {
            sp3Eph.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "SP3 File Load Error." << endl;

            return 1;
        }
    }

    // GPS
//    CivilTime CT(2015,1,1,12,0,0.0, TimeSystem::GPS);
//    CommonTime GPS( CT.convertToCommonTime() );

//    cout << "GPS: " << CT << endl;

    // UTC
//    CommonTime UTC( rs.GPS2UTC(GPS) );
//    cout << "UTC: " << CivilTime(UTC) << endl;

    // TT
//    CommonTime TT( rs.UTC2TT(UTC) );

    CivilTime CT(2014,1,1,0,0,0.0, TimeSystem::UTC);
    CommonTime UTC( CT.convertToCommonTime() );

    CommonTime GPS( rs.UTC2GPS(UTC) );
    CommonTime TT ( rs.UTC2TT (UTC) );


    for(int i=0; i<=24*360; ++i)
    {
        CommonTime time = UTC + i*3600.0;

        Matrix<double> T2C( rs.T2CMatrix(time) );

        CivilTime temp(time);

        cout << setw(4) << temp.year << " "
             << setw(2) << temp.month << " "
             << setw(2) << temp.day << " "
             << setw(2) << temp.hour << " "
             << setw(2) << temp.minute << " "
             << setw(10) << temp.second << " ";

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
    }

/*
    // T2C matrix and its time dot
    Matrix<double> T2C( rs.T2CMatrix(UTC) );
    Matrix<double> dT2C( rs.dT2CMatrix(UTC) );

    cout << fixed << setprecision(10);
    cout << "C2T: " << endl
         << setw(18) << T2C(0,0) << " "
         << setw(18) << T2C(0,1) << " "
         << setw(18) << T2C(0,2) << endl
         << setw(18) << T2C(1,0) << " "
         << setw(18) << T2C(1,1) << " "
         << setw(18) << T2C(1,2) << endl
         << setw(18) << T2C(2,0) << " "
         << setw(18) << T2C(2,1) << " "
         << setw(18) << T2C(2,2) << endl;

    cout << "dC2T: " << endl
         << setw(18) << dT2C(0,0) << " "
         << setw(18) << dT2C(0,1) << " "
         << setw(18) << dT2C(0,2) << endl
         << setw(18) << dT2C(1,0) << " "
         << setw(18) << dT2C(1,1) << " "
         << setw(18) << dT2C(1,2) << endl
         << setw(18) << dT2C(2,0) << " "
         << setw(18) << dT2C(2,1) << " "
         << setw(18) << dT2C(2,2) << endl;


    // Satellite ID
    SatID sat(1, SatID::systemGPS);

    // Position and velocity in ITRS
    Vector<double> r_itrs(3,0.0), v_itrs(3,0.0);
    try
    {
        r_itrs = sp3Eph.getXvt(sat,GPS).x.toVector();
        v_itrs = sp3Eph.getXvt(sat,GPS).v.toVector();
    }
    catch(...)
    {
        cerr << "Get Position and Velocity from SP3 File Error." << endl;

        return 1;
    }

    // Position and velocity in ICRS
    Vector<double> r_icrs( T2C * r_itrs );
    Vector<double> v_icrs( T2C * v_itrs + dT2C * r_itrs);

    cout << fixed << setprecision(6);

    cout << "r_itrs: " << endl
         << setw(18) << r_itrs[0] << " "
         << setw(18) << r_itrs[1] << " "
         << setw(18) << r_itrs[2] << endl;

    cout << "v_itrs: " << endl
         << setw(18) << v_itrs[0] << " "
         << setw(18) << v_itrs[1] << " "
         << setw(18) << v_itrs[2] << endl;

    cout << "r_icrs: " << endl
         << setw(18) << r_icrs[0] << " "
         << setw(18) << r_icrs[1] << " "
         << setw(18) << r_icrs[2] << endl;

    cout << "v_icrs: " << endl
         << setw(18) << v_icrs[0] << " "
         << setw(18) << v_icrs[1] << " "
         << setw(18) << v_icrs[2] << endl;
*/
    return 0;
}

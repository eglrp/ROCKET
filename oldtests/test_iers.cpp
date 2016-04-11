/*
 * Test for the IERSConventions, mainly of the transformation between
 * ITRS and ICRS.
 */

#include <iostream>
#include "IERSConventions.hpp"
#include "SP3EphemerisStore.hpp"

#include "SunPosition.hpp"
#include "MoonPosition.hpp"
#include "Epoch.hpp"

#include "GeodeticFrames.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    // global files
    try
    {
        LoadIERSERPFile("../tables/finals2000A.all");
    }
    catch(...)
    {
        cerr << "IERS Earth Orientation Parameters File Load Error." << endl;
    }
    
    try
    {
        LoadIERSLSFile("../tables/Leap_Second_History.dat");
    }
    catch(...)
    {
        cerr << "IERS Leap Second File Load Error." << endl;
    }

    try
    {
        LoadJPLEphFile("../tables/JPLEPH2000");
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File Load Error." << endl;
    }

    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    try
    {
        sp3Eph.loadFile("../workplace/igs18253.sp3");
        sp3Eph.loadFile("../workplace/igs18254.sp3");
        sp3Eph.loadFile("../workplace/igs18255.sp3");
    }
    catch(...)
    {
        cerr << "SP3 File Load Error." << endl;
    }

    CivilTime ct(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime GPS( ct.convertToCommonTime() );
    CommonTime UTC( GPS2UTC(GPS) );

    cout << fixed << setprecision(4);
    cout << ct << endl;
    // UTC
//    cout << "UTC: " << UTC << endl;
//    cout << "MJD_UTC: " << MJD(UTC).mjd << endl;

//    CommonTime TAI( UTC2TAI(UTC) );
//    cout << "TAI: " << TAI << endl;
//    cout << "MJD_TAI: " << MJD(TAI).mjd << endl;

    // TT
    CommonTime TT( UTC2TT(UTC) );
//    cout << "TT:  " << TT << endl;
//    cout << "MJD_TT: " << MJD(TT).mjd << endl;

    // UT1
//    CommonTime UT1( UTC2UT1(UTC) );
//    cout << "UT1: " << UT1 << endl;
//    cout << "MJD_UT1: " << MJD(UT1).mjd << endl;

    Matrix<double> C2T( C2TMatrix(UTC) );
//    cout << "C2T: " << endl << C2T << endl;
    Matrix<double> dC2T( dC2TMatrix(UTC) );
//    cout << "dC2T: " << endl << dC2T << endl;


    SatID sat(1, SatID::systemGPS);

    Vector<double> r_ecef(3,0.0), v_ecef(3,0.0);

    try
    {
        r_ecef = sp3Eph.getXvt(sat,GPS).x.toVector();
        v_ecef = sp3Eph.getXvt(sat,GPS).v.toVector();
    }
    catch(...)
    {
        cerr << "Get ECEF Position and Velocity from SP3 File Error." << endl;
    }

    TT = CivilTime(2000,1,1,0,0,0.0, TimeSystem::TT);
    Vector<double> r_eci( transpose(C2T)*r_ecef );
    Vector<double> v_eci( transpose(C2T)*v_ecef + transpose(dC2T)*r_ecef);
    cout << "r_eci: " << endl << r_eci << endl;
    cout << "v_eci: " << endl << v_eci << endl;

    // Analytical Position
    Vector<double> r_sun( SunJ2kPosition(TT) );
    Vector<double> r_moon( MoonJ2kPosition(TT) );
    cout << "r_sun: " << r_sun << endl;
    cout << "r_moon: " << r_moon << endl;


    return 0;
}

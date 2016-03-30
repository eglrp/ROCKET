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
    LoadIERSERPFile("finals2000A.all");
    LoadIERSLSFile("Leap_Second_History.dat");
    LoadJPLEphFile("JPLEPH2000");

    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    try {
    
        sp3Eph.loadFile("igs18253.sp3");
        sp3Eph.loadFile("igs18254.sp3");
        sp3Eph.loadFile("igs18255.sp3");

    } catch (...) {

        cerr << "sp3 file load error." << endl;
    }

    CivilTime cv(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime GPS( cv.convertToCommonTime() );
    CommonTime UTC( GPS2UTC(GPS) );

    cout << fixed << setprecision(15);
    // UTC
//    cout << "UTC: " << UTC << endl;
//    cout << "MJD_UTC: " << MJD(UTC).mjd << endl;

//    CommonTime TAI( UTC2TAI(UTC) );
//    cout << "TAI: " << TAI << endl;
//    cout << "MJD_TAI: " << MJD(TAI).mjd << endl;

    // TT
//    CommonTime TT( UTC2TT(UTC) );
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

    Vector<double> r_ecef(3,0.0);
    r_ecef = sp3Eph.getXvt(sat,GPS).x.toVector();
    Vector<double> v_ecef(3,0.0);
    v_ecef = sp3Eph.getXvt(sat,GPS).v.toVector();

    Vector<double> r_eci( transpose(C2T)*r_ecef );
    Vector<double> v_eci( transpose(C2T)*v_ecef + transpose(dC2T)*r_ecef);

    cout << "r_eci: " << endl << r_eci << endl;
    cout << "v_eci: " << endl << v_eci << endl;


//    TT = YDSTime(2000,1,0.0, TimeSystem::TT);
//    TT = CivilTime(2000,1,1,0,0,0.0, TimeSystem::TT);
/*
    cout << fixed << setprecision(0);

    for(int i=0; i<365*1; i++)
    {
        cout << i << ' ';               // 1
        // JPL Position
        Vector<double> r_sun( J2kPosition(TT, SolarSystem::Sun) );
        Vector<double> r_moon( J2kPosition(TT, SolarSystem::Moon) );
        cout << r_sun << ' ';           // 2,3,4
        cout << r_moon << ' ';          // 5,6,7

        // Analytical Position 1
        Vector<double> r_sun2( SunJ2kPosition(TT) );
        Vector<double> r_moon2( MoonJ2kPosition(TT) );
        cout << r_sun2 << ' ';          // 8,9,10
        cout << r_moon2 << ' ';         // 11,12,13

        // Analytical Position 2
        SunPosition sun;
        Position r_sun3( sun.getPositionCIS(TT) );
        cout << r_sun3.X() << ' '       // 14
             << r_sun3.Y() << ' '       // 15
             << r_sun3.Z() << ' ';      // 16

        MoonPosition moon;
        Position r_moon3( moon.getPositionCIS(TT) );
        cout << r_moon3.X() << ' '      // 17
             << r_moon3.Y() << ' '      // 18
             << r_moon3.Z() << endl;    // 19

        TT.addDays(1.0);

//        break;
   }
*/

    return 0;
}

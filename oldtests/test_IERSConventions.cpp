#include <iostream>
#include "IERSConventions.hpp"
#include "SunPosition.hpp"
#include "MoonPosition.hpp"
#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    LoadIERSERPFile("finals.data");
    LoadIERSLSFile("Leap_Second_History.dat");
    LoadJPLEphFile("JPLEPH2000");

    CivilTime cv(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime GPS( cv.convertToCommonTime() );
    CommonTime UTC( GPS2UTC(GPS) );

/*
    cout << fixed << setprecision(6);
    cout << "UT1-UTC (s):   " << UT1mUTC(UTC) << endl;
    cout << "X Pole (as):   " << PolarMotionX(UTC) << endl;
    cout << "Y Pole (as):   " << PolarMotionY(UTC) << endl;
    cout << "dPsi (as):     " << NutationDPsi(UTC) << endl;
    cout << "dEps (as):     " << NutationDEps(UTC) << endl;
*/

    Matrix<double> C2T( C2TMatrix(UTC) );
    Matrix<double> dC2T( dC2TMatrix(UTC) );
/*
    Vector<double> r_eci(3,0.0);
    r_eci[0] =  17253546.070962253;
    r_eci[1] = -19971157.156486035;
    r_eci[2] =  3645801.3286026035;

    Vector<double> v_eci(3,0.0);
    v_eci[0] =  1973.2092921706201;
    v_eci[1] =  1123.3113892016910;
    v_eci[2] = -3124.1454542639446;

    Vector<double> r_ecef( C2T*r_eci );
    Vector<double> v_ecef( C2T*v_eci + dC2T*r_eci);

    cout << fixed << setprecision(3);
    cout << "r_ecef: " << endl << r_ecef << endl;
    cout << "v_ecef: " << endl << v_ecef << endl;
*/

    cout << fixed << setprecision(3);

    Vector<double> r_ecef(3,0.0);
    r_ecef[0] = -22815430.735;
    r_ecef[1] = -13068825.221;
    r_ecef[2] =   4288645.688;
    cout << "norm(r): " << norm(r_ecef) << endl;

    Vector<double> v_ecef(3,0.0);
    v_ecef[0] = - 287.108;
    v_ecef[1] = - 535.199;
    v_ecef[2] = -3104.660;
    cout << "norm(v): " << norm(v_ecef) << endl;

    Vector<double> r_eci( transpose(C2T)*r_ecef );
    Vector<double> v_eci( transpose(C2T)*v_ecef+transpose(dC2T)*r_ecef );

    cout << "r_eci: " << r_eci << endl;
    cout << "v_eci: " << v_eci << endl;

/*
    CivilTime cv(1999,3,4,0,0,0.0, TimeSystem::GPS);
    CommonTime GPS( cv.convertToCommonTime() );
    CommonTime UTC( GPS2UTC(GPS) );

    cout << fixed << setprecision(15);
    // UTC
//    cout << "UTC: " << UTC << endl;
//    cout << "MJD_UTC: " << MJD(UTC).mjd << endl;

    CommonTime TAI( UTC2TAI(UTC) );
//    cout << "TAI: " << TAI << endl;
//    cout << "MJD_TAI: " << MJD(TAI).mjd << endl;

    // TT
    CommonTime TT( UTC2TT(UTC) );
//    cout << "TT:  " << TT << endl;
//    cout << "MJD_TT: " << MJD(TT).mjd << endl;

    // UT1
    CommonTime UT1( UTC2UT1(UTC) );
//    cout << "UT1: " << UT1 << endl;
//    cout << "MJD_UT1: " << MJD(UT1).mjd << endl;

    double epsa = MeanObliquity80(TT);
//    cout << "epsa: " << epsa << endl;

    Matrix<double> P( PreMatrix76(TT) );
//    cout << "P: " << endl << P << endl;

    double dPsi, dEps;
    NutAngles80(TT, dPsi, dEps);
//    cout << "dPsi: " << dPsi << ' ' << "dEps: " << dEps << endl;

    Matrix<double> N( NutMatrix80(TT) );
//    cout << "N: " << endl << N << endl;

    double gmst( GMST82(UT1) );
    Matrix<double> R( GHAMatrix(UT1) );
//    cout << "R: " << endl << R << endl;

    Matrix<double> W( PoleMatrix(UTC) );
//    cout << "W: " << endl << W << endl;

    Matrix<double> C2T( C2TMatrix(UTC) );
    cout << "C2T: " << endl << C2T << endl;
    Matrix<double> dC2T( dC2TMatrix(UTC) );
    cout << "dC2T: " << endl << dC2T << endl;

    Vector<double> r_ecef(3,0.0);
    r_ecef[0] = 19440953.805;
    r_ecef[1] = 16881609.273;
    r_ecef[2] = -6777115.092;

    Vector<double> v_ecef(3,0.0);
    v_ecef[0] = -811.1827456;
    v_ecef[1] = -257.3799137;
    v_ecef[2] = -3068.9508125;

    Vector<double> r_eci( transpose(C2T)*r_ecef );
    Vector<double> v_eci( transpose(C2T)*v_ecef + transpose(dC2T)*r_ecef);

    cout << "r_eci: " << endl << r_eci << endl;
    cout << "v_eci: " << endl << v_eci << endl;
*/

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

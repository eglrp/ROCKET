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
    LoadIERSERPFile("../tables/finals2000A.all");
    LoadIERSLSFile("../tables/Leap_Second_History.dat");
    LoadJPLEphFile("../tables/JPLEPH2000");

    // sp3 files
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    try {
    
        sp3Eph.loadFile("../workplace/igs18253.sp3");
        sp3Eph.loadFile("../workplace/igs18254.sp3");
        sp3Eph.loadFile("../workplace/igs18255.sp3");

    } catch (...) {

        cerr << "sp3 file load error." << endl;
    }

    // initial epoch
    CivilTime cv(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime GPS( cv.convertToCommonTime() );
    CommonTime UTC( GPS2UTC(GPS) );

    cout << fixed << setprecision(6);

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

    // matrix from ICRS to ITRS
    Matrix<double> C2T( C2TMatrix(UTC) );
//    cout << "C2T: " << endl << C2T << endl;

    // time dot of matrix from ICRS to ITRS
    Matrix<double> dC2T( dC2TMatrix(UTC) );
//    cout << "dC2T: " << endl << dC2T << endl;

    // sat
    SatID sat(1, SatID::systemGPS);

    // get ECEF position and velocity from sp3 files
    Vector<double> r_ecef(3,0.0);
    r_ecef = sp3Eph.getXvt(sat,GPS).x.toVector();
    Vector<double> v_ecef(3,0.0);
    v_ecef = sp3Eph.getXvt(sat,GPS).v.toVector();

    // transform ECEF position and velocity to ECI
    Vector<double> r_eci( transpose(C2T)*r_ecef );
    Vector<double> v_eci( transpose(C2T)*v_ecef + transpose(dC2T)*r_ecef);

    cout << "r_ecef: " << endl << r_ecef << endl;
    cout << "r_eci: " << endl << r_eci << endl;

    cout << "v_ecef: " << endl << v_ecef << endl;
    cout << "v_eci: " << endl << v_eci << endl;

    return 0;
}

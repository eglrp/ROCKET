/*
 * Test for the transformation between ITRS and ICRS.
 *
 * Test time: 2015/01/01,12h, 0m, 0.0s, UTC
 *
 * Matrix from http://hpiers.obspm.fr/eop-pc/index.php?index=matrice&lang=en#description
 *
 *    0.184438630509  0.982842935794  0.001468038215
 *   -0.982843980602  0.184438898953 -0.000048456351
 *   -0.000318388334 -0.001433915300  0.999998921257
 *
 * Matrix from finals2000A.all
 *
 *    0.184438630470  0.982842935802  0.001468037600
 *   -0.982843980609  0.184438898913 -0.000048455779
 *   -0.000318387659 -0.001433914801  0.999998921258
 *
 * Matrix difference
 *
 *   -3.9e-11   0.8e-11  -6.2e-10
 *   -0.7e-11  -4.0e-11   5.7e-10
 *    6.8e-10   5.0e-10   0.1e-11
 *
 * Position & Velocity in ICRS
 *
 *    17136494.765728, -20036699.180210,  3829990.621173
 *    1994.595123,      1098.432731,     -3119.495350
 *
 * Matrix from eopc04_08_IAU2000.62-now
 *
 *    0.184438630447  0.982842935806  0.001468037936
 *   -0.982843980613  0.184438898891 -0.000048456120
 *   -0.000318388056 -0.001433915068  0.999998921258
 *
 * Matrix difference
 *
 *   -6.2e-11   1.2e-11  -2.8e-10
 *   -1.1e-11   6.2e-11   2.3e-10
 *    2.8e-10   2.3e-10   0.1e-11
 *
 * Position & Velocity in ICRS
 *
 *    17136494.766552, -20036699.181912,  3829990.608585
 *    1994.595122,      1098.432732,     -3119.495350
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "IERSConventions.hpp"

#include "SP3EphemerisStore.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    // Configuration file
    ConfDataReader confReader;
    try
    {
        confReader.open("../../rocket/oldtests/testconf.txt");
    }
    catch(...)
    {
        cerr << "Configuration file open error." << endl;

        return 1;
    }

    
    // IERS EOP file
    string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");
    try
    {
        LoadIERSEOPFile(eopFile);
    }
    catch(...)
    {
        cerr << "IERS Earth Orientation Parameters File Load Error." << endl;

        return 1;
    }
    
    // IERS LeapSecond file
    string lsFile  = confReader.getValue("IERSLSFile", "DEFAULT");
    try
    {
        LoadIERSLSFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS Leap Second File Load Error." << endl;

        return 1;
    }

    // IGS SP3 file
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    string sp3File;
    while( (sp3File = confReader.fetchListValue("IGSSP3List", "DEFAULT")) != "" )
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

    // Time
    CivilTime ct(2015,1,1,12,0,0.0, TimeSystem::UTC);
    CommonTime UTC( ct.convertToCommonTime() );
    CommonTime GPS( UTC2GPS(UTC) );

    cout << fixed << setprecision(4);
    cout << ct << endl;

    // Transform matrix
    Matrix<double> C2T( C2TMatrix(UTC) );
    cout << fixed << setprecision(12);
    cout << "C2T: " << endl << C2T << endl;
    // Transform matrix time dot
    Matrix<double> dC2T( dC2TMatrix(UTC) );
    cout << fixed << setprecision(12);
    cout << "dC2T: " << endl << dC2T << endl;

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
    Vector<double> r_icrs( transpose(C2T)*r_itrs );
    Vector<double> v_icrs( transpose(C2T)*v_itrs + transpose(dC2T)*r_itrs);

    // Output
    cout << fixed << setprecision(6);
    cout << "r_itrs: " << endl;
    cout << setw(12) << r_itrs << endl;
    cout << "v_itrs: " << endl;
    cout << setw(12) << v_itrs << endl;
    cout << "r_icrs: " << endl;
    cout << setw(12) << r_icrs << endl;
    cout << "v_icrs: " << endl;
    cout << setw(12) << v_icrs << endl;

    return 0;
}

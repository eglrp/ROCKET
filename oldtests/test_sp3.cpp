#include <iostream>
#include <iomanip>

#include "SP3EphemerisStore.hpp"
#include "DataStructures.hpp"
#include "IERSConventions.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    // sp3 files
	SP3EphemerisStore sp3Eph;
   sp3Eph.rejectBadPositions(true);
	sp3Eph.setPosGapInterval(901);
	sp3Eph.setPosMaxInterval(8101);

	try
	{
        sp3Eph.loadFile("../../rocket/workplace/igs18253.sp3");
        sp3Eph.loadFile("../../rocket/workplace/igs18254.sp3");
        sp3Eph.loadFile("../../rocket/workplace/igs18255.sp3");
	}
	catch(...)
	{
      cerr << "sp3 file load error." << endl;
      return 1;
	}

    try
    {
        LoadIERSEOPFile("../../rocket/tables/finals2000A.all");
    }
    catch(...)
    {
        cerr << "eop file load error." << endl;
        return 1;
    }

    try
    {
       LoadIERSLSFile("../../rocket/tables/Leap_Second_History.dat");
    }
    catch(...)
    {
       cerr << "ls file load error." << endl;
       return 1;
    }

    CivilTime ct0(2015,1,1,0,0,0.0, TimeSystem::GPS);
    CommonTime gps0( ct0.convertToCommonTime() );

    SatID sat(1,SatID::systemGPS);

    ofstream outfile("sp3orbit.txt");

    gps0 += 22.0;
    Vector<double> ecefPos = sp3Eph.getXvt(sat, gps0).x.toVector();

    cout << fixed;
    cout << ecefPos << endl;

    double dt = 5.0;

    for(int i=0; i<=60/int(dt); i++)
    {
        CommonTime utc( GPS2UTC(gps0) );
        Matrix<double> c2t(C2TMatrix(utc));
        Matrix<double> dc2t(dC2TMatrix(utc));

        Vector<double> sp3Pos(3,0.0), sp3Vel(3,0.0);
        Vector<double> eciPos(3,0.0), eciVel(3,0.0);

        try
        {
            sp3Pos = sp3Eph.getXvt(sat, gps0).x.toVector();
            sp3Vel = sp3Eph.getXvt(sat, gps0).v.toVector();

            eciPos = transpose(c2t) * sp3Pos;
            eciVel = transpose(c2t) * sp3Vel + transpose(dc2t) * sp3Pos;

//            if(yds.sod>=3600 && yds.sod<=7200)
//            {
                outfile << fixed << setprecision(3);
                outfile << setw(9)  << gps0.getSecondOfDay()
                        << setw(12) << eciPos
//                        << setw(12) << eciVel
                        << endl;
//            }
        }
        catch(...)
        {
            break;
//            continue;
        }

        gps0 += dt;
    }

    outfile.close();

	return 0;
}

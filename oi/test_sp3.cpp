#include <iostream>
#include <iomanip>

#include "SP3EphemerisStore.hpp"
#include "DataStructures.hpp"
#include "IERSConventions.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{

	SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.rejectBadClocks(true);
	sp3Eph.setPosGapInterval(901);
	sp3Eph.setPosMaxInterval(8101);

	try
	{
/*
		sp3Eph.loadFile("igs17732.sp3");
		sp3Eph.loadFile("igs17733.sp3");
        sp3Eph.loadFile("igs17734.sp3");
*/        
        sp3Eph.loadFile("igs18253.sp3");
        sp3Eph.loadFile("igs18254.sp3");
        sp3Eph.loadFile("igs18255.sp3");

//        cout << "sp3 file load over." << endl;
	}
	catch(...)
	{
		cerr << "sp3 file load error." << endl;
	}

    try
    {
        LoadIERSERPFile("finals.data");
//        cout << "eop file load over." << endl;
    }
    catch(...)
    {
        cerr << "eop file load error." << endl;
    }

//    CivilTime ct0(2014,1,1,0,0,0.0, TimeSystem::GPS);
    CivilTime ct0(2015,1,1,0,0,0.0, TimeSystem::GPS);
    YDSTime yds0( ct0.convertToCommonTime() );

	SatID sat(1,SatID::systemGPS);

    ofstream outfile("sp3orbit.txt");

    double dt = 75.0;

    for(int i=0; i<=3600/int(dt)*12; i++)
    {
        YDSTime tmp(yds0);
        int day = int(i*dt+tmp.sod) / 86400;
        double sec = i*dt - 86400.0*day;
        tmp.doy = tmp.doy + day;
        tmp.sod = tmp.sod + sec;
        cout << tmp << endl;

        CommonTime gps( tmp.convertToCommonTime() );

//        gRin.header.epoch = ct;

        Vector<double> sp3Pos(3,0.0), sp3Vel(3,0.0), ecefPosVel(6,0.0);

        try
        {
            sp3Pos = sp3Eph.getXvt(sat, gps).x.toVector();
            sp3Vel = sp3Eph.getXvt(sat, gps).v.toVector();

            ecefPosVel[0] = sp3Pos[0]; ecefPosVel[1] = sp3Pos[1]; ecefPosVel[2] = sp3Pos[2];
            ecefPosVel[3] = sp3Vel[0]; ecefPosVel[4] = sp3Vel[1]; ecefPosVel[5] = sp3Vel[2];

//            if(yds.sod>=3600 && yds.sod<=7200)
//            {
                outfile << fixed << setprecision(3);
                outfile << setw(9)  << i*dt+yds0.sod
                        << setw(12) << sp3Pos
                        << setw(12)  << sp3Vel
                        << endl;
//            }
        }
        catch(...)
        {
            cerr << tmp.doy << ' ' << tmp.sod << endl;
            break;
//            continue;
        }
    }

    outfile.close();

	return 0;
}

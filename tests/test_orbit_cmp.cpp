#include <iostream>

#include "SP3EphemerisStore.hpp"
#include "ReferenceSystem.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

int main(void)
{
    // IGU Orbit
    SP3EphemerisStore IGU;
    IGU.rejectBadPositions(true);
    IGU.setPosGapInterval(900+1);
    IGU.setPosMaxInterval(9*900+1);

    IGU.loadFile("../../ROCKET/workplace/sp3/igu1");

    // IGS Orbit
    SP3EphemerisStore IGS;
    IGS.rejectBadPositions(true);
    IGS.setPosGapInterval(900+1);
    IGS.setPosMaxInterval(9*900+1);

    IGS.loadFile("../../ROCKET/workplace/sp3/igs18254.sp3");
    IGS.loadFile("../../ROCKET/workplace/sp3/igs18255.sp3");
    IGS.loadFile("../../ROCKET/workplace/sp3/igs18256.sp3");

    // Reference System
    ReferenceSystem refSys;


    CivilTime ct0(2015,1,2,0,0,0.0, TimeSystem::GPS);
    CommonTime gps0( ct0.convertToCommonTime() );

    Vector<double> r_igu(3,0.0);
    Vector<double> r_igs(3,0.0), v_igs(3,0.0);

    int mode(1);


    for(int prn=1; prn<=32; ++prn)
    {
        SatID sat(prn, SatID::systemGPS);

        string outFile( asString(sat.id) );
        outFile = rightJustify(outFile, 2, '0');
        outFile = string("./oi/") + string("G") + outFile + string(".IGU");

        cout << outFile << endl;

        ofstream fout;
        fout.open(outFile.c_str());

        fout << fixed;

        for(int i=0; i<=96; ++i)
        {
            CommonTime gps(gps0 + i*900.0);

            try
            {
                r_igu = IGU.getXvt(sat,gps).x.toVector();

            }
            catch(...)
            {
                cerr << "Get IGU Orbit for "
                     << CivilTime(gps) << " Error."
                     << endl;
                continue;
            }

            try
            {
                r_igs = IGS.getXvt(sat,gps).x.toVector();
                v_igs = IGS.getXvt(sat,gps).v.toVector();
            }
            catch(...)
            {
                cerr << "Get IGS Orbit for "
                     << CivilTime(gps) << " Error."
                     << endl;
                continue;
            }

            Vector<double> dXYZ(r_igs - r_igu);
            Vector<double> dRTN( refSys.XYZ2RTN(dXYZ,r_igs,v_igs) );

            fout << setprecision(0);
            fout << setw( 5) << YDSTime(gps).doy
                 << setw(10) << YDSTime(gps).sod;

            fout << setprecision(3);

            if(mode)
                fout << setw(10) << dRTN(0)
                     << setw(10) << dRTN(1)
                     << setw(10) << dRTN(2)
                     << setw(10) << norm(dRTN)
                     << endl;
            else
                fout << setw(10) << dXYZ(0)
                     << setw(10) << dXYZ(1)
                     << setw(10) << dXYZ(2)
                     << setw(10) << norm(dXYZ)
                     << endl;
        }

        fout.close();
    }

    return 0;
}

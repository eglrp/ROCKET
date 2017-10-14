
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

#include "Rinex3EphemerisStore2.hpp"

#include "Epoch.hpp"

using namespace std;
using namespace gpstk;


int main(int argc, const char *argv[])
{

    Rinex3EphemerisStore2 ephStore;

    try
    {
        ephStore.loadFile(argv[1]);
    }
    catch(...)
    {
        cerr << argv[1] << " load error." << endl;
        exit(-1);
    }


    CivilTime T0(2016,1,8,0,0,0.0, TimeSystem::GPS);
    double GALmGPS(0.0);
    CommonTime T0_GPS(T0);

    Vector<double> pos, vel;
    double clkbias, clkdrift;
    double tgd, tgda, tgdb;
    SatID sat;

    cout << fixed << setprecision(3);

    for(int i=0; i<1; i++)
    {
        CommonTime T_GPS = T0_GPS + i*900.0;

        GALmGPS = TimeSystem::Correction(TimeSystem::GPS,TimeSystem::GAL, 2016,1,8);

        CommonTime T_GAL( T_GPS + GALmGPS );
        T_GAL.setTimeSystem(TimeSystem::GAL);

        cout << CivilTime(T_GPS) << endl;

        // GPS
        for(int i=1; i<=32; i++)
        {
            sat.id = i;
            sat.system = SatID::systemGPS;

            try
            {
                Xvt pvt = ephStore.getXvt(sat,T_GPS);
                pos = pvt.x.toVector();
                clkbias = pvt.clkbias;
                clkdrift = pvt.clkdrift;
                tgd = ephStore.getGPSEphemerisStore().findEphemeris(sat,T_GPS).Tgd;

                cout << sat;

                cout << setw(20) << pos(0)
                     << setw(20) << pos(1)
                     << setw(20) << pos(2)
                     << setw(20) << tgd*1e9
                     << endl;
            }
            catch(...)
            {
                continue;
            }
        }


        // Galileo
        for(int i=1; i<=32; i++)
        {
            sat.id = i;
            sat.system = SatID::systemGalileo;

            try
            {
                Xvt pvt = ephStore.getXvt(sat,T_GPS);
                pos = pvt.x.toVector();
                clkbias = pvt.clkbias;
                clkdrift = pvt.clkdrift;
                tgda = ephStore.getGalEphemerisStore().findEphemeris(sat,T_GAL).Tgda;

                cout << sat;

                cout << setw(20) << pos(0)
                     << setw(20) << pos(1)
                     << setw(20) << pos(2)
                     << setw(20) << tgda*1e9
                     << endl;
            }
            catch(...)
            {
                continue;
            }
        }
    }

    return 0;
}

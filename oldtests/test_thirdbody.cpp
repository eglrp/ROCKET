#include <iostream>

#include "SunGravitation.hpp"
#include "MoonGravitation.hpp"
#include "SatDataReader.hpp"
#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    EOPDataStore eopDataStore;
    eopDataStore.loadIERSFile("../../ROCKET/tables/finals2000A.all");
    
    LeapSecStore leapSecStore;
    leapSecStore.loadFile("../../ROCKET/tables/Leap_Second_History.dat");

    ReferenceSystem refSys(eopDataStore,
                           leapSecStore);

    SolarSystem solSys;
    solSys.initializeWithBinaryFile("../../ROCKET/tables/1980_2040.405");

    CivilTime cv0(2015,1,1,12,1,15.0, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( refSys.GPS2UTC(gps0) );

    // G01, BLOCK IIF
    Vector<double> rv0(6,0.0);
    rv0(0) =  17253546.070962253;
    rv0(1) = -19971157.156486035;
    rv0(2) =  3645801.3286026035;
    rv0(3) =  1973.2092921706201;
    rv0(4) =  1123.3113892016910;
    rv0(5) = -3124.1454542639446;

    // Force model parameters
    Vector<double> p0;
    
    // Earth reference body
    EarthBody rb;

    // Satellite ID
    SatID sat(1, SatID::systemGPS);

    // Satellite data
    SatDataReader satReader;
    satReader.open("../../ROCKET/tables/SatInfo.txt");

    // Spacecraft settings
    Spacecraft sc;
    sc.setSatID(sat);
    sc.setEpoch(utc0);
    sc.setBlock(satReader.getBlock(sat,utc0));
    sc.setMass(satReader.getMass(sat,utc0));
    sc.initStateVector(rv0, p0);

    // Sun gravitation
    SunGravitation sun;
    sun.setSolarSystem(solSys);
    sun.setReferenceSystem(refSys);

    sun.doCompute(utc0, rb, sc);

    Vector<double> Gsun( sun.getAccel() );
    double Tsun( norm(Gsun) );

    cout << setprecision(15);

    cout << "Sun gravitation: " << endl
         << setw(16) << Gsun << ' '
         << setw(16) << Tsun << endl;

    // Moon gravitation
    MoonGravitation moon;
    moon.setSolarSystem(solSys);
    moon.setReferenceSystem(refSys);

    moon.doCompute(utc0, rb, sc);

    Vector<double> Gmoon( moon.getAccel() );
    double Tmoon( norm(Gmoon) );

    cout << "Moon gravitation: " << endl
         << setw(16) << Gmoon << ' '
         << setw(16) << Tmoon << endl;

    return 0;
}

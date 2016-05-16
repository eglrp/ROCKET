/* Test for Orbit Integration. */

#include <iostream>

#include "IERSConventions.hpp"

#include "SP3EphemerisStore.hpp"

#include "SatDataReader.hpp"

#include "EGM08GravityModel.hpp"

#include "SunForce.hpp"
#include "MoonForce.hpp"

#include "ROCKPressure.hpp"
#include "CODEPressure.hpp"

#include "RelativityEffect.hpp"


#include "SatOrbit.hpp"

#include "RungeKuttaFehlberg.hpp"

#include "SatOrbitPropagator.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{
    // IERS EOP file
    try
    {
        LoadIERSEOPFile("../../ROCKET/tables/finals2000A.all");
    }
    catch(...)
    {
        cerr << "IERS EOP File Load Error." << endl;

        return 1;
    }

    // IERS LeapSecond file
    try
    {
        LoadIERSLSFile("../../ROCKET/tables/Leap_Second_History.dat");
    }
    catch(...)
    {
        cerr << "IERS LeapSecond File Load Error." << endl;

        return 1;
    }

    // JPL Ephemeris file
    try
    {
        LoadJPLEphFile("../../ROCKET/tables/1980_2040.405");
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File Load Error." << endl;

        return 1;
    }


    // Initial time
    CivilTime cv0(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( GPS2UTC(gps0) );

    // Satellite ID
    SatID sat(1, SatID::systemGPS);

    // IGS SP3 file
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    try
    {
        sp3Eph.loadFile("../../ROCKET/workplace/sp3/igs18253.sp3");
        sp3Eph.loadFile("../../ROCKET/workplace/sp3/igs18254.sp3");
        sp3Eph.loadFile("../../ROCKET/workplace/sp3/igs18255.sp3");
    }
    catch(...)
    {
        cerr << "SP3 File Load Error." << endl;

        return 1;
    }

    // r0, v0 in ITRS
    Vector<double> r0_itrs, v0_itrs;
    try
    {
        r0_itrs = sp3Eph.getXvt(sat, gps0).x.toVector();
        v0_itrs = sp3Eph.getXvt(sat, gps0).v.toVector();
    }
    catch(...)
    {
        cerr << "Get r0 and v0 from SP3 file error." << endl;

        return 1;
    }

    // Transform matrix between ICRS and ITRS
    Matrix<double> c2t( C2TMatrix(utc0) );
    Matrix<double> dc2t( dC2TMatrix(utc0) );

    // r0, v0 in ICRS
    Vector<double> r0_icrs, v0_icrs;
    r0_icrs = transpose(c2t) * r0_itrs;
    v0_icrs = transpose(c2t) * v0_itrs + transpose(dc2t) * r0_itrs;


    // Initial state: r0, v0
    Vector<double> rv0(6,0.0);
    rv0(0) = r0_icrs(0); rv0(1) = r0_icrs(1); rv0(2) = r0_icrs(2);
    rv0(3) = v0_icrs(0); rv0(4) = v0_icrs(1); rv0(5) = v0_icrs(2);

    cout << fixed << setprecision(6);
    cout << rv0 << endl;

    // Intial state: p0
    Vector<double> p0(9,0.0);
    p0[0] = 0.999989506;
    p0[1] = -0.000367154;
    p0[2] = -0.003625165;
    p0[3] = 0.015272206;
    p0[4] = 0.000216184;
    p0[5] = -0.000701573;
    p0[6] = 0.000985358;
    p0[7] = 0.009091403;
    p0[8] = -0.001752761;

    // SatData file
    SatDataReader satData;
    try
    {
        satData.open("../../ROCKET/tables/SatInfo.txt");
    }
    catch(...)
    {
        cerr << "SatData file open error." << endl;

        return 1;
    }

    // Spacecraft
    Spacecraft sc;
    sc.setSatID(sat);
    sc.setEpoch(utc0);
    sc.setBlock(satData.getBlock(sat,utc0));
    sc.setMass(satData.getMass(sat,utc0));
    sc.initStateVector(rv0, p0);


    // Configurations
    SatOrbit so;
    so.setRefEpoch(utc0);
    so.setSpacecraft(sc);

    so.enableGeopotential(SatOrbit::GM_EGM08, 12, 12, true, false, false);
    so.enableThirdBodyPerturbation(true, true);
    so.enableSolarRadiationPressure(SatOrbit::SRPM_CODE, true);
    so.enableRelativeEffect(true);

    // State to be estimated: p0
    set<ForceModel::ForceModelType> fmt;
    fmt.insert(ForceModel::D0);
    fmt.insert(ForceModel::Y0);
    fmt.insert(ForceModel::B0);
    fmt.insert(ForceModel::Dc);
    fmt.insert(ForceModel::Ds);
    fmt.insert(ForceModel::Yc);
    fmt.insert(ForceModel::Ys);
    fmt.insert(ForceModel::Bc);
    fmt.insert(ForceModel::Bs);

    so.setForceModelType(fmt);

//    cout << so.getSpacecraft().P() << endl;

    // Integrator
    RungeKuttaFehlberg rkf78;
    rkf78.setStepSize(30.0);


    // Temp variables
    Vector<double> state;

    double t(0.0);
    double tt(12*3600.0);
    double step(30.0);

    // Out file
    ofstream outfile("myorbit.txt");

    int i=0;
/*
    while(t < tt)
    {
        state = rkf78.integrateTo(t, sc.getStateVector(), &so, t+step);
//        cout << sc.P() << endl;

//        Vector<double> rv(6,0.0);
//        rv[0] = state[0]; rv[1] = state[1]; rv[2] = state[2];
//        rv[3] = state[3]; rv[4] = state[4]; rv[5] = state[5];

//        Vector<double> p = p0;

//        cout << sc.getStateVector() << endl;
        sc.setStateVector(state);
//        cout << sc.P() << endl;
//        cout << sc.getStateVector() << endl;

//        Vector<double> myorbit(6,0.0);
//        myorbit[0] = sc.R()[0]; myorbit[1] = sc.R()[1]; myorbit[2] = sc.R()[2];
//        myorbit[3] = sc.V()[0]; myorbit[4] = sc.V()[1]; myorbit[5] = sc.V()[2];

        outfile << fixed << setprecision(3);
        outfile << setw(9) << t+step
                << setw(12) << sc.R()
                << setw(12) << sc.V()
//                << setw(12) << sc.dR_dR0()
//                << setw(12) << sc.dR_dV0()
//                << setw(12) << sc.dR_dP0()
                << endl;
//        outfile << setw(9) << t+step << endl       // #1 sod
//                << setw(12) << state    // #2 state
//                << setw(12) << sc.R() << endl      // #2 r
//                << setw(12) << sc.V() << endl      // #3 v
//                << setw(12) << sc.dR_dR0() << endl // #4 dr/dr0
//                << setw(12) << sc.dR_dV0() << endl // #5 dr/dv0
//                << setw(12) << sc.dR_dP0() << endl // #6 dr/dp0
//                << endl << endl;

        t += step;
        i++;

        if(i%60 == 0) cerr << ".";

//        if(i>1) break;

    }
*/
    cerr << endl;

    outfile.close();

    return 0;
}

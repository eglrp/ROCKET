#include <iostream>

#include "IERSConventions.hpp"

#include "EGM96GravityModel.hpp"
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

    LoadIERSERPFile("finals.data");
    LoadIERSLSFile("Leap_Second_History.dat");
    LoadJPLEphFile("JPLEPH2000");

    CivilTime cv0(2014,1,1,0,0,0.0, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( GPS2UTC(gps0) );

    // G01, BLOCK IIF
    Vector<double> rv0(6,0.0);
    rv0(0) = -20507847.810;
    rv0(1) = - 9658703.459;
    rv0(2) =  13899930.594;
    rv0(3) = - 957.429;
    rv0(4) = -1499.537;
    rv0(5) = -2437.047;


    // force mdoel parameters
    Vector<double> p0;

    // srp = 0, ROCK srp model
    // srp = 1, CODE srp model
    bool srp(0);

    cerr << "select SRP model (0 - ROCK model, 1 - CODE model): ";

    cin >> srp;

    if(srp)
    {
        // CODE srp model, include 9 parameters (D0, Y0, B0, Dc, Ds, Yc, Ys, Bc, Bs)
        p0.resize(9,0.0);
//        cerr << "input CODE SRP coeffients: " << endl;
//        cin >> p0[0] >> p0[1] >> p0[2]
//            >> p0[3] >> p0[4] >> p0[5]
//            >> p0[6] >> p0[7] >> p0[8];
        p0[0] = 0.999989506;
        p0[1] = -0.000367154;
        p0[2] = -0.003625165;
        p0[3] = 0.015272206;
        p0[4] = 0.000216184;
        p0[5] = -0.000701573;
        p0[6] = 0.000985358;
        p0[7] = 0.009091403;
        p0[8] = -0.001752761;

    }
    else
    {
        // ROCK srp model, include 3 parameters (Gx, Gy, Gz)
        p0.resize(3,0.0);
//        cerr << "input ROCK SRP coeffients: " << endl;
//        cin >> p0[0] >> p0[1] >> p0[2];
        p0[0] = -0.06890;
        p0[1] =  0.00352;
        p0[2] = -0.01866;
    }

    
    EarthBody rb;

    // Spacecraft settings
    Spacecraft sc;
    sc.initStateVector(rv0, p0);
    sc.setDryMass(1555.3);
    sc.setBlockNum(7);

/*
    // Earth gravitation
    EGM96GravityModel egm96(12,12);
    egm96.enableSolidTide(true);
    egm96.setOceanTideFile("OT_CSRC.TID");
    egm96.enableOceanTide(true);
    egm96.enablePoleTide(true);
    egm96.doCompute(utc0, rb, sc);

    Vector<double> Gearth( egm96.getAccel() );
    double Tearth( norm(Gearth) );

    cout << setprecision(12);
    cout << "Earth gravity: " << endl
         << setw(16) << Gearth << ' '
         << setw(16) << Tearth << endl;

    // Sun gravitation
    SunForce sun;
    sun.doCompute(utc0, rb, sc);

    Vector<double> Gsun( sun.getAccel() );
    double Tsun( norm(Gsun) );

    cout << "Sun gravity: " << endl
         << setw(16) << Gsun << ' '
         << setw(16) << Tsun << endl;

    // Moon gravitation
    MoonForce moon;
    moon.doCompute(utc0, rb, sc);

    Vector<double> Gmoon( moon.getAccel() );
    double Tmoon( norm(Gmoon) );

    cout << "Moon gravity: " << endl
         << setw(16) << Gmoon << ' '
         << setw(16) << Tmoon << endl;


    Vector<double> Fsrp(3,0.0);
    // Solar radiation pressure
    if(srp)     // CODE SRP model
    {
        CODEPressure code;
        code.doCompute(utc0, rb, sc);

        Fsrp = code.getAccel();
        double Tsrp( norm(Fsrp) );

        cout << "Solar radiation pressure: " << endl
             << setw(16) << Fsrp << ' '
             << setw(16) << Tsrp << endl;
    }
    else        // ROCK SRP model
    {
        ROCKPressure rock;
        rock.doCompute(utc0, rb, sc);

        Fsrp = rock.getAccel();
        double Tsrp( norm(Fsrp) );

        cout << "Solar radiation pressure: " << endl
             << setw(16) << Fsrp << ' '
             << setw(16) << Tsrp << endl;
    }

    // Relativity effect
    RelativityEffect rel;
    rel.doCompute(utc0, rb, sc);

    Vector<double> Frel( rel.getAccel() );
    double Trel( norm(Frel) );

    cout << "Relativity effect: " << endl
         << setw(16) << Frel << ' '
         << setw(16) << Trel << endl;


    cout << "Total force: " << endl
         << setw(16) << (Gearth+Gsun+Gmoon+Fsrp+Frel) << endl;
*/


    SatOrbit so;
    so.setRefEpoch(utc0);
    so.setSpacecraft(sc);

    so.enableGeopotential(SatOrbit::GM_EGM96, 12, 12, true, false, false);
    so.enableThirdBodyPerturbation(true, true);
    if(srp)
    {
        so.enableSolarRadiationPressure(SatOrbit::SRPM_CODE, true);
    }
    else
    {
        so.enableSolarRadiationPressure(SatOrbit::SRPM_ROCK, true);
    }
    so.enableRelativeEffect(true);

    // force model type to be estimated
    set<ForceModel::ForceModelType> fmt;

    if(srp)
    {
        fmt.insert(ForceModel::D0);
        fmt.insert(ForceModel::Y0);
        fmt.insert(ForceModel::B0);
        fmt.insert(ForceModel::Dc);
        fmt.insert(ForceModel::Ds);
        fmt.insert(ForceModel::Yc);
        fmt.insert(ForceModel::Ys);
        fmt.insert(ForceModel::Bc);
        fmt.insert(ForceModel::Bs);
    }
    else
    {
        fmt.insert(ForceModel::Gx);
        fmt.insert(ForceModel::Gy);
        fmt.insert(ForceModel::Gz);
    }


    so.setForceModelType(fmt);

//    cout << so.getSpacecraft().P() << endl;

    // rkf78 integrator
    RungeKuttaFehlberg rkf78;
    rkf78.setStepSize(30.0);


    Vector<double> state;

    double t(0.0);
    double tt(12*3600.0);
    double step(30.0);

    ofstream outfile("myorbit.txt");

    int i=0;

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

    cerr << endl;

    outfile.close();

    return 0;
}

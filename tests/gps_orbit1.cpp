#pragma ident "$Id$"

#include <algorithm>

#include "ConfDataReader.hpp"

#include "EOPDataStore2.hpp"
#include "LeapSecStore.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

#include "GNSSOrbit.hpp"

#include "RKF78Integrator.hpp"

#include "SP3EphemerisStore.hpp"

#include "DataStructures.hpp"

#include "Counter.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


int main(int argc, char* argv[])
{

    // conf file
    string confFileName("orbit_clock.conf");

    ConfDataReader confReader;

    try
    {
        confReader.open(confFileName);
    }
    catch(...)
    {
        cerr << "conf file '" << confFileName
             << "' open error." << endl;
        exit(-1);
    }

    confReader.setFallback2Default(true);


    // initial time
    string time;
    try
    {
        time = confReader.getValue("initialTime", "DEFAULT");
    }
    catch(...)
    {
        cerr << "initial time get error." << endl;
        exit(-1);
    }

    int year = asInt( time.substr( 0,4) );
    int mon  = asInt( time.substr( 5,2) );
    int day  = asInt( time.substr( 8,2) );
    int hour = asInt( time.substr(11,2) );
    int min  = asInt( time.substr(14,2) );
    int sec  = asDouble( time.substr(17,2) );

    CivilTime civilTime(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime gps0( civilTime.convertToCommonTime() );

//    cout << "initial time: " << civilTime << endl;


    // eop file
    string eopFileName;

    try
    {
        eopFileName = confReader.getValue("eopFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "eop file name get error." << endl;
        exit(-1);
    }

    EOPDataStore2 eopStore;

    try
    {
        eopStore.loadIERSFile(eopFileName);
    }
    catch(...)
    {
        cerr << "eop file '" << eopFileName
             << "' load error." << endl;
        exit(-1);
    }


    // leap second file
    string lsFileName;

    try
    {
        lsFileName = confReader.getValue("lsFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "leap second file name get error." << endl;
        exit(-1);
    }

    LeapSecStore lsStore;

    try
    {
        lsStore.loadFile(lsFileName);
    }
    catch(...)
    {
        cerr << "leap second file '" << lsFileName
             << "' load error." << endl;
        exit(-1);
    }


    // reference system
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopStore);
    refSys.setLeapSecStore(lsStore);


    // sp3 file
    string sp3FileListName;

    try
    {
        sp3FileListName = confReader.getValue("sp3FileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "sp3 file list name get error." << endl;
        exit(-1);
    }

    vector<string> sp3FileListVec;

    ifstream sp3FileListStream;

    sp3FileListStream.open(sp3FileListName.c_str());

    if(!sp3FileListStream)
    {
        cerr << "sp3 file list '" << sp3FileListName
             << "' open error." << endl;
        exit(-1);
    }

    string sp3File;
    while(sp3FileListStream >> sp3File)
    {
        sp3FileListVec.push_back(sp3File);
    }

    sp3FileListStream.close();

    if(sp3FileListVec.size() == 0)
    {
        cerr << "sp3 file list is empty." << endl;
        exit(-1);
    }


    // clk file
    string clkFileListName;

    try
    {
        clkFileListName = confReader.getValue("clkFileListName", "DEFAULT");
    }

    catch(...)
    {
        cerr << "clk file list name get error." << endl;
        exit(-1);
    }

    vector<string> clkFileListVec;

    ifstream clkFileListStream;

    clkFileListStream.open(clkFileListName.c_str());

    if(!clkFileListStream)
    {
        cerr << "clk file list '" << clkFileListName
             << "' open error." << endl;
        exit(-1);
    }

    string clkFile;
    while(clkFileListStream >> clkFile)
    {
        clkFileListVec.push_back(clkFile);
    }

    clkFileListStream.close();

    if(clkFileListVec.size() == 0)
    {
        cerr << "clk file list is empty." << endl;
        exit(-1);
    }


    SP3EphemerisStore sp3Store;
    sp3Store.rejectBadPositions(true);
    sp3Store.rejectBadClocks(true);
    sp3Store.setPosGapInterval(900+1);
    sp3Store.setPosMaxInterval(9*900+1);

    vector<string>::const_iterator sp3_it = sp3FileListVec.begin();
    while(sp3_it != sp3FileListVec.end())
    {
        sp3File = (*sp3_it);

        try
        {
            sp3Store.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "sp3 file '" << sp3File
                 << "' load error." << endl;

            ++sp3_it;
            continue;
        }

        ++sp3_it;
    }

    vector<string>::const_iterator clk_it = clkFileListVec.begin();
    while(clk_it != clkFileListVec.end())
    {
        clkFile = (*clk_it);

        try
        {
            sp3Store.loadRinexClockFile(clkFile);
        }
        catch(...)
        {
            cerr << "clk file '" << clkFile
                 << "' load error." << endl;

            ++clk_it;
            continue;
        }

        ++clk_it;
    }


    // initial conditions
    CommonTime utc0( refSys.GPS2UTC(gps0) );
    CommonTime tt0( refSys.GPS2TT(gps0) );

    Matrix<double> t2cRaw(3,3,0.0), t2cDot(3,3,0.0);

    Vector<double> rsat_t(3,0.0), vsat_t(3,0.0);
    Vector<double> rsat_c(3,0.0), vsat_c(3,0.0);

    // srpc
    Vector<double> srpc0(5,0.0);
    srpc0(0) = -1.0;
//    srpc0(0) = -1.076;
//    srpc0(1) = -0.000;
//    srpc0(2) = 0.010;
//    srpc0(3) = -0.004;
//    srpc0(4) = -0.006;

    satVectorMap satSRPC0;

    // (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
    Vector<double> orbit0(72,0.0);
    orbit0( 6) = 1.0; orbit0(10) = 1.0; orbit0(14) = 1.0; // dr/dr0
    orbit0(33) = 1.0; orbit0(37) = 1.0; orbit0(41) = 1.0; // dv/dv0

    satVectorMap satOrbit0;

    // state and covariance
    Vector<double> state0(11,0.0);
    Matrix<double> covar0(11,11,0.0);

    satVectorMap satState0;
    satMatrixMap satCovar0;


    SatID sat;

//    for(int i=1; i<=MAX_PRN_GPS; ++i)
    for(int i=2; i<=2; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        t2cRaw = refSys.T2CMatrix(utc0);
        t2cDot = refSys.dT2CMatrix(utc0);

        try
        {
            rsat_t = sp3Store.getXvt(sat,gps0).x.toVector();
            vsat_t = sp3Store.getXvt(sat,gps0).v.toVector();
        }
        catch(...)
        {
            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        rsat_c = t2cRaw * rsat_t;
        vsat_c = t2cRaw * vsat_t + t2cDot * rsat_t;

        for(int m=0; m<3; ++m)
        {
            orbit0(m+0) = rsat_c(m);
            orbit0(m+3) = vsat_c(m);
        }

        satOrbit0[sat] = orbit0;

        satSRPC0[sat] = srpc0;

        // X0 and P0
        for(int m=0; m<3; ++m)
        {
            state0(m+0) = rsat_c(m);
            state0(m+3) = vsat_c(m);

            covar0(m+0,m+0) = 10.0 * 10.0;
            covar0(m+3,m+3) = 0.1 * 0.1;
        }

        for(int m=0; m<5; ++m)
        {
            state0(m+6) = srpc0(m);

            covar0(m+6,m+6) = 0.2 * 0.2;
        }

        satState0[sat] = state0;
        satCovar0[sat] = covar0;
    }


    // jpl ephemeris file
    string jplFileName;

    try
    {
        jplFileName = confReader.getValue("jplFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "jpl file name get error." << endl;
        exit(-1);
    }


    // solar system
    SolarSystem solSys;

    try
    {
        solSys.initializeWithBinaryFile(jplFileName);
    }
    catch(...)
    {
        cerr << "solar system initialize error." << endl;
        exit(-1);
    }


    // egm file
    string egmFileName;

    try
    {
        egmFileName = confReader.getValue("egmFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "egm file name get error." << endl;
        exit(-1);
    }


    // earth gravitation
    EGM08Model egm(12,12);

    try
    {
        egm.loadFile(egmFileName);
    }
    catch(...)
    {
        cerr << "egm file '" << egmFileName << "' load error." << endl;
        exit(-1);
    }

    egm.setReferenceSystem(refSys);


    // solid tide
    EarthSolidTide solidTide;
    solidTide.setReferenceSystem(refSys);
    solidTide.setSolarSystem(solSys);

    egm.setEarthSolidTide(solidTide);


    // ocean tide file
    string otFileName;

    try
    {
        otFileName = confReader.getValue("otFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "ocean tide file name get error." << endl;
        exit(-1);
    }


    // ocean tide
    EarthOceanTide oceanTide(8,8);
    oceanTide.setReferenceSystem(refSys);

    try
    {
        oceanTide.loadFile(otFileName);
    }
    catch(...)
    {
        cerr << "ocean tide file '" << otFileName << "' load error." << endl;
        exit(-1);
    }

    egm.setEarthOceanTide(oceanTide);


    // pole tide
    EarthPoleTide poleTide;
    poleTide.setReferenceSystem(refSys);

    egm.setEarthPoleTide(poleTide);


    // third body gravitation
    ThirdBody thd;
    thd.setReferenceSystem(refSys);
    thd.setSolarSystem(solSys);
    thd.enableAllPlanets();


    // solar radiation pressure
    ECOM1Model srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);
    srp.setSRPCoeff(satSRPC0);


    // relativity
    Relativity rel;


    // gnss orbit
    GNSSOrbit gnss;
    gnss.setEGMModel(egm);
    gnss.setThirdBody(thd);
    gnss.setSRPModel(srp);
    gnss.setRelativity(rel);


    // rkf78 integrator
    RKF78Integrator rkf78;
    rkf78.setStepSize(60.0);
    rkf78.setEquationOfMotion(gnss);

    rkf78.setCurrentTime(tt0);
    rkf78.setCurrentState(satOrbit0);


    Vector<double> orbit(orbit0);
    satVectorMap satOrbit(satOrbit0);

    Vector<double> srpc(srpc0);
    satVectorMap satSRPC(satSRPC0);

    Vector<double> state(state0);
    satVectorMap satState(satState0);

    Matrix<double> covar(covar0);
    satMatrixMap satCovar(satCovar0);


    Vector<double> X0(11,0.0);
    Matrix<double> P0(11,11,0.0);

    Vector<double> X1(11,0.0);
    Matrix<double> P1(11,11,0.0);

    Vector<double> X2(11,0.0);
    Matrix<double> P2(11,11,0.0);


    Matrix<double> phi(11,11,0.0);
    for(int i=0; i<5; ++i)
    {
        phi(i+6,i+6) = 1.0;
    }

    Matrix<double> phit(11,11,0.0);

    Matrix<double> Q(11,11,0.0);
    for(int i=0; i<11; ++i)
    {
        Q(i,i) = 1e-12;
    }

    Matrix<double> H(3,6+5,0.0);
    for(int i=0; i<3; ++i)
    {
        H(i,i) = 1.0;
    }

    Matrix<double> HT( transpose(H) );

    Matrix<double> R(3,3,0.0);
    for(int i=0; i<3; ++i)
    {
        R(i,i) = 0.5 * 0.5;
    }

    Vector<double> omc(3,0.0);

    Matrix<double> K;

    Matrix<double> I = ident<double>(11);


    cout << fixed << setprecision(3);


    CommonTime gps(gps0);
    CommonTime utc,tt;

    while( true )
    {
        gps = gps + 60.0;

        if((gps-gps0) > 10*3600.0) break;

        cout << CivilTime(gps);

        utc = refSys.GPS2UTC(gps);
        tt = refSys.GPS2TT(gps);

        t2cRaw = refSys.T2CMatrix(utc);
        t2cDot = refSys.dT2CMatrix(utc);

        satOrbit = rkf78.integrateTo(tt);

        SatIDSet satRejectedSet;

        for(satVectorMap::iterator it = satOrbit.begin();
            it != satOrbit.end();
            ++it)
        {
            sat = it->first;

            orbit = satOrbit[sat];
            srpc = satSRPC[sat];

            P0 = satCovar[sat];


            //// time update ////

            for(int i=0; i<6; ++i)
            {
                // r and v, from oi
                X1(i+0) = orbit(i);
            }

            for(int i=0; i<5; ++i)
            {
                // p, from last epoch
                X1(i+6) = srpc(i);
            }

            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<3; ++j)
                {
                    phi(i+0,j+0) = orbit( 6+3*i+j);     // dr/dr0
                    phi(i+0,j+3) = orbit(15+3*i+j);     // dr/dv0
                    phi(i+3,j+0) = orbit(24+3*i+j);     // dv/dr0
                    phi(i+3,j+3) = orbit(33+3*i+j);     // dv/dv0
                }

                for(int j=0; j<5; ++j)
                {
                    phi(i+0,j+6) = orbit(42+5*i+j);     // dr/dp0
                    phi(i+3,j+6) = orbit(57+5*i+j);     // dv/dp0
                }
            }

            phit = transpose(phi);

            P1 = phi * P0 * phit + Q;

            //// meas update ////

            try
            {
                rsat_t = sp3Store.getXvt(sat,gps).x.toVector();
                vsat_t = sp3Store.getXvt(sat,gps).v.toVector();
            }
            catch(...)
            {
                satRejectedSet.insert(sat);
                continue;
            }

            rsat_c = t2cRaw * rsat_t;
            vsat_c = t2cRaw * vsat_t + t2cDot * rsat_t;

            omc(0) = rsat_c(0) - orbit(0);
            omc(1) = rsat_c(1) - orbit(1);
            omc(2) = rsat_c(2) - orbit(2);

            K = P1 * HT * inverse(H * P1 * HT + R);

            X2 = X1 + K * omc;

            P2 = (I - K * H) * P1;

//            cout << sat;
            cout << setw(10) << X2(0) - rsat_c(0)
                 << setw(10) << X2(1) - rsat_c(1)
                 << setw(10) << X2(2) - rsat_c(2)
//                 << setw(10) << X2(3) - vsat_c(0)
//                 << setw(10) << X2(4) - vsat_c(1)
//                 << setw(10) << X2(5) - vsat_c(2)
                 << setw(10) << X2( 6)
                 << setw(10) << X2( 7)
                 << setw(10) << X2( 8)
                 << setw(10) << X2( 9)
                 << setw(10) << X2(10);
//            cout << endl;

            // update orbit
            for(int i=0; i<3; ++i)
            {
                orbit(i+0) = X2(i+0);
                orbit(i+3) = X2(i+3);
            }

            orbit( 6) = 1.0; orbit(10) = 1.0; orbit(14) = 1.0;
            orbit(33) = 1.0; orbit(37) = 1.0; orbit(41) = 1.0;

            for(int i=42; i<72; ++i)
            {
                orbit(i) = 0.0;
            }

            satOrbit[sat] = orbit;

            // update SRPC
            for(int i=0; i<5; ++i)
            {
                srpc(i) = X2(i+6);
            }

            satSRPC[sat] = srpc;

            // update covariance
            satCovar[sat] = P2;
        }

        cout << endl;

        // update srp
        srp.setSRPCoeff(satSRPC);
        gnss.setSRPModel(srp);
        rkf78.setEquationOfMotion(gnss);

        // update rkf
        rkf78.setCurrentTime(tt);
        rkf78.setCurrentState(satOrbit);

    } // End of 'while( true )'

    double clock_end( Counter::now() );

//    cout << clock_end - clock_start << endl;

    return 0;

}

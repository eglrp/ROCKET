#pragma ident "$Id$"

#include <algorithm>

#include "ConfDataReader.hpp"

#include "EOPDataStore2.hpp"
#include "LeapSecStore.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

#include "GNSSOrbit.hpp"

#include "RKF78Integrator.hpp"

#include "MSCStore.hpp"

#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

#include "Rinex3EphemerisStore2.hpp"

#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"

#include "SP3EphemerisStore.hpp"

#include "NetworkObsStreams.hpp"

#include "DataStructures.hpp"

#include "ProcessingList.hpp"

#include "RequireObservables.hpp"

#include "DCBDataReader.hpp"

#include "CC2NONCC.hpp"

#include "LinearCombinations.hpp"
#include "ComputeLinear.hpp"

#include "BasicModel.hpp"

#include "BasicModel2.hpp"

#include "GravitationalDelay.hpp"

#include "AntexReader.hpp"

#include "ComputeElevWeights.hpp"

#include "ComputeSatPCenter2.hpp"

#include "ComputeTides.hpp"

#include "CorrectObservables2.hpp"

#include "ComputeTropModel.hpp"

#include "ComputeStaClock.hpp"

#include "Epoch.hpp"

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


    // nav file
    string navFileListName;

    try
    {
        navFileListName = confReader.getValue("navFileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "nav file list name get error." << endl;
        exit(-1);
    }

    vector<string> navFileListVec;

    ifstream navFileListStream;

    navFileListStream.open(navFileListName.c_str());

    if(!navFileListStream)
    {
        cerr << "nav file list '" << navFileListName
             << "' open error." << endl;
        exit(-1);
    }

    string navFile;
    while(navFileListStream >> navFile)
    {
        navFileListVec.push_back(navFile);
    }

    navFileListStream.close();

    if(navFileListVec.size() == 0)
    {
        cerr << "nav file list is empty." << endl;
        exit(-1);
    }


    Rinex3EphemerisStore2 bceStore;

    vector<string>::const_iterator nav_it = navFileListVec.begin();
    while(nav_it != navFileListVec.end())
    {
        string navFile = (*nav_it);

        try
        {
            bceStore.loadFile(navFile);
        }
        catch(...)
        {
            cerr << "nav file '" << navFile
                 << "' load error." << endl;

            ++nav_it;

            continue;
        }

        ++nav_it;
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

    satVectorMap satSRPC0;

    // (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
    Vector<double> orbit0(72,0.0);
    orbit0( 6) = 1.0; orbit0(10) = 1.0; orbit0(14) = 1.0; // dr/dr0
    orbit0(33) = 1.0; orbit0(37) = 1.0; orbit0(41) = 1.0; // dv/dv0

    satVectorMap satOrbit0;

    // clock
    double clock0;
    satValueMap satClock0;


    SatID sat;
    SatIDSet allSatSet;

    for(int i=1; i<=1; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        t2cRaw = refSys.T2CMatrix(utc0);
        t2cDot = refSys.dT2CMatrix(utc0);

        try
        {
            rsat_t = sp3Store.getXvt(sat,gps0).x.toVector();
            vsat_t = sp3Store.getXvt(sat,gps0).v.toVector();
            clock0 = bceStore.getXvt(sat,gps0).clkbias * C_MPS;
        }
        catch(...)
        {
            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        rsat_c = t2cRaw * rsat_t;
        vsat_c = t2cRaw * vsat_t + t2cDot * rsat_t;

        orbit0(0) = rsat_c[0]; orbit0(1) = rsat_c[1]; orbit0(2) = rsat_c[2];
        orbit0(3) = vsat_c[0]; orbit0(4) = vsat_c[1]; orbit0(5) = vsat_c[2];

        allSatSet.insert(sat);
        satOrbit0[sat] = orbit0;
        satClock0[sat] = clock0;
        satSRPC0[sat] = srpc0;
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
    rkf78.setStepSize(30.0);


    satVectorMap satOrbit(satOrbit0);
    Vector<double> orbit;

    satVectorMap satSRPC(satSRPC0);
    Vector<double> srpc;

    Vector<double> dx(6,0.0);
    for(int i=0; i<3; ++i) dx(i+0) = 1.00;
    for(int i=0; i<3; ++i) dx(i+3) = 1e-3;

    Vector<double> dp(5,0.0);
    for(int i=0; i<5; ++i) dp(i+0) = 0.01;

    int index(10);

    for(SatIDSet::iterator it = allSatSet.begin();
        it != allSatSet.end();
        ++it)
    {
        sat = *it;

        orbit = satOrbit[sat];
        srpc = satSRPC[sat];

        if(index < 6)
            orbit(index) += dx(index);
        else
            srpc(index-6) += dp(index-6);

        satOrbit[sat] = orbit;
        satSRPC[sat] = srpc;
    }


    satVectorMap satOrbit1, satOrbit2;
    Vector<double> orbit1, orbit2;

    // no pertubation
    srp.setSRPCoeff(satSRPC0);
    gnss.setSRPModel(srp);
    rkf78.setEquationOfMotion(gnss);

    rkf78.setCurrentTime(tt0);
    rkf78.setCurrentState(satOrbit0);
    satOrbit1 = rkf78.integrateTo(tt0+30.0);

    // pertubation
    srp.setSRPCoeff(satSRPC);
    gnss.setSRPModel(srp);
    rkf78.setEquationOfMotion(gnss);

    rkf78.setCurrentTime(tt0);
    rkf78.setCurrentState(satOrbit);
    satOrbit2 = rkf78.integrateTo(tt0+30.0);

    cout << fixed;

    for(SatIDSet::iterator it = allSatSet.begin();
        it != allSatSet.end();
        ++it)
    {
        sat = *it;

        orbit1 = satOrbit1[sat];
        orbit2 = satOrbit2[sat];

        Vector<double> dd(6,0.0);

        for(int i=0; i<6; ++i)
            dd(i) = orbit2(i)-orbit1(i);

        if(index < 6)
            dd = dd/dx(index);
        else
            dd = dd/dp(index-6);

        cout << setprecision(8);

        for(int i=6; i<24; ++i)
        {
            if((i-6)%3 == 0) cout << endl;
            cout << setw(15) << orbit1(i);
        }
        cout << endl;

        for(int i=24; i<42; ++i)
        {
            if((i-24)%3 == 0) cout << endl;
            cout << setw(15) << orbit1(i);
        }
        cout << endl;

        for(int i=42; i<72; ++i)
        {
            if((i-42)%5 == 0) cout << endl;
            cout << setw(15) << orbit1(i);
        }
        cout << endl << endl;

        cout << setw(15) << dd(0)
             << setw(15) << dd(1)
             << setw(15) << dd(2)
             << setw(15) << dd(3)
             << setw(15) << dd(4)
             << setw(15) << dd(5)
             << endl << endl;

    }

    return 0;

}

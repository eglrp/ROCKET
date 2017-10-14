#pragma ident "$Id$"

#include <algorithm>

#include "ConfDataReader.hpp"

#include "EOPDataStore2.hpp"

#include "LeapSecStore.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

#include "Rinex3EphemerisStore2.hpp"

#include "DataStructures.hpp"

#include "ProcessingList.hpp"

#include "ComputeSatPCenter.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;



int main(int argc, char* argv[])
{

    // conf file
    string confFileName("conf.txt");

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
    CommonTime t0_gps( civilTime.convertToCommonTime() );

    cout << "initial time: " << civilTime << endl;


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


    // nav file
    string navFileListName = confReader.getValue("navFileListName", "DEFAULT");

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
        cerr << navFileListName << "navFileList is empty!! " << endl;
        exit(-1);
    }

    Rinex3EphemerisStore2 ephStore;

    vector<string>::const_iterator nav_it = navFileListVec.begin();
    while(nav_it != navFileListVec.end())
    {
        string navFile = (*nav_it);

        try
        {
            ephStore.loadFile(navFile);
        }
        catch(...)
        {
            cerr << "nav file '" << navFile
                 << "' load error." << endl;

            continue;
        }

        ++nav_it;
    }


    // initial conditions
    CommonTime t0_utc( refSys.GPS2UTC(t0_gps) );

    Matrix<double> t2c(3,3,0.0);
    Matrix<double> dt2c(3,3,0.0);

    Vector<double> r0_itrs(3,0.0);
    Vector<double> v0_itrs(3,0.0);

    Vector<double> r0_icrs(3,0.0);
    Vector<double> v0_icrs(3,0.0);

    satTypeValueMap initialConditions;

    // GPS
    for(int i=1; i<=MAX_PRN_GPS; i++)
    {
        SatID sat(i, SatID::systemGPS);

        typeValueMap tvmap;

        try
        {
            r0_itrs = ephStore.getXvt(sat,t0_gps).x.toVector();
            v0_itrs = ephStore.getXvt(sat,t0_gps).v.toVector();
        }
        catch(...)
        {
//            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        tvmap[TypeID::satX]  = r0_itrs[0];
        tvmap[TypeID::satY]  = r0_itrs[1];
        tvmap[TypeID::satZ]  = r0_itrs[2];
        tvmap[TypeID::satVX] = v0_itrs[0];
        tvmap[TypeID::satVY] = v0_itrs[1];
        tvmap[TypeID::satVZ] = v0_itrs[2];

        initialConditions[sat] = tvmap;
    }


    // Galileo
    for(int i=1; i<=MAX_PRN_GAL; i++)
    {
        SatID sat(i, SatID::systemGalileo);

        typeValueMap tvmap;

        try
        {
            r0_itrs = ephStore.getXvt(sat,t0_gps).x.toVector();
            v0_itrs = ephStore.getXvt(sat,t0_gps).v.toVector();
        }
        catch(...)
        {
//            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        tvmap[TypeID::satX]  = r0_itrs[0];
        tvmap[TypeID::satY]  = r0_itrs[1];
        tvmap[TypeID::satZ]  = r0_itrs[2];
        tvmap[TypeID::satVX] = v0_itrs[0];
        tvmap[TypeID::satVY] = v0_itrs[1];
        tvmap[TypeID::satVZ] = v0_itrs[2];

        initialConditions[sat] = tvmap;
    }


    // BDS
    for(int i=1; i<=MAX_PRN_BDS; i++)
    {
        SatID sat(i, SatID::systemBDS);

        typeValueMap tvmap;

        try
        {
            r0_itrs = ephStore.getXvt(sat,t0_gps).x.toVector();
            v0_itrs = ephStore.getXvt(sat,t0_gps).v.toVector();
        }
        catch(...)
        {
//            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        tvmap[TypeID::satX]  = r0_itrs[0];
        tvmap[TypeID::satY]  = r0_itrs[1];
        tvmap[TypeID::satZ]  = r0_itrs[2];
        tvmap[TypeID::satVX] = v0_itrs[0];
        tvmap[TypeID::satVY] = v0_itrs[1];
        tvmap[TypeID::satVZ] = v0_itrs[2];

        initialConditions[sat] = tvmap;
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


    // nominal position
    Position nominalPos(0.0, 0.0, 0.0);


    // antex file
    string antFileName;

    try
    {
        antFileName = confReader.getValue("antFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "ant file name get error." << endl;
        exit(-1);
    }

    AntexReader antexReader;

    try
    {
        antexReader.open( antFileName );
    }
    catch(...)
    {
        cerr << "ant file '" << antFileName
             << "' open error." << endl;
        exit(-1);
    }


    // satellite phase center
    ComputeSatPCenter satPCenter;
    satPCenter.setNominalPosition( nominalPos );
    satPCenter.setEphStore( ephStore );
    satPCenter.setAntexReader( antexReader );

    satPCenter.Process(t0_gps, initialConditions);


    cout << setprecision(3);

    Vector<SatID> sats( initialConditions.getVectorOfSatID() );

    for(int i=0; i<sats.size(); i++)
    {
        SatID sat( sats(i) );

        Vector<double> x1(3,0.0), dx(4,0.0), x2(3,0.0);
        x1(0) = initialConditions.getValue(sat, TypeID::satX);
        x1(1) = initialConditions.getValue(sat, TypeID::satY);
        x1(2) = initialConditions.getValue(sat, TypeID::satZ);

        dx(0) = initialConditions.getValue(sat, TypeID::satPCenterX);
        dx(1) = initialConditions.getValue(sat, TypeID::satPCenterY);
        dx(2) = initialConditions.getValue(sat, TypeID::satPCenterZ);
        dx(3) = initialConditions.getValue(sat, TypeID::satPCenter);

        x2(0) = x1(0) - dx(0);
        x2(1) = x1(1) - dx(1);
        x2(2) = x1(2) - dx(2);

        double r1( norm(x1) ), r2( norm(x2) );

        cout << sat
             << setw(10) << r1-r2
             << setw(10) << dx(3)
             << endl;
    }

    return 0;
}

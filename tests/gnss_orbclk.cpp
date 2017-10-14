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

#include "DataStructures.hpp"

#include "ProcessingList.hpp"

#include "RequireObservables.hpp"

#include "BasicModel.hpp"

#include "ComputeElevWeights.hpp"

#include "CorrectObservables.hpp"

#include "TropModel.hpp"

#include "ComputeTropModel.hpp"

#include "LinearCombinations.hpp"

#include "ComputeLinear.hpp"

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
    CommonTime gps0( civilTime.convertToCommonTime() );

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

            ++nav_it;

            continue;
        }

        ++nav_it;
    }


    // initial conditions
    CommonTime utc0( refSys.GPS2UTC(gps0) );

    Matrix<double> t2cRaw(3,3,0.0);
    Matrix<double> t2cDot(3,3,0.0);

    Vector<double> r0_itrs(3,0.0);
    Vector<double> v0_itrs(3,0.0);

    Vector<double> r0_icrs(3,0.0);
    Vector<double> v0_icrs(3,0.0);

    // r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0
    Vector<double> orbit(96,0.0);

    // dr/dr0
    orbit( 6) = 1.0; orbit(10) = 1.0; orbit(14) = 1.0;
    // dv/dv0
    orbit(33) = 1.0; orbit(37) = 1.0; orbit(41) = 1.0;

    satOrbitMap somap;

    int satNums(MAX_PRN_GPS+MAX_PRN_GAL+MAX_PRN_BDS);

    SatID sat;

    for(int i=1; i<=satNums; ++i)
    {
        if(i<=MAX_PRN_GPS)
        {
            sat.id = i;
            sat.system = SatID::systemGPS;
        }
        else if(i<=MAX_PRN_GPS+MAX_PRN_GAL)
        {
            sat.id = i - MAX_PRN_GPS;
            sat.system = SatID::systemGalileo;
        }
        else if(i<=MAX_PRN_GPS+MAX_PRN_GAL+MAX_PRN_BDS)
        {
            sat.id = i - MAX_PRN_GPS - MAX_PRN_GAL;
            sat.system = SatID::systemBDS;
        }

        t2cRaw = refSys.T2CMatrix(utc0);
        t2cDot = refSys.dT2CMatrix(utc0);

        try
        {
            r0_itrs = ephStore.getXvt(sat,gps0).x.toVector();
            v0_itrs = ephStore.getXvt(sat,gps0).v.toVector();
        }
        catch(...)
        {
//            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        r0_icrs = t2cRaw * r0_itrs;
        v0_icrs = t2cRaw * v0_itrs + t2cDot * r0_itrs;

        // r0
        orbit(0) = r0_icrs[0]; orbit(1) = r0_icrs[1]; orbit(2) = r0_icrs[2];

        // v0
        orbit(3) = v0_icrs[0]; orbit(4) = v0_icrs[1]; orbit(5) = v0_icrs[2];

        somap[sat] = orbit;
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
    ECOM2Model srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);

    Vector<double> p0_srp(9,0.0);
    p0_srp(0) = -100.0;
    srp.setSRPCoeff(p0_srp);


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
    rkf78.setEquationOfMotion(gnss);



    // msc file
    string mscFileName = confReader.getValue("mscFileName", "DEFAULT");

    MSCStore mscStore;

    try
    {
        mscStore.loadFile( mscFileName.c_str() );
    }
    catch(...)
    {
        cerr << "msc file '" << mscFileName << "' open error." << endl;
        exit(-1);
    }


    // obs file
    string obsFileListName = confReader.getValue("obsFileListName", "DEFAULT");

    vector<string> obsFileListVec;

    ifstream obsFileListStream;

    obsFileListStream.open(obsFileListName.c_str());

    if(!obsFileListStream)
    {
        cerr << "obs file list '" << obsFileListName << "' open error." << endl;
        exit(-1);
    }

    string obsFile;
    while(obsFileListStream >> obsFile)
    {
        obsFileListVec.push_back(obsFile);
    }

    obsFileListStream.close();

    if(obsFileListVec.size() == 0)
    {
        cerr << obsFileListName << "obsFileList is empty!! " << endl;
        exit(-1);
    }

    gnssDataMap gdsMap;

    vector<string>::const_iterator obs_it = obsFileListVec.begin();
    while(obs_it != obsFileListVec.end())
    {
        string obsFile = (*obs_it);

        Rinex3ObsStream ros;
        ros.exceptions(ios::failbit);

        try
        {
            ros.open(obsFile.c_str(), ios::in);
        }
        catch(...)
        {
            cerr << "Problem opening file '"
                 << obsFile
                 << "'." << endl;

            cerr << "Maybe it doesn't exist or you don't have "
                 << "proper read permissions."
                 << endl;

            cerr << "Skipping obs file '" << obsFile << "'."
                 << endl;

            ros.close();

            ++obs_it;

            continue;
        }


        Rinex3ObsHeader roh;

        try
        {
            ros >> roh;
        }
        catch(...)
        {
            cerr << "Problem in reading file '"
                 << obsFile
                 << "'." << endl;

            cerr << "Skipping obs file '" << obsFile << "'."
                 << endl;

            ros.close();

            ++obs_it;

            continue;
        }

        string station = roh.markerName;


        //
        RinexObsID GC1C("GC1C"), GC2W("GC2W");
        RinexObsID EC1X("EC1X"), EC5X("EC5X");

        map<string,vector<RinexObsID> > mapObsTypes(roh.mapObsTypes);
        vector<RinexObsID> roi;

        if(mapObsTypes.find("G") != mapObsTypes.end())
        {
            roi = mapObsTypes["G"];

            if(find(roi.begin(),roi.end(),GC1C) == roi.end())
            {
                ros.close();
                ++obs_it;
                continue;
            }

            if(find(roi.begin(),roi.end(),GC2W) == roi.end())
            {
                ros.close();
                ++obs_it;
                continue;
            }
        }

        if(mapObsTypes.find("E") != mapObsTypes.end())
        {
            roi = mapObsTypes["E"];

            if(find(roi.begin(),roi.end(),EC1X) == roi.end())
            {
                ros.close();
                ++obs_it;
                continue;
            }

            if(find(roi.begin(),roi.end(),EC5X) == roi.end())
            {
                ros.close();
                ++obs_it;
                continue;
            }
        }


        cout << "Starting processing for station: '" << station << "'." << endl;

        gps0.setTimeSystem(TimeSystem::Unknown);

        MSCData mscData;

        try
        {
            mscData = mscStore.findMSC(station, gps0);
        }
        catch(...)
        {
            cerr << "The station " << station
                 << " isn't included in MSC file." << endl;

            ++obs_it;

            continue;
        }

        gps0.setTimeSystem(TimeSystem::GPS);

        Position nominalPos( mscData.coordinates );


        // processing list
        ProcessingList pList;


        // RequireObservables
        RequireObservables requireObs;
        requireObs.addRequiredType(SatID::systemGPS, TypeID::C1);
        requireObs.addRequiredType(SatID::systemGPS, TypeID::C2);
        requireObs.addRequiredType(SatID::systemGPS, TypeID::L1);
        requireObs.addRequiredType(SatID::systemGPS, TypeID::L2);
        requireObs.addRequiredType(SatID::systemGalileo, TypeID::C1);
        requireObs.addRequiredType(SatID::systemGalileo, TypeID::C5);
        requireObs.addRequiredType(SatID::systemGalileo, TypeID::L1);
        requireObs.addRequiredType(SatID::systemGalileo, TypeID::L5);

        pList.push_back(requireObs);


        // ComputeLinear
        LinearCombinations linearComb;

        ComputeLinear linear1;
        linear1.addLinear(SatID::systemGPS,     linearComb.mwCombOfGPS);
        linear1.addLinear(SatID::systemGPS,     linearComb.liCombOfGPS);

        ComputeLinear linear2;
        linear2.addLinear(SatID::systemGPS,     linearComb.pcCombOfGPS);
        linear2.addLinear(SatID::systemGPS,     linearComb.lcCombOfGPS);
        linear2.addLinear(SatID::systemGalileo, linearComb.pcCombOfGAL);
        linear2.addLinear(SatID::systemGalileo, linearComb.lcCombOfGAL);

        ComputeLinear linear3;
        linear3.addLinear(SatID::systemGPS,     linearComb.pcPrefit);
        linear3.addLinear(SatID::systemGPS,     linearComb.lcPrefitOfGPS);
        linear3.addLinear(SatID::systemGalileo, linearComb.pcPrefit);
        linear3.addLinear(SatID::systemGalileo, linearComb.lcPrefitOfGAL);


        // BasicModel
        BasicModel basicModel(nominalPos, ephStore);
        basicModel.setMinElev(10.0);
        basicModel.setDefaultObservable(SatID::systemGPS,     TypeID::PC);
        basicModel.setDefaultObservable(SatID::systemGalileo, TypeID::PC);

        pList.push_back(basicModel);


        // CorrectObservables
        Triple offsetARP( roh.antennaDeltaHEN );

        CorrectObservables correctObs(ephStore);
        correctObs.setNominalPosition(nominalPos);
        correctObs.setMonument(offsetARP);

        pList.push_back(correctObs);


        // ComputeElevWeights
        ComputeElevWeights elevWeights;

        pList.push_back(elevWeights);


        // ComputeTropModel
        NeillTropModel neillTM(nominalPos, gps0);

        ComputeTropModel computeTM(neillTM);

        pList.push_back(computeTM);



        // gnssRinex
        gnssRinex gRin;

        while(ros >> gRin)
        {
            CommonTime time(gRin.header.epoch);

            gRin.header.antennaPosition = nominalPos;

            try
            {
                gRin >> requireObs
                     >> linear1;
            }
            catch(...)
            {
                cerr << "Exception for station '" << station
                     << "' at epoch: " << time << endl;
                continue;
            }

        }   // End of 'while(ros...)'

        ros.close();

        ++obs_it;

    }


    return 0;
}

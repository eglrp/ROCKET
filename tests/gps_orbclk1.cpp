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

#include "LICSDetector.hpp"
#include "MWCSDetector2.hpp"

#include "SatArcMarker.hpp"

#include "BasicModel.hpp"

#include "BasicModel2.hpp"

#include "GravitationalDelay.hpp"

#include "AntexReader.hpp"

#include "Antenna.hpp"

#include "ComputeElevWeights.hpp"

#include "ComputeSatPCenter.hpp"

#include "ComputeTides.hpp"

#include "CorrectObservables.hpp"

#include "ComputeWindUp.hpp"

#include "PhaseCodeAlignment.hpp"

#include "TropModel.hpp"
#include "ComputeTropModel.hpp"

#include "SolidTides.hpp"

#include "OceanLoading.hpp"

#include "PoleTides.hpp"

#include "ComputeStaClock.hpp"

#include "StochasticModel2.hpp"

#include "TimeUpdate.hpp"

#include "MeasUpdate.hpp"

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

    Vector<double> r0_itrs(3,0.0), v0_itrs(3,0.0);
    Vector<double> r0_icrs(3,0.0), v0_icrs(3,0.0);

    // srpc
    int numSRP(5);
    Vector<double> srpc0(numSRP,0.0);
    srpc0(0) = -1.0;

    satVectorMap satSRPC0;

    // (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
    Vector<double> orbit0(42+6*numSRP,0.0);
    orbit0( 6) = 1.0; orbit0(10) = 1.0; orbit0(14) = 1.0; // dr/dr0
    orbit0(33) = 1.0; orbit0(37) = 1.0; orbit0(41) = 1.0; // dv/dv0

    satVectorMap satOrbit0;

    SatID sat;

    for(int i=1; i<=MAX_PRN_GPS; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        t2cRaw = refSys.T2CMatrix(utc0);
        t2cDot = refSys.dT2CMatrix(utc0);

        try
        {
            r0_itrs = sp3Store.getXvt(sat,gps0).x.toVector();
            v0_itrs = sp3Store.getXvt(sat,gps0).v.toVector();
        }
        catch(...)
        {
            cerr << "initial conditions of " << sat << " get error." << endl;
            continue;
        }

        r0_icrs = t2cRaw * r0_itrs;
        v0_icrs = t2cRaw * v0_itrs + t2cDot * r0_itrs;

        // r0
        orbit0(0) = r0_icrs[0]; orbit0(1) = r0_icrs[1]; orbit0(2) = r0_icrs[2];

        // v0
        orbit0(3) = v0_icrs[0]; orbit0(4) = v0_icrs[1]; orbit0(5) = v0_icrs[2];

        satOrbit0[sat] = orbit0;

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
    rkf78.setEquationOfMotion(gnss);


    // msc file
    string mscFileName;
    try
    {
        mscFileName = confReader.getValue("mscFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "get msc file name error." << endl;
        exit(-1);
    }

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


    // dcb file
    string dcbFileName;

    try
    {
        dcbFileName = confReader.getValue("dcbFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "dcb file name get error." << endl;
        exit(-1);
    }

    DCBDataReader dcbReader;

    try
    {
        dcbReader.open( dcbFileName );
    }
    catch(...)
    {
        cerr << "dcb file '" << dcbFileName << "' open error." << endl;
        exit(-1);
    }


    // atx file
    string atxFileName;

    try
    {
        atxFileName = confReader.getValue("atxFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "get atx file name error." << endl;
        exit(-1);
    }

    AntexReader antexReader;

    try
    {
        antexReader.open( atxFileName );
    }
    catch(...)
    {
        cerr << "atx file '" << atxFileName << "' open error." << endl;
        exit(-1);
    }


    // blq file
    BLQDataReader blqStore;

    string blqFileName;

    try
    {
        blqFileName = confReader.getValue("blqFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "get blq file name error." << endl;
        exit(-1);
    }

    try
    {
        blqStore.open( blqFileName );
    }
    catch(...)
    {
        cerr << "blq file '" << blqFileName << "' open error." << endl;
        exit(-1);
    }


    // gpt2 file
    string gpt2FileName;

    try
    {
        gpt2FileName = confReader.getValue("gpt2FileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "get gpt2 file name error." << endl;
        exit(-1);
    }


    // obs file
    NetworkObsStreams obsStreams;

    string obsFileListName;
    try
    {
        obsFileListName = confReader.getValue("obsFileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "obs file list name get error." << endl;
        exit(-1);
    }

    ifstream obsFileListStream( obsFileListName.c_str() );

    if( !obsFileListStream.is_open() )
    {
        cerr << "obs file list '" << obsFileListName << "' open error." << endl;
        exit(-1);
    }


    double clock_start( Counter::now() );


    SourceIDSet allSourceSet;
    map<SourceID, Position> sourcePositionMap;
    map<SourceID, Triple> sourceMonumentMap;
    map<SourceID, Antenna> sourceAntennaMap;


    // ObsID on GPS L1 Frequency
    RinexObsID GC1C("GC1C"), GL1C("GL1C");
    // ObsID on GPS L2 Frequency
    RinexObsID GC2W("GC2W"), GL2W("GL2W");

    string obsFile;
    while(obsFileListStream >> obsFile)
    {
        Rinex3ObsStream ros;
        ros.exceptions(ios::failbit);

        try
        {
            ros.open(obsFile.c_str(), ios::in);
        }
        catch(...)
        {
            cerr << "obs file '" << obsFile << "' open error." << endl;
            ros.close(); continue;
        }

        Rinex3ObsHeader roh;

        try
        {
            ros >> roh;
        }
        catch(...)
        {
            cerr << "obs header of '" << obsFile << "' read error." << endl;
            ros.close(); continue;
        }


        SourceID source( SourceID::Mixed, roh.markerName, roh.markerNumber );

        string station( source.sourceName );
        transform(station.begin(),station.end(), station.begin(), ::toupper);


        // check if station is included in MSC file
        gps0.setTimeSystem(TimeSystem::Unknown);

        MSCData mscData;

        try
        {
            mscData = mscStore.findMSC(station, gps0);
        }
        catch(...)
        {
            cerr << "station '" << station << "' not exist in MSC file." << endl;
            ros.close(); continue;
        }

        gps0.setTimeSystem(TimeSystem::GPS);


        // check if current station exist in BLQ file
        if( !blqStore.isValid(station) )
        {
            cerr << "station '" << station << "' not exist in BLQ file." << endl;
            ros.close(); continue;
        }


        // check if required signals exist in OBS file
        map< string,vector<RinexObsID> > mapObsTypes;
        mapObsTypes = roh.mapObsTypes;

        vector<RinexObsID> roi;

        if(mapObsTypes.find("G") != mapObsTypes.end())
        {
            roi = mapObsTypes["G"];

            if(find(roi.begin(),roi.end(),GC1C) == roi.end())
            {
                ros.close(); continue;
            }

            if(find(roi.begin(),roi.end(),GC2W) == roi.end())
            {
                ros.close(); continue;
            }

            if(find(roi.begin(),roi.end(),GL1C) == roi.end())
            {
                ros.close(); continue;
            }

            if(find(roi.begin(),roi.end(),GL2W) == roi.end())
            {
                ros.close(); continue;
            }
        }

        ros.close();

        // now, we can add OBS file to OBS streams
        if( !obsStreams.addRinexObsFile( obsFile ) )
        {
            cerr << "obs file '" << obsFile << "' add error." << endl;
            continue;
        }

        string antennaModel( roh.antType );
        Antenna antenna;

        try
        {
            antenna = antexReader.getAntenna( antennaModel );
        }
        catch(ObjectNotFound& notFound)
        {
            antennaModel.replace(16,4,"NONE");
            antenna = antexReader.getAntenna( antennaModel );
        }


        allSourceSet.insert(source);
        sourcePositionMap[source] = mscData.coordinates;
        sourceMonumentMap[source] = roh.antennaDeltaHEN;
        sourceAntennaMap[source] = antenna;
    }

    obsFileListStream.close();


    // processing list
    ProcessingList pList;


    // RequireObservables
    RequireObservables requireObs;
    requireObs.addRequiredType(SatID::systemGPS, TypeID::C1);
    requireObs.addRequiredType(SatID::systemGPS, TypeID::C2);


    // CC2NONCC
    CC2NONCC cc2noncc;
    cc2noncc.setDCBDataReader(dcbReader);


    // LinearCombinations
    LinearCombinations linearComb;


    // ComputeLinear, PC
    ComputeLinear linearPC;
    linearPC.addLinear(SatID::systemGPS, linearComb.pcCombOfGPS);


    // ComputeLinear, prefit PC with satellite clock
    ComputeLinear prefitPCWithSatClock;
    prefitPCWithSatClock.addLinear(SatID::systemGPS, linearComb.pcPrefitWithSatClock);

    // ComputeLinear, prefit PC with station clock
    ComputeLinear prefitPCWithStaClock;
    prefitPCWithStaClock.addLinear(SatID::systemGPS, linearComb.pcPrefitWithStaClock);

    // ComputeLinear, prefit PC without clock
    ComputeLinear prefitPCWithoutClock;
    prefitPCWithoutClock.addLinear(SatID::systemGPS, linearComb.pcPrefitWithoutClock);

    // ComputeLinear, prefit PC with both clock
    ComputeLinear prefitPCWithClock;
    prefitPCWithClock.addLinear(SatID::systemGPS, linearComb.pcPrefit);


    // BasicModel
    BasicModel basicModel;
    basicModel.setEphStore(sp3Store);
    basicModel.setMSCStore(mscStore);
    basicModel.setMinElev(12.5);
    basicModel.setDefaultObs(SatID::systemGPS, TypeID::PC);


    // BasicModel2
    BasicModel2 basicModel2;
    basicModel2.setReferenceSystem(refSys);
    basicModel2.setMSCStore(mscStore);


    // ComputeElevWeights
    ComputeElevWeights elevWeights;


    // GravitationalDelay
    GravitationalDelay gravDelay;
    gravDelay.setMSCStore(mscStore);


    // ComputeSatPCenter
    ComputeSatPCenter satPCenter;
    satPCenter.setEphStore(bceStore);
    satPCenter.setMSCStore(mscStore);
    satPCenter.setAntexReader(antexReader);


    // ComputeTides
    ComputeTides staTides;
    staTides.setBLQReader(blqStore);
    staTides.setEOPDataStore(eopStore);


    // CorrectObservables
    CorrectObservables correctObs;
    correctObs.setEphStore(bceStore);
    correctObs.setMSCStore(mscStore);
    correctObs.setTideCorr(staTides);
    correctObs.setMonumentMap(sourceMonumentMap);
    correctObs.setAntennaMap(sourceAntennaMap);


    // ComputeTropModel
    ViennaTropModel viennaTM;
    viennaTM.loadFile(gpt2FileName);
    ComputeTropModel computeTM;
    computeTM.setTropModel(viennaTM);
    computeTM.setMSCStore(mscStore);


    // ComputeStaClock
    ComputeStaClock computeStaClock;


    // Satellite Orbit
    SatOrbRandomWalkModel satOrbModel;

    Variable satX(TypeID::dSatX, &satOrbModel);
    satX.setSourceIndexed(false);
    satX.setSatIndexed(true);
    satX.setInitialVariance(1.0);

    Variable satY(TypeID::dSatY, &satOrbModel);
    satY.setSourceIndexed(false);
    satY.setSatIndexed(true);
    satY.setInitialVariance(1.0);

    Variable satZ(TypeID::dSatZ, &satOrbModel);
    satZ.setSourceIndexed(false);
    satZ.setSatIndexed(true);
    satZ.setInitialVariance(1.0);

    Variable satVX(TypeID::dSatVX, &satOrbModel);
    satVX.setSourceIndexed(false);
    satVX.setSatIndexed(true);
    satVX.setInitialVariance(1e-2);

    Variable satVY(TypeID::dSatVY, &satOrbModel);
    satVY.setSourceIndexed(false);
    satVY.setSatIndexed(true);
    satVY.setInitialVariance(1e-2);

    Variable satVZ(TypeID::dSatVZ, &satOrbModel);
    satVZ.setSourceIndexed(false);
    satVZ.setSatIndexed(true);
    satVZ.setInitialVariance(1e-2);

    Variable satSRPC1(TypeID::dSRPC1, &satOrbModel);
    satSRPC1.setSourceIndexed(false);
    satSRPC1.setSatIndexed(true);
    satSRPC1.setInitialVariance(0.2);

    Variable satSRPC2(TypeID::dSRPC2, &satOrbModel);
    satSRPC2.setSourceIndexed(false);
    satSRPC2.setSatIndexed(true);
    satSRPC2.setInitialVariance(0.1);

    Variable satSRPC3(TypeID::dSRPC3, &satOrbModel);
    satSRPC3.setSourceIndexed(false);
    satSRPC3.setSatIndexed(true);
    satSRPC3.setInitialVariance(0.1);

    Variable satSRPC4(TypeID::dSRPC4, &satOrbModel);
    satSRPC4.setSourceIndexed(false);
    satSRPC4.setSatIndexed(true);
    satSRPC4.setInitialVariance(0.1);

    Variable satSRPC5(TypeID::dSRPC5, &satOrbModel);
    satSRPC5.setSourceIndexed(false);
    satSRPC5.setSatIndexed(true);
    satSRPC5.setInitialVariance(0.1);


    // Satellite Clock
    WhiteNoiseModel2 satClkModel;
    satClkModel.addTypeID(TypeID::dcdtSat);
    satClkModel.setSigma(3e5);

    Variable satClk(TypeID::dcdtSat, &satClkModel);
    satClk.setSourceIndexed(false);
    satClk.setSatIndexed(true);
    satClk.setDefaultCoefficient(-1.0);
    satClk.setDefaultForced(true);

    // Station Clock
    WhiteNoiseModel2 staClkModel;
    staClkModel.addTypeID(TypeID::dcdtSta);
    staClkModel.setSigma(1e2);

    Variable staClk(TypeID::dcdtSta, &staClkModel);
    staClk.setSourceIndexed(true);
    staClk.setSatIndexed(false);
    staClk.setDefaultCoefficient(+1.0);
    staClk.setDefaultForced(true);


    // Station Troposphere
    TropoRandomWalkModel2 tropoModel;

    Variable staTropo(TypeID::wetMap, &tropoModel);
    staTropo.setSourceIndexed(true);
    staTropo.setSatIndexed(false);
    staTropo.setInitialVariance(0.5);


    // Prefit of Observations
    Variable prefitPC( TypeID::prefitC );


    // PC Equation for Reference Station
    Equation equPCRef( prefitPC );
    equPCRef.addVariable( satX );
    equPCRef.addVariable( satY );
    equPCRef.addVariable( satZ );
    equPCRef.addVariable( satVX );
    equPCRef.addVariable( satVY );
    equPCRef.addVariable( satVZ );
    equPCRef.addVariable( satSRPC1, true, 0.0 );
    equPCRef.addVariable( satSRPC2, true, 0.0 );
    equPCRef.addVariable( satSRPC3, true, 0.0 );
    equPCRef.addVariable( satSRPC4, true, 0.0 );
    equPCRef.addVariable( satSRPC5, true, 0.0 );
    equPCRef.addVariable( satClk, true, -1.0 );
    equPCRef.addVariable( staClk, true, +1.0 );
    equPCRef.addVariable( staTropo );


    // State Store
    StateStore stateStore;
    stateStore.setEphStore( bceStore );
    stateStore.setMSCStore( mscStore );


    // Time Update
    TimeUpdate timeUpdate;
    timeUpdate.setStateStore( stateStore );

    // Measurement Update
    MeasUpdate measUpdate;
    measUpdate.setStateStore( stateStore );


    // keep only necessary types
    TypeIDSet keepTypes;


    gnssDataMap gData, gDataBak;

    SourceIDSet sourceSet;
    SatIDSet satSet;

    SourceID source;

    SatIDSet allSatSet;

    for(int i=1; i<MAX_PRN_GPS; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        allSatSet.insert(sat);
    }


    sourceValueMap sourceClock;


    Vector<double> orbit(orbit0);
    Vector<double> srpc(srpc0);
    satVectorMap satOrbit(satOrbit0);
    satVectorMap satSRPC(satSRPC0);

    Vector<double> r_itrs(3,0.0), v_itrs(3,0.0);
    Vector<double> r_icrs(3,0.0), v_icrs(3,0.0);


    rkf78.setCurrentTime(tt0);
    rkf78.setCurrentState(satOrbit0);


    cout << fixed << setprecision(3);

    CommonTime gps,utc,tt;

    double dt(0.0);

    // process epoch by epoch
    while( obsStreams.readEpochData(gData) )
    {
        gps = gData.begin()->first;

        dt = gps - gps0;

        if(dt < 0.1) continue;
//        if(int(dt)%60 != 0) continue;
        if(dt > 4*120*30.0) break;


        try
        {
            utc = refSys.GPS2UTC(gps);

            t2cRaw = refSys.T2CMatrix(utc);
            t2cDot = refSys.dT2CMatrix(utc);

            tt = refSys.GPS2TT(gps);

            satOrbit = rkf78.integrateTo(tt);

            // set basicModel with predicted orbit
            basicModel2.setSatState( satOrbit );

            // set satOrbModel with predicted orbit
            satOrbModel.setSatOrbit( satOrbit );

            stateStore.setStateEpoch( gps );
            stateStore.updateNominalPos( gData );

            // preprocessing
            gData >> requireObs             // C1C,C2W
                  >> cc2noncc               // C1W,C2W
                  >> linearPC               // PC
                  >> basicModel             // r,v,cdt of sat
                                            // rho,elev,azim of sat-sta
                  >> elevWeights            // weight
                  >> gravDelay              // gravitational delay
                  >> correctObs             // tide,ARP,PCO,PCV of sta
                  >> computeTM              // troposphere
                  >> satPCenter             // PCO,PCV of sat
                  >> linearPC               // PC
                  >> prefitPCWithSatClock
                  >> computeStaClock;

            sourceClock = computeStaClock.getSourceClock();

            basicModel2.setSourceClock(sourceClock);

            gData >> basicModel2            // drho/drsat,drho/dvsat
                  >> prefitPCWithStaClock;

            for( gnssDataMap::iterator gdmIt = gData.begin();
                 gdmIt != gData.end();
                 ++gdmIt )
            {
                for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                     sdmIt != gdmIt->second.end();
                     ++sdmIt )
                {
                    cout << sdmIt->first << endl;

                    for( satTypeValueMap::iterator stvmIt = sdmIt->second.begin();
                         stvmIt != sdmIt->second.end();
                         ++stvmIt )
                    {
                        double prefit( 0.0 );
                        prefit = stvmIt->second[TypeID::prefitCWithStaClock];
                        stvmIt->second[TypeID::prefitC] = prefit;
                        cout << stvmIt->first << setw(15) << prefit;
                    }
                    cout << endl;
                }
            }

            break;

            sourceSet = gData.getSourceIDSet();
            satSet = gData.getSatIDSet();

            if(sourceSet.size()==0 || satSet.size()==0)
            {
                cerr << "gnssDataMap is empty." << endl;
                break;
            }


            // time update
            timeUpdate.clearEquations();
            sourceSet = gData.getSourceIDSet();
            for( SourceIDSet::iterator source_it = sourceSet.begin();
                 source_it != sourceSet.end();
                 ++source_it )
            {
                timeUpdate.addEquation2Source( equPCRef, *source_it );
            }

            timeUpdate.Process( gData );


            // measurement update
            measUpdate.clearEquations();
            sourceSet = gData.getSourceIDSet();
            for( SourceIDSet::iterator source_it = sourceSet.begin();
                 source_it != sourceSet.end();
                 ++source_it )
            {
                measUpdate.addEquation2Source( equPCRef, *source_it );
            }

            measUpdate.Process( gData );

        }
        catch(...)
        {
            cerr << "processing error." << endl;
            break;
        }


        stateStore.getSourceIDSet(sourceSet);
        stateStore.getSatIDSet(satSet);

        double dSatX(0.0),  dSatY(0.0),  dSatZ(0.0);
        double dSatVX(0.0), dSatVY(0.0), dSatVZ(0.0);
        double dSRPC1(0.0), dSRPC2(0.0), dSRPC3(0.0);
        double dSRPC4(0.0), dSRPC5(0.0);
        double comClk(0.0), refClk(0.0);


        cout << setprecision(1)
             << setw(4) << CivilTime(gps).year
             << setw(4) << CivilTime(gps).month
             << setw(4) << CivilTime(gps).day
             << setw(4) << CivilTime(gps).hour
             << setw(4) << CivilTime(gps).minute
             << setprecision(6)
             << setw(10) << CivilTime(gps).second;
//        cout << endl;

        for(SatIDSet::iterator it = allSatSet.begin();
            it != allSatSet.end();
            ++it)
        {
            sat = *it;

            if(satSet.find(sat) == satSet.end())
            {
                cout << setw(30) << string("");
                continue;
            }

            if(satOrbit.find(sat) == satOrbit.end())
            {
                cout << setw(30) << string("");
                continue;
            }

            if(satSRPC.find(sat) == satSRPC.end())
            {
                cout << setw(30) << string("");
                continue;
            }

            try
            {
                r_itrs = sp3Store.getXvt(sat,gps).x.toVector();
                v_itrs = sp3Store.getXvt(sat,gps).v.toVector();
                refClk = sp3Store.getXvt(sat,gps).clkbias;
            }
            catch(...)
            {
                cout << setw(30) << string("");
                continue;
            }

            r_icrs = t2cRaw * r_itrs;
            v_icrs = t2cRaw * v_itrs + t2cDot * r_itrs;

            try
            {
                dSatX  = stateStore.getSolution(TypeID::dSatX, sat);
                dSatY  = stateStore.getSolution(TypeID::dSatY, sat);
                dSatZ  = stateStore.getSolution(TypeID::dSatZ, sat);
                dSatVX = stateStore.getSolution(TypeID::dSatVX, sat);
                dSatVY = stateStore.getSolution(TypeID::dSatVY, sat);
                dSatVZ = stateStore.getSolution(TypeID::dSatVZ, sat);
            }
            catch(...)
            {
                cout << setw(30) << string("");
                continue;
            }


            // update orbit
            orbit = satOrbit[sat];

            // (rx, ry, rz)
            orbit(0) += dSatX;  orbit(1) += dSatY;  orbit(2) += dSatZ;

            // (vx, vy, vz)
            orbit(3) += dSatVX; orbit(4) += dSatVY; orbit(5) += dSatVZ;

            for(int i=0; i<36+6*numSRP; ++i) orbit(i+6) = 0.0;

            // (drx/drx0, dry/dry0, drz/drz0)
            orbit( 6) = 1.0; orbit(10) = 1.0; orbit(14) = 1.0;

            // (drx/drx0, dry/dry0, dvz/dvz0)
            orbit(33) = 1.0; orbit(37) = 1.0; orbit(41) = 1.0;

            satOrbit[sat] = orbit;

            // update srpc
            srpc = satSRPC[sat];

            // (D0, Y0, B0, BC, BS)
            srpc(0) += dSRPC1;
            srpc(1) += dSRPC2;
            srpc(2) += dSRPC3; srpc(3) += dSRPC4; srpc(4) += dSRPC5;

            satSRPC[sat] = srpc;


            Vector<double> dXYZ(3,0.0);
            dXYZ(0) = orbit(0) - r_icrs(0);
            dXYZ(1) = orbit(1) - r_icrs(1);
            dXYZ(2) = orbit(2) - r_icrs(2);

            Vector<double> dRTN(3,0.0);
            dRTN = refSys.XYZ2RTN(dXYZ, r_icrs,v_icrs);

//            cout << sat;
            cout << setprecision(3)
                 << setw(10) << dRTN(0)
                 << setw(10) << dRTN(1)
                 << setw(10) << dRTN(2);
//            cout << setprecision(3)
//                 << setw(10) << orbit(3) - v_icrs(0)
//                 << setw(10) << orbit(4) - v_icrs(1)
//                 << setw(10) << orbit(5) - v_icrs(2)
//                 << setw(10) << dSRPC1
//                 << setw(10) << dSRPC2
//                 << setw(10) << dSRPC3
//                 << setw(10) << dSRPC4
//                 << setw(10) << dSRPC5;
        }
        cout << endl;

        // update srp
//        srp.setSRPCoeff(satSRPC);
//        gnss.setSRPModel(srp);
//        rkf78.setEquationOfMotion(gnss);

        // update rkf
        rkf78.setCurrentTime(tt);
        rkf78.setCurrentState(satOrbit);

        // reset state
        stateStore.setSolution(TypeID::dSatX,  0.0);
        stateStore.setSolution(TypeID::dSatY,  0.0);
        stateStore.setSolution(TypeID::dSatZ,  0.0);
        stateStore.setSolution(TypeID::dSatVX, 0.0);
        stateStore.setSolution(TypeID::dSatVY, 0.0);
        stateStore.setSolution(TypeID::dSatVZ, 0.0);
        stateStore.setSolution(TypeID::dSRPC1, 0.0);
        stateStore.setSolution(TypeID::dSRPC2, 0.0);
        stateStore.setSolution(TypeID::dSRPC3, 0.0);
        stateStore.setSolution(TypeID::dSRPC4, 0.0);
        stateStore.setSolution(TypeID::dSRPC5, 0.0);

    } // End of 'while( obsStreams.readEpochData(gData) )'

    double clock_end( Counter::now() );

//    cout << clock_end - clock_start << endl;

    return 0;

}

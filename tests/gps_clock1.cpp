#pragma ident "$Id$"

#include <algorithm>

#include "ConfDataReader.hpp"

#include "EOPDataStore2.hpp"
#include "LeapSecStore.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

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

#include "BasicModel1.hpp"

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
    string confFileName("clock.conf");

    ConfDataReader confReader;

    try
    {
        confReader.open(confFileName);
    }
    catch(...)
    {
        cerr << "conf file '" << confFileName << "' open error." << endl;
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
        cerr << "eop file '" << eopFileName << "' load error." << endl;
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
        cerr << "leap second file '" << lsFileName << "' load error." << endl;
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
        cerr << "sp3 file list '" << sp3FileListName << "' open error." << endl;
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
        cerr << "clk file list '" << clkFileListName << "' open error." << endl;
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
            cerr << "sp3 file '" << sp3File << "' load error." << endl;

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
            cerr << "clk file '" << clkFile << "' load error." << endl;

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
        cerr << "nav file list '" << navFileListName << "' open error." << endl;
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
        navFile = (*nav_it);

        try
        {
            bceStore.loadFile(navFile);
        }
        catch(...)
        {
            cerr << "nav file '" << navFile << "' load error." << endl;

            ++nav_it;
            continue;
        }

        ++nav_it;
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


    // msc file
    string mscFileName;
    try
    {
        mscFileName = confReader.getValue("mscFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "msc file name get error." << endl;
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
        cerr << "atx file name get error." << endl;
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
        cerr << "blq file name get error." << endl;
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
        cerr << "gpt2 file name get error." << endl;
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
    map<SourceID, string> sourceRecTypeMap;


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
        sourceRecTypeMap[source] = roh.recType;
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


    // BasicModel
    BasicModel1 basicModel;
    basicModel.setSP3Store(sp3Store);
    basicModel.setBCEStore(bceStore);
    basicModel.setMSCStore(mscStore);
    basicModel.setMinElev(12.5);
    basicModel.setDefaultObs(SatID::systemGPS, TypeID::PC);


    // ComputeElevWeights
    ComputeElevWeights elevWeights;


    // GravitationalDelay
    GravitationalDelay gravDelay;
    gravDelay.setMSCStore(mscStore);


    // ComputeSatPCenter
    ComputeSatPCenter satPCenter;
    satPCenter.setEphStore(sp3Store);
    satPCenter.setMSCStore(mscStore);
    satPCenter.setAntexReader(antexReader);


    // ComputeTides
    ComputeTides staTides;
    staTides.setBLQReader(blqStore);
    staTides.setEOPDataStore(eopStore);


    // CorrectObservables
    CorrectObservables correctObs;
    correctObs.setEphStore(sp3Store);
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


    // Station Clock
    WhiteNoiseModel2 staClkModel;
    staClkModel.addTypeID( TypeID::dcdtSta );
    staClkModel.setSigma( 1e2 );

    Variable staClk(TypeID::dcdtSta, &staClkModel);
    staClk.setSourceIndexed(true);
    staClk.setSatIndexed(false);
    staClk.setDefaultCoefficient(+1.0);
    staClk.setDefaultForced(true);

    // Satellite Clock
    WhiteNoiseModel2 satClkModel;
    satClkModel.addTypeID( TypeID::dcdtSat );
    satClkModel.setSigma( 3e5 );

    Variable satClk(TypeID::dcdtSat, &satClkModel);
    satClk.setSourceIndexed(false);
    satClk.setSatIndexed(true);
    satClk.setDefaultCoefficient(-1.0);
    satClk.setDefaultForced(true);

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
    equPCRef.addVariable( satClk, true, -1.0 );
    equPCRef.addVariable( staClk, true, +1.0 );
    equPCRef.addVariable( staTropo );


    // State Store
    StateStore stateStore;
    stateStore.setEphStore( sp3Store );
    stateStore.setMSCStore( mscStore );


    // Time Update
    TimeUpdate timeUpdate;
    timeUpdate.setStateStore( stateStore );

    // Measurement Update
    MeasUpdate measUpdate;
    measUpdate.setStateStore( stateStore );


    // keep only necessary types
    TypeIDSet keepTypes;

    keepTypes.insert(TypeID::weightC);      // weight of code observation
    keepTypes.insert(TypeID::prefitC);      // prefit residual of PC
    keepTypes.insert(TypeID::wetMap);       // coefficient of sta wet tropo
    keepTypes.insert(TypeID::cdtSat);       // clock bias of sat


    gnssDataMap gData;

    SourceIDSet sourceSet;
    SatIDSet satSet;

    SourceID source;
    SatID sat;

    SatIDSet allSatSet;

    for(int i=1; i<=MAX_PRN_GPS; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        allSatSet.insert(sat);
    }

    cout << fixed << setprecision(1);

    CommonTime gps;

    // process epoch by epoch
    while( obsStreams.readEpochData(gData) )
    {
        gps = gData.begin()->first;

        double dt(gps - gps0);

        if(dt < 0.1) continue;
//        if(dt > 10*30.0) break;

        stateStore.setStateEpoch( gps );
        stateStore.updateNominalPos( gData );

        try
        {
            // preprocessing
            gData >> requireObs
                  >> cc2noncc
                  >> linearPC
                  >> basicModel
                  >> elevWeights
                  >> gravDelay
                  >> satPCenter
                  >> correctObs
                  >> computeTM
                  >> linearPC
                  >> prefitPCWithoutClock;
//                  >> prefitPCWithSatClock
//                  >> computeStaClock
//                  >> prefitPCWithStaClock;

            for( gnssDataMap::iterator gdmIt = gData.begin();
                 gdmIt != gData.end();
                 ++gdmIt )
            {
                for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                     sdmIt != gdmIt->second.end();
                     ++sdmIt )
                {
                    for( satTypeValueMap::iterator stvmIt = sdmIt->second.begin();
                         stvmIt != sdmIt->second.end();
                         ++stvmIt )
                    {
                        double prefit( 0.0 );
                        prefit = stvmIt->second[TypeID::prefitCWithoutClock];
                        stvmIt->second[TypeID::prefitC] = prefit;
                    }
                }
            }

            sourceSet = gData.getSourceIDSet();
            satSet = gData.getSatIDSet();

            if(sourceSet.size()==0 || satSet.size()==0) break;


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


            gData.keepOnlyTypeID(keepTypes);


            // meas update
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


        cout << setw(4) << CivilTime(gps).year
             << setw(4) << CivilTime(gps).month
             << setw(4) << CivilTime(gps).day
             << setw(4) << CivilTime(gps).hour
             << setw(4) << CivilTime(gps).minute
             << setprecision(6)
             << setw(10) << CivilTime(gps).second;

        stateStore.getSatIDSet(satSet);

        TypeID type1(TypeID::dcdtSat);
        double comClkBias(0.0);
        double refClkBias(0.0);

        for(SatIDSet::iterator it = allSatSet.begin();
            it != allSatSet.end();
            ++it)
        {
            sat = (*it);

            if(satSet.find(sat) == satSet.end())
            {
                cout << setw(30) << string("");
                continue;
            }

            try
            {
                comClkBias = stateStore.getSolution(type1,sat)/C_MPS;
            }
            catch(...)
            {
                cout << setw(30) << string("");
                continue;
            }

            try
            {
                refClkBias = sp3Store.getXvt(sat,gps).clkbias;
            }
            catch(...)
            {
                cout << setw(30) << string("");
                continue;
            }

//            cout << setw(10) << sat;
            cout << setprecision(3)
                 << setw(15) << comClkBias*1e9
                 << setw(15) << refClkBias*1e9;
        }
        cout << endl;

    } // End of 'while( obsStreams.readEpochData(gData) )'


    double clock_end( Counter::now() );

    cerr << "time elapsed: " << setw(10) << clock_end - clock_start << endl;

    return 0;
}

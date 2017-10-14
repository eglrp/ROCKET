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


    SourceID source;
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


        source = SourceID(SourceID::Mixed,roh.markerName,roh.markerNumber);

        string station( source.sourceName );
        transform(station.begin(),station.end(),station.begin(),::toupper);


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


    gnssDataMap gData;

    SatID sat;

    SourceIDSet sourceSet;
    SatIDSet satSet;

    sourceValueMap sourceClock;
    satValueMap satClock;
    double clock;

    map<SourceID,int> sourceIndex;
    map<SatID,int> satIndex;

    Matrix<double> noise;

    Vector<double> state;
    Matrix<double> covar;

    TypeIDSet keepTypes;
    keepTypes.insert(TypeID::prefitCWithStaClock);
    keepTypes.insert(TypeID::weightC);
    keepTypes.insert(TypeID::wetMap);


    cout << fixed << setprecision(3);

    CommonTime gps;
    double dt(30.0);

    bool first(true);

    // process epoch by epoch
    while( obsStreams.readEpochData(gData) )
    {
        gps = gData.begin()->first;

        try
        {
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
                  >> computeStaClock
                  >> prefitPCWithStaClock;

            // source clock
            sourceClock = computeStaClock.getSourceClock();

            // sat clock
            satClock = basicModel.getSatClock();


            //////// start of filter initialization ////////

            sourceSet = gData.getSourceIDSet();
            satSet = gData.getSatIDSet();

            // number of source, sat and unknown
            int numSource( sourceSet.size() );
            int numSat( satSet.size() );
            int numUnknown( numSource*2 + numSat*1 );

            if(numSource == 0 || numSat == 0) continue;

            int id(0);

            // if it is first time, initialize the filter
            if(first)
            {
                state.resize(numUnknown,0.0);
                covar.resize(numUnknown,numUnknown,0.0);

                id = 0;

                // initialize source-related parameters
                for(SourceIDSet::iterator itSource = sourceSet.begin();
                    itSource != sourceSet.end();
                    ++itSource)
                {
                    source = *itSource;

                    // initialize the clock of this source
                    state(id+0) = 0.0;

                    // initialize the tropo of this source
                    state(id+1) = 0.0;

                    // initialize the covariance of clock
                    covar(id+0,id+0) = 1e2 * 1e2;

                    // initialize the covariance of tropo
                    covar(id+1,id+1) = 0.5 * 0.5;

                    // set the index of this source
                    sourceIndex[source] = id;

                    id = id + 2;
                }

                // initialize sat-related parameters
                for(SatIDSet::iterator itSat = satSet.begin();
                    itSat != satSet.end();
                    ++itSat)
                {
                    sat = *itSat;

                    clock = satClock[sat];

                    // initialize the clock of this sat
                    // be aware that the estimated one IS the true one
                    state(id) = clock;

                    // initialize the covariance of clock
                    covar(id,id) = 1e2 * 1e2;

                    // set the index of this sat
                    satIndex[sat] = id;

                    id = id + 1;
                }

                // finished
                first = false;
            }

            if(state.size() != numUnknown)
            {
                Vector<double> state2(numUnknown,0.0);
                Matrix<double> covar2(numUnknown,numUnknown,0.0);
                map<SourceID,int> sourceIndex2;
                map<SatID,int> satIndex2;

                int id2(0);

                for(SourceIDSet::iterator itSource = sourceSet.begin();
                    itSource != sourceSet.end();
                    ++itSource)
                {
                    source = *itSource;
                    sourceIndex2[source] = id2;

                    if(sourceIndex.find(source) != sourceIndex.end())
                    {
                        id = sourceIndex[source];

                        state2(id2+0) = state(id+0);
                        state2(id2+1) = state(id+1);

                        for(int i=0; i<2; ++i)
                        {
                            for(int j=0; j<2; ++j)
                            {
                                covar2(id2+i,id2+j) = covar(id+i,id+j);
                            }
                        }

                        id2 = id2 + 2;
                    }
                    else
                    {
                        state2(id2+0) = 0.0;
                        state2(id2+1) = 0.0;

                        covar2(id2+0,id2+0) = 1e2 * 1e2;
                        covar2(id2+1,id2+1) = 0.5 * 0.5;

                        id2 = id2 + 2;
                    }
                }

                for(SatIDSet::iterator itSat = satSet.begin();
                    itSat != satSet.end();
                    ++itSat)
                {
                    sat = *itSat;
                    satIndex2[sat] = id2;

                    if(satIndex.find(sat) != satIndex.end())
                    {
                        id = satIndex[sat];

                        state2(id2) = state(id);
                        covar2(id2,id2) = covar(id,id);

                        id2 = id2 + 1;
                    }
                    else
                    {
                        state2(id2) = satClock[sat];

                        covar2(id2,id2) = 1e2 * 1e2;

                        id2 = id2 + 1;
                    }
                }

                state = state2;
                covar = covar2;
                sourceIndex = sourceIndex2;
                satIndex = satIndex2;
            }

//            cout << "after initialization" << endl;
//            for(int i=0; i<numUnknown; i=i+1)
//            {
//                cout << setw(15) << state(i)
//                     << setw(25) << covar(i,i)
//                     << endl;
//            }
//            cout << endl;

            //////// end of filter initialization ////////


            //////// start of clock constraint ////////

            double obs(0.0);
            for(SatIDSet::iterator itSat = satSet.begin();
                itSat != satSet.end();
                ++itSat)
            {
                sat = *itSat;
                obs += satClock[sat];
            }

            double com(0.0);
            for(int i=numSource*2; i<numUnknown; i=i+1)
            {
                com += state(i);
            }

            // omc
            double omc(obs - com);

            // weight
            double weight(2.0);

            // h
            Vector<double> h(numUnknown,0.0);
            for(int i=numSource*2; i<numUnknown; i=i+1)
            {
                h(i) = 1.0;
            }

            // p * h'
            Vector<double> pht(numUnknown,0.0);
            for(int i=0; i<numUnknown; ++i)
            {
                for(int j=0; j<numUnknown; ++j)
                {
                    if(h(j) != 0.0) pht(i) += covar(i,j) * h(j);
                }
            }

            // h * p * h'
            double hpht( 0.0 );
            for(int i=0; i<numUnknown; ++i)
            {
                if(h(i) != 0.0) hpht += h(i) * pht(i);
            }

            double beta( 1.0/weight + hpht );

            // kalman gain
            Vector<double> gamma( pht/beta );

            // state update
            state = state + gamma*omc;

            // covariance update
            for(int i=0; i<numUnknown; ++i)
            {
                covar(i,i) = covar(i,i) - gamma(i)*pht(i);

                for(int j=i+1; j<numUnknown; ++j)
                {
                    covar(i,j) = covar(j,i) = covar(i,j) - gamma(i)*pht(j);
                }
            }

//            cout << "after clock constraint" << endl;
//            for(int i=0; i<numUnknown; i=i+1)
//            {
//                cout << setw(15) << state(i)
//                     << setw(25) << covar(i,i)
//                     << endl;
//            }
//            cout << endl;

//            break;

            //////// end of clock constraint ////////


            //////// start of measment update ////////

            int idSource(0), idSat(0);

            double wmf(0.0);

            for(gnssDataMap::iterator gdmIt = gData.begin();
                gdmIt != gData.end();
                ++gdmIt)
            {
                for(sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                    sdmIt != gdmIt->second.end();
                    ++sdmIt)
                {
                    source = sdmIt->first;
                    idSource = sourceIndex[source];

                    for(satTypeValueMap::iterator stvmIt = sdmIt->second.begin();
                        stvmIt != sdmIt->second.end();
                        ++stvmIt)
                    {
                        sat = stvmIt->first;
                        idSat = satIndex[sat];

                        // omc, weight, wmf
                        omc = stvmIt->second[TypeID::prefitCWithStaClock];
                        weight = stvmIt->second[TypeID::weightC];
                        wmf = stvmIt->second[TypeID::wetMap];

                        omc = omc - 1.0 * state(idSource+0)
                                  - wmf * state(idSource+1)
                                  + 1.0 * state(idSat+0);

//                        cout << omc << endl;
//
//                        break;

                        // h
                        h.resize(numUnknown,0.0);
                        h(idSource+0) = +1.0;
                        h(idSource+1) = wmf;
                        h(idSat+0) = -1.0;

//                        for(int i=0; i<numUnknown; ++i)
//                        {
//                            if(h(i) != 0.0) omc = omc - h(i) * state(i);
//                        }

                        // p * h'
                        Vector<double> pht(numUnknown,0.0);

                        for(int i=0; i<numUnknown; ++i)
                        {
                            for(int j=0; j<numUnknown; ++j)
                            {
                                if(h(j) != 0.0) pht(i) += covar(i,j) * h(j);
                            }
                        }

                        // h * p * h'
                        double hpht(0.0);

                        for(int i=0; i<numUnknown; ++i)
                        {
                            if(h(i) != 0.0) hpht += h(i) * pht(i);
                        }

                        double beta( 1.0/weight + hpht );

                        // kalman gain
                        Vector<double> gamma( pht/beta );

                        // state update
                        state = state + gamma*omc;

                        // covariance update
                        for(int i=0; i<numUnknown; ++i)
                        {
                            covar(i,i) = covar(i,i) - gamma(i)*pht(i);

                            for(int j=i+1; j<numUnknown; ++j)
                            {
                                covar(i,j) = covar(j,i) = covar(i,j) - gamma(i)*pht(j);
                            }
                        }

                    } // End of for(satTypeValueMap::iterator stvmIt = ...)

//                    break;

                } // End of for(sourceDataMap::iterator sdmIt = ...)

//                break;

            } // End of for(gnssDataMap::iterator gdmIt = ...)

//            break;
            cout << "after meas update" << endl;
            for(int i=numSource*2; i<numSource*2+1; i=i+1)
            {
                cout << setprecision(3) << setw(15) << state(i)/C_MPS*1e9
                     << setprecision(6) << setw(15) << covar(i,i)
                     << endl;
            }

//            cout << setw(4) << CivilTime(gps).year
//                 << setw(4) << CivilTime(gps).month
//                 << setw(4) << CivilTime(gps).day
//                 << setw(4) << CivilTime(gps).hour
//                 << setw(4) << CivilTime(gps).minute
//                 << setprecision(3)
//                 << setw(10) << CivilTime(gps).second;
//
//            cout << setw(5) << numSource
//                 << setw(5) << numSat
//                 << setw(5) << numUnknown;
//
//            for(SatIDSet::iterator itSat = satSet.begin();
//                itSat != satSet.end();
//                ++itSat)
//            {
//                sat = *itSat;
//                id = satIndex[sat];
//
//                double clock1 = state(id) / C_MPS;
//
//                double clock2(0.0);
//                try
//                {
//                    clock2 = sp3Store.getXvt(sat,gps).clkbias;
//                }
//                catch(...)
//                {
//                    continue;
//                }
//
////                cout << sat;
//                cout << setprecision(3)
//                     << setw(15) << clock1 * 1e9
//                     << setw(15) << clock2 * 1e9;
////                cout << endl;
//            }
//            cout << endl;

            //////// end of measment update ////////


            //////// start of time update ////////

            gps = gps + dt;

            // time update for source-related parameters

            // process noise
            noise.resize(2,2,0.0);
            noise(0,0) = 1e+2 * dt;
            noise(1,1) = 3e-8 * dt;

            for(SourceIDSet::iterator itSource = sourceSet.begin();
                itSource != sourceSet.end();
                ++itSource)
            {
                source = *itSource;

                id = sourceIndex[source];

                // state update
//                state(id) = 0.0;

                // covariance update
                covar(id+0,id+0) += noise(0,0);
                covar(id+1,id+1) += noise(1,1);
            }


            // time update for sat-related parameters

            // process noise
            noise.resize(1,1,0.0);
            noise(0,0) = 1e-3 * dt;

            for(SatIDSet::iterator itSat = satSet.begin();
                itSat != satSet.end();
                ++itSat)
            {
                sat = *itSat;

                id = satIndex[sat];

                // state update
//                state(id) = 0.0;

                // covariance update
                covar(id,id) += noise(0,0);
            }

            cout << "after time update" << endl;
            for(int i=numSource*2; i<numSource*2+1; i=i+1)
            {
                cout << setprecision(3) << setw(15) << state(i)/C_MPS*1e9
                     << setprecision(6) << setw(15) << covar(i,i)
                     << endl;
            }

            //////// end of time update ////////

            if(gps-gps0 > 100 * 30.0) break;

        }
        catch(...)
        {
            cerr << "processing error." << endl;
            break;
        }

    } // End of 'while( obsStreams.readEpochData(gData) )'

    double clock_end( Counter::now() );

    cerr << "time elapsed: " << setw(10) << clock_end - clock_start << endl;

    return 0;

}

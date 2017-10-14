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

    for(int i=1; i<=MAX_PRN_GPS; ++i)
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

    // ComputeLinear, prefit PC for POD
    ComputeLinear prefitPCForPOD;
    prefitPCForPOD.addLinear(SatID::systemGPS, linearComb.pcPrefitForPOD);


    // ComputeTides
    ComputeTides tides;
    tides.setBLQReader(blqStore);
    tides.setEOPDataStore(eopStore);


    // BasicModel
    BasicModel basicModel;
    basicModel.setEphStore(bceStore);
    basicModel.setMSCStore(mscStore);
    basicModel.setMinElev(12.5);
    basicModel.setDefaultObs(SatID::systemGPS, TypeID::PC);


    // BasicModel2
    BasicModel2 basicModel2;
    basicModel2.setReferenceSystem(refSys);
    basicModel2.setMSCStore(mscStore);
    basicModel2.setMinElev(12.5);


    // ComputeElevWeights
    ComputeElevWeights elevWeights;


    // GravitationalDelay
    GravitationalDelay gravDelay;
    gravDelay.setMSCStore(mscStore);


    // ComputeSatPCenter
    ComputeSatPCenter2 satPCenter;
    satPCenter.setMSCStore(mscStore);
    satPCenter.setAntexReader(antexReader);


    // ComputeTides
    ComputeTides staTides;
    staTides.setBLQReader(blqStore);
    staTides.setEOPDataStore(eopStore);


    // CorrectObservables
    CorrectObservables2 correctObs;
    correctObs.setMSCStore(mscStore);
    correctObs.setTideCorr(staTides);
    correctObs.setSourceMonument(sourceMonumentMap);
    correctObs.setSourceAntenna(sourceAntennaMap);


    // ComputeTropModel
    ViennaTropModel viennaTM;
    viennaTM.loadFile(gpt2FileName);
    ComputeTropModel computeTM;
    computeTM.setTropModel(viennaTM);
    computeTM.setMSCStore(mscStore);


    // ComputeStaClock
    ComputeStaClock computeStaClock;


    gnssDataMap gData, gDataBak;

    SourceIDSet sourceSet;
    SatIDSet satSet;


    sourceVectorMap sourcePosECI, sourcePosECEF;
    sourceValueMap sourceClock;

    satVectorMap satPosECI, satPosECEF;

    Vector<double> orbit;
    Vector<double> srpc;
    double clock;

    satVectorMap satOrbit(satOrbit0);
    satVectorMap satSRPC(satSRPC0);
    satValueMap satClock(satClock0);

    map<SourceID,int> sourceIndex;
    map<SatID,int> satIndex;

    Vector<double> state;
    Matrix<double> covar;

    Matrix<double> phi, phit;
    Matrix<double> noise;

    cout << fixed;

    CommonTime gps,utc,tt;

    double dt(30.0);

    bool first(true);

    // process epoch by epoch
    while( obsStreams.readEpochData(gData) )
    {
        gps = gData.begin()->first;

        utc = refSys.GPS2UTC(gps);
        tt = refSys.GPS2TT(gps);

        t2cRaw = refSys.T2CMatrix(utc);
        t2cDot = refSys.dT2CMatrix(utc);

        try
        {
            gDataBak = gData;

            // preprocessing
            gDataBak >> requireObs             // C1C,C2W
                     >> cc2noncc               // C1W,C2W
                     >> linearPC                // PC
                     >> basicModel              // r,v,cdt of sat
                                                // rho,elev,azim of sat-sta
                     >> elevWeights            // weight
                     >> correctObs             // disp,ARP,PCO,PCV of sta
                     >> computeTM              // tropo delay
                     >> gravDelay              // gravi delay
                     >> linearPC               // PC
                     >> prefitPCWithSatClock
                     >> computeStaClock;

            // sat clock
            satClock = basicModel.getSatClock();

            // source clock
            sourceClock = computeStaClock.getSourceClock();

            basicModel2.setSourceClock( sourceClock );
            basicModel2.setSatState( satOrbit );

            gData >> requireObs
                  >> cc2noncc
                  >> basicModel2
                  >> elevWeights
                  >> satPCenter
                  >> correctObs
                  >> computeTM
                  >> gravDelay
                  >> linearPC
                  >> prefitPCForPOD;

            sourcePosECI = basicModel2.getSourcePosECI();
            sourcePosECEF = basicModel2.getSourcePosECEF();

            satPosECI = basicModel2.getSatPosECI();
            satPosECEF = basicModel2.getSatPosECEF();


            //////// start of initialization ////////

            sourceSet = gData.getSourceIDSet();
            satSet = gData.getSatIDSet();

            // number of source, sat and unknown
            // for source, estimate clock and tropo, 2 for each source
            // for sat, estimate orbit, srpc and clock, 12 for each sat
            int numSource( sourceSet.size() );
            int numSat( satSet.size() );
            int numUnknown( numSource*2 + numSat*12 );

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
                    // be aware that the estimated one IS NOT the true one
                    state(id+0) = 0.0;

                    // initialize the tropo of this source
                    state(id+1) = 0.0;

                    // initialize the covariance of clock
                    covar(id+0,id+0) = 1e2 * 1e2;

                    // initialize the covariance of tropo
                    covar(id+1,id+1) = 0.5 * 0.5;

                    // setup the index of this source
                    sourceIndex[source] = id;

                    id = id + 2;
                }

                // initialize sat-related parameters
                for(SatIDSet::iterator itSat = satSet.begin();
                    itSat != satSet.end();
                    ++itSat)
                {
                    sat = *itSat;

                    orbit = satOrbit[sat];
                    srpc = satSRPC[sat];
                    clock = satClock[sat];

                    // initialize the pos and its covariance of this sat
                    for(int i=0; i<3; ++i)
                    {
                        state(id+i) = orbit(i);
                        covar(id+i,id+i) = 1.0 * 1.0;
                    }

                    // initialize the vel and its covariance of this sat
                    for(int i=3; i<6; ++i)
                    {
                        state(id+i) = orbit(i);
                        covar(id+i,id+i) = 0.001 * 0.001;
                    }

                    // initialize the srpc and its covariance of this sat
                    for(int i=6; i<11; ++i)
                    {
                        state(id+i) = srpc(i-6);
                    }

                    covar(id+ 6,id+ 6) = 1.0 * 1.0;
                    covar(id+ 7,id+ 7) = 1e-3*1e-3;
                    covar(id+ 8,id+ 8) = 1e-3*1e-3;
                    covar(id+ 9,id+ 9) = 1e-3*1e-3;
                    covar(id+10,id+10) = 1e-3*1e-3;

                    // initialize the clock of this sat
                    // be aware that the estimated one IS the true one
                    state(id+11) = clock;

                    // initialize the covariance of clock
                    covar(id+11,id+11) = 1e2 * 1e2;

                    // setup the index of this sat
                    satIndex[sat] = id;

                    id = id + 12;
                }

                // finish filter initialization
                first = false;
            }

            // in the following epoches, if the state size changes,
            // reset the state, covariance and index of source and sat
            if(state.size() != numUnknown)
            {
                Vector<double> state2(numUnknown,0.0);
                Matrix<double> covar2(numUnknown,numUnknown,0.0);
                map<SourceID,int> sourceIndex2;
                map<SatID,int> satIndex2;

                int id2(0);

                // source-related parameters
                for(SourceIDSet::iterator itSource = sourceSet.begin();
                    itSource != sourceSet.end();
                    ++itSource)
                {
                    source = *itSource;
                    sourceIndex2[source] = id2;

                    // for old source, initialize it from previous state
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
                    // for new source, initialize it with empirical info
                    else
                    {
                        state2(id2+0) = 0.0;
                        state2(id2+1) = 0.0;

                        covar2(id2+0,id2+0) = 1e2 * 1e2;
                        covar2(id2+1,id2+1) = 0.5 * 0.5;

                        id2 = id2 + 2;
                    }
                }


                // sat-related parameters
                for(SatIDSet::iterator itSat = satSet.begin();
                    itSat != satSet.end();
                    ++itSat)
                {
                    sat = *itSat;
                    satIndex2[sat] = id2;

                    // for old sat, initialize it from previous state
                    if(satIndex.find(sat) != satIndex.end())
                    {
                        id = satIndex[sat];

                        for(int i=0; i<12; ++i)
                        {
                            state2(id2+i) = state(id+i);
                        }

                        for(int i=0; i<12; ++i)
                        {
                            for(int j=0; j<12; ++j)
                            {
                                covar2(id2+i,id2+j) = covar(id+i,id+j);
                            }
                        }

                        id2 = id2 + 12;
                    }
                    // for new sat, initialize it with empirical info
                    else
                    {
                        try
                        {
                            rsat_t = sp3Store.getXvt(sat,gps).x.toVector();
                            vsat_t = sp3Store.getXvt(sat,gps).v.toVector();
                        }
                        catch(...)
                        {
                            cerr << "new sat initialize error." << endl;
                            return 1;
                        }

                        rsat_c = t2cRaw * rsat_t;
                        vsat_c = t2cRaw * vsat_t + t2cDot * rsat_t;

                        for(int i=0; i<3; ++i) state2(id2+i+0) = rsat_c(i);
                        for(int i=0; i<3; ++i) state2(id2+i+3) = vsat_c(i);
                        for(int i=0; i<5; ++i) state2(id2+i+6) = srpc0(i);

                        state2(id2+11) = satClock[sat];

                        for(int i=0; i<3; ++i)
                        {
                            covar2(id2+i,id2+i) = 1.0 * 1.0;
                        }

                        for(int i=3; i<6; ++i)
                        {
                            covar2(id2+i,id2+i) = 0.001 * 0.001;
                        }

                        covar2(id2+ 6,id2+ 6) = 1.0*1.0;
                        covar2(id2+ 7,id2+ 7) = 1e-3*1e-3;
                        covar2(id2+ 8,id2+ 8) = 1e-3*1e-3;
                        covar2(id2+ 9,id2+ 9) = 1e-3*1e-3;
                        covar2(id2+10,id2+10) = 1e-3*1e-3;

                        covar2(id2+11,id2+11) = 1e2 * 1e2;

                        id2 = id2 + 12;
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
//                cout << setw(15) << setprecision(3) << state(i)
//                     << setw(25) << setprecision(6) << covar(i,i)
//                     << endl;
//            }
//            cout << endl;

            //////// end of initialization ////////


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
            for(int i=numSource*2; i<numUnknown; i=i+12)
            {
                com += state(i+11);
            }

            // omc
            double omc(obs - com);

            // weight
            double weight(2.0);

            // h
            Vector<double> h(numUnknown,0.0);
            for(int i=numSource*2; i<numUnknown; i=i+12)
            {
                h(i+11) = 1.0;
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
//                cout << setprecision(3) << setw(15) << state(i)
//                     << setprecision(6) << setw(25) << covar(i,i)
//                     << endl;
//            }
//            cout << endl;

            //////// end of clock constraint ////////


            //////// start of measment update ////////

            int idSource(0), idSat(0);

            double wmf(0.0);
            double dSatX(0.0), dSatY(0.0), dSatZ(0.0);
            double dSatVX(0.0), dSatVY(0.0), dSatVZ(0.0);
            Vector<double> posSourceECI(3,0.0);
            Vector<double> rsat(3,0.0), vsat(3,0.0);
            Vector<double> posSatECI(3,0.0), velSatECI(3,0.0);

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

                    posSourceECI = sourcePosECI[source];
                    clock = sourceClock[source];

                    for(satTypeValueMap::iterator stvmIt = sdmIt->second.begin();
                        stvmIt != sdmIt->second.end();
                        ++stvmIt)
                    {
                        sat = stvmIt->first;
                        idSat = satIndex[sat];

                        rsat(0) = state(idSat+0);
                        rsat(1) = state(idSat+1);
                        rsat(2) = state(idSat+2);
                        vsat(0) = state(idSat+3);
                        vsat(1) = state(idSat+4);
                        vsat(2) = state(idSat+5);

                        posSatECI = rsat;
                        velSatECI = vsat;

                        double r( norm(rsat) );
                        double r2( r * r );
                        double r3( r2 * r );
                        double k( GM_EARTH/r3 );

                        double rho( norm(posSatECI - posSourceECI) );
                        double dlight( rho / C_MPS );
                        double dclock( clock / C_MPS );
                        double drel( 0.0 );

                        double dT(dlight + dclock - drel);
                        double dT2( dT * dT );

                        for(int i=0; i<3; ++i)
                        {
                            velSatECI = vsat + k*rsat*dT;
                            posSatECI = rsat - vsat*dT - 0.5*k*rsat*dT2;

                            rho = norm(posSatECI - posSourceECI);
                            dlight = rho / C_MPS;

                            drel = -2.0*( posSatECI(0)*velSatECI(0) +
                                          posSatECI(1)*velSatECI(1) +
                                          posSatECI(2)*velSatECI(2) )/C_MPS/C_MPS;

                            dT = dlight + dclock - drel;
                        }

                        Matrix<double> I = ident<double>(3);

                        Matrix<double> rrt( outer(rsat,rsat) );

                        Matrix<double> phi( I - 0.5*k*(I-3.0*rrt/r2)*dT2 );

                        Vector<double> par( (posSatECI-posSourceECI)/rho );

                        Vector<double> rho_rsat( phi*par );
                        Vector<double> rho_vsat( -par*dT );

                        // omc, weight, wmf
                        // dSatX, dSatY, dSatZ
                        // dSatVX, dSatVY, dSatVZ
                        omc = stvmIt->second[TypeID::prefitCForPOD];
                        weight = stvmIt->second[TypeID::weightC];
                        wmf = stvmIt->second[TypeID::wetMap];

                        dSatX = rho_rsat(0);
                        dSatY = rho_rsat(1);
                        dSatZ = rho_rsat(2);
                        dSatVX = rho_vsat(0);
                        dSatVY = rho_vsat(1);
                        dSatVZ = rho_vsat(2);

                        // h
                        Vector<double> h(numUnknown,0.0);
                        h(idSource+0) = +1.0;
                        h(idSource+1) = wmf;
                        h(idSat+ 0) = dSatX;
                        h(idSat+ 1) = dSatY;
                        h(idSat+ 2) = dSatZ;
                        h(idSat+ 3) = dSatVX;
                        h(idSat+ 4) = dSatVY;
                        h(idSat+ 5) = dSatVZ;
                        h(idSat+11) = -1.0;

                        // z - h(x)
                        omc = omc - rho
                                  - 1.0 * state(idSource+0)
                                  - wmf * state(idSource+1)
                                  + 1.0 * state(idSat+11);

//                        if(omc > 2.0) continue;

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

//                        cout << "after meas update" << endl;
//                        cout << sat << endl;
//                        for(int i=numSource*2; i<numSource*2+12*5; ++i)
//                        {
//                            cout << setprecision(3) << setw(15) << state(i)
//                                 << setprecision(6) << setw(25) << covar(i,i);
//                            cout << endl;
//                        }
//                        cout << endl;

                    } // End of for(satTypeValueMap::iterator stvmIt = ...)

                } // End of for(sourceDataMap::iterator sdmIt = ...)

            } // End of for(gnssDataMap::iterator gdmIt = ...)


            cout << "after meas update" << endl;
            for(int i=numSource*2; i<numSource*2+12; ++i)
            {
                cout << setprecision(3) << setw(15) << state(i)
                     << setprecision(6) << setw(25) << covar(i,i);
                cout << endl;
            }
            cout << endl;


//            for(SatIDSet::iterator itSat = satSet.begin();
//                itSat != satSet.end();
//                ++itSat)
//            {
//                sat = *itSat;
//
//                idSat = satIndex[sat];
//
//                Vector<double> rcom_c(3,0.0);
//                rcom_c(0) = state(idSat+0);
//                rcom_c(1) = state(idSat+1);
//                rcom_c(2) = state(idSat+2);
//
//                Vector<double> rcom_t(3,0.0);
//                rcom_t = transpose(t2cRaw) * rcom_c;
//
//                Vector<double> rref_t(3,0.0);
//
//                try
//                {
//                    rref_t = sp3Store.getXvt(sat,gps).x.toVector();
//                }
//                catch(...)
//                {
//                    continue;
//                }
//
//                Vector<double> dxyz( rcom_t-rref_t );
//
////                cout << sat;
//                cout << setprecision(3)
//                     << setw(10) << dxyz(0)
//                     << setw(10) << dxyz(1)
//                     << setw(10) << dxyz(2);
//            }
//            cout << endl;


            //////// end of measment update ////////


            // update orbit and srpc
            for(SatIDSet::iterator itSat = satSet.begin();
                itSat != satSet.end();
                ++itSat)
            {
                sat = *itSat;
                id = satIndex[sat];

                // update orbit
                for(int i=0; i<72; ++i) orbit(i) = 0.0;
                for(int i=0; i<6; ++i) orbit(i) = state(id+i+0);
                orbit( 6) = 1.0; orbit(10) = 1.0; orbit(14) = 1.0;
                orbit(33) = 1.0; orbit(37) = 1.0; orbit(41) = 1.0;

                // update srpc
                for(int i=0; i<5; ++i) srpc(i) = state(id+i+6);

                satOrbit[sat] = orbit;
                satSRPC[sat] = srpc;
            }

            srp.setSRPCoeff(satSRPC);
            gnss.setSRPModel(srp);
            rkf78.setEquationOfMotion(gnss);

            rkf78.setCurrentTime(tt);
            rkf78.setCurrentState(satOrbit);


            //////// start of time update ////////

            gps = gps + dt;
            utc = refSys.GPS2UTC(gps);
            tt = refSys.GPS2TT(gps);

            // time update for source-related parameters
            Matrix<double> pSource1(2,2,0.0), pSource2(2,2,0.0);

            // process noise of source-related parameters
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
                // constant for clock and dwet

                // covariance update
                covar(id+0,id+0) += noise(0,0);
                covar(id+1,id+1) += noise(1,1);
            }

            // orbit integration
            satOrbit = rkf78.integrateTo(tt);

            // time update for sat-related parameters
            Matrix<double> pSat1(12,12,0.0), pSat2(12,12,0.0);

            // process noise
            noise.resize(12,12,0.0);
            noise(11,11) = 1.0 * dt;

            for(SatIDSet::iterator itSat = satSet.begin();
                itSat != satSet.end();
                ++itSat)
            {
                sat = *itSat;
                id = satIndex[sat];

                orbit = satOrbit[sat];
                srpc = satSRPC[sat];

                phi = ident<double>(12);

                for(int i=0; i<3; ++i)
                {
                    for(int j=0; j<3; ++j)
                    {
                        phi(i+0,j+0) = orbit( 6+3*i+j); // dr1/dr0
                        phi(i+0,j+3) = orbit(15+3*i+j); // dr1/dv0
                        phi(i+3,j+0) = orbit(24+3*i+j); // dv1/dr0
                        phi(i+3,j+3) = orbit(33+3*i+j); // dv1/dv0
                    }

                    for(int j=0; j<5; ++j)
                    {
                        phi(i+0,j+6) = orbit(42+5*i+j); // dr1/dp0
                        phi(i+3,j+6) = orbit(57+5*i+j); // dv1/dp0
                    }
                }

                phit = transpose(phi);

//                cout << sat << endl;
//
//                for(int i=0; i<11; ++i)
//                {
//                    for(int j=0; j<11; ++j)
//                    {
//                        cout << setw(10) << phi(i,j);
//                    }
//                    cout << endl;
//                }

                // state update

                // integration for orbit
                // constant for srpc and clock
                for(int i=0; i<6; ++i) state(id+i+0) = orbit(i);

                // covariance update

                // first, get the covariance of this sat
                for(int i=0; i<12; ++i)
                {
                    for(int j=0; j<12; ++j)
                    {
                        pSat1(i,j) = covar(id+i,id+j);
                    }
                }

                // second, perform covariance update for this sat
                pSat2 = phi * pSat1 * phit + noise;

                // last, set the covariance of this sat
                for(int i=0; i<12; ++i)
                {
                    for(int j=0; j<12; ++j)
                    {
                        covar(id+i,id+j) = pSat2(i,j);
                    }
                }
            }

            cout << "after time update" << endl;
            for(int i=numSource*2; i<numSource*2+12; i=i+1)
            {
                cout << setprecision(3) << setw(15) << state(i)
                     << setprecision(6) << setw(25) << covar(i,i)
                     << endl;
            }
            cout << endl;


            //////// end of time update ////////

            if(gps-gps0 > 60 * 30.0) break;

        }
        catch(...)
        {
            cerr << "processing error." << endl;
            break;
        }

//        break;

    } // End of 'while( obsStreams.readEpochData(gData) )'

    double clock_end( Counter::now() );

//    cout << clock_end - clock_start << endl;

    return 0;

}

#pragma ident "$Id$"

#include <algorithm>

#include "ConfDataReader.hpp"

#include "MSCStore.hpp"
#include "BLQDataReader.hpp"

#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"

#include "DataStructures.hpp"

#include "ProcessingList.hpp"

#include "RequireObservables.hpp"

#include "LinearCombinations.hpp"
#include "ComputeLinear.hpp"

#include "LICSDetector.hpp"
#include "MWCSDetector.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;



int main(int argc, char* argv[])
{

    // conf file
    string confFileName("station.conf");

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


    // blq file
    BLQDataReader blqStore;

    string blqFileName = confReader.getValue("blqFileName", "DEFAULT");

    try
    {
        blqStore.open( blqFileName );
    }
    catch(...)
    {
        cerr << "blq file '" << blqFileName << "' open error." << endl;
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


    map<SourceID, Position> sourcePos;


    // loop for stations
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

        transform(station.begin(),station.end(),station.begin(),::tolower);

        SourceID source(SourceID::Mixed, station);

        transform(station.begin(),station.end(),station.begin(),::toupper);

        // GPS ObsID
        RinexObsID GC1C("GC1C"), GC2W("GC2W"), GL1C("GL1C"), GL2W("GL2W");

        map<string,vector<RinexObsID> > mapObsTypes(roh.mapObsTypes);
        vector<RinexObsID> roi;

        if(mapObsTypes.find("G") != mapObsTypes.end())
        {
            roi = mapObsTypes["G"];

            if(find(roi.begin(),roi.end(),GC1C) == roi.end())
            {
                ros.close(); ++obs_it; continue;
            }

            if(find(roi.begin(),roi.end(),GC2W) == roi.end())
            {
                ros.close(); ++obs_it; continue;
            }

            if(find(roi.begin(),roi.end(),GL1C) == roi.end())
            {
                ros.close(); ++obs_it; continue;
            }

            if(find(roi.begin(),roi.end(),GL2W) == roi.end())
            {
                ros.close(); ++obs_it; continue;
            }
        }


        // check if station is included in BLQ file
        if( !blqStore.isValid(station) )
        {
            cerr << "The station " << station
                 << " isn't included in BLQ file." << endl;

            ++obs_it;

            continue;
        }


        gps0.setTimeSystem(TimeSystem::Unknown);

        MSCData mscData;

        // station position
        Position nominalPos( roh.antennaPosition );

        // check if station is included in MSC file
        try
        {
            mscData = mscStore.findMSC(station, gps0);
            nominalPos = mscData.coordinates;
        }
        catch(...)
        {
            cerr << "The station " << station
                 << " isn't included in MSC file." << endl;

            ++obs_it;

            continue;
        }

        gps0.setTimeSystem(TimeSystem::GPS);

        sourcePos[source] = nominalPos;

        ros.close();

        ++obs_it;

    } // End of 'while(obs_it...)'


    // limit
    double limit;

    try
    {
        limit = confReader.getValueAsDouble("distanceLimit", "DEFAULT");
    }
    catch(...)
    {
        cerr << "distance limit get error." << endl;
        exit(-1);
    }


    vector<SourceID> sourceRejectVec;

    double diff(0.0);
    for(map<SourceID,Position>::iterator source1 = sourcePos.begin();
        source1 != sourcePos.end();
        ++source1)
    {
        for(map<SourceID,Position>::iterator source2 = sourcePos.begin();
            source2 != sourcePos.end();
            ++source2)
        {
            if(source2->first == source1->first) continue;

            if(range(source1->second,source2->second) < limit)
            {
                sourceRejectVec.push_back(source2->first);
            }
        }
    }

    for(vector<SourceID>::iterator sourceIt = sourceRejectVec.begin();
        sourceIt != sourceRejectVec.end();
        ++sourceIt)
    {
        if(sourcePos.find(*sourceIt) != sourcePos.end())
            sourcePos.erase(*sourceIt);
    }

    cout << "satisfied station number: " << sourcePos.size() << endl;


    ofstream fobs("obsFileListName.txt");

    string name1, name2;

    for(vector<string>::iterator obsIt = obsFileListVec.begin();
        obsIt != obsFileListVec.end();
        ++obsIt)
    {
        name1 = *obsIt;

        for(map<SourceID,Position>::iterator spIt = sourcePos.begin();
            spIt != sourcePos.end();
            ++spIt)
        {
            name2 = spIt->first.sourceName;

            if(name1.find(name2) != string::npos)
            {
                fobs << name1 << endl;
                break;
            }
        }
    }

    fobs.close();



    ofstream fsta("station.list");
    fsta << fixed << setprecision(8);

    for(map<SourceID,Position>::iterator source = sourcePos.begin();
        source != sourcePos.end();
        ++source)
    {
        fsta << setw(15) << source->second.getLongitude()
             << setw(15) << source->second.getGeodeticLatitude()
             << setw(10) << source->first.sourceName
             << endl;
    }

    fsta.close();


    return 0;
}

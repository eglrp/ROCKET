#pragma ident "$Id$"

/**
 * @file rinex_obs_test.cpp
 * tests Rinex3ObsStream, Rinex3ObsHeader, Rinex3ObsData.
 */

#include "ConfDataReader.hpp"
#include "NetworkObsStreams.hpp"

using namespace std;
using namespace gpstk;

/// Returns 0 on success.
int main(int argc, char *argv[])
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
        cerr << "conf file '" << confFileName
             << "' open error." << endl;
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

        ros.close();

        // now, we can add OBS file to OBS streams
        if( !obsStreams.addRinexObsFile( obsFile ) )
        {
            cerr << "obs file '" << obsFile << "' add error." << endl;
            continue;
        }
    }

    obsFileListStream.close();


    gnssDataMap gData;

    SourceID source;
    SatID sat;

    cout << fixed << setprecision(3);

    // process epoch by epoch
    while( obsStreams.readEpochData(gData) )
    {
        CommonTime gps( gData.begin()->first );

        cout << CivilTime(gps)
             << setw(10) << gData.getSourceIDSet().size()
             << setw(10) << gData.getSatIDSet().size()
             << endl;
    }

    return 0;

} // main()

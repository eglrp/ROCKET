#pragma ident "$Id$"

#include <algorithm>

#include "ConfDataReader.hpp"

#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"

#include "DataStructures.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;



int main(int argc, char* argv[])
{

    // conf file
    string confFileName;

    if(argc < 1) exit(-1);

    ConfDataReader confReader;

    try
    {
        confReader.open(confFileName.c_str());
    }
    catch(...)
    {
        cerr << "conf file '" << confFileName
             << "' open error." << endl;
        exit(-1);
    }

    confReader.setFallback2Default(true);


    // obs file
    string obsFileListName = confReader.getValue("obsFileListName");

    vector<string> obsFileListVec;

    ifstream obsFileListStream;

    obsFileListStream.open(obsFileListName.c_str());

    if(!obsFileListStream)
    {
        cerr << "obs file list '" << obsFileListName
             << "' open error." << endl;
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

    int year, doy;

    int N_GC1C(0), N_GC1S(0), N_GC1L(0), N_GC1X(0), N_GC1P(0), N_GC1W(0),
        N_GC1Y(0), N_GC1M(0);
    int N_GC2C(0), N_GC2D(0), N_GC2S(0), N_GC2L(0), N_GC2X(0), N_GC2P(0),
        N_GC2W(0), N_GC2Y(0), N_GC2M(0);
    int N_GC5I(0), N_GC5Q(0), N_GC5X(0);

    int N_EC1A(0), N_EC1B(0), N_EC1C(0), N_EC1X(0), N_EC1Z(0);
    int N_EC5I(0), N_EC5Q(0), N_EC5X(0);
    int N_EC7I(0), N_EC7Q(0), N_EC7X(0);
    int N_EC8I(0), N_EC8Q(0), N_EC8X(0);
    int N_EC6A(0), N_EC6B(0), N_EC6C(0), N_EC6X(0), N_EC6Z(0);

    int N_CC2I(0), N_CC2Q(0), N_CC2X(0);
    int N_CC7I(0), N_CC7Q(0), N_CC7X(0);
    int N_CC6I(0), N_CC6Q(0), N_CC6X(0);

    RinexObsID temp;

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


        map<string,vector<RinexObsID> > mapObsTypes(roh.mapObsTypes);

        map<string,vector<RinexObsID> >::const_iterator it_mot;

        for(it_mot=mapObsTypes.begin(); it_mot!=mapObsTypes.end(); ++it_mot)
        {
            string sys = it_mot->first;

            vector<RinexObsID> roi = it_mot->second;

            if(sys == "G")
            {
                temp = RinexObsID("GC1C");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1C++;

                temp = RinexObsID("GC1S");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1S++;

                temp = RinexObsID("GC1L");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1L++;

                temp = RinexObsID("GC1X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1X++;

                temp = RinexObsID("GC1P");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1P++;

                temp = RinexObsID("GC1W");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1W++;

                temp = RinexObsID("GC1Y");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1Y++;

                temp = RinexObsID("GC1M");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC1M++;


                temp = RinexObsID("GC2C");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2C++;

                temp = RinexObsID("GC2D");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2D++;

                temp = RinexObsID("GC2S");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2S++;

                temp = RinexObsID("GC2L");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2L++;

                temp = RinexObsID("GC2X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2X++;

                temp = RinexObsID("GC2P");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2P++;

                temp = RinexObsID("GC2W");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2W++;

                temp = RinexObsID("GC2Y");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2Y++;

                temp = RinexObsID("GC2M");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC2M++;


                temp = RinexObsID("GC5I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC5I++;

                temp = RinexObsID("GC5Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC5Q++;

                temp = RinexObsID("GC5X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_GC5X++;

            }
            else if(sys == "E")
            {
                temp = RinexObsID("EC1A");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC1A++;

                temp = RinexObsID("EC1B");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC1B++;

                temp = RinexObsID("EC1C");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC1C++;

                temp = RinexObsID("EC1X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC1X++;

                temp = RinexObsID("EC1Z");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC1Z++;


                temp = RinexObsID("EC5I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC5I++;

                temp = RinexObsID("EC5Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC5Q++;

                temp = RinexObsID("EC5X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC5X++;


                temp = RinexObsID("EC7I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC7I++;

                temp = RinexObsID("EC7Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC7Q++;

                temp = RinexObsID("EC7X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC7X++;


                temp = RinexObsID("EC8I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC8I++;

                temp = RinexObsID("EC8Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC8Q++;

                temp = RinexObsID("EC8X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC8X++;


                temp = RinexObsID("EC6A");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC6A++;

                temp = RinexObsID("EC6B");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC6B++;

                temp = RinexObsID("EC6C");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC6C++;

                temp = RinexObsID("EC6X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC6X++;

                temp = RinexObsID("EC6Z");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_EC6Z++;

            }
            else if(sys == "C")
            {
                temp = RinexObsID("CC1I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC2I++;
                temp = RinexObsID("CC2I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC2I++;

                temp = RinexObsID("CC1Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC2Q++;
                temp = RinexObsID("CC2Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC2Q++;

                temp = RinexObsID("CC1X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC2X++;
                temp = RinexObsID("CC2X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC2X++;


                temp = RinexObsID("CC7I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC7I++;

                temp = RinexObsID("CC7Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC7Q++;

                temp = RinexObsID("CC7X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC7X++;


                temp = RinexObsID("CC6I");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC6I++;

                temp = RinexObsID("CC6Q");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC6Q++;

                temp = RinexObsID("CC6X");
                if(find(roi.begin(),roi.end(),temp) != roi.end()) N_CC6X++;

            }
        }

        ros.close();

        ++obs_it;
    }

    cout << "GPS: " << endl
         << "C1C: " << setw(4) << N_GC1C << "  "
         << "C1S: " << setw(4) << N_GC1S << "  "
         << "C1L: " << setw(4) << N_GC1L << "  "
         << "C1X: " << setw(4) << N_GC1X << "  "
         << "C1P: " << setw(4) << N_GC1P << "  "
         << "C1W: " << setw(4) << N_GC1W << "  "
         << "C1Y: " << setw(4) << N_GC1Y << "  "
         << "C1M: " << setw(4) << N_GC1M << endl
         << "C2C: " << setw(4) << N_GC2C << "  "
         << "C2D: " << setw(4) << N_GC2D << "  "
         << "C2S: " << setw(4) << N_GC2S << "  "
         << "C2L: " << setw(4) << N_GC2L << "  "
         << "C2X: " << setw(4) << N_GC2X << "  "
         << "C2P: " << setw(4) << N_GC2P << "  "
         << "C2W: " << setw(4) << N_GC2W << "  "
         << "C2Y: " << setw(4) << N_GC2Y << "  "
         << "C2M: " << setw(4) << N_GC2M << endl
         << "C5I: " << setw(4) << N_GC5I << "  "
         << "C5Q: " << setw(4) << N_GC5Q << "  "
         << "C5X: " << setw(4) << N_GC5X << endl;

    cout << "Galileo: " << endl
         << "C1A: " << setw(4) << N_EC1A << "  "
         << "C1B: " << setw(4) << N_EC1B << "  "
         << "C1C: " << setw(4) << N_EC1C << "  "
         << "C1X: " << setw(4) << N_EC1X << "  "
         << "C1Z: " << setw(4) << N_EC1Z << endl
         << "C5I: " << setw(4) << N_EC5I << "  "
         << "C5Q: " << setw(4) << N_EC5Q << "  "
         << "C5X: " << setw(4) << N_EC5X << endl
         << "C7I: " << setw(4) << N_EC7I << "  "
         << "C7Q: " << setw(4) << N_EC7Q << "  "
         << "C7X: " << setw(4) << N_EC7X << endl
         << "C8I: " << setw(4) << N_EC8I << "  "
         << "C8Q: " << setw(4) << N_EC8Q << "  "
         << "C8X: " << setw(4) << N_EC8X << endl
         << "C6A: " << setw(4) << N_EC6A << "  "
         << "C6B: " << setw(4) << N_EC6B << "  "
         << "C6C: " << setw(4) << N_EC6C << "  "
         << "C6X: " << setw(4) << N_EC6X << "  "
         << "C6Z: " << setw(4) << N_EC6Z << endl;

    cout << "BDS: " << endl
         << "C2I: " << setw(4) << N_CC2I << "  "
         << "C2Q: " << setw(4) << N_CC2Q << "  "
         << "C2X: " << setw(4) << N_CC2X << endl
         << "C7I: " << setw(4) << N_CC7I << "  "
         << "C7Q: " << setw(4) << N_CC7Q << "  "
         << "C7X: " << setw(4) << N_CC7X << endl
         << "C6I: " << setw(4) << N_CC6I << "  "
         << "C6Q: " << setw(4) << N_CC6Q << "  "
         << "C6X: " << setw(4) << N_CC6X << endl;

    return 0;
}

#pragma ident "$Id$"

/**
 * @file sp3_compare.cpp
 */

#include "ConfDataReader.hpp"
#include "SP3EphemerisStore.hpp"
#include "DataStructures.hpp"

using namespace std;
using namespace gpstk;


int main(int argc, char *argv[])
{

    // conf file
    string confFileName("compare.conf");

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


    // sp3/clk file
    string sp3File1( confReader.getValue("sp3File1", "DEFAULT") );
    string clkFile1( confReader.getValue("clkFile1", "DEFAULT") );

    SP3EphemerisStore sp3Store1;
    sp3Store1.rejectBadPositions(true);
    sp3Store1.rejectBadClocks(true);

    try
    {
        sp3Store1.loadFile(sp3File1);
    }
    catch(...)
    {
        cerr << "sp3 file '" << sp3File1
             << "' load error." << endl;
    }


    try
    {
        sp3Store1.loadRinexClockFile(clkFile1);
    }
    catch(...)
    {
        cerr << "clk file '" << clkFile1
             << "' load error." << endl;
    }


    string sp3File2( confReader.getValue("sp3File2", "DEFAULT") );
    string clkFile2( confReader.getValue("clkFile2", "DEFAULT") );

    SP3EphemerisStore sp3Store2;
    sp3Store2.rejectBadPositions(true);
    sp3Store2.rejectBadClocks(true);

    try
    {
        sp3Store2.loadFile(sp3File2);
    }
    catch(...)
    {
        cerr << "sp3 file '" << sp3File2
             << "' load error." << endl;
    }

    try
    {
        sp3Store2.loadRinexClockFile(clkFile2);
    }
    catch(...)
    {
        cerr << "clk file '" << clkFile2
             << "' load error." << endl;
    }


    SatIDSet allSatSet;

    SatID sat;

    for(int i=1; i<=MAX_PRN_GPS; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        allSatSet.insert(sat);
    }


    CommonTime t_beg( sp3Store1.getInitialTime() );
    CommonTime t_end( sp3Store1.getFinalTime() );


    CommonTime t_curr( t_beg );

    double clk1(0.0), clk2(0.0);

    cout << fixed;

    for(int i=0; i<24*120; ++i)
    {
        t_curr = t_beg + 30.0*i;

//        cout << CivilTime(t_curr) << endl;

        for(SatIDSet::iterator it = allSatSet.begin();
            it != allSatSet.end();
            ++it)
        {
            sat = *it;

            try
            {
                clk1 = sp3Store1.getXvt(sat,t_curr).clkbias;
                clk2 = sp3Store2.getXvt(sat,t_curr).clkbias;
            }
            catch(...)
            {
                continue;
            }

            cout << setw(15) << (clk1-clk2)*1e9;
//            cout << setw(15) << clk1*1e9;
//            cout << setw(15) << clk2*1e9;
        }

        cout << endl;
    }

    return 0;

}

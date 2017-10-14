#include <iostream>

#include "ConfDataReader.hpp"

#include "EOPDataStore2.hpp"
#include "LeapSecStore.hpp"

#include "ReferenceSystem.hpp"

#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main(void)
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


    CivilTime ct(2016,1,9,0,0,0.0,TimeSystem::GPS);
    CommonTime gps( ct.convertToCommonTime() );
    CommonTime utc( refSys.GPS2UTC(gps) );

    Matrix<double> t2cRaw( refSys.T2CMatrix(utc) );

    Vector<double> rsta0_t(3,0.0);
    rsta0_t(0) = -3425750.198;
    rsta0_t(1) = -1214685.845;
    rsta0_t(2) =  5223662.717;

    // R * (r_t + dr)
    Vector<double> rsta1_t(3,0.0);
    for(int i=0; i<3; ++i) rsta1_t(i) = rsta0_t(i) + 1.0;
    Vector<double> rsta1_c( t2cRaw * rsta1_t );

    // R * r_c + dr
    Vector<double> rsta2_c( t2cRaw * rsta0_t );
    for(int i=0; i<3; ++i) rsta2_c(i) = rsta2_c(i) + 1.0;

    cout << fixed << setprecision(3);
    cout << setw(15) << rsta1_c(0)
         << setw(15) << rsta1_c(1)
         << setw(15) << rsta1_c(2)
         << endl
         << setw(15) << rsta2_c(0)
         << setw(15) << rsta2_c(1)
         << setw(15) << rsta2_c(2)
         << endl;

    return 0;
}

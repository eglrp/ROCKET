/*
 * Test for the EOP time series.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"
#include "ReferenceSystem.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{
    // Configuration file
    ConfDataReader confReader;
    try
    {
        confReader.open("test.conf");
    }
    catch(...)
    {
        cerr << "Configuration file open error." << endl;

        return 1;
    }

    // EOP Data Store
    EOPDataStore2 eopDataStore;
    eopDataStore.setUseBulletinB(false);
    eopDataStore.setInterpPoints(4);
    eopDataStore.setRegularization(false);


    // IERS EOP file
    string eopFile = confReader.getValue("IERSEOPFILE", "DEFAULT");
    try
    {
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "EOP File Load Error." << endl;

        return 1;
    }

    // Leap Sec Store
    LeapSecStore leapSecStore;

    // IERS LeapSecond file
    string lsFile  = confReader.getValue("IERSLSFILE", "DEFAULT");
    try
    {
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS Leap Second File Load Error." << endl;

        return 1;
    }

    // Reference System
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);


    CivilTime ct0(2016,1,1,0,0,0.0, TimeSystem::UTC);

    double dclock(0.001), dlight(0.07);
    double dtotal(dclock + dlight);

    CommonTime t1( ct0.convertToCommonTime() );
    CommonTime t2( t1 - dclock );
    CommonTime t3( t1 - dtotal );

    double wt1(OMEGA_EARTH * dclock);
    double wt2(OMEGA_EARTH * dlight);

    // Rotation Matrix from ECEF(t) to ECEF(t-dclock)
    Matrix<double> t2t_1(3,3,0.0);
    t2t_1(0,0) = +std::cos(-wt1);
    t2t_1(0,1) = +std::sin(-wt1);
    t2t_1(1,0) = -std::sin(-wt1);
    t2t_1(1,1) = +std::cos(-wt1);
    t2t_1(2,2) = +1.0;

    // Rotation Matrix from ECEF(t-dtotal) to ECEF(t-dclock)
    Matrix<double> t2t_2(3,3,0.0);
    t2t_2(0,0) = +std::cos(wt2);
    t2t_2(0,1) = +std::sin(wt2);
    t2t_2(1,0) = -std::sin(wt2);
    t2t_2(1,1) = +std::cos(wt2);
    t2t_2(2,2) = +1.0;

    // Rotation Matrix from ECI(t) to ECEF(t)
    Matrix<double> c2t_1( refSys.C2TMatrix(t1) );

    // Rotation Matrix from ECI(t-dclock) to ECEF(t-dclock)
    Matrix<double> c2t_2( refSys.C2TMatrix(t2) );

    // Rotation Matrix from ECI(t-dtotal) to ECEF(t-dtotal)
    Matrix<double> c2t_3( refSys.C2TMatrix(t3) );

    // Rotation Matrix from ECI(t-dclock) to ECEF(t-dclock)
    Matrix<double> c2t_4( t2t_1 * c2t_1 );
    Matrix<double> c2t_5( t2t_2 * c2t_3 );

    Matrix<double> c2t( refSys.C2TMatrix(t3) );
    Matrix<double> t2c( refSys.T2CMatrix(t2) );

    Matrix<double> sum( c2t * t2c );

    Vector<double> rsta1(3,0.0);
    rsta1(0) = 6400*1e3;

    Vector<double> rsta2( sum * rsta1 );

    cout << fixed << setprecision(10);

    cout << setw(15) << rsta2(0)
         << setw(15) << rsta2(1)
         << setw(15) << rsta2(2)
         << endl
         << endl;

    cout << setw(15) << c2t_2(0,0)
         << setw(15) << c2t_2(0,1)
         << setw(15) << c2t_2(0,2)
         << endl
         << setw(15) << c2t_2(1,0)
         << setw(15) << c2t_2(1,1)
         << setw(15) << c2t_2(1,2)
         << endl
         << setw(15) << c2t_2(2,0)
         << setw(15) << c2t_2(2,1)
         << setw(15) << c2t_2(2,2)
         << endl
         << endl;

    cout << setw(15) << c2t_4(0,0)
         << setw(15) << c2t_4(0,1)
         << setw(15) << c2t_4(0,2)
         << endl
         << setw(15) << c2t_4(1,0)
         << setw(15) << c2t_4(1,1)
         << setw(15) << c2t_4(1,2)
         << endl
         << setw(15) << c2t_4(2,0)
         << setw(15) << c2t_4(2,1)
         << setw(15) << c2t_4(2,2)
         << endl
         << endl;

    cout << setw(15) << c2t_5(0,0)
         << setw(15) << c2t_5(0,1)
         << setw(15) << c2t_5(0,2)
         << endl
         << setw(15) << c2t_5(1,0)
         << setw(15) << c2t_5(1,1)
         << setw(15) << c2t_5(1,2)
         << endl
         << setw(15) << c2t_5(2,0)
         << setw(15) << c2t_5(2,1)
         << setw(15) << c2t_5(2,2)
         << endl
         << endl;

    // Time
//    CivilTime ct0(2000,1,1,0,0,0.0, TimeSystem::UTC);
//    CommonTime utc0( ct0.convertToCommonTime() );
//    CommonTime utcx( utc0 );
//
//    EOPDataStore2::EOPData eopx;
//
//    cout << fixed << setprecision(5);
//
//    for(int i=0; i<24*15; ++i)
//    {
//        MJD mjd_utc(utcx);
//
//        CommonTime tt( refSys.UTC2TT(utcx) );
//        MJD mjd_tt(tt);
//
//        cout << CivilTime(utcx);
//
//        Vector<double> cor_ocean1 = PMUT_OCEAN_IERS10(utcx);
////        cout << setw(20) << mjd_utc.mjd
////             << setw(20) << cor_ocean1(0)*1e6
////             << setw(20) << cor_ocean1(1)*1e6
////             << setw(20) << cor_ocean1(2)*1e6
////             << endl;
//
//        Vector<double> cor_ocean2 = ortho_eop(utcx);
////        cout << setw(20) << mjd_utc.mjd
////             << setw(20) << cor_ocean2(0)
////             << setw(20) << cor_ocean2(1)
////             << setw(20) << cor_ocean2(2)
////             << endl;
//
//        cout << setw(20) << cor_ocean1(0)*1e6 - cor_ocean2(0)
//             << setw(20) << cor_ocean1(1)*1e6 - cor_ocean2(1)
//             << setw(20) << cor_ocean1(2)*1e6 - cor_ocean2(2)
//             << endl;
//
//        utcx = utcx + 3600.0;
//
//    }

    return 0;
}

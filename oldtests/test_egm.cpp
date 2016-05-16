/*
 * Test for Earth Gravitation Model.
 *
 * Test Time: 2015/01/01, 12h, 1m, 15s, GPS
 *
 * Test Satellite: GPS PRN 01
 *
 * Earth Gravitation Model: EGM2008 (12*12)
 *
 * Solid Tide: IERS 2003
 * Ocean Tide: CSR TIDE
 * Pole  Tide: IERS 2003
 *
 * Earth Gravitation:
 *    -0.363686527222444 0.420970296488611 -0.076863995695723
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "EGM96GravityModel.hpp"
#include "EGM08GravityModel.hpp"

#include "IERSConventions.hpp"

#include "SP3EphemerisStore.hpp"
#include "SatDataReader.hpp"

#include "CivilTime.hpp"

using namespace std;
using namespace gpstk;


int main(void)
{
    // Configuration file
    ConfDataReader confReader;
    try
    {
        confReader.open("../../rocket/oldtests/test.conf");
    }
    catch(...)
    {
        cerr << "Configuration file open error." << endl;

        return 1;
    }

    
    // IERS EOP file
    string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");
    try
    {
        LoadIERSEOPFile(eopFile);
    }
    catch(...)
    {
        cerr << "IERS Earth Orientation Parameters File Load Error." << endl;

        return 1;
    }
    
    // IERS LeapSecond file
    string lsFile  = confReader.getValue("IERSLSFile", "DEFAULT");
    try
    {
        LoadIERSLSFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS Leap Second File Load Error." << endl;

        return 1;
    }

    // Satellite ID
    SatID sat(1,SatID::systemGPS);

    // Time
    CivilTime ct(2015,1,1,12,1,15.0, TimeSystem::GPS);
    CommonTime gps( ct.convertToCommonTime() );
    CommonTime utc( GPS2UTC(gps) );

    // IGS SP3 file
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    string sp3File;
    while( (sp3File = confReader.fetchListValue("IGSSP3List", "DEFAULT")) != "" )
    {
    
        try
        {
            sp3Eph.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "IGS SP3 File Load Error." << endl;

            return 1;
        }
    }
    

    // Position and velocity in ITRS
    Vector<double> r_itrs, v_itrs;

    try
    {
        r_itrs = sp3Eph.getXvt(sat, gps).x.toVector();
        v_itrs = sp3Eph.getXvt(sat, gps).v.toVector();
    }
    catch(...)
    {
        cerr << "Get Position and Velocity from SP3 File Error." << endl;

        return 1;
    }

    // Transform matrix
    Matrix<double> c2t( C2TMatrix(utc) );
    // Transform matrix time dot
    Matrix<double> dc2t( dC2TMatrix(utc) );

    // Position and velocity in ICRS
    Vector<double> r_icrs = transpose(c2t) * r_itrs;
    Vector<double> v_icrs = transpose(c2t) * v_itrs + transpose(dc2t) * r_itrs;


    // Initial state: r0, v0
    Vector<double> rv0(6,0.0);
    rv0(0) = r_icrs(0); rv0(1) = r_icrs(1); rv0(2) = r_icrs(2);
    rv0(3) = v_icrs(0); rv0(4) = v_icrs(1); rv0(5) = v_icrs(2);

    // Reference body
    EarthBody eb;

    // Initial state: p0
    Vector<double> p0;

    // SatData file
    SatDataReader satReader;
    string satDataFile = confReader.getValue("SatDataFile", "DEFAULT");
    try
    {
        satReader.open(satDataFile);
    }
    catch(...)
    {
        cerr << "SatData File Open Error." << endl;

        return 1;
    }

    // Spacecraft
    Spacecraft sc;
    sc.setSatID(sat);
    sc.setEpoch(utc);
    sc.setBlock(satReader.getBlock(sat,utc));
    sc.setMass(satReader.getMass(sat,utc));
    sc.initStateVector(rv0, p0);

    // Earth Gravitation
    cout << fixed << setprecision(15);

    EGM08GravityModel egm;

    string egmFile = confReader.getValue("EGMFile", "DEFAULT");
    try
    {
       egm.loadEGMFile(egmFile);
    }
    catch(...)
    {
       cerr << "EGM File Load Error." << endl;

       return 1;
    }

    int degree = confReader.getValueAsInt("EGMDegree", "DEFAULT");
    int order  = confReader.getValueAsInt("EGMOrder",  "DEFAULT");
    egm.setDesiredDegreeOrder(degree, order);

//    egm.enableSolidTide(true);
//    egm.enableOceanTide(true);
//    egm.enablePoleTide(true);
    egm.doCompute(utc, eb, sc);

    cout << "EGM08: " << egm.getAccel() << endl;

    return 0;
}

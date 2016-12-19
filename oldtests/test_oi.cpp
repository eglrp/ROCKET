/* Test for Orbit Integration.
 *
 * Both the equation of motion and the variational equation are considered.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

#include "SP3EphemerisStore.hpp"

#include "SatDataReader.hpp"

#include "GNSSOrbit.hpp"

#include "RKF78Integrator.hpp"
#include "AdamsIntegrator.hpp"

#include "Epoch.hpp"

#include "StringUtils.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


int main(void)
{

    //---------- Global Data ----------//

    // Configuration File
    ConfDataReader confData;
    try
    {
        confData.open("../../ROCKET/oldtests/test.conf");
    }
    catch(...)
    {
        cerr << "Configuration File Open Error." << endl;
        return 1;
    }

    // EOP File
    EOPDataStore2 eopDataStore;
    string eopFile;
    try
    {
        eopFile = confData.getValue("IERSEOPFILE", "DEFAULT");
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "IERS EOP File Load Error." << endl;
        return 1;
    }
    eopDataStore.setInterpPoints(8);
    eopDataStore.setRegularization(true);
    eopDataStore.setOceanTides(true);
    eopDataStore.setLibration(true);

    // LeapSecond File
    LeapSecStore leapSecStore;

    string lsFile;
    try
    {
        lsFile = confData.getValue("IERSLSFILE", "DEFAULT");
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS LeapSecond File Load Error." << endl;
        return 1;
    }

    // Reference System
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);

    // Solar System
    SolarSystem solSys;

    string ephFile;
    try
    {
        ephFile = confData.getValue("JPLEPHFILE", "DEFAULT");
        solSys.initializeWithBinaryFile(ephFile);
    }
    catch(...)
    {
        cerr << "Solar System Initialize Error." << endl;
        return 1;
    }

    // Satellite Data File
    SatDataReader satData;

    string satDataFile = confData.getValue("SATDATAFILE", "DEFAULT");
    try
    {
        satData.open(satDataFile);
    }
    catch(...)
    {
        cerr << "Sat Data File Open Error." << endl;
        return 1;
    }


    //---------- Force Model Configuration ----------//

    // Earth Gravitation
    EGM08GravityModel egm;

    int EGMDEG(0), EGMORD(0);
    try
    {
        EGMDEG = confData.getValueAsInt("EGMDEG", "FORCEMODEL");
        EGMORD = confData.getValueAsInt("EGMORD", "FORCEMODEL");
    }
    catch(...)
    {
        cerr << "Get EGM Degree and Order Error." << endl;
        return 1;
    }

    // NOTE: We MUST call setDesiredDegreeOrder() method before
    //       calling loadFile() method to ensure the variable
    //       storing the spherical harmonic coefficients has
    //       been resized.
    egm.setDesiredDegreeOrder(EGMDEG, EGMORD);
    egm.setReferenceSystem(refSys);

    string egmFile;
    try
    {
        egmFile = confData.getValue("EGMFILE", "FORCEMODEL");
        egm.loadFile(egmFile);
    }
    catch(...)
    {
        cerr << "EGM File Load Error." << endl;
        return 1;
    }


    // Earth Solid Tide
    EarthSolidTide solidTide;
    solidTide.setReferenceSystem(refSys);
    solidTide.setSolarSystem(solSys);

    bool correctSolidTide = confData.getValueAsBoolean("SOLIDTIDE", "FORCEMODEL");
    if(correctSolidTide)
    {
        egm.setEarthSolidTide(solidTide);
    }

    // Earth Ocean Tide
    EarthOceanTide oceanTide;
    oceanTide.setReferenceSystem(refSys);

    int EOTDEG(0), EOTORD(0);
    try
    {
        EOTDEG = confData.getValueAsInt("EOTDEG", "FORCEMODEL");
        EOTORD = confData.getValueAsInt("EOTORD", "FORCEMODEL");
    }
    catch(...)
    {
        cerr << "Get EOT Degree and Order Error." << endl;
        return 1;
    }
    oceanTide.setDesiredDegreeOrder(EOTDEG, EOTORD);

    string eotFile = confData.getValue("EOTFILE", "FORCEMODEL");
    try
    {
        oceanTide.loadFile(eotFile);
    }
    catch(...)
    {
        cerr << "EOT File Load Error." << endl;
        return 1;
    }

    bool correctOceanTide = confData.getValueAsBoolean("OCEANTIDE", "FORCEMODEL");
    if(correctOceanTide)
    {
        egm.setEarthOceanTide(oceanTide);
    }


    // Earth Pole Tide
    EarthPoleTide poleTide;
    poleTide.setReferenceSystem(refSys);

    bool correctPoleTide = confData.getValueAsBoolean("POLETIDE", "FORCEMODEL");
    if(correctPoleTide)
    {
        egm.setEarthPoleTide(poleTide);
    }

    // Third Body
    ThirdBody thd;
    thd.setSolarSystem(solSys);
    thd.setReferenceSystem(refSys);
    thd.enableAllPlanets();

    // Solar Pressure
    CODEPressure srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);

    // Relativity Effect
    RelativityEffect rel;


    //---------- Intergrator Configuration ----------//

    // RKF78 Integrator
    RKF78Integrator rkf78;

    double size_rkf78;
    try
    {
        size_rkf78 = confData.getValueAsDouble("STEPSIZE_RKF78", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get RKF78 Step Size Error." << endl;
        return 1;
    }

    rkf78.setStepSize(size_rkf78);


    // Adams Integrator
    AdamsIntegrator adams;

    double size_adams;
    try
    {
        size_adams = confData.getValueAsDouble("STEPSIZE_ADAMS", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Adams Step Size Error." << endl;
        return 1;
    }

    adams.setStepSize(size_adams);


    // Arc Settings
    double arcLen;
    try
    {
        arcLen = confData.getValueAsDouble("ARCLENGTH", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Arc Length Error." << endl;
        return 1;
    }

    double arcInt;
    try
    {
        arcInt = confData.getValueAsDouble("ARCINTERVAL", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Arc Interval Error." << endl;
        return 1;
    }


    //---------- Initial Time ----------//
    string t0;
    try
    {
        t0 = confData.getValue("InitialTime", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Initial Time Error." << endl;
        return 1;
    }

    int     year =    asInt( t0.substr( 0, 4) );
    int     mon  =    asInt( t0.substr( 5, 2) );
    int     day  =    asInt( t0.substr( 8, 2) );
    int     hour =    asInt( t0.substr(11, 2) );
    int     min  =    asInt( t0.substr(14, 2) );
    double  sec  = asDouble( t0.substr(17, 2) );

    CivilTime cv0(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( refSys.GPS2UTC(gps0) );

    cout << CivilTime(gps0) << endl;

    //---------- Number of SRP Parameters ----------//
    int np_srp;
    try
    {
        np_srp = confData.getValueAsInt("SRPPARAMNUM", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get SRP Parameters Number Error." << endl;
        return 1;
    }


    //---------- Reference Orbit ----------//
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    string sp3File;
    while( (sp3File=confData.fetchListValue("IGSSP3LIST", "DEFAULT")) != "" )
    {
        try
        {
            sp3Eph.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "SP3 File Load Error." << endl;
            return 1;
        }
    }


    //---------- Orbit Integration ----------//

    // Initial Conditions
    Vector<double> r0(3,0.0);
    Vector<double> v0(3,0.0);
    Vector<double> p0_srp(np_srp,0.0);

    // Observed Position and Velocity
    Vector<double> r_obs(3,0.0), v_obs(3,0.0);

    // Computed Position and Velocity
    Vector<double> r_com(3,0.0), v_com(3,0.0);

    // Transformation Matrices
    Matrix<double>  t2c(3,3,0.0);
    Matrix<double> dt2c(3,3,0.0);

    // Sat ICS File
    string satICSFile;
    try
    {
        satICSFile = confData.getValue("SATICSFILE", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Sat ICS File Error." << endl;
    }

    ifstream fics(satICSFile.c_str());

    string line;
    while(!fics.eof())
    {
        getline(fics,line);

        if(fics.eof()) break;

        int    ok  = asInt( line.substr( 0, 1) );
        string sys =        line.substr( 2, 3);
        int    prn = asInt( line.substr( 6, 2) );

        // SatID
        SatID sat;
        sat.id = prn;
        if(sys == "GPS")        sat.system = SatID::systemGPS;
        else if(sys == "GLO")   sat.system = SatID::systemGlonass;
        else if(sys == "GAL")   sat.system = SatID::systemGalileo;
        else if(sys == "BDS")   sat.system = SatID::systemBeiDou;

        if( !ok )
        {
            cerr << "The ICS of " << sat << " is not ready." << endl;
            continue;
        }

        // r0
        r0(0) = asDouble( line.substr(  9,20) );
        r0(1) = asDouble( line.substr( 30,20) );
        r0(2) = asDouble( line.substr( 52,20) );

        // v0
        v0(0) = asDouble( line.substr( 73,15) );
        v0(1) = asDouble( line.substr( 89,15) );
        v0(2) = asDouble( line.substr(105,15) );

        // p0_srp
        if(np_srp == 9)
        {
            p0_srp(0) = asDouble( line.substr(121,10) ); // D0
            p0_srp(1) = asDouble( line.substr(132,10) ); // Y0
            p0_srp(2) = asDouble( line.substr(143,10) ); // B0
            p0_srp(3) = asDouble( line.substr(154,10) ); // Dc
            p0_srp(4) = asDouble( line.substr(165,10) ); // Ds
            p0_srp(5) = asDouble( line.substr(176,10) ); // Yc
            p0_srp(6) = asDouble( line.substr(187,10) ); // Ys
            p0_srp(7) = asDouble( line.substr(198,10) ); // Bc
            p0_srp(8) = asDouble( line.substr(209,10) ); // Bs
        }
        else if(np_srp == 5)
        {
            p0_srp(0) = asDouble( line.substr(121,10) ); // D0
            p0_srp(1) = asDouble( line.substr(132,10) ); // Y0
            p0_srp(2) = asDouble( line.substr(143,10) ); // B0
            p0_srp(3) = asDouble( line.substr(154,10) ); // Bc
            p0_srp(4) = asDouble( line.substr(165,10) ); // Bs
        }

        srp.setSRPCoeff(p0_srp);


        // out file
        string outFile( asString(sat.id) );
        outFile = rightJustify(outFile, 2, '0');
        outFile = outFile + string(".txt");

        cout << outFile << endl;

        ofstream fout;
        fout.open(outFile.c_str());


        cout << "Orbit Integration for " << sat << endl;

        // Spacecraft
        Spacecraft sc;
        sc.setCurrentTime(utc0);
        sc.setSatID(sat);
        sc.setCurrentPos(r0);
        sc.setCurrentVel(v0);
        sc.setNumOfParam(np_srp);
        sc.setSatData(satData);

        // GNSS Orbit
        GNSSOrbit gnss;
        gnss.setRefEpoch(utc0);
        gnss.setSpacecraft(sc);

        bool bEGM(false), bThd(false), bSRP(false), bRel(false);
        try
        {
            bEGM = confData.getValueAsBoolean("EnableEGM", "FORCEMODEL");
            bThd = confData.getValueAsBoolean("EnableThd", "FORCEMODEL");
            bSRP = confData.getValueAsBoolean("EnableSRP", "FORCEMODEL");
            bRel = confData.getValueAsBoolean("EnableRel", "FORCEMODEL");
        }
        catch(...)
        {
            cerr << "Get Force Model Settings Error." << endl;
            break;
        }

        if(bEGM) gnss.setEarthGravitation(egm);
        if(bThd) gnss.setThirdBody(thd);
        if(bSRP) gnss.setSolarPressure(srp);
        if(bRel) gnss.setRelativityEffect(rel);

        // Current time
        double t( 0.0 );
        CommonTime gps( gps0 );
        CommonTime utc( utc0 );

        // Current state
        Vector<double> y( sc.getCurrentState() );

        vector< double > t_curr;
        vector< Vector<double> > y_curr;

        t_curr.push_back(t);
        y_curr.push_back(y);


        //////// RKF78 Integrator ////////

        // Compute 8 start points for Adams Integrator
        for(int i=0; i<8; ++i)
        {
            // the gnss object use t and y to get derivatives, since
            // the spacecraft object is updated during this process,
            // we do not need to update it after each iteration.
            rkf78.integrateTo(t, y, &gnss, t+size_adams);

            t_curr.push_back(t);
            y_curr.push_back(y);

            for(int j=0; j<3; ++j)
            {
                r_com(j) = y(j+0);
                v_com(j) = y(j+3);
            }

            gps = gps0 + t;
            utc = refSys.GPS2UTC(gps);

            t2c  = refSys.T2CMatrix(utc);
            dt2c = refSys.dT2CMatrix(utc);

            try
            {
                Vector<double> r_sp3 = sp3Eph.getXvt(sat,gps).x.toVector();
                Vector<double> v_sp3 = sp3Eph.getXvt(sat,gps).v.toVector();

                r_obs = t2c * r_sp3;
                v_obs = t2c * v_sp3 + dt2c * r_sp3;
            }
            catch(...)
            {
                cerr << "Get Reference Orbit Error." << endl;
                continue;
            }

            Vector<double> dxyz( r_obs-r_com );

            double doy( YDSTime(gps).doy );
            double sod( YDSTime(gps).sod );

            if(std::fmod(t,arcInt) == 0.0)
            {
                fout << fixed;
                fout << setprecision(0);
                fout << setw( 5) << doy;
                fout << setw(10) << sod;

                fout << setprecision(3);
                fout << setw(10) << dxyz(0)
                     << setw(10) << dxyz(1)
                     << setw(10) << dxyz(2)
                     << endl;
            }

        }  // End of RKF78


        //////// Adams Integrator ////////

        while(t < (arcLen+6)*3600)
        {
            adams.integrateTo(t_curr, y_curr, &gnss, t+size_adams);

            t = t_curr[8];
            y = y_curr[8];

            r_com(0) = y(0); r_com(1) = y(1); r_com(2) = y(2);
            v_com(0) = y(3); v_com(1) = y(4); v_com(2) = y(5);

            gps = gps0 + t;
            utc = refSys.GPS2UTC(gps);

            t2c  = refSys.T2CMatrix(utc);
            dt2c = refSys.dT2CMatrix(utc);

            try
            {
                Vector<double> r_sp3 = sp3Eph.getXvt(sat,gps).x.toVector();
                Vector<double> v_sp3 = sp3Eph.getXvt(sat,gps).v.toVector();

                r_obs = t2c * r_sp3;
                v_obs = t2c * v_sp3 + dt2c * r_sp3;
            }
            catch(...)
            {
                cerr << "Get Reference Orbit Error." << endl;
                continue;
            }

            Vector<double> dxyz( r_obs-r_com );

            double doy( YDSTime(gps).doy );
            double sod( YDSTime(gps).sod );

            if(std::fmod(t,arcInt) == 0.0)
            {
                fout << fixed;
                fout << setprecision(0);
                fout << setw( 5) << doy;
                fout << setw(10) << sod;

                fout << setprecision(3);
                fout << setw(10) << dxyz(0)
                     << setw(10) << dxyz(1)
                     << setw(10) << dxyz(2)
                     << endl;
            }

        }  // End of Adams

        fout.close();


    }  // End of 'while(!fics.eof())'

    fics.close();

    return 0;
}

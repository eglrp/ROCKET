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


using namespace std;
using namespace gpstk;


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
        cerr << "SatData File Open Error." << endl;
        return 1;
    }



    //---------- Initial Conditions ----------//

    // Initial Time
    int year, mon, day, hour, min;
    double sec;
    try
    {
        year = confData.getValueAsInt("YEAR", "DEFAULT");
        mon  = confData.getValueAsInt("MON", "DEFAULT");
        day  = confData.getValueAsInt("DAY", "DEFAULT");
        hour = confData.getValueAsInt("HOUR", "DEFAULT");
        min  = confData.getValueAsInt("MIN", "DEFAULT");
        sec  = confData.getValueAsDouble("SEC", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Initial Time Error." << endl;
        return 1;
    }

    CivilTime cv0(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( refSys.GPS2UTC(gps0) );


    // Initial Force Model Parameters - SRP Coefficients
    int np_srp(0);
    try
    {
        np_srp = confData.getValueAsInt("SRPNUM", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get The Number of SRP Parameters Error." << endl;
        return 1;
    }

    Vector<double> p0_srp(np_srp,0.0);
    try
    {
        if(9 == np_srp)
        {
            p0_srp(0) = confData.getValueAsDouble("D0", "DEFAULT");
            p0_srp(1) = confData.getValueAsDouble("DC", "DEFAULT");
            p0_srp(2) = confData.getValueAsDouble("DS", "DEFAULT");
            p0_srp(3) = confData.getValueAsDouble("Y0", "DEFAULT");
            p0_srp(4) = confData.getValueAsDouble("YC", "DEFAULT");
            p0_srp(5) = confData.getValueAsDouble("YS", "DEFAULT");
            p0_srp(6) = confData.getValueAsDouble("B0", "DEFAULT");
            p0_srp(7) = confData.getValueAsDouble("BC", "DEFAULT");
            p0_srp(8) = confData.getValueAsDouble("BS", "DEFAULT");
        }
        else if(5 == np_srp)
        {
            p0_srp(0) = confData.getValueAsDouble("D0", "DEFAULT");
            p0_srp(1) = confData.getValueAsDouble("Y0", "DEFAULT");
            p0_srp(2) = confData.getValueAsDouble("B0", "DEFAULT");
            p0_srp(3) = confData.getValueAsDouble("Bc", "DEFAULT");
            p0_srp(4) = confData.getValueAsDouble("Bs", "DEFAULT");
        }
    }
    catch(...)
    {
        cerr << "Get SRP Coefficients Error." << endl;
        return 1;
    }


    //---------- Reference Orbit ----------//

    // IGS SP3 File
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
    srp.setSRPCoeff(p0_srp);

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


    //---------- Orbit Integration ----------//

    // Output File
    string filename("OI.DAT");
    ofstream fout(filename.c_str());
    fout << fixed;

    // Observed Position and Velocity
    Vector<double> r_obs(3,0.0), v_obs(3,0.0);

    // Computed Position and Velocity
    Vector<double> r_com(3,0.0), v_com(3,0.0);


    // Initial Position and Velocity
    Vector<double> r0(3,0.0), v0(3,0.0);


    // Loop for all GPS satellites
    for(int prn=1; prn<32; ++prn)
    {
        // SatID
        SatID sat(prn, SatID::systemGPS);

        cout << "Orbit Integration for " << sat << endl;

        Matrix<double>  t2c( refSys.T2CMatrix(utc0)  );
        Matrix<double> dt2c( refSys.dT2CMatrix(utc0) );

        try
        {
            Vector<double> r_sp3 = sp3Eph.getXvt(sat,gps0).x.toVector();
            Vector<double> v_sp3 = sp3Eph.getXvt(sat,gps0).v.toVector();

            r0 = t2c * r_sp3;
            v0 = t2c * v_sp3 + dt2c * r_sp3;
        }
        catch(...)
        {
            cerr << "Get Initial Position and Velocity Error." << endl;
            continue;
        }

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

        while(t < arcLen*3600)
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

    }  // End of Loop for all GPS satellites

    fout.close();

    return 0;
}

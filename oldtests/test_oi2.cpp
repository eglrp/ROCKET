/* Test for Orbit Integration.
 *
 * Only the equation of motion is considered.
 *
 */

#include <iostream>
#include <fstream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

#include "SP3EphemerisStore.hpp"

#include "SatDataReader.hpp"

#include "GNSSOrbit2.hpp"

#include "RKF67Integrator.hpp"
#include "RKF78Integrator.hpp"
#include "AdamsIntegrator.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{

    //////////////////// Global Files ////////////////////

    // Configuration File
    ConfDataReader confData;
    confData.open("../../ROCKET/oldtests/test.conf");

    // EOP File
    EOPDataStore2 eopDataStore;
    string eopFile;
    eopFile = confData.getValue("IERSEOPFILE", "DEFAULT");
    eopDataStore.loadIERSFile(eopFile);

    eopDataStore.setInterpPoints(8);
    eopDataStore.setRegularization(true);
    eopDataStore.setOceanTides(true);
    eopDataStore.setLibration(true);

    // LeapSecond File
    LeapSecStore leapSecStore;
    string lsFile;
    lsFile = confData.getValue("IERSLSFILE", "DEFAULT");
    leapSecStore.loadFile(lsFile);

    // Reference System
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);

    // Solar System
    SolarSystem solSys;
    string ephFile;
    ephFile = confData.getValue("JPLEPHFILE", "DEFAULT");
    solSys.initializeWithBinaryFile(ephFile);

    // SatData File
    SatDataReader satData;
    string satDataFile;
    satDataFile = confData.getValue("SATDATAFILE", "DEFAULT");
    satData.open(satDataFile);

    // SP3 File
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    string sp3File;
    while( (sp3File=confData.fetchListValue("IGSSP3LIST", "DEFAULT")) != "" )
    {
        sp3Eph.loadFile(sp3File);
    }



    //////////////////// Intial Conditions ////////////////////

    // Initial Time
    int year = confData.getValueAsInt("YEAR", "DEFAULT");
    int mon  = confData.getValueAsInt("MON", "DEFAULT");
    int day  = confData.getValueAsInt("DAY", "DEFAULT");
    int hour = confData.getValueAsInt("HOUR", "DEFAULT");
    int min  = confData.getValueAsInt("MIN", "DEFAULT");
    double sec  = confData.getValueAsDouble("SEC", "DEFAULT");

    CivilTime cv0(year,mon,day,hour,min,sec, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( refSys.GPS2UTC(gps0) );



    //////////////////// Orbit Modeling ////////////////////

    // Earth Gravitation
    EGM08GravityModel egm;
    egm.setDesiredDegreeOrder(12,12);
    egm.setReferenceSystem(refSys);

    string egmFile;
    egmFile = confData.getValue("EGMFILE", "FORCEMODEL");
    egm.loadFile(egmFile);

    // Earth Solid Tide
    EarthSolidTide solidTide;
    solidTide.setReferenceSystem(refSys);
    solidTide.setSolarSystem(solSys);
    egm.setEarthSolidTide(solidTide);

    // Earth Ocean Tide
    EarthOceanTide oceanTide;
    oceanTide.setDesiredDegreeOrder(8,8);
    oceanTide.setReferenceSystem(refSys);
    string eotFile = confData.getValue("EOTFILE", "FORCEMODEL");
    oceanTide.loadFile(eotFile);
    egm.setEarthOceanTide(oceanTide);

    // Earth Pole Tide
    EarthPoleTide poleTide;
    poleTide.setReferenceSystem(refSys);
    egm.setEarthPoleTide(poleTide);

    // Third Body
    ThirdBody thd;
    thd.setSolarSystem(solSys);
    thd.setReferenceSystem(refSys);
    thd.enableMoon();
    thd.enableSun();
    thd.enableVenus();
    thd.enableJupiter();
    thd.enableMars();

    // Initial Force Model Parameters - SRP Coefficients
    Vector<double> p0_srp(5,0.0);
    p0_srp(0) = confData.getValueAsDouble("D0", "DEFAULT");
    p0_srp(1) = confData.getValueAsDouble("Y0", "DEFAULT");
    p0_srp(2) = confData.getValueAsDouble("B0", "DEFAULT");
    p0_srp(3) = confData.getValueAsDouble("BC", "DEFAULT");
    p0_srp(4) = confData.getValueAsDouble("BS", "DEFAULT");

    // Solar Pressure
    CODEPressure srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);
    srp.setSRPCoeff(p0_srp);

    // Relativity Effect
    RelativityEffect rel;


    //////// Integrator Setting ////////

    // RKF Integrator
    RKF78Integrator rkf78;
    double rkf78_step;
    rkf78_step = confData.getValueAsDouble("STEPSIZE_RKF78", "INTEGRATOR");
    rkf78.setStepSize(rkf78_step);

    // Adams Integrator
    AdamsIntegrator adams;
    double adams_step;
    adams_step = confData.getValueAsDouble("STEPSIZE_ADAMS", "INTEGRATOR");
    adams.setStepSize(adams_step);

    // Arc Length
    double arcLen;
    arcLen = confData.getValueAsDouble("ARCLENGTH", "INTEGRATOR");

    // Arc Interval
    double arcInt;
    arcInt = confData.getValueAsDouble("ARCINTERVAL", "INTEGRATOR");

    //////// Orbit Integration ////////

    // Output Setting
    string fname[32];
    fname[ 0] = "GPS_PRN01.OI";
    fname[ 1] = "GPS_PRN02.OI";
    fname[ 2] = "GPS_PRN03.OI";
    fname[ 3] = "GPS_PRN04.OI";
    fname[ 4] = "GPS_PRN05.OI";
    fname[ 5] = "GPS_PRN06.OI";
    fname[ 6] = "GPS_PRN07.OI";
    fname[ 7] = "GPS_PRN08.OI";
    fname[ 8] = "GPS_PRN09.OI";
    fname[ 9] = "GPS_PRN10.OI";
    fname[10] = "GPS_PRN11.OI";
    fname[11] = "GPS_PRN12.OI";
    fname[12] = "GPS_PRN13.OI";
    fname[13] = "GPS_PRN14.OI";
    fname[14] = "GPS_PRN15.OI";
    fname[15] = "GPS_PRN16.OI";
    fname[16] = "GPS_PRN17.OI";
    fname[17] = "GPS_PRN18.OI";
    fname[18] = "GPS_PRN19.OI";
    fname[19] = "GPS_PRN20.OI";
    fname[20] = "GPS_PRN21.OI";
    fname[21] = "GPS_PRN22.OI";
    fname[22] = "GPS_PRN23.OI";
    fname[23] = "GPS_PRN24.OI";
    fname[24] = "GPS_PRN25.OI";
    fname[25] = "GPS_PRN26.OI";
    fname[26] = "GPS_PRN27.OI";
    fname[27] = "GPS_PRN28.OI";
    fname[28] = "GPS_PRN29.OI";
    fname[29] = "GPS_PRN30.OI";
    fname[30] = "GPS_PRN31.OI";
    fname[31] = "GPS_PRN32.OI";

    cout << fixed;

    // Observed Position and Velocity
    Vector<double> r_obs(3,0.0), v_obs(3,0.0);

    // Computed Position and Velocity
    Vector<double> r_com(3,0.0), v_com(3,0.0);

    // Initial Position and Velocity
    Vector<double> r0(3,0.0), v0(3,0.0);

    // Loop for all GPS satellites
    for(int prn=1; prn<=32; ++prn)
    {
        // SatID
        SatID sat(prn, SatID::systemGPS);

        cout << "Orbit Integration for " << sat << endl;

        // Initial Position and Velocity
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
        sc.setNumOfParam(5);
        sc.setSatData(satData);

        // GNSS Orbit
        GNSSOrbit2 gnss;
        gnss.setRefEpoch(utc0);
        gnss.setSpacecraft(sc);
        gnss.setEarthGravitation(egm);
        gnss.setThirdBody(thd);
        gnss.setSolarPressure(srp);
        gnss.setRelativityEffect(rel);


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


        ofstream fout(fname[prn-1].c_str());
        fout << fixed;

        //////// RKF78 Integrator ////////

        // Integrate forward
        for(int i=0; i<8; ++i)
        {
            rkf78.integrateTo(t, y, &gnss, t+adams_step);

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

                fout << setprecision(0);

                fout << setw(10) << dxyz(0)
                     << setw(10) << dxyz(1)
                     << setw(10) << dxyz(2)
                     << endl;

            }

        }  // End of RKF78


        //////// Adams Integrator ////////

        while(t < arcLen*3600.0-900.0)
        {
            adams.integrateTo(t_curr, y_curr, &gnss, t+adams_step);

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

                fout << setprecision(0);
                fout << setw(10) << dxyz(0)
                     << setw(10) << dxyz(1)
                     << setw(10) << dxyz(2)
                     << endl;

            }

        }  // End of Adams

        fout.close();

    }  // End of for(int prn=1; prn<=32; ++prn)

    return 0;
}

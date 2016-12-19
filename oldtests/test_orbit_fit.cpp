/* Test for Orbit Fit. */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

#include "SP3EphemerisStore.hpp"

#include "SatDataReader.hpp"

#include "EGM08GravityModel.hpp"

#include "ThirdBody.hpp"

#include "CODEPressure.hpp"

#include "RelativityEffect.hpp"

#include "GNSSOrbit.hpp"

#include "RKF78Integrator.hpp"

#include "Epoch.hpp"

#include "StringUtils.hpp"

#include "OrbitPrediction.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


int main(void)
{

    //---------- GLobal Data ----------//

    // Configuration File
    ConfDataReader confData;
    try
    {
        confData.open("../../ROCKET/oldtests/test.conf");
    }
    catch(...)
    {
        cerr << "Conf File Open Error." << endl;
        return 1;
    }


    // EOP File
    EOPDataStore2 eopDataStore;
    string eopFile = confData.getValue("IERSEOPFILE", "DEFAULT");
    try
    {
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
    string lsFile = confData.getValue("IERSLSFILE", "DEFAULT");
    try
    {
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
    string ephFile = confData.getValue("JPLEPHFILE", "DEFAULT");
    try
    {
        solSys.initializeWithBinaryFile(ephFile);
    }
    catch(...)
    {
        cerr << "Solar System Initialize Error." << endl;
        return 1;
    }


    // SatData File
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

    egm.setDesiredDegreeOrder(EGMDEG, EGMORD);
    egm.setReferenceSystem(refSys);

    string egmFile = confData.getValue("EGMFILE", "FORCEMODEL");
    try
    {
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

    int EOTDEG, EOTORD;
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
    int np_srp(0);
    try
    {
        np_srp = confData.getValueAsInt("SRPParamNum", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get SRP Parameters Number Error." << endl;
        return 1;
    }

    Vector<double> p0_srp(np_srp,0.0);
    p0_srp(0) = 1.0;

    CODEPressure srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);
    srp.setSRPCoeff(p0_srp);

    // Relativity Effect
    RelativityEffect rel;


    //---------- Integrator Configuration ----------//

    // RKF78 Integrator
    RKF78Integrator rkf78;

    double step_rkf78;
    try
    {
        step_rkf78 = confData.getValueAsDouble("STEPSIZE_RKF78", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Step Size of RKF78 Error." << endl;
        return 1;
    }

    rkf78.setStepSize(step_rkf78);


    // Adams Integrator
    AdamsIntegrator adams;

    double step_adams;
    try
    {
        step_adams = confData.getValueAsDouble("STEPSIZE_ADAMS", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Step Size of Adams Error." << endl;
        return 1;
    }

    adams.setStepSize(step_adams);

    // Arc Length
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

    // Arc Interval
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

    CommonTime utc0;
    CommonTime gps0;

    CivilTime cv0;
    cv0.year    =    asInt( t0.substr( 0, 4) );
    cv0.month   =    asInt( t0.substr( 5, 2) );
    cv0.day     =    asInt( t0.substr( 8, 2) );
    cv0.hour    =    asInt( t0.substr(11, 2) );
    cv0.minute  =    asInt( t0.substr(14, 2) );
    cv0.second  = asDouble( t0.substr(17, 5) );

    string  sys  =  t0.substr(23, 3);

    if("UTC" == sys)        // UTC
    {
        cv0.setTimeSystem(TimeSystem::UTC);
        utc0 = cv0;
        gps0 = refSys.UTC2GPS(utc0);
    }
    else if("GPS" == sys)   // GPS
    {
        cv0.setTimeSystem(TimeSystem::GPS);
        gps0 = cv0;
        utc0 = refSys.GPS2UTC(gps0);
    }

    cout << cv0 << endl;

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


    //---------- Orbit Fit ----------//

    ofstream fics("../../ROCKET/tables/Satellite_ICS.dat");
    fics << fixed;

    Matrix<double>  t2c(3,3,0.0);
    Matrix<double> dt2c(3,3,0.0);

    // Loop for all GPS satellites
    for(int prn=1; prn<=32; ++prn)
    {
        // SatID
        SatID sat(prn, SatID::systemGPS);

        // Initial Conditions
        Vector<double> r0(3,0.0), v0(3,0.0);

         t2c = refSys.T2CMatrix(utc0);
        dt2c = refSys.dT2CMatrix(utc0);

        try
        {
            Vector<double> r_sp3 = sp3Eph.getXvt(sat,gps0).x.toVector();
            Vector<double> v_sp3 = sp3Eph.getXvt(sat,gps0).v.toVector();

            r0 = t2c * r_sp3;
            v0 = t2c * v_sp3 + dt2c * r_sp3;
        }
        catch(...)
        {
            cerr << "Get ICS for " << sat << " Error." << endl;
            continue;
        }

        // Spacecraft
        Spacecraft sc;
        sc.setCurrentTime(utc0);
        sc.setSatID(sat);
        sc.setSatData(satData);
        sc.setCurrentPos(r0);
        sc.setCurrentVel(v0);
        sc.setNumOfParam(np_srp);

        // GNSS Orbit
        GNSSOrbit gnss;
        gnss.setRefEpoch(utc0);
        gnss.setSpacecraft(sc);
        gnss.setEarthGravitation(egm);
        gnss.setThirdBody(thd);
        gnss.setSolarPressure(srp);
        gnss.setRelativityEffect(rel);

        // Orbit Prediction
        OrbitPrediction op;
        op.setInitialTime(utc0);
        op.setSpacecraft(sc);
        op.setArcLength(arcLen);
        op.setArcInterval(arcInt);
        op.setSatOrbit(gnss);
        op.setRKF78Integrator(rkf78);
        op.setAdamsIntegrator(adams);

        // Observed Orbit in ICRS
        int point( int(arcLen)*3600/int(arcInt) );
        Vector<double> orb_obs(3*point, 0.0);

        CommonTime gps, utc;

        Vector<double> r_obs(3,0.0);

        for(int i=0; i<point; ++i)
        {
            gps = gps0 + (i+1)*arcInt;
            utc = refSys.GPS2UTC(gps);

            t2c = refSys.T2CMatrix(utc);

            try
            {
                r_obs = t2c * sp3Eph.getXvt(sat,gps).x.toVector();
            }
            catch(...)
            {
                cerr << "Get Reference Orbit Error." << endl;
                continue;
            }

            orb_obs(i*3+0) = r_obs(0);
            orb_obs(i*3+1) = r_obs(1);
            orb_obs(i*3+2) = r_obs(2);
        }

        // Computed Orbit in ICRS
        Matrix<double> orb_com;

        // Variables for Norm Equation
        Vector<double> l(3*point,0.0);
        Matrix<double> b(3*point,6+np_srp,0.0);
        Vector<double> dx(6+np_srp,0.0);

        orb_com = op.OrbitInt();

        for(int i=0; i<3*point; ++i)
        {
            l(i) = orb_obs(i) - orb_com(i,0);

            for(int j=0; j<6+np_srp; ++j)
            {
                b(i,j) = orb_com(i,j+1);
            }
        }

        Matrix<double> A = transpose(b)*b;
        Vector<double> B = transpose(b)*l;

        dx = inverse(A) * B;

        double sigma(0.0);

        l -= b*dx;

        for(int i=1; i<=3*point; ++i)
        {
            sigma += l(i-1)*l(i-1);
        }

        sigma = std::sqrt(sigma/(3*point-15));

        cout << "Postfit RMS of " << sat << ": "
             << setw(10) << setprecision(3)
             << sigma*1e2 << " (cm)" << endl;

        fics << setprecision(6);

        if(sigma <= 0.02)
        {
            fics << "1" << " " << sat;

            for(int i=0; i<3; ++i)
            {
                fics << " " << setw(20) << r0(i)+dx(i+0);
            }

            for(int i=0; i<3; ++i)
            {
                fics << " " << setw(15) << v0(i)+dx(i+3);
            }

            for(int i=0; i<np_srp; ++i)
            {
                fics << " " << setw(10) << p0_srp(i)+dx(i+6);
            }

            fics << endl;
        }

    }  // End of 'for(int prn=0, ...)'

    fics.close();

    return 0;
}

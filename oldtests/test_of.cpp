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

#include "OrbitPrediction.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{

    // Conf File
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


    // Initial Time in UTC and GPS
    CommonTime utc0;
    CommonTime gps0;

    CivilTime ct;

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

    ct.year     =   year;
    ct.month    =   mon;
    ct.day      =   day;
    ct.hour     =   hour;
    ct.minute   =   min;
    ct.second   =   sec;

    string timeSys;
    try
    {
        timeSys = confData.getValue("TIMESYS", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Initial Time System Error." << endl;
        return 1;
    }

    if("UTC" == timeSys)        // UTC
    {
        ct.setTimeSystem(TimeSystem::UTC);
        utc0 = ct;
        gps0 = refSys.UTC2GPS(utc0);
    }
    else if("GPS" == timeSys)   // GPS
    {
        ct.setTimeSystem(TimeSystem::GPS);
        gps0 = ct;
        utc0 = refSys.GPS2UTC(gps0);
    }


    // SatID
    SatID sat;

    int prn;
    try
    {
        prn = confData.getValueAsInt("SATPRN", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Sat PRN Error." << endl;
        return 1;
    }

    sat.id = prn;

    string satSys;
    try
    {
        satSys = confData.getValueAsInt("SATSYS", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Sat System Error." << endl;
        return 1;
    }

    if("G" == satSys)       // GPS
    {
        sat.system = SatID::systemGPS;
    }
    else if("R" == satSys)  // GLONASS
    {
        sat.system = SatID::systemGlonass;
    }
    else if("E" == satSys)  // GALILEO
    {
        sat.system = SatID::systemGalileo;
    }
    else if("C" == satSys)  // BeiDou
    {
        sat.system = SatID::systemBeiDou;
    }


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


    // Initial Position and Velocity in ITRS
    Vector<double> r_sp3, v_sp3;

    // Transform Matrix at Initial Time
    Matrix<double>  t2c( refSys.T2CMatrix(utc0) );
    Matrix<double> dt2c( refSys.dT2CMatrix(utc0) );

    // Initial Position and Velocity in ICRS
    Vector<double> r0(3,0.0), v0(3,0.0);
    try
    {
        r_sp3 = sp3Eph.getXvt(sat,gps0).x.toVector();
        v_sp3 = sp3Eph.getXvt(sat,gps0).v.toVector();

        r0 = t2c * r_sp3;
        v0 = t2c * v_sp3 + dt2c * r_sp3;
    }
    catch(...)
    {
        cerr << "Get Initial Position and Velocity Error." << endl;
        return 1;
    }


    // Initial SRP Coefficients
    int np = confData.getValueAsInt("SRPNUM", "DEFAULT");
    Vector<double> p0(np,0.0);

    try
    {
        if(9 == np)
        {
            p0(0) = confData.getValueAsDouble("D0", "DEFAULT");
            p0(1) = confData.getValueAsDouble("DC", "DEFAULT");
            p0(2) = confData.getValueAsDouble("DS", "DEFAULT");
            p0(3) = confData.getValueAsDouble("Y0", "DEFAULT");
            p0(4) = confData.getValueAsDouble("YC", "DEFAULT");
            p0(5) = confData.getValueAsDouble("YS", "DEFAULT");
            p0(6) = confData.getValueAsDouble("B0", "DEFAULT");
            p0(7) = confData.getValueAsDouble("BC", "DEFAULT");
            p0(8) = confData.getValueAsDouble("BS", "DEFAULT");
        }
        else if(5 == np)
        {
            p0(0) = confData.getValueAsDouble("D0", "DEFAULT");
            p0(1) = confData.getValueAsDouble("Y0", "DEFAULT");
            p0(2) = confData.getValueAsDouble("B0", "DEFAULT");
            p0(3) = confData.getValueAsDouble("Bc", "DEFAULT");
            p0(4) = confData.getValueAsDouble("Bs", "DEFAULT");
        }
    }
    catch(...)
    {
        cerr << "Get SRP Parameters Error." << endl;
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


    // Spacecraft
    Spacecraft sc;
    sc.setCurrentTime(utc0);
    sc.setSatID(sat);
    sc.setSatData(satData);
    sc.setCurrentPos(r0);
    sc.setCurrentVel(v0);
    sc.setNumOfParam(np);

    // Earth Gravitation
    EGM08GravityModel egm;

    int EGMDEG, EGMORD;
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

    // SRP
    CODEPressure srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);
    srp.setSRPCoeff(p0);


    // Relativity Effect
    RelativityEffect rel;


    // GNSS Orbit
    GNSSOrbit gnss;
    gnss.setRefEpoch(utc0);
    gnss.setSpacecraft(sc);
    gnss.setEarthGravitation(egm);
    gnss.setThirdBody(thd);
    gnss.setSolarPressure(srp);
    gnss.setRelativityEffect(rel);


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
    double length;
    try
    {
        length = confData.getValueAsDouble("ARCLENGTH", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Arc Length Error." << endl;
        return 1;
    }

    // Arc Interval
    double interval;
    try
    {
        interval = confData.getValueAsDouble("ARCINTERVAL", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Arc Interval Error." << endl;
        return 1;
    }

    // Orbit Prediction
    OrbitPrediction op;
    op.setInitialTime(utc0);
    op.setSpacecraft(sc);
    op.setArcLength(length);
    op.setArcInterval(interval);
    op.setSatOrbit(gnss);
    op.setRKF78Integrator(rkf78);
    op.setAdamsIntegrator(adams);


    // Debug
    int mode;
    try
    {
        mode = confData.getValueAsInt("MODE", "DEBUG");
    }
    catch(...)
    {
        cerr << "Get Debug Mode Error." << endl;
        return 1;
    }


    // Observed Orbit in ICRS
    int point( int(length)*3600/int(interval) );
    Vector<double> orb_obs(3*point, 0.0);

    CommonTime gps, utc;

    Vector<double> r_obs(3,0.0);

    for(int i=0; i<point; ++i)
    {
        gps = gps0 + (i+1)*interval;
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
    Matrix<double> b(3*point,6+np,0.0);
    Vector<double> dx(6+np,0.0);

    while(true)
    {
        orb_com = op.OrbitInt();

        for(int i=0; i<3*point; ++i)
        {
            l(i) = orb_obs(i) - orb_com(i,0);

            for(int j=0; j<6+np; ++j)
            {
                b(i,j) = orb_com(i,j+1);
            }
        }

        Matrix<double> A = transpose(b)*b;
        Vector<double> B = transpose(b)*l;

        dx = inverse(A) * B;

        cout << "ICS Corrections: " << endl;

        cout << fixed << setprecision(6);

        for(int i=0; i<6+np; ++i)
        {
            cout << setw(20) << dx(i);
            if((i+1)%3 == 0) cout << endl;
        }

        cout << "Corrected ICS: " << endl;
        for(int i=0; i<3; ++i)
        {
            r0(i) += dx(i+0);
            cout << setw(20) << r0(i);
            if((i+1)%3 == 0) cout << endl;
        }

        for(int i=0; i<3; ++i)
        {
            v0(i) += dx(i+3);
            cout << setw(20) << v0(i);
            if((i+1)%3 == 0) cout << endl;
        }

        for(int i=0; i<np; ++i)
        {
            p0(i) += dx(i+6);
            cout << setw(20) << p0(i);
            if((i+1)%3 == 0) cout << endl;
        }

        double sigma(0.0);

        l -= b*dx;

        for(int i=1; i<=3*point; ++i)
        {
            sigma += l(i-1)*l(i-1);
        }

        sigma = std::sqrt(sigma/point);

        cout << "Postfit RMS of "
             << sat
             << ": "
             << setw(20) << sigma
             << "(m)"
             << endl;

        break;
    }

    return 0;
}

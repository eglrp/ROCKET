/* Test for Orbit Fit. */

#include <iostream>

#include "ConfDataReader.hpp"

#include "DataStructures.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

#include "Rinex3EphemerisStore2.hpp"

#include "SP3EphemerisStore.hpp"

#include "AntexReader.hpp"

#include "ComputeSatPCenter.hpp"

#include "SatDataReader.hpp"

#include "GNSSOrbit.hpp"

#include "RKF78Integrator.hpp"

#include "AdamsIntegrator.hpp"

#include "Epoch.hpp"

#include "StringUtils.hpp"

#include "Counter.hpp"


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
        confData.open("orbit.conf");
    }
    catch(...)
    {
        cerr << "Configuration File Open Error." << endl;
        return 1;
    }


    // EOP File
    EOPDataStore2 eopDataStore;
    try
    {
        string eopFile = confData.getValue("eopFileName", "DEFAULT");
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "EOP File Load Error." << endl;
        return 1;
    }

    eopDataStore.setInterpPoints(4);
    eopDataStore.setRegularization(true);


    // LeapSecond File
    LeapSecStore leapSecStore;
    try
    {
        string lsFile = confData.getValue("lsFileName", "DEFAULT");
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "LeapSecond File Load Error." << endl;
        return 1;
    }


    // Reference System
    ReferenceSystem refSys;
    refSys.setEOPDataStore(eopDataStore);
    refSys.setLeapSecStore(leapSecStore);


    // Solar System
    SolarSystem solSys;
    try
    {
        string ephFile = confData.getValue("jplFileName", "DEFAULT");
        solSys.initializeWithBinaryFile(ephFile);
    }
    catch(...)
    {
        cerr << "Solar System Initialize Error." << endl;
        return 1;
    }


    // SatData File
    SatDataReader satData;
    try
    {
        string satDataFile = confData.getValue("satDataFileName", "DEFAULT");
        satData.open(satDataFile);
    }
    catch(...)
    {
        cerr << "SatData File Open Error." << endl;
        return 1;
    }



    //---------- Force Model Configuration ----------//

    // Earth Gravitation
    EGM08Model egm;

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

    try
    {
        string egmFile = confData.getValue("EGMFILE", "FORCEMODEL");
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

    try
    {
        string eotFile = confData.getValue("EOTFILE", "FORCEMODEL");
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
    int numSRP(5);
    Vector<double> p0_srp(numSRP,0.0);
//    p0_srp(0) = -100.0;

    satVectorMap satSRPCoeff;

    SatID sat;
    for(int i=1; i<=MAX_PRN_GPS; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        satSRPCoeff[sat] = p0_srp;
    }

    ECOM1Model srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);
    srp.setSRPCoeff(satSRPCoeff);


    // Relativity
    Relativity rel;


    // GNSS Orbit
    GNSSOrbit gnss;
    gnss.setEGMModel(egm);
    gnss.setThirdBody(thd);
    gnss.setSRPModel(srp);
    gnss.setRelativity(rel);


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

    rkf78.setEquationOfMotion(gnss);


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

    adams.setEquationOfMotion(gnss);


    // Arc Length
    double arcLen;
    try
    {
        arcLen = confData.getValueAsDouble("OF_ARCLENGTH", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Orbit Fit Arc Length Error." << endl;
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

    CommonTime gps0,utc0,tt0;

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

    tt0 = refSys.GPS2TT(gps0);

    cout << "Initial Time: " << CivilTime(gps0) << endl;


    //---------- Reference Orbit ----------//

    // sp3 file
    string sp3FileListName;

    try
    {
        sp3FileListName = confData.getValue("sp3FileListName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "sp3 file list name get error." << endl;
        exit(-1);
    }

    vector<string> sp3FileListVec;

    ifstream sp3FileListStream;

    sp3FileListStream.open(sp3FileListName.c_str());

    if(!sp3FileListStream)
    {
        cerr << "sp3 file list '" << sp3FileListName
             << "' open error." << endl;
        exit(-1);
    }

    string sp3File;
    while(sp3FileListStream >> sp3File)
    {
        sp3FileListVec.push_back(sp3File);
    }

    sp3FileListStream.close();

    if(sp3FileListVec.size() == 0)
    {
        cerr << "sp3 file list is empty." << endl;
        exit(-1);
    }

    SP3EphemerisStore sp3Store;
    sp3Store.rejectBadPositions(true);
    sp3Store.rejectBadClocks(true);
    sp3Store.setPosGapInterval(900+1);
    sp3Store.setPosMaxInterval(9*900+1);

    vector<string>::const_iterator sp3_it = sp3FileListVec.begin();
    while(sp3_it != sp3FileListVec.end())
    {
        sp3File = (*sp3_it);

        try
        {
            sp3Store.loadFile(sp3File);
        }
        catch(...)
        {
            cerr << "sp3 file '" << sp3File
                 << "' load error." << endl;

            ++sp3_it;
            continue;
        }

        ++sp3_it;
    }


    //---------- Orbit Fit ----------//

    Matrix<double> t2cRaw(3,3,0.0);
    Matrix<double> t2cDot(3,3,0.0);

    Vector<double> r0_itrs(3,0.0);
    Vector<double> v0_itrs(3,0.0);

    Vector<double> r0_icrs(3,0.0);
    Vector<double> v0_icrs(3,0.0);

    // (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
    Vector<double> orbit0(42+6*numSRP,0.0);
    orbit0( 6) = 1.0; orbit0(10) = 1.0; orbit0(14) = 1.0;   // dr0/dr0
    orbit0(33) = 1.0; orbit0(37) = 1.0; orbit0(41) = 1.0;   // dv0/dv0

    satVectorMap satOrbit0;

    string block;
    map<SatID,string> satBlock;

    for(int i=1; i<=MAX_PRN_GPS; ++i)
    {
        sat.id = i;
        sat.system = SatID::systemGPS;

        t2cRaw = refSys.T2CMatrix(utc0);
        t2cDot = refSys.dT2CMatrix(utc0);

        try
        {
            r0_itrs = sp3Store.getXvt(sat,gps0).x.toVector();
            v0_itrs = sp3Store.getXvt(sat,gps0).v.toVector();
        }
        catch(...)
        {
            cerr << "get sp3 orbit of " << sat << " error." << endl;
            continue;
        }

        r0_icrs = t2cRaw * r0_itrs;
        v0_icrs = t2cRaw * v0_itrs + t2cDot * r0_itrs;

//        cout << fixed << setprecision(6);
//        cout << setw(15) << v0_itrs(0)
//             << setw(15) << v0_itrs(1)
//             << setw(15) << v0_itrs(2)
//             << endl;
//        cout << setw(15) << v0_icrs(0)
//             << setw(15) << v0_icrs(1)
//             << setw(15) << v0_icrs(2)
//             << endl;
//
//        break;

        // r0, v0
        orbit0(0) = r0_icrs(0); orbit0(1) = r0_icrs(1); orbit0(2) = r0_icrs(2);
        orbit0(3) = v0_icrs(0); orbit0(4) = v0_icrs(1); orbit0(5) = v0_icrs(2);

        satOrbit0[sat] = orbit0;

        block = satData.getBlock(sat,gps0);
        satBlock[sat] = block;

//        cout << sat << ' ' << block << endl;
    }


    // Orbit Fit
    bool forward(false);

    cout << "Start Orbit Fit: " << endl;

    CommonTime gps(gps0);
    CommonTime utc(utc0);
    CommonTime tt(tt0);

    satVectorMap satOrbit(satOrbit0);

    vector< CommonTime > times;
    times.push_back(tt0);

    vector<satVectorMap> satOrbits;
    satOrbits.push_back(satOrbit0);


    double dt(0.0);

    Vector<double> orbit(42+6*numSRP,0.0);
    Vector<double> r_obs(3,0.0);


    epochMatrixMap epochOrbit;
    satEpochMatrixMap satEpochOrbit;

    Matrix<double> equation(3,7+numSRP,0.0);

    SolarSystem::Planet earth(SolarSystem::Earth);
    SolarSystem::Planet sun(SolarSystem::Sun);
    SolarSystem::Planet moon(SolarSystem::Moon);

    double jd_tt;
    double rv_sun[6] = {0.0};
    double rv_moon[6] = {0.0};
    Vector<double> r_sun(3,0.0);
    Vector<double> r_moon(3,0.0);
    Vector<double> r_sat(3,0.0);
    double shadow(0.0);

//    cout << Counter::now() << endl;

    // rkf78
    rkf78.setCurrentTime(tt0);
    rkf78.setCurrentState(satOrbit0);

//    cout << fixed;

    for(int i=0; i<5*8; ++i)
    {
        if(forward) gps = gps + step_rkf78;
        else        gps = gps - step_rkf78;

        utc = refSys.GPS2UTC(gps);
        tt = refSys.GPS2TT(gps);

//        cout << CivilTime(gps) << endl;

        satOrbit = rkf78.integrateTo(tt);

        rkf78.setCurrentTime(tt);
        rkf78.setCurrentState(satOrbit);

        dt = gps - gps0;

        if( int(dt)%int(step_adams) == 0 )
        {
            times.push_back(tt);
            satOrbits.push_back(satOrbit);
        }

        if( int(dt)%int(arcInt) != 0 ) continue;

        jd_tt = JulianDate(tt).jd;

        solSys.computeState(jd_tt,sun,earth,rv_sun);
        r_sun(0) = rv_sun[0]*1e3;
        r_sun(1) = rv_sun[1]*1e3;
        r_sun(2) = rv_sun[2]*1e3;

        solSys.computeState(jd_tt,moon,earth,rv_moon);
        r_moon(0) = rv_moon[0]*1e3;
        r_moon(1) = rv_moon[1]*1e3;
        r_moon(2) = rv_moon[2]*1e3;

        t2cRaw = refSys.T2CMatrix(utc);

        for(satVectorMap::const_iterator it = satOrbit.begin();
            it != satOrbit.end();
            ++it)
        {
            sat = it->first;
            orbit = it->second;

            r_sat(0) = orbit(0);
            r_sat(1) = orbit(1);
            r_sat(2) = orbit(2);

            shadow = srp.getShadowFunction(r_sat,r_sun,r_moon);

            if(satBlock[sat]=="IIA" && shadow!=1.0)
            {
//                cout << sat << ' ' << shadow << endl;
                continue;
            }

            try
            {
                r_obs = t2cRaw * sp3Store.getXvt(sat,gps).x.toVector();
            }
            catch(...)
            {
                cerr << "get sp3 orbit of " << sat << " error." << endl;
                continue;
            }

            for(int m=0; m<3; ++m)
            {
                equation(m,0) = r_obs(m) - orbit(m);

                for(int n=0; n<3; ++n)
                {
                    equation(m,1+n) = orbit( 6+3*m+n);
                    equation(m,4+n) = orbit(15+3*m+n);
                }

                for(int n=0; n<numSRP; ++n)
                {
                    equation(m,7+n) = orbit(42+numSRP*m+n);
                }

//                for(int n=0; n<6+numSRP; ++n)
//                {
//                    cout << setw(20) << equation(m,1+n);
//                }
//                cout << endl;
            }

//            cout << fixed;
//            cout << sat;
//            cout << setw(20) << equation(0,0)
//                 << setw(20) << equation(1,0)
//                 << setw(20) << equation(2,0)
//                 << endl;

            satEpochMatrixMap::iterator it2( satEpochOrbit.find(sat) );

            // if sat exists
            if(it2 != satEpochOrbit.end())
            {
                epochOrbit = it2->second;

                epochMatrixMap::iterator it3( epochOrbit.find(gps) );

                // if epoch exists
                if(it3 != epochOrbit.end())
                {
                    continue;
                }
                // if epoch not exist, add it
                else
                {
                    epochOrbit[gps] = equation;
                    it2->second = epochOrbit;
                }
            }
            // if sat not exist
            else
            {
                epochOrbit[gps] = equation;
                satEpochOrbit[sat] = epochOrbit;
            }
        }

//        break;

    } // End of 'for(int i=0;...)'

//    cout << Counter::now() << endl;

//    return 0;

    // adams
    adams.setCurrentTime(times);
    adams.setCurrentState(satOrbits);

    vector<SatID> invalidSats;
    CommonTime gps_temp, utc_temp, tt_temp;
    satVectorMap invalidSatOrbit;

    while(true)
    {
        gps_temp = gps;
        utc_temp = refSys.GPS2UTC(gps_temp);
        tt_temp = refSys.GPS2TT(gps_temp);

        if(forward) gps = gps + step_adams;
        else        gps = gps - step_adams;

        utc = refSys.GPS2UTC(gps);
        tt = refSys.GPS2TT(gps);

        satOrbit = adams.integrateTo(tt);

        invalidSats = adams.getInvalidSats();

//        cout << CivilTime(gps) << ' ' << invalidSats.size() << endl;

        if(invalidSats.size() != 0)
        {
            // prepare initial state of invalid sats
            for(int i=0; i<invalidSats.size(); ++i)
            {
                sat = invalidSats[i];

                invalidSatOrbit[sat] = (satOrbits[8])[sat];
            }

            // set initial time and state of rkf78
            rkf78.setCurrentTime(tt_temp);
            rkf78.setCurrentState(invalidSatOrbit);

            // perform rkf78
            for(int j=0; j<5; ++j)
            {
                if(forward) gps_temp = gps_temp + step_rkf78;
                else        gps_temp = gps_temp - step_rkf78;

                utc_temp = refSys.GPS2UTC(gps_temp);
                tt_temp = refSys.GPS2TT(gps_temp);

                invalidSatOrbit = rkf78.integrateTo(tt_temp);

                rkf78.setCurrentTime(tt_temp);
                rkf78.setCurrentState(invalidSatOrbit);
            }

            // replace state of invalid sats
            for(int i=0; i<invalidSats.size(); ++i)
            {
                sat = invalidSats[i];

                satOrbit[sat] = invalidSatOrbit[sat];
            }
        }

        times.push_back(tt);
        times.erase(times.begin());
        satOrbits.push_back(satOrbit);
        satOrbits.erase(satOrbits.begin());

        adams.setCurrentTime(times);
        adams.setCurrentState(satOrbits);

        dt = gps - gps0;

        if(std::abs(dt) >= arcLen*3600.0) break;

        if( int(dt)%int(arcInt) != 0 ) continue;

        jd_tt = JulianDate(tt).jd;

        solSys.computeState(jd_tt,sun,earth,rv_sun);
        r_sun(0) = rv_sun[0]*1e3;
        r_sun(1) = rv_sun[1]*1e3;
        r_sun(2) = rv_sun[2]*1e3;

        solSys.computeState(jd_tt,moon,earth,rv_moon);
        r_moon(0) = rv_moon[0]*1e3;
        r_moon(1) = rv_moon[1]*1e3;
        r_moon(2) = rv_moon[2]*1e3;

        t2cRaw = refSys.T2CMatrix(utc);

        for(satVectorMap::const_iterator it = satOrbit.begin();
            it != satOrbit.end();
            ++it)
        {
            sat = it->first;
            orbit = it->second;

            r_sat(0) = orbit(0);
            r_sat(1) = orbit(1);
            r_sat(2) = orbit(2);

            shadow = srp.getShadowFunction(r_sat,r_sun,r_moon);

            if(satBlock[sat]=="IIA" && shadow!=1.0)
            {
//                cout << sat << ' ' << shadow << endl;
                continue;
            }

            try
            {
                r_obs = t2cRaw * sp3Store.getXvt(sat,gps).x.toVector();
            }
            catch(...)
            {
                cerr << "get sp3 orbit of " << sat << " error." << endl;
                continue;
            }

            for(int m=0; m<3; ++m)
            {
                equation(m,0) = r_obs(m) - orbit(m);

                for(int n=0; n<3; ++n)
                {
                    equation(m,1+n) = orbit( 6+3*m+n);
                    equation(m,4+n) = orbit(15+3*m+n);
                }

                for(int n=0; n<numSRP; ++n)
                {
                    equation(m,7+n) = orbit(42+numSRP*m+n);
                }
            }

//            cout << fixed;
//            cout << sat;
//            cout << setw(20) << equation(0,0)
//                 << setw(20) << equation(1,0)
//                 << setw(20) << equation(2,0)
//                 << endl;

            satEpochMatrixMap::iterator it2( satEpochOrbit.find(sat) );

            // if sat exists
            if(it2 != satEpochOrbit.end())
            {
                epochOrbit = it2->second;

                epochMatrixMap::iterator it3( epochOrbit.find(gps) );

                // if epoch exists
                if(it3 != epochOrbit.end())
                {
                    continue;
                }
                // if epoch not exist, add it
                else
                {
                    epochOrbit[gps] = equation;
                    it2->second = epochOrbit;
                }
            }
            // if sat not exist
            else
            {
                epochOrbit[gps] = equation;
                satEpochOrbit[sat] = epochOrbit;
            }
        }

    } // End of 'while(true)'

//    cout << Counter::now() << endl;

    string icsFile;

    try
    {
        icsFile = confData.getValue("satICsFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "ICs File Name Get Error." << endl;
        return 1;
    }

    ofstream fics(icsFile.c_str());
    fics << fixed;

    for(satEpochMatrixMap::const_iterator it = satEpochOrbit.begin();
        it != satEpochOrbit.end();
        ++it)
    {
        sat = it->first;
        epochOrbit = it->second;

        int numEqu( epochOrbit.size() );

        Vector<double> omc(numEqu*3,0.0);
        Matrix<double> design(numEqu*3,6+numSRP,0.0);

        int count(0);
        for(epochMatrixMap::const_iterator it2 = epochOrbit.begin();
            it2 != epochOrbit.end();
            ++it2)
        {
            equation = it2->second;

            for(int i=0; i<3; ++i)
            {
                omc(3*count+i) = equation(i,0);

                for(int j=1; j<7+numSRP; ++j)
                {
                    design(3*count+i,j-1) = equation(i,j);
                }
            }

            ++count;
        }


        Matrix<double> A( transpose(design)*design );
        Vector<double> B( transpose(design)*omc );

        Vector<double> dx( inverse(A)*B );

        double sigma(0.0);

        omc -= design*dx;

        for(int i=1; i<=numEqu*3; ++i)
            sigma += omc(i-1)*omc(i-1);

        sigma = std::sqrt(sigma/(numEqu*3-6-numSRP));

        if(arcLen > 40 && sigma > 0.05) continue;
//        if(arcLen < 40 && sigma > 0.02) continue;

        cout << "Postfit RMS of "
             << sat << ": "
             << setw(10)
             << asString(sigma*1e2,3)
             << " (cm)" << endl;

        orbit0 = satOrbit0[sat];

        fics << sat;

        fics << fixed
             << setprecision(6)
             << setw(20) << dx( 0) + orbit0(0)
             << setw(20) << dx( 1) + orbit0(1)
             << setw(20) << dx( 2) + orbit0(2)
             << setprecision(6)
             << setw(15) << dx( 3) + orbit0(3)
             << setw(15) << dx( 4) + orbit0(4)
             << setw(15) << dx( 5) + orbit0(5);

        for(int i=0; i<numSRP; ++i)
        {
            fics << setprecision(3)
                 << setw(10) << dx(6+i) + p0_srp(i);
        }

        fics << endl;
    }

//    cout << Counter::now() << endl;


    fics.close();

    return 0;

}

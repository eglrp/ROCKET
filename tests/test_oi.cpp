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
        confData.open("orbit.conf");
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
        eopFile = confData.getValue("eopFileName", "DEFAULT");
        eopDataStore.loadIERSFile(eopFile);
    }
    catch(...)
    {
        cerr << "EOP File Load Error." << endl;
        return 1;
    }
    eopDataStore.setInterpPoints(2);
    eopDataStore.setRegularization(true);

    // LeapSecond File
    LeapSecStore leapSecStore;

    string lsFile;
    try
    {
        lsFile = confData.getValue("lsFileName", "DEFAULT");
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

    string ephFile;
    try
    {
        ephFile = confData.getValue("jplFileName", "DEFAULT");
        solSys.initializeWithBinaryFile(ephFile);
    }
    catch(...)
    {
        cerr << "Solar System Initialize Error." << endl;
        return 1;
    }

    // Satellite Data File
    SatDataReader satData;

    string satDataFile = confData.getValue("satDataFileName", "DEFAULT");
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
    ECOM1Model srp;
    srp.setReferenceSystem(refSys);
    srp.setSolarSystem(solSys);

    // Relativity
    Relativity rel;


    //---------- Intergrator Configuration ----------//

    // RKF78 Integrator
    RKF78Integrator rkf78;

    double step_rkf78;
    try
    {
        step_rkf78 = confData.getValueAsDouble("STEPSIZE_RKF78", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get RKF78 Step Size Error." << endl;
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
        cerr << "Get Adams Step Size Error." << endl;
        return 1;
    }

    adams.setStepSize(step_adams);


    // Arc Settings
    double arcLen;
    try
    {
        arcLen = confData.getValueAsDouble("OI_ARCLENGTH", "INTEGRATOR");
    }
    catch(...)
    {
        cerr << "Get Orbit Integration Arc Length Error." << endl;
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
    CommonTime tt0( refSys.GPS2TT(gps0) );

//    cout << "Initial Time: " << CivilTime(gps0) << endl;


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
        return 1;
    }

    vector<string> sp3FileListVec;

    ifstream sp3FileListStream;

    sp3FileListStream.open(sp3FileListName.c_str());

    if(!sp3FileListStream)
    {
        cerr << "sp3 file list '" << sp3FileListName
             << "' open error." << endl;
        return 1;
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
        return 1;
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


    //---------- Orbit Integration ----------//

    int mode(1);

    int numSRP(5);

    // Initial Conditions
    Vector<double> r0(3,0.0);
    Vector<double> v0(3,0.0);
    Vector<double> p0_srp(numSRP,0.0);

    // (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
    Vector<double> orbit0(96,0.0);
    orbit0( 6) = 1.0; orbit0(10) = 1.0; orbit0(14) = 1.0;   // dr0/dr0
    orbit0(33) = 1.0; orbit0(37) = 1.0; orbit0(41) = 1.0;   // dv0/dv0
    satVectorMap satOrbit0;

    satVectorMap satSRP0;


    // Transformation Matrices
    Matrix<double> t2cRaw(3,3,0.0);
    Matrix<double> t2cDot(3,3,0.0);

    // Sat ICs File
    string satICsFile;
    try
    {
        satICsFile = confData.getValue("satICsFileName", "DEFAULT");
    }
    catch(...)
    {
        cerr << "Get Sat ICs File Error." << endl;
        return 1;
    }

    ifstream fics(satICsFile.c_str());

    SatID sat;

    string line;
    while(!fics.eof())
    {
        getline(fics,line);

        if(fics.eof()) break;

        string sys =        line.substr( 0, 1);
        int    prn = asInt( line.substr( 2, 2) );

        // SatID
        sat.id = prn;
        if(sys == "G")        sat.system = SatID::systemGPS;
        else if(sys == "R")   sat.system = SatID::systemGLONASS;
        else if(sys == "E")   sat.system = SatID::systemGalileo;
        else if(sys == "C")   sat.system = SatID::systemBDS;

        // initial orbit
        // r0
        orbit0(0) = asDouble( line.substr(  5,20) );
        orbit0(1) = asDouble( line.substr( 25,20) );
        orbit0(2) = asDouble( line.substr( 45,20) );

        // v0
        orbit0(3) = asDouble( line.substr( 65,15) );
        orbit0(4) = asDouble( line.substr( 80,15) );
        orbit0(5) = asDouble( line.substr( 95,15) );

        satOrbit0[sat] = orbit0;

        // srp0
        for(int i=0; i<numSRP; ++i)
        {
            p0_srp(i) = asDouble( line.substr(110+10*i,10) );
        }

        satSRP0[sat] = p0_srp;
    }

    fics.close();


    srp.setSRPCoeff(satSRP0);


    // GNSS Orbit
    GNSSOrbit gnss;

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
        return 1;
    }

    if(bEGM) gnss.setEGMModel(egm);
    if(bThd) gnss.setThirdBody(thd);
    if(bSRP) gnss.setSRPModel(srp);
    if(bRel) gnss.setRelativity(rel);

    rkf78.setEquationOfMotion(gnss);
    adams.setEquationOfMotion(gnss);

    // current time
    CommonTime gps(gps0);
    CommonTime utc(utc0);
    CommonTime tt(tt0);
    vector<CommonTime> times;
    times.push_back(tt0);

    // current orbit
    Vector<double> orbit;
    satVectorMap satOrbit;
    vector<satVectorMap> satOrbits;
    satOrbits.push_back(satOrbit0);


    Vector<double> r_com(3,0.0);
    Vector<double> r_tmp(3,0.0), v_tmp(3,0.0);
    Vector<double> r_ref(3,0.0), v_ref(3,0.0);

    Vector<double> dXYZ(3,0.0);
    Vector<double> dRTN(3,0.0);


    double dt(0.0);

    cout << fixed << setprecision(3);

    //////// RKF78 Integrator ////////

    rkf78.setCurrentTime(tt0);
    rkf78.setCurrentState(satOrbit0);

    // Compute 8 start points for Adams Integrator
    for(int i=0; i<5*8; ++i)
    {
        gps = gps + step_rkf78;
        utc = refSys.GPS2UTC(gps);
        tt = refSys.GPS2TT(gps);

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

        t2cRaw = refSys.T2CMatrix(utc);
        t2cDot = refSys.dT2CMatrix(utc);

        cout << setw( 5) << YDSTime(gps).doy
             << setw(10) << YDSTime(gps).sod;

        for(satVectorMap::const_iterator it = satOrbit.begin();
            it != satOrbit.end();
            ++it)
        {
            sat = it->first;
            orbit = it->second;

            r_com(0) = orbit(0);
            r_com(1) = orbit(1);
            r_com(2) = orbit(2);

            try
            {
                r_tmp = sp3Store.getXvt(sat,gps).x.toVector();
                v_tmp = sp3Store.getXvt(sat,gps).v.toVector();

                r_ref = t2cRaw * r_tmp;
                v_ref = t2cRaw * v_tmp + t2cDot * r_tmp;
            }
            catch(...)
            {
                cerr << "get reference orbit error." << endl;
                cout << setw(24) << string("");
                continue;
            }

            dXYZ = r_com - r_ref;
            dRTN = refSys.XYZ2RTN(dXYZ, r_ref,v_ref);

//            cout << setw(5) << sat;
            cout << setw(8) << dRTN(0)
                 << setw(8) << dRTN(1)
                 << setw(8) << dRTN(2);
//                 << setw(8) << norm(dRTN);
//            cout << setw(5) << sat
//                 << setw(20) << r_com(0)
//                 << setw(20) << r_com(1)
//                 << setw(20) << r_com(2);
        }

        cout << endl;

    }  // End of RKF78


    //////// Adams Integrator ////////

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

        gps = gps + step_adams;
        utc = refSys.GPS2UTC(gps);
        tt = refSys.GPS2TT(gps);

        satOrbit = adams.integrateTo(tt);

        invalidSats = adams.getInvalidSats();

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
                gps_temp = gps_temp + step_rkf78;
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

        if(std::abs(dt) > arcLen*3600.0) break;

        if( int(dt)%int(arcInt) != 0 ) continue;

        t2cRaw = refSys.T2CMatrix(utc);

        cout << setw( 5) << YDSTime(gps).doy
             << setw(10) << YDSTime(gps).sod;

        for(satVectorMap::const_iterator it = satOrbit.begin();
            it != satOrbit.end();
            ++it)
        {
            sat = it->first;
            orbit = it->second;

            r_com(0) = orbit(0);
            r_com(1) = orbit(1);
            r_com(2) = orbit(2);

            try
            {
                r_tmp = sp3Store.getXvt(sat,gps).x.toVector();
                v_tmp = sp3Store.getXvt(sat,gps).v.toVector();

                r_ref = t2cRaw * r_tmp;
                v_ref = t2cRaw * v_tmp + t2cDot * r_tmp;
            }
            catch(...)
            {
                cerr << "get reference orbit error." << endl;
                cout << setw(24) << string("");
                continue;
            }

            dXYZ = r_com - r_ref;
            dRTN = refSys.XYZ2RTN(dXYZ, r_ref,v_ref);

//            cout << setw(5) << sat;
            cout << setw(8) << dRTN(0)
                 << setw(8) << dRTN(1)
                 << setw(8) << dRTN(2);
//                 << setw(8) << norm(dRTN);
//            cout << setw(5) << sat
//                 << setw(20) << r_com(0)
//                 << setw(20) << r_com(1)
//                 << setw(20) << r_com(2);
        }

        cout << endl;

    }  // End of Adams

    return 0;
}

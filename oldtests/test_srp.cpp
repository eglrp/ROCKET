#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"

#include "SolarSystem.hpp"

#include "CODEPressure.hpp"

#include "SatDataReader.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{
   // Configuration Data
   ConfDataReader confReader;

   try
   {
      confReader.open("../../ROCKET/oldtests/test.conf");
   }
   catch(...)
   {
      cerr << "Configuration File Open Error." << endl;

      return 1;
   }

   // EOP Data
   EOPDataStore eopDataStore;

   string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");

   try
   {
      eopDataStore.loadIERSFile(eopFile);
   }
   catch(...)
   {
      cerr << "EOP File Load Error." << endl;

      return 1;
   }

   LeapSecStore leapSecStore;

   string lsFile = confReader.getValue("IERSLSFile", "DEFAULT");

   try
   {
      leapSecStore.loadFile(lsFile);
   }
   catch(...)
   {
      cerr << "LS File Load Error." << endl;

      return 1;
   }

   // Reference System
   ReferenceSystem refSys;
   refSys.setEOPDataStore(eopDataStore);
   refSys.setLeapSecStore(leapSecStore);

   // Solar System
   SolarSystem solSys;

   string ephFile = confReader.getValue("JPLEPHFile", "DEFAULT");

   try
   {
      solSys.initializeWithBinaryFile(ephFile);
   }
   catch(...)
   {
      cerr << "EPH File Load Error." << endl;

      return 1;
   }

   // sat ID
   SatID sat(1, SatID::systemGPS);

   // time
   CivilTime ct(2015,1,1,12,1,15.0, TimeSystem::GPS);
   CommonTime gps( ct.convertToCommonTime() );
   CommonTime utc( refSys.GPS2UTC(gps) );

   // initial conditions
   Vector<double> r0(3,0.0);
   r0[0] =  17253.546071e3;
   r0[1] = -19971.1571565e3;
   r0[2] =   3645.8013286e3;

   Vector<double> v0(3,0.0);
   v0[0] =  1.9732093e3;
   v0[1] =  1.1233114e3;
   v0[2] = -3.1241455e3;

   Vector<double> p0(9,0.0);
   p0[0] = 0.999989506;
   p0[1] = 0.015272206;
   p0[2] = 0.000216184;

   p0[3] = -0.000367154;
   p0[4] = -0.000701573;
   p0[5] =  0.000985358;

   p0[6] = -0.003625165;
   p0[7] =  0.009091403;
   p0[8] = -0.001752761;

   EarthBody rb;

   // Sat Data
   SatDataReader satReader;

   string satFile = confReader.getValue("SatDataFile", "DEFAULT");

   try
   {
      satReader.open(satFile);
   }
   catch(...)
   {
      cerr << "Sat File Load Error." << endl;

      return 1;
   }

   // Spacecraft
   Spacecraft sc;
   sc.setPosition(r0);
   sc.setVelocity(v0);
   sc.setNumOfParam(9);

   sc.setSatID(sat);
   sc.setCurrentTime(utc);
   sc.setBlockType( satReader.getBlock(sat,utc) );
   sc.setMass( satReader.getMass(sat,utc) );

   // CODE SRP
   CODEPressure code;
   code.setReferenceSystem(refSys);
   code.setSolarSystem(solSys);
   code.setSRPCoeff(p0);

   code.doCompute(utc, rb, sc);

   Vector<double> f_srp(3,0.0);
   f_srp = code.getAcceleration();

   cout << setprecision(8);
   cout << "CODE SRP: " << endl
        << f_srp << endl;

    return 0;
}

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

   // sat id
   SatID satid(1, SatID::systemGPS);

   // time
   CivilTime ct(2015,1,1,12,1,15.0, TimeSystem::GPS);
   CommonTime gps( ct.convertToCommonTime() );
   CommonTime utc( refSys.GPS2UTC(gps) );

   // initial conditions
   Vector<double> rv(6,0.0);
   rv[0] =  17253.546071e3;
   rv[1] = -19971.1571565e3;
   rv[2] =   3645.8013286e3;
   rv[3] =  1.9732093e3;
   rv[4] =  1.1233114e3;
   rv[5] = -3.1241455e3;

   Vector<double> p(9,0.0);
   p[0] = 0.999989506;
   p[1] = -0.000367154;
   p[2] = -0.003625165;
   p[3] = 0.015272206;
   p[4] = 0.000216184;
   p[5] = -0.000701573;
   p[6] = 0.000985358;
   p[7] = 0.009091403;
   p[8] = -0.001752761;

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
   sc.initStateVector(rv, p);
   sc.setSatID(satid);
   sc.setEpoch(utc);
   sc.setBlock( satReader.getBlock(satid,utc) );
   sc.setMass( satReader.getMass(satid,utc) );

   // CODE SRP
   CODEPressure code;
   code.setReferenceSystem(refSys);
   code.setSolarSystem(solSys);

   code.doCompute(utc, rb, sc);

   Vector<double> f_srp(3,0.0);
   f_srp = code.getAccel();

   cout << setprecision(8);
   cout << "CODE SRP: " << endl
        << f_srp << endl;

    return 0;
}

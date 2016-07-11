/* Test for Orbit Integration. */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

#include "SP3EphemerisStore.hpp"

#include "SatDataReader.hpp"

#include "EGM08GravityModel.hpp"

#include "MoonGravitation.hpp"
#include "SunGravitation.hpp"

#include "CODEPressure.hpp"

#include "RelativityEffect.hpp"

#include "RungeKuttaFehlberg.hpp"

#include "Epoch.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{
   // Conf File
   ConfDataReader confReader;

   try
   {
      confReader.open("../../ROCKET/oldtests/test.conf");
   }
   catch(...)
   {
      cerr << "Conf File Open Error." << endl;

      return 1;
   }

   // EOP File
   EOPDataStore eopDataStore;
   string eopFile = confReader.getValue("IERSEOPFILE", "DEFAULT");

   try
   {
      eopDataStore.loadIERSFile(eopFile); 
   }
   catch(...)
   {
      cerr << "IERS EOP File Load Error." << endl;

      return 1;
   }
 
   // LeapSecond Data
   LeapSecStore leapSecStore;

   string lsFile = confReader.getValue("IERSLSFILE", "DEFAULT");

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

   string ephFile = confReader.getValue("JPLEPHFILE", "DEFAULT");
   try
   {
      solSys.initializeWithBinaryFile(ephFile);
   }
   catch(...)
   {
      cerr << "Solar System Initialize Error." << endl;

      return 1;
   }


   // Initial time
   CivilTime cv0(2015,1,1,12,0,0.0, TimeSystem::GPS);
   CommonTime gps0( cv0.convertToCommonTime() );
   CommonTime utc0( refSys.GPS2UTC(gps0) );

   // Satellite ID
   SatID sat(1, SatID::systemGPS);

   // IGS SP3 File
   SP3EphemerisStore sp3Eph;
   sp3Eph.rejectBadPositions(true);
   sp3Eph.setPosGapInterval(900+1);
   sp3Eph.setPosMaxInterval(9*900+1);

   string sp3File;

   while( (sp3File=confReader.getValue("IGSSP3LIST", "DEFAULT")) != "" )
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

   // r0, v0 in ITRS
   Vector<double> r0_itrs, v0_itrs;
   try
   {
      r0_itrs = sp3Eph.getXvt(sat, gps0).x.toVector();
      v0_itrs = sp3Eph.getXvt(sat, gps0).v.toVector();
   }
   catch(...)
   {
      cerr << "Get r0 and v0 from SP3 File Error." << endl;

      return 1;
   }

   // Transform Matrix between ICRS and ITRS
   Matrix<double> c2t ( refSys.C2TMatrix(utc0)  );
   Matrix<double> dc2t( refSys.dC2TMatrix(utc0) );

   // r0, v0 in ICRS
   Vector<double> r0_icrs, v0_icrs;
   r0_icrs = transpose(c2t) * r0_itrs;
   v0_icrs = transpose(c2t) * v0_itrs + transpose(dc2t) * r0_itrs;


   // Initial State: r0, v0
   Vector<double> rv0(6,0.0);
   rv0(0) = r0_icrs(0); rv0(1) = r0_icrs(1); rv0(2) = r0_icrs(2);
   rv0(3) = v0_icrs(0); rv0(4) = v0_icrs(1); rv0(5) = v0_icrs(2);

   cout << fixed << setprecision(6);
   cout << rv0 << endl;

   // Intial State: p0
   Vector<double> p0(9,0.0);
   p0[0] = 0.999989506;
   p0[1] = -0.000367154;
   p0[2] = -0.003625165;
   p0[3] = 0.015272206;
   p0[4] = 0.000216184;
   p0[5] = -0.000701573;
   p0[6] = 0.000985358;
   p0[7] = 0.009091403;
   p0[8] = -0.001752761;

   // SatData File
   SatDataReader satData;

   string satDataFile = confReader.getValue("SATDATAFILE", "DEFAULT");
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
   sc.setSatID(sat);
   sc.setEpoch(utc0);
   sc.setBlock(satData.getBlock(sat,utc0));
   sc.setMass(satData.getMass(sat,utc0));
   sc.initStateVector(rv0, p0);


   // to do it later ...


   return 0;
}

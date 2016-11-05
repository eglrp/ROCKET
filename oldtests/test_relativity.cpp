/*
 * Test for Relativity Effect.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "RelativityEffect.hpp"

#include "SP3EphemerisStore.hpp"
#include "SatDataReader.hpp"

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
   EOPDataStore2 eopDataStore;

   try
   {
      string eopFile = confReader.getValue("IERSEOPFILE", "DEFAULT");
      eopDataStore.loadIERSFile(eopFile);
   }
   catch(...)
   {
      cerr << "EOP File Load Error." << endl;

      return 1;
   }

   // LeapSec File
   LeapSecStore leapSecStore;

   try
   {
      string lsFile  = confReader.getValue("IERSLSFILE", "DEFAULT");
      leapSecStore.loadFile(lsFile);
   }
   catch(...)
   {
      cerr << "LeapSec File Load Error." << endl;

      return 1;
   }

   // Reference System
   ReferenceSystem refSys;
   refSys.setEOPDataStore(eopDataStore);
   refSys.setLeapSecStore(leapSecStore);

   // Sat ID
   int prn;
   try
   {
       prn = confReader.getValueAsInt("SATPRN", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get Sat PRN Error." << endl;

       return 1;
   }

   SatID sat(prn, SatID::systemGPS);

   // Initial Time
   int year, mon, day, hour, min;
   double sec;

   try
   {
       year = confReader.getValueAsInt("YEAR", "DEFAULT");
       mon  = confReader.getValueAsInt("MON", "DEFAULT");
       day  = confReader.getValueAsInt("DAY", "DEFAULT");
       hour = confReader.getValueAsInt("HOUR", "DEFAULT");
       min  = confReader.getValueAsInt("MIN", "DEFAULT");
       sec  = confReader.getValueAsDouble("SEC", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get Initial Time Error." << endl;

       return 1;
   }

   CivilTime CT(year,mon,day,hour,min,sec, TimeSystem::GPS);
   CommonTime GPS0( CT.convertToCommonTime() );
   CommonTime UTC0( refSys.GPS2UTC(GPS0) );

   // SP3 File
   SP3EphemerisStore sp3Eph;
   sp3Eph.rejectBadPositions(true);
   sp3Eph.setPosGapInterval(900+1);
   sp3Eph.setPosMaxInterval(9*900+1);

   string sp3File;
   while(true)
   {
      try
      {
         sp3File = confReader.fetchListValue("IGSSP3LIST", "DEFAULT");

         if(sp3File == "") break;

         sp3Eph.loadFile(sp3File);
      }
      catch(...)
      {
         cerr << "IGS SP3 File Load Error." << endl;

         return 1;
      }
   }

   // Earth Body
   EarthBody eb;

   // SatData File
   SatDataReader satData;

   try
   {
       string satDataFile = confReader.getValue("SatDataFile", "DEFAULT");
       satData.open(satDataFile);
   }
   catch(...)
   {
       cerr << "SatData File Open Error." << endl;

       return 1;
   }

   // Spacecraft
   Spacecraft sc;
   sc.setNumOfParam(0);
   sc.setSatID(sat);
   sc.setSatData(satData);

   // Relativity Effect
   RelativityEffect rel;


   // Length
   double length;

   try
   {
       length = confReader.getValueAsDouble("ARCLENGTH", "INTEGRATOR");
   }
   catch(...)
   {
       cerr << "Get Time Length Error." << endl;

       return 1;
   }

   cout << fixed << setprecision(15);

   int i = 0;

   while(true)
   {
       // Current Time
       CommonTime GPS( GPS0 + i*900.0 );
       CommonTime UTC( refSys.GPS2UTC(GPS) );

       // Current Position and Velocity in ITRS
       Vector<double> r_itrs, v_itrs;

       try
       {
           r_itrs = sp3Eph.getXvt(sat, GPS).x.toVector();
           v_itrs = sp3Eph.getXvt(sat, GPS).v.toVector();
       }
       catch(...)
       {
           cerr << "Get Position and Velocity from SP3 File Error." << endl;

           return 1;
       }

       // Current Transform Matrix
       Matrix<double> c2t ( refSys.C2TMatrix(UTC)  );
       Matrix<double> t2c ( transpose(c2t) );

       // Current Transform Matrix Time Dot
       Matrix<double> dc2t( refSys.dC2TMatrix(UTC) );
       Matrix<double> dt2c( transpose(dc2t) );

       // Current Position and Velocity in ICRS
       Vector<double> r_icrs = t2c * r_itrs;
       Vector<double> v_icrs = t2c * v_itrs + dt2c * r_itrs;

       // Current Spacecraft
       sc.setCurrentPos(r_icrs);
       sc.setCurrentVel(v_icrs);
       sc.setCurrentTime(UTC);

       // Current Acceleration
       rel.doCompute(UTC, eb, sc);
       Vector<double> a_icrs( rel.getAcceleration() );
       Vector<double> a_itrs( c2t * a_icrs );

       cout << CivilTime(GPS);
       cout << setw(20) << a_icrs(0)
            << setw(20) << a_icrs(1)
            << setw(20) << a_icrs(2)
            << endl;

       i++;

       if(i > length*3600/900) break;
   }

   return 0;
}

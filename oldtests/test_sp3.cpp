#include <iostream>
#include <iomanip>

#include "ConfDataReader.hpp"

#include "SP3EphemerisStore.hpp"

#include "ReferenceSystem.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
   ConfDataReader confReader;

   try
   {
      confReader.open("../../ROCKET/oldtests/test.conf");
   }
   catch(...)
   {
      cerr << "Configuration file open error." << endl;

      return 1;
   }

   // sp3 files
   SP3EphemerisStore sp3Eph;
   sp3Eph.rejectBadPositions(true);
   sp3Eph.setPosGapInterval(901);
   sp3Eph.setPosMaxInterval(8101);

   string sp3File;
   while( (sp3File=confReader.fetchListValue("IGSSP3List", "DEFAULT")) != "" )
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


   // EOP Data
   EOPDataStore2 eopDataStore;

   string eopFile = confReader.getValue("IERSEOPFile", "DEFAULT");
   try
   {
      eopDataStore.loadIERSFile(eopFile);
   }
   catch(...)
   {
      cerr << "IERS EOP File Load Error." << endl;

      return 1;
   }

   // Leap Second Data
   LeapSecStore leapSecStore;

   string lsFile = confReader.getValue("IERSLSFile", "DEFAULT");
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

   // Initial Time
   int year = confReader.getValueAsInt("YEAR", "DEFAULT");
   int mon  = confReader.getValueAsInt("MON", "DEFAULT");
   int day  = confReader.getValueAsInt("DAY", "DEFAULT");
   int hour = confReader.getValueAsInt("HOUR", "DEFAULT");
   int min  = confReader.getValueAsInt("MIN", "DEFAULT");
   double sec = confReader.getValueAsDouble("SEC", "DEFAULT");

   CivilTime ct0(year,mon,day,hour,min,sec, TimeSystem::GPS);
   CommonTime gps0( ct0.convertToCommonTime() );

   // Sat ID
   int prn = confReader.getValueAsInt("PRN", "DEFAULT");
   SatID sat(prn,SatID::systemGPS);


   for(int i=0; i<=24*3600/900; i++)
   {
      CommonTime gps = gps0 + i*900.0;

      CommonTime utc( refSys.GPS2UTC(gps) );

      Matrix<double>  c2t( refSys.C2TMatrix(utc) );
      Matrix<double> dc2t( refSys.dC2TMatrix(utc) );

      Vector<double> sp3Pos(3,0.0), sp3Vel(3,0.0);
      Vector<double> eciPos(3,0.0), eciVel(3,0.0);

      try
      {
         sp3Pos = sp3Eph.getXvt(sat, gps).x.toVector();
         sp3Vel = sp3Eph.getXvt(sat, gps).v.toVector();

         eciPos = transpose(c2t) * sp3Pos;
         eciVel = transpose(c2t) * sp3Vel + transpose(dc2t) * sp3Pos;

         cout << fixed << setprecision(3);
         cout << setw( 9)  << CivilTime(gps);
         cout << setw(18) << eciPos(0)
              << setw(18) << eciPos(1)
              << setw(18) << eciPos(2)
              << endl;
      }
      catch(...)
      {
         break;
      }

   }

   return 0;
}

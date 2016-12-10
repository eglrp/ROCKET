/*
 * Test for Earth Tides.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "EarthSolidTide.hpp"

#include "EarthOceanTide.hpp"

#include "EarthPoleTide.hpp"

#include "CivilTime.hpp"


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

   // Solar System
   SolarSystem solSys;

   try
   {
      string ephFile = confReader.getValue("JPLEPHFILE", "DEFAULT");
      solSys.initializeWithBinaryFile(ephFile);
   }
   catch(...)
   {
      cerr << "SolarSystem Initialize Error." << endl;

      return 1;
   }


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

   CivilTime ct0(year,mon,day,hour,min,sec, TimeSystem::GPS);
   CommonTime gps0( ct0.convertToCommonTime() );
   CommonTime utc0( refSys.GPS2UTC(gps0) );


   // Earth Solid Tide
   EarthSolidTide solidTide;
   solidTide.setReferenceSystem(refSys);
   solidTide.setSolarSystem(solSys);


   // Earth Ocean Tide
   EarthOceanTide oceanTide;
   oceanTide.setReferenceSystem(refSys);

   double degree, order;
   try
   {
       degree = confReader.getValueAsInt("EOTDEG", "FORCEMODEL");
       order  = confReader.getValueAsInt("EOTORD", "FORCEMODEL");
   }
   catch(...)
   {
       cerr << "Get Degree and Order of OceanTide Error." << endl;

       return 1;
   }

   oceanTide.setDesiredDegreeOrder(degree, order);

   // EOT File
   try
   {
      string eotFile = confReader.getValue("EOTFILE", "FORCEMODEL");
      oceanTide.loadFile(eotFile);
   }
   catch(...)
   {
      cerr << "OceanTide File Load Error." << endl;

      return 1;
   }


   // Earth Pole Tide
   EarthPoleTide poleTide;
   poleTide.setReferenceSystem(refSys);


   // Correction of Cnm and Snm
   Matrix<double> dCS;

   MJD mjd(54530.5 + 0.49983796296296296, TimeSystem::UTC);

   CommonTime utc( mjd.convertToCommonTime() );

   dCS = solidTide.getSolidTide(utc);

   cout << fixed << setprecision(12);

   cout << CivilTime(utc) << endl;
   cout << "dC20: "
        << setw(20) << dCS(3,0)     // dC20
        << endl
        << "dS20: "
        << setw(20) << dCS(3,1)     // dS20
        << endl
        << "dC21: "
        << setw(20) << dCS(4,0)     // dC21
        << endl
        << "dS21: "
        << setw(20) << dCS(4,1)     // dS21
        << endl
        << "dC22: "
        << setw(20) << dCS(5,0)     // dC22
        << endl
        << "dS22: "
        << setw(20) << dCS(5,1)     // dS22
        << endl
        << "dC30: "
        << setw(20) << dCS(6,0)     // dC30
        << endl
        << "dS30: "
        << setw(20) << dCS(6,1)     // dS30
        << endl
        << "dC31: "
        << setw(20) << dCS(7,0)     // dC31
        << endl
        << "dS31: "
        << setw(20) << dCS(7,1)     // dS31
        << endl
        << "dC32: "
        << setw(20) << dCS(8,0)     // dC32
        << endl
        << "dS32: "
        << setw(20) << dCS(8,1)     // dS32
        << endl
        << "dC33: "
        << setw(20) << dCS(9,0)     // dC33
        << endl
        << "dS33: "
        << setw(20) << dCS(9,1)     // dS33
        << endl
        << "dC40: "
        << setw(20) << dCS(10,0)    // dC40
        << endl
        << "dS40: "
        << setw(20) << dCS(10,1)    // dS40
        << endl
        << "dC41: "
        << setw(20) << dCS(11,0)    // dC41
        << endl
        << "dS41: "
        << setw(20) << dCS(11,1)    // dS41
        << endl
        << "dC42: "
        << setw(20) << dCS(12,0)    // dC42
        << endl
        << "dS42: "
        << setw(20) << dCS(12,1)    // dS42
        << endl;
/*
   int i = 0;

   while(true)
   {
       // Current Time
       CommonTime gps( gps0 + i*900.0 );
       CommonTime utc( refSys.GPS2UTC(gps) );

       dCS = solidTide.getSolidTide(utc);

       cout << setw(20) << dCS(3,0)     // dC20
            << setw(20) << dCS(3,1)     // dS20
            << setw(20) << dCS(4,0)     // dC21
            << setw(20) << dCS(4,1)     // dS21
            << setw(20) << dCS(5,0)     // dC22
            << setw(20) << dCS(5,1)     // dS22
            << setw(20) << dCS(6,0)     // dC30
            << setw(20) << dCS(6,1)     // dS30
            << setw(20) << dCS(7,0)     // dC31
            << setw(20) << dCS(7,1)     // dS31
            << setw(20) << dCS(8,0)     // dC32
            << setw(20) << dCS(8,1)     // dS32
            << setw(20) << dCS(9,0)     // dC33
            << setw(20) << dCS(9,1)     // dS33
            << setw(20) << dCS(10,0)    // dC40
            << setw(20) << dCS(10,1)    // dS40
            << setw(20) << dCS(11,0)    // dC41
            << setw(20) << dCS(11,1)    // dS41
            << setw(20) << dCS(12,0)    // dC42
            << setw(20) << dCS(12,1)    // dS42
            << endl;

       break;

       i++;

       if(i > 24*3600/900) break;
   }
*/

   return 0;
}

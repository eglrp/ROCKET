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
       degree = confReader.getValueAsInt("EOTDEG", "DEFAULT");
       order  = confReader.getValueAsInt("EOTORD", "DEFAULT");
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
      string eotFile = confReader.getValue("EOTFILE", "DEFAULT");
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


   // Cnm and Snm
   Matrix<double> CS(20,20,0.0);

   cout << fixed << setprecision(12);

   int i = 0;

   while(true)
   {
       // Current Time
       CommonTime gps( gps0 + i*900.0 );
       CommonTime utc( refSys.GPS2UTC(gps) );

       solidTide.getSolidTide(utc,CS);

       i++;

       if(i > 24*3600/900) break;
   }

   return 0;
}

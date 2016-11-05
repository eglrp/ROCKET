/*
 * Test for Earth Gravitation Model.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "EGM08GravityModel.hpp"

#include "SP3EphemerisStore.hpp"
#include "SatDataReader.hpp"

#include "Epoch.hpp"

#include "StringUtils.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


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

   // EGM08 Gravity Model
   EGM08GravityModel egm;

   // EGM Degree and Order
   int degree, order;
   try
   {
       degree = confReader.getValueAsInt("EGMDEG", "FORCEMODEL");
       order  = confReader.getValueAsInt("EGMORD", "FORCEMODEL");
   }
   catch(...)
   {
       cerr << "Get Degree and Order of EGM Error." << endl;

       return 1;
   }

   egm.setDesiredDegreeOrder(degree, order);
   egm.setReferenceSystem(refSys);

   // EGM File
   try
   {
      string egmFile = confReader.getValue("EGMFILE", "FORCEMODEL");
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

   bool correctSolidTide;
   try
   {
      correctSolidTide = confReader.getValueAsBoolean("SOLIDTIDE", "FORCEMODEL");
   }
   catch(...)
   {
       cerr << "Get SolidTide Correction Indicator Error." << endl;

       return 1;
   }

   if(correctSolidTide)
   {
      egm.setEarthSolidTide(solidTide);
   }

   // Earth Ocean Tide
   EarthOceanTide oceanTide;
   oceanTide.setReferenceSystem(refSys);

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

   bool correctOceanTide;
   try
   {
       correctOceanTide = confReader.getValueAsBoolean("OCEANTIDE", "FORCEMODEL");
   }
   catch(...)
   {
       cerr << "Get OceanTide Correction Indicator Error." << endl;

       return 1;
   }

   if(correctOceanTide)
   {
      egm.setEarthOceanTide(oceanTide);
   }

   // Earth Pole Tide
   EarthPoleTide poleTide;
   poleTide.setReferenceSystem(refSys);

   bool correctPoleTide;

   try
   {
       correctPoleTide = confReader.getValueAsBoolean("POLETIDE", "FORCEMODEL");
   }
   catch(...)
   {
       cerr << "Get PoleTide Correction Indicator Error." << endl;

       return 1;
   }

   if(correctPoleTide)
   {
      egm.setEarthPoleTide(poleTide);
   }

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

   // Position
   ifstream fin("coor.log");

   CivilTime ct(2014,2,1,0,0,0.0, TimeSystem::GPS);
   CommonTime gps0( ct.convertToCommonTime() );
   CommonTime utc0( refSys.GPS2UTC(gps0) );

   Vector<double> r_icrs(3,0.0), v_icrs(3,0.0);
   Vector<double> a_icrs(3,0.0);

   cout << fixed << setprecision(15);

   string line;
   while(!fin.eof() && fin.good())
   {
       getline(fin,line);

       if( fin.eof() ) break;

       double t = asDouble( line.substr( 0,10) );
       CommonTime utc = utc0 + t;

       r_icrs(0) = asDouble( line.substr(10,15) );
       r_icrs(1) = asDouble( line.substr(25,15) );
       r_icrs(2) = asDouble( line.substr(40,15) );
       v_icrs(0) = asDouble( line.substr(55,15) );
       v_icrs(1) = asDouble( line.substr(70,15) );
       v_icrs(2) = asDouble( line.substr(85,15) );

       sc.setCurrentTime(utc);
       sc.setCurrentPos(r_icrs);
       sc.setCurrentVel(v_icrs);

       egm.doCompute(utc, eb, sc);
       a_icrs = egm.getAcceleration();

       cout << setw(20) << t;
       cout << setw(20) << a_icrs(0)
            << setw(20) << a_icrs(1)
            << setw(20) << a_icrs(2)
            << endl;

   }

   fin.close();

   return 0;
}

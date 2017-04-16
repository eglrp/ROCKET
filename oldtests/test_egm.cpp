/*
 * Test for Earth Gravitation Model.
 *
 */

#include <iostream>

#include "ConfDataReader.hpp"

#include "EGM08GravityModel.hpp"
#include "SolarSystem.hpp"

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
       prn = confReader.getValueAsInt("PRN", "DEFAULT");
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

   CivilTime ct0(year,mon,day,hour,min,sec, TimeSystem::GPS);
   CommonTime gps0( ct0.convertToCommonTime() );
   CommonTime utc0( refSys.GPS2UTC(gps0) );

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
   SatDataReader satReader;

   try
   {
       string satDataFile = confReader.getValue("SatDataFile", "DEFAULT");
       satReader.open(satDataFile);
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

   // EGM08 Gravity Model
   EGM08GravityModel egm;

   // EGM Degree and Order
   int degree, order;
   try
   {
       degree = confReader.getValueAsInt("EGMDEG", "DEFAULT");
       order  = confReader.getValueAsInt("EGMORD", "DEFAULT");
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
      string egmFile = confReader.getValue("EGMFILE", "DEFAULT");
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
      correctSolidTide = confReader.getValueAsBoolean("SOLIDTIDE", "DEFAULT");
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

   bool correctOceanTide;
   try
   {
       correctOceanTide = confReader.getValueAsBoolean("OCEANTIDE", "DEFAULT");
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
       correctPoleTide = confReader.getValueAsBoolean("POLETIDE", "DEFAULT");
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
       length = confReader.getValueAsDouble("LENGTH", "DEFAULT");
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
       CommonTime gps( gps0 + i*900.0 );
       CommonTime utc( refSys.GPS2UTC(gps) );

       // Current Position and Velocity in ITRS
       Vector<double> r_itrs, v_itrs;

       try
       {
           r_itrs = sp3Eph.getXvt(sat, gps).x.toVector();
           v_itrs = sp3Eph.getXvt(sat, gps).v.toVector();
       }
       catch(...)
       {
           cerr << "Get Position and Velocity from SP3 File Error." << endl;

           return 1;
       }

       // Current Transform Matrix
       Matrix<double> c2t ( refSys.C2TMatrix(utc)  );
       Matrix<double> t2c ( transpose(c2t) );

       // Current Transform Matrix Time Dot
       Matrix<double> dc2t( refSys.dC2TMatrix(utc) );
       Matrix<double> dt2c( transpose(dc2t) );

       // Current Position and Velocity in ICRS
       Vector<double> r_icrs = t2c * r_itrs;
       Vector<double> v_icrs = t2c * v_itrs + dt2c * r_itrs;

       // Current Spacecraft
       sc.setPosition(r_icrs);
       sc.setVelocity(v_icrs);
       sc.setCurrentTime(utc);
       sc.setBlockType(satReader.getBlock(sat,utc));
       sc.setMass(satReader.getMass(sat,utc));

       // Current Acceleration
       egm.doCompute(utc, eb, sc);
       Vector<double> a_icrs( egm.getAcceleration() );
       Vector<double> a_itrs( c2t * a_icrs );

       cout << setw(20) << i*900.0/3600.0;
       cout << setw(20) << a_icrs(0)
            << setw(20) << a_icrs(1)
            << setw(20) << a_icrs(2)
            << endl;

       i++;

       if(i >= length*3600/900) break;
   }

   return 0;
}

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

#include "GNSSOrbit.hpp"

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
   EOPDataStore2 eopDataStore;
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

   // LeapSecond File
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
   int year = confReader.getValueAsInt("YEAR", "DEFAULT");
   int mon  = confReader.getValueAsInt("MON", "DEFAULT");
   int day  = confReader.getValueAsInt("DAY", "DEFAULT");
   int hour = confReader.getValueAsInt("HOUR", "DEFAULT");
   int min  = confReader.getValueAsInt("MIN", "DEFAULT");
   double sec  = confReader.getValueAsDouble("SEC", "DEFAULT");

   CivilTime cv0(year,mon,day,hour,min,sec, TimeSystem::GPS);
   CommonTime gps0( cv0.convertToCommonTime() );
   CommonTime utc0( refSys.GPS2UTC(gps0) );

   // Satellite ID
   int prn = confReader.getValueAsInt("PRN", "DEFAULT");
   SatID sat(prn, SatID::systemGPS);

   // IGS SP3 File
   SP3EphemerisStore sp3Eph;
   sp3Eph.rejectBadPositions(true);
   sp3Eph.setPosGapInterval(900+1);
   sp3Eph.setPosMaxInterval(9*900+1);

   string sp3File;

   while( (sp3File=confReader.fetchListValue("IGSSP3LIST", "DEFAULT")) != "" )
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
   Matrix<double> t2c ( refSys.T2CMatrix(utc0)  );
   // Transform Matrix Time Dot between ICRS and ITRS
   Matrix<double> dt2c( refSys.dT2CMatrix(utc0) );

   // r0, v0 in ICRS
   Vector<double> r0_icrs, v0_icrs;
   r0_icrs = t2c * r0_itrs;
   v0_icrs = t2c * v0_itrs + dt2c * r0_itrs;

   // p0
   int np = confReader.getValueAsInt("SRP_NUM", "DEFAULT");
   Vector<double> p0(np,0.0);

   p0(0) = confReader.getValueAsDouble("D0", "DEFAULT");
   p0(1) = confReader.getValueAsDouble("DC", "DEFAULT");
   p0(2) = confReader.getValueAsDouble("DS", "DEFAULT");
   p0(3) = confReader.getValueAsDouble("Y0", "DEFAULT");
   p0(4) = confReader.getValueAsDouble("YC", "DEFAULT");
   p0(5) = confReader.getValueAsDouble("YS", "DEFAULT");
   p0(6) = confReader.getValueAsDouble("B0", "DEFAULT");
   p0(7) = confReader.getValueAsDouble("BC", "DEFAULT");
   p0(8) = confReader.getValueAsDouble("BS", "DEFAULT");


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
   sc.setCurrentTime(utc0);
   sc.setSatID(sat);
   sc.setBlockType(satData.getBlock(sat,utc0));
   sc.setMass(satData.getMass(sat,utc0));
   sc.setPosition(r0_icrs);
   sc.setVelocity(v0_icrs);
   sc.setNumOfParam(np);

   // Earth gravitation
   EGM08GravityModel egm;

   int EGMDEG = confReader.getValueAsInt("EGMDEG", "DEFAULT");
   int EGMORD = confReader.getValueAsInt("EGMORD", "DEFAULT");

   egm.setDesiredDegreeOrder(EGMDEG, EGMORD);
   egm.setReferenceSystem(refSys);

   string egmFile = confReader.getValue("EGMFILE", "DEFAULT");
   try
   {
      egm.loadFile(egmFile);
   }
   catch(...)
   {
      cerr << "EGM File Load Error." << endl;

      return 1;
   }


   // Earth solid tide
   EarthSolidTide solidTide;
   solidTide.setReferenceSystem(refSys);
   solidTide.setSolarSystem(solSys);

   bool correctSolidTide = confReader.getValueAsBoolean("SOLIDTIDE", "DEFAULT");
   if(correctSolidTide)
   {
      egm.setEarthSolidTide(solidTide);
   }

   // Earth ocean tide
   EarthOceanTide oceanTide;
   oceanTide.setReferenceSystem(refSys);

   int EOTDEG = confReader.getValueAsInt("EOTDEG", "DEFAULT");
   int EOTORD = confReader.getValueAsInt("EOTORD", "DEFAULT");

   oceanTide.setDesiredDegreeOrder(EOTDEG, EOTORD);

   string eotFile = confReader.getValue("EOTFILE", "DEFAULT");
   try
   {
      oceanTide.loadFile(eotFile);
   }
   catch(...)
   {
      cerr << "EOT File Load Error." << endl;

      return 1;
   }

   bool correctOceanTide = confReader.getValueAsBoolean("OCEANTIDE", "DEFAULT");
   if(correctOceanTide)
   {
      egm.setEarthOceanTide(oceanTide);
   }


   // Earth pole tide
   EarthPoleTide poleTide;
   poleTide.setReferenceSystem(refSys);

   bool correctPoleTide = confReader.getValueAsBoolean("POLETIDE", "DEFAULT");
   if(correctPoleTide)
   {
      egm.setEarthPoleTide(poleTide);
   }


   // Sun gravitation
   SunGravitation sun;
   sun.setSolarSystem(solSys);
   sun.setReferenceSystem(refSys);

   // Moon gravitation
   MoonGravitation moon;
   moon.setSolarSystem(solSys);
   moon.setReferenceSystem(refSys);

   // Solar pressure
   CODEPressure srp;
   srp.setReferenceSystem(refSys);
   srp.setSolarSystem(solSys);
   srp.setSRPCoeff(p0);

   // Relativity effect
   RelativityEffect rel;

   // GNSS orbit
   GNSSOrbit gnss;

   gnss.setRefEpoch(utc0);
   gnss.setSpacecraft(sc);

   gnss.setEarthGravitation(egm);
   gnss.setSunGravitation(sun);
   gnss.setMoonGravitation(moon);
   gnss.setSolarPressure(srp);
   gnss.setRelativityEffect(rel);


   // RKF integrator
   RungeKuttaFehlberg rkf;

   double stepSize = confReader.getValueAsDouble("STEPSIZE", "DEFAULT");
   rkf.setStepSize(stepSize);

   double length = confReader.getValueAsDouble("LENGTH", "DEFAULT");

   // Debug
   int debug = confReader.getValueAsInt("DEBUG", "DEFAULT");

   double t = 0.0;

   while(true)
   {
      // Current time
      CommonTime gps = gps0 + t;
      CommonTime utc = refSys.GPS2UTC(gps);

      // Transformation matrix
      Matrix<double> c2t( refSys.C2TMatrix(utc) );
      Matrix<double> t2c( transpose(c2t) );

      // SP3 position
      Vector<double> r_sp3(3,0.0);

      try
      {
         r_sp3 = t2c * sp3Eph.getXvt(sat,gps).x.toVector();
      }
      catch(...)
      {
         cerr << "Get SP3 position error." << endl;
         continue;
      }

      // OI position
      Vector<double> y = sc.getStateVector();

      Vector<double> rv_oi(6,0.0);
      rv_oi(0) = y(0);
      rv_oi(1) = y(1);
      rv_oi(2) = y(2);
      rv_oi(3) = y(3);
      rv_oi(4) = y(4);
      rv_oi(5) = y(5);

      // Difference position
      Vector<double> r_diff(3,0.0);
      r_diff(0) = rv_oi(0) - r_sp3(0);
      r_diff(1) = rv_oi(1) - r_sp3(1);
      r_diff(2) = rv_oi(2) - r_sp3(2);

      // Output
      cout << fixed;
      cout << setprecision(9);

      if(0 == debug)
      {
          cout << setw(20) << YDSTime(gps).sod/3600.0;
          // dr
          cout << setw(20) << r_diff(0)
               << setw(20) << r_diff(1)
               << setw(20) << r_diff(2)
               << endl;
      }

      if(1 == debug)
      {
          // t
          cout << CivilTime(gps) << endl;

          // r_oi
          cout << setw(20) << rv_oi(0)
               << setw(20) << rv_oi(1)
               << setw(20) << rv_oi(2)
               << endl;

          // r_sp3
          cout << setw(20) << r_sp3(0)
               << setw(20) << r_sp3(1)
               << setw(20) << r_sp3(2)
               << endl;

          // dr
          cout << setw(20) << r_diff(0)
               << setw(20) << r_diff(1)
               << setw(20) << r_diff(2)
               << endl;

          // dr/dr0
          for(int j=0; j<3; ++j)
          {
              cout << setw(20) << y( 6+j)
                   << setw(20) << y( 9+j)
                   << setw(20) << y(12+j)
                   << endl;
          }

          // dr/dv0
          for(int j=0; j<3; ++j)
          {
              cout << setw(20) << y(15+j)
                   << setw(20) << y(18+j)
                   << setw(20) << y(21+j)
                   << endl;
          }

          // dr/dp0
          for(int j=0; j<np; ++j)
          {
              cout << setw(20) << y(24+0*np+j)
                   << setw(20) << y(24+1*np+j)
                   << setw(20) << y(24+2*np+j)
                   << endl;
          }

          cout << endl;

      }

      if(2 == debug)
      {
          for(int j=0; j<3; ++j)
          {
              // dr
              cout << setw(20) << r_diff(j);

              // dr/dr0
              for(int k=0; k<3; ++k)
              {
                  cout << setw(20) << y( 6+3*j+k);
              }

              // dr/dv0
              for(int k=0; k<3; ++k)
              {
                  cout << setw(20) << y(15+3*j+k);
              }

              // dr/dp0
              for(int k=0; k<np; ++k)
              {
                  cout << setw(20) << y(24+3*j+k);
              }

              cout << endl;
          }

      }


      // Integration
      rkf.integrateTo(t, y, &gnss, t+900.0);

      // Update spacecraft
      sc.setCurrentTime(utc);
      sc.setStateVector(y);
      sc.setBlockType(satData.getBlock(sat,utc));
      sc.setMass(satData.getMass(sat,utc));

      // Update gnss orbit
      gnss.setSpacecraft(sc);

      if(t >= length*3600) break;

   }

   return 0;
}

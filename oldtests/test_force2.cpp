/* Test for Force Models. */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

#include "SP3EphemerisStore.hpp"

#include "SatDataReader.hpp"

#include "EGM08GravityModel.hpp"

#include "ThirdBody.hpp"

#include "CODEPressure.hpp"

#include "RelativityEffect.hpp"

#include "GNSSOrbit.hpp"

#include "RKF78Integrator.hpp"

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

   CivilTime cv0(year,mon,day,hour,min,sec, TimeSystem::GPS);
   CommonTime gps0( cv0.convertToCommonTime() );
   CommonTime utc0( refSys.GPS2UTC(gps0) );

   // SatID
   int prn;
   try
   {
       prn = confReader.getValueAsInt("SATPRN", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get Satellite PRN Error." << endl;

       return 1;
   }

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

   Matrix<double>  t2c( refSys.T2CMatrix(utc0) );
   Matrix<double> dt2c( refSys.dT2CMatrix(utc0) );

   // Initial Position and Velocity
   Vector<double> r0(3,0.0), v0(3,0.0);
   try
   {
       Vector<double> r_sp3 = sp3Eph.getXvt(sat,gps0).x.toVector();
       Vector<double> v_sp3 = sp3Eph.getXvt(sat,gps0).v.toVector();

       r0 = t2c * r_sp3;
       v0 = t2c * v_sp3 + dt2c * r_sp3;
//      r0(0) = confReader.getValueAsDouble("RX0", "DEFAULT");
//      r0(1) = confReader.getValueAsDouble("RY0", "DEFAULT");
//      r0(2) = confReader.getValueAsDouble("RZ0", "DEFAULT");
//      v0(0) = confReader.getValueAsDouble("VX0", "DEFAULT");
//      v0(1) = confReader.getValueAsDouble("VY0", "DEFAULT");
//      v0(2) = confReader.getValueAsDouble("VZ0", "DEFAULT");
   }
   catch(...)
   {
      cerr << "Get Initial Position and Velocity Error." << endl;

      return 1;
   }


   // Initial Force Model Parameters - SRP Coefficients
   int np;
   try
   {
       np = confReader.getValueAsInt("SRPNUM", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get SRP Number Error." << endl;

       return 1;
   }

   Vector<double> p0(np,0.0);
   try
   {
       if(9 == np)
       {
           p0(0) = confReader.getValueAsDouble("D0", "DEFAULT");
           p0(1) = confReader.getValueAsDouble("DC", "DEFAULT");
           p0(2) = confReader.getValueAsDouble("DS", "DEFAULT");
           p0(3) = confReader.getValueAsDouble("Y0", "DEFAULT");
           p0(4) = confReader.getValueAsDouble("YC", "DEFAULT");
           p0(5) = confReader.getValueAsDouble("YS", "DEFAULT");
           p0(6) = confReader.getValueAsDouble("B0", "DEFAULT");
           p0(7) = confReader.getValueAsDouble("BC", "DEFAULT");
           p0(8) = confReader.getValueAsDouble("BS", "DEFAULT");
       }
       else if(5 == np)
       {
           p0(0) = confReader.getValueAsDouble("D0", "DEFAULT");
           p0(1) = confReader.getValueAsDouble("Y0", "DEFAULT");
           p0(2) = confReader.getValueAsDouble("B0", "DEFAULT");
           p0(3) = confReader.getValueAsDouble("Bc", "DEFAULT");
           p0(4) = confReader.getValueAsDouble("Bs", "DEFAULT");
       }
   }
   catch(...)
   {
       cerr << "Get SRP Coefficients Error." << endl;

       return 1;
   }


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
   // SatID and NumOfParam do not change during one integration
   sc.setSatID(sat);
   sc.setSatData(satData);
   sc.setNumOfParam(np);
   // Time, Block, Mass, Pos, Vel change during one integration
   sc.setCurrentTime(utc0);
   sc.setCurrentPos(r0);
   sc.setCurrentVel(v0);


   // Earth Gravitation
   EGM08GravityModel egm;

   int EGMDEG = confReader.getValueAsInt("EGMDEG", "DEFAULT");
   int EGMORD = confReader.getValueAsInt("EGMORD", "DEFAULT");

   // NOTE: We MUST call setDesiredDegreeOrder() method before
   //       calling loadFile() method to ensure the variable
   //       storing the spherical harmonic coefficients has
   //       been resized.
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


   // Earth Solid Tide
   EarthSolidTide solidTide;
   solidTide.setReferenceSystem(refSys);
   solidTide.setSolarSystem(solSys);

   bool correctSolidTide = confReader.getValueAsBoolean("SOLIDTIDE", "DEFAULT");
   if(correctSolidTide)
   {
      egm.setEarthSolidTide(solidTide);
   }

   // Earth Ocean Tide
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


   // Earth Pole Tide
   EarthPoleTide poleTide;
   poleTide.setReferenceSystem(refSys);

   bool correctPoleTide = confReader.getValueAsBoolean("POLETIDE", "DEFAULT");
   if(correctPoleTide)
   {
      egm.setEarthPoleTide(poleTide);
   }

   // Third Body
   ThirdBody third;
   third.setSolarSystem(solSys);
   third.setReferenceSystem(refSys);
   third.enableSun();
   third.enableMoon();

   // Solar Pressure
   CODEPressure srp;
   srp.setReferenceSystem(refSys);
   srp.setSolarSystem(solSys);
   srp.setSRPCoeff(p0);

   // Relativity Effect
   RelativityEffect rel;


   // GNSS Orbit
   GNSSOrbit gnss;
   // Reference Epoch and Force Models do not change during one integration
   gnss.setRefEpoch(utc0);
   gnss.setEarthGravitation(egm);
   gnss.setThirdBody(third);
   gnss.setSolarPressure(srp);
   gnss.setRelativityEffect(rel);
   // Spacecraft Epoch, Block, Mass, Pos, Vel change during one integration
   gnss.setSpacecraft(sc);


   // RKF78 Integrator
   RKF78Integrator rkf;

   double size;
   try
   {
       size = confReader.getValueAsDouble("STEPSIZE", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get Step Size Error." << endl;
       return 1;
   }

   rkf.setStepSize(size);

   double length;
   try
   {
       length = confReader.getValueAsDouble("ARCLENGTH", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get Arc Length Error." << endl;
       return 1;
   }

   double interval;
   try
   {
       interval = confReader.getValueAsDouble("ARCINTERVAL", "DEFAULT");
   }
   catch(...)
   {
       cerr << "Get Arc Interval Error." << endl;
       return 1;
   }

   // Earth Body
   EarthBody eb;

   // Loop
   double t = 0.0;

   int count = 0;

   while(true)
   {
      count++;

      // Current state
      Vector<double> y = sc.getCurrentState();

      // Integration
      rkf.integrateTo(t, y, &gnss, t+interval);

      // Integrated position and velocity in ICRS
      Vector<double> r_com(3,0.0);
      r_com(0) = y(0);
      r_com(1) = y(1);
      r_com(2) = y(2);

      Vector<double> v_com(3,0.0);
      v_com(0) = y(3);
      v_com(1) = y(4);
      v_com(2) = y(5);

      // Current time
      CommonTime gps = gps0 + t;
      CommonTime utc = refSys.GPS2UTC(gps);

      // Transformation matrix
      Matrix<double> t2c( refSys.T2CMatrix(utc) );
      Matrix<double> dt2c( refSys.dT2CMatrix(utc) );
      Matrix<double> c2t( transpose(t2c) );

      // SP3 position and velocity in ITRS
      Vector<double> r_sp3(3,0.0), v_sp3(3,0.0);

      // SP3 position and velocity in ICRS
      Vector<double> r_ref(3,0.0), v_ref(3,0.0);

      try
      {
         r_sp3 = sp3Eph.getXvt(sat,gps).x.toVector();
         v_sp3 = sp3Eph.getXvt(sat,gps).v.toVector();

         r_ref = t2c * r_sp3;
         v_ref = t2c * v_sp3 + dt2c * r_sp3;
      }
      catch(...)
      {
         cerr << "Get Reference Position and Velocity Error." << endl;

         break;
      }

      Vector<double> r_diff(3,0.0);
      r_diff = r_sp3 - c2t*r_com;

      cout << setw(20) << t;
      cout << setw(20) << r_diff(0)
           << setw(20) << r_diff(1)
           << setw(20) << r_diff(2)
           << endl;

      // Update Spacecraft
      sc.setCurrentTime(utc);
      sc.setCurrentState(y);


//      cout << setprecision(3);

//      cout << setw(20) << t << endl;

//      egm.doCompute(utc,eb,sc);
//      Vector<double> a_com(3,0.0);
//      a_com = egm.getAcceleration();

//      cout << setprecision(12);
//      cout << setw(20) << a_com(0)
//           << setw(20) << a_com(1)
//           << setw(20) << a_com(2)
//           << endl;

      // Spacecraft with SP3 position and velocity
//      Spacecraft sc2( sc );
//      sc2.setCurrentPos(r_ref);
//      sc2.setCurrentVel(v_ref);

//      egm.doCompute(utc,eb,sc2);
//      Vector<double> a_ref(3,0.0);
//      a_ref = egm.getAcceleration();

//      cout << setprecision(12);
//      cout << setw(20) << a_ref(0)
//           << setw(20) << a_ref(1)
//           << setw(20) << a_ref(2)
//           << endl << endl;


      // Update GNSSOrbit
      gnss.setSpacecraft(sc);

      if(t >= length*3600) break;

//      if(count >= 2) break;

   }

   return 0;
}

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
   CivilTime cv0(2015,1,1,12,1,15.0, TimeSystem::GPS);
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
   Matrix<double> c2t ( refSys.C2TMatrix(utc0)  );
   // Transform Matrix Time Dot between ICRS and ITRS
   Matrix<double> dc2t( refSys.dC2TMatrix(utc0) );

   // r0, v0 in ICRS
   Vector<double> r0_icrs, v0_icrs;
   r0_icrs = transpose(c2t) * r0_itrs;
   v0_icrs = transpose(c2t) * v0_itrs + transpose(dc2t) * r0_itrs;

   // p0
   int np(9);
   Vector<double> p0(np,0.0);
   p0(0) =  0.999989;   // D0
   p0(1) =  0.015272;   // Dc
   p0(2) =  0.000216;   // Ds
   p0(3) = -0.000367;   // Y0
   p0(4) = -0.000702;   // Yc
   p0(5) =  0.000985;   // Ys
   p0(6) = -0.003625;   // B0
   p0(7) =  0.009091;   // Bc
   p0(8) = -0.001753;   // Bs


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
   sc.setNumOfParam(9);

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
   SunGravitation sg;
   sg.setSolarSystem(solSys);
   sg.setReferenceSystem(refSys);

   // Moon gravitation
   MoonGravitation mg;
   mg.setSolarSystem(solSys);
   mg.setReferenceSystem(refSys);

   // Solar pressure
   CODEPressure sp;
   sp.setReferenceSystem(refSys);
   sp.setSolarSystem(solSys);
   sp.setSRPCoeff(p0);

   // Relativity effect
   RelativityEffect re;

   // Earth body
   EarthBody rb;

   egm.doCompute(utc0, rb, sc);
   sg.doCompute(utc0, rb, sc);
   mg.doCompute(utc0, rb, sc);
   sp.doCompute(utc0, rb, sc);
   re.doCompute(utc0, rb, sc);

   cout << CivilTime(gps0) << endl;

   cout << fixed << setprecision(12);

   cout << r0_icrs << endl;

   cout << egm.getAcceleration() << endl
        << sg.getAcceleration() << endl
        << mg.getAcceleration() << endl
        << sp.getAcceleration() << endl
        << re.getAcceleration() << endl;

   // GNSS orbit
   GNSSOrbit gnss;
   gnss.setRefEpoch(utc0);
   gnss.setSpacecraft(sc);
   gnss.setEarthGravitation(egm);
   gnss.setSunGravitation(sg);
   gnss.setMoonGravitation(mg);
   gnss.setSolarPressure(sp);
   gnss.setRelativityEffect(re);

   Vector<double> y(sc.getStateVector());

   Vector<double> dy = gnss.getDerivatives(75.0, y);

   cout << dy(3) << ' '
        << dy(4) << ' '
        << dy(5) << endl;

   return 0;
}

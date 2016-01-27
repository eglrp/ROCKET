//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file IERSConventions2003.cpp
 * IERS Conventions 2003
 */

#include "IERSConventions.hpp"
#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{
   // IERS Data Handling
   //--------------------------------------------------------------------------

      // ERP data file from IERS
   void LoadIERSERPFile(const std::string& fileName)
   {
       eopDataTable.clear();

       try
       {
           eopDataTable.loadIERSFile(fileName);
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Failed to load IERS ERP File " + fileName));
       }
   }

      // ERP data file from IGS
   void LoadIGSERPFile(const std::string& fileName)
   {
      eopDataTable.clear();

      try
      {
          eopDataTable.loadIGSFile(fileName);
      }
      catch(...)
      {
          GPSTK_THROW(Exception("Failed to load IGS ERP File " + fileName));
      }
   }

      // ERP data file from STK 
   void LoadSTKERPFile(const std::string& fileName)
   {
       eopDataTable.clear();

       try
       {
           eopDataTable.loadSTKFile(fileName);
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Failed to load STK ERP File " + fileName));
       }
   }

   void LoadIERSLSFile(const std::string& fileName)
   {
       lsDataTable.clear();
       
       try
       {
           lsDataTable.loadFile(fileName);
       }
       catch(...)
       {
            GPSTK_THROW(Exception("Failed to load IERS LS File " + fileName));
       }
   }

      // Request EOP Data
   EOPDataStore::EOPData EOPData(const CommonTime& UTC)
   {
       EOPDataStore::EOPData eopData;
       try
       {
           eopData = eopDataTable.getEOPData( UTC );
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Faile to get EOP Data." + CivilTime(UTC).asString()));
       }

       return eopData;
   }

      // Request Leap Second
   double LeapSec(const CommonTime& UTC)
   {
       double leapSec;
       try
       {
           leapSec = lsDataTable.getLeapSec( UTC );
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Faile to get Leap Second." + CivilTime(UTC).asString()));
       }

       return leapSec;
   }

      // xp, in arcsecond
   double PolarMotionX(const CommonTime& UTC)
   {
       double xp;
    
       try
       {
           xp = EOPData(UTC).xp;
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Failed to get xp data on" + CivilTime(UTC).asString()));
       }

       return xp;
   }

      // yp, in arcsecond
   double PolarMotionY(const CommonTime& UTC)
   {
       double yp;

       try
       {
           return EOPData(UTC).yp;
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Failed to get xp data on " + CivilTime(UTC).asString()));
       }

       return yp;
   }

      // UT1 - UTC, in second
   double UT1mUTC(const CommonTime& UTC)
   {
       double dut1;
       try
       {
           dut1 = EOPData(UTC).UT1mUTC;
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Failed to get UT1mUTC data on " + CivilTime(UTC).asString()));
       }

       return dut1;
   }

      // dPsi, in arcsecond
   double NutationDPsi(const CommonTime& UTC)
   {
       double dPsi;
       try
       {
           dPsi = EOPData(UTC).dPsi;
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Failed to get dPsi data on "+CivilTime(UTC).asString()));
       }

       return dPsi;
   }

      // dEps, in arcsecond
   double NutationDEps(const CommonTime& UTC)
   {
       double dEps;
       try
       {
           dEps = EOPData(UTC).dEps;
       }
       catch(...)
       {

           GPSTK_THROW(Exception("Failed to get dEps data on "+CivilTime(UTC).asString()));
       }

       return dEps;
   }

   // Time System Handling
   //--------------------------------------------------------------------------
 
      // TAI - UTC, in second
   double TAImUTC(const CommonTime& UTC)
   {
       double res;
       try
       {
           res = LeapSec( UTC );
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Fail to get TAI-UTC data on" + CivilTime(UTC).asString()));
       }

       return res;
   }

      // TT - TAI, in second
   double TTmTAI(void)
   {
       return 32.184;
   }

      // TAI - GPS, in second
   double TAImGPS(void)
   {
       return 19.0;
   }


      // Convert GPS to UTC
   CommonTime GPS2UTC(const CommonTime& GPS)
   {
       // The input time for "LeapSec" should be "UTC"
       // if the input is GPS, you should amends it.
       CommonTime GPS1(GPS);
       GPS1.setTimeSystem(TimeSystem::UTC);

       double leapSec( LeapSec(GPS1) );

       // The formula of GPS and UTC equals with:
       // GPS = UTC + (leapSec - 19.0)
       CommonTime UTC1;
       UTC1 = GPS1 - (leapSec - 19.0);

       // Now, correct the leap second by feeding the utc instead
       // of the 'gpst', if not fix, the following would happen:
       //
       //  1-JUL-1993 00:00:10 GPS TIME --->  1-JUL-1993 00:00:01 UTC TIME
       //  1-JUL-1993 00:00:09 GPS TIME --->  1-JUL-1993 00:00:01 UTC TIME
       //  1-JUL-1993 00:00:08 GPS TIME --->  1-JUL-1993 00:00:00 UTC TIME
       //  1-JUL-1993 00:00:07 GPS TIME ---> 30-JUN-1993 23:59:59 UTC TIME
       //  
       //  The fix allows the following conversion: 
       //  
       //  1-JUL-1993 00:00:10 GPS TIME --->  1-JUL-1993 00:00:01 UTC TIME
       //  1-JUL-1993 00:00:09 GPS TIME --->  1-JUL-1993 00:00:00 UTC TIME
       //  1-JUL-1993 00:00:08 GPS TIME --->  1-JUL-1993 00:00:00 UTC TIME
       //  1-JUL-1993 00:00:07 GPS TIME ---> 30-JUN-1993 23:59:59 UTC TIME
       //
       //  \reference Please refer to the subroutine 'gps2utc' in GRACE 
       //  data format converting software from GFZ.
       leapSec = LeapSec(UTC1);

       CommonTime UTC;
       UTC = GPS1 -  (leapSec - 19.0);

       // The TimeSystem of UTC is already set to TimeSystem::UTC.
       return UTC;
   }

      // Convert UTC to GPS
   CommonTime UTC2GPS(const CommonTime& UTC)
   {
       // Now, set GPS with UTC
       CommonTime TAI;

       // Convert UTC to TAI by correcting the LeapSec
       TAI = UTC + TAImUTC(UTC);

       // Convert TAI to GPS
       CommonTime GPS;
       GPS = TAI - TAImGPS();
       GPS.setTimeSystem(TimeSystem::GPS);

       return GPS;
   }

      // Convert UT1 to UTC
   CommonTime UT12UTC(const CommonTime& UT1)
   {
       // The input time should be UTC
       CommonTime UTC1(UT1);
       UTC1.setTimeSystem(TimeSystem::UTC);
       UTC1 = UT1 - UT1mUTC(UTC1);
       UTC1.setTimeSystem(TimeSystem::UTC);

       // Correct UTC by computing new UT1mUTC with UTC1
       CommonTime UTC2;
       UTC2 = UT1 - UT1mUTC(UTC1);
       UTC2.setTimeSystem(TimeSystem::UTC);

       // Correct UTC by computing new UT1mUTC with UTC2
       CommonTime UTC;
       UTC = UT1 - UT1mUTC(UTC2);
       UTC.setTimeSystem(TimeSystem::UTC);

       return UTC;
   }

      // Convert UTC to UT1
   CommonTime UTC2UT1(const CommonTime& UTC)
   {
       CommonTime UT1;
       UT1 = UTC + UT1mUTC(UTC);
       UT1.setTimeSystem(TimeSystem::UT1);

       return UT1;
   }

      // Convert TAI to UTC
   CommonTime TAI2UTC(const CommonTime& TAI)
   {
       // Initial UTC1
       CommonTime UTC1(TAI);
       UTC1.setTimeSystem(TimeSystem::UTC);

       // Get leapSec
       double leapSec( LeapSec(UTC1) );

       // Update UTC1
       UTC1 = UTC1 - leapSec;

       // Update leapSec
       leapSec = LeapSec(UTC1);

       // Get UTC
       CommonTime UTC(TAI-leapSec);
       UTC.setTimeSystem(TimeSystem::UTC);

       return UTC;
   }

      // Convert UTC to TAI
   CommonTime UTC2TAI(const CommonTime& UTC)
   {
       // Get leapSec
       double leapSec( LeapSec(UTC) );

       // Get TAI
       CommonTime TAI(UTC + leapSec);
       TAI.setTimeSystem(TimeSystem::TAI);

       return TAI;
   }

      // Convert TT to UTC
   CommonTime TT2UTC(const CommonTime& TT)
   {
       // Convert TT to TAI
       CommonTime TAI( TT - TTmTAI() );
       TAI.setTimeSystem(TimeSystem::TAI);

       // Convert TAI to UTC
       CommonTime UTC( TAI2UTC(TAI) );

       return UTC;
   }

      // Convert UTC to TT
   CommonTime UTC2TT(const CommonTime& UTC)
   {
       // Convert UTC to TAI
       CommonTime TAI( UTC2TAI(UTC) );

       // Convert TAI to TT
       CommonTime TT( TAI + TTmTAI() );
       TT.setTimeSystem(TimeSystem::TT);

       return TT;
   }


      // Convert BDT to UTC
   CommonTime BDT2UTC(const CommonTime& BDT)
   {
       // Convert BDT to GPS
       CommonTime GPS(BDT + 14.0);
       GPS.setTimeSystem(TimeSystem::GPS);

       // Convert GPS to UTC
       CommonTime UTC( GPS2UTC(GPS) );

       return UTC;
   }

      // Convert UTC to BDT
   CommonTime UTC2BDT(const CommonTime& UTC)
   {
       // Convert UTC to GPS
       CommonTime GPS( UTC2GPS(UTC) );

       // Convert GPS to BDT
       CommonTime BDT(GPS - 14.0);
       BDT.setTimeSystem(TimeSystem::BDT);

       return BDT;
   }


   // Reference System Handling
   //--------------------------------------------------------------------------

      // Normalize angle into the range 0 <= a <= 2PI
   double Anp(double a)
   {
       double w = std::fmod(a, TWO_PI);
       if(fabs(w) >= PI)
       {
           if(a > 0.0)  w -=  TWO_PI;
           else         w -= -TWO_PI;
       }

       return w;
   }

      // Normalize angle into the range -PI <= a <= +PI
   double Anpm(double a)
   {
       double w = std::fmod(a, TWO_PI);
       if(fabs(w) >= PI)
       {
           if(a > 0.0)  w -=  TWO_PI;
           else         w -= -TWO_PI;
       }

       return w;
   }


      // Mean obliquity of the ecliptic, IAU 1980 model
   double MeanObliquity80(const CommonTime& TT)
   {
       const double T = (MJD(TT).mjd - MJD_J2000) / JC;

       return AS_TO_RAD * ( 84381.448 +
                          (-46.8150   +
                          (-0.00059   +
                          ( 0.001813) * T) * T) * T);
   }

      // Precession matrix from J2000.0 to a specified date, IAU 1976 model
   Matrix<double> PreMatrix76(const CommonTime& TT)
   {
       // Interval between fundamental epoch J2000.0 and start date (JC)
       const double t0 = 0.0;

       // Interval over which precession required (JC)
       const double t = ( MJD(TT).mjd - MJD_J2000 ) / JC;

       // Euler angles.
       double tas2r = t * AS_TO_RAD;
       double w = 2306.2181 + (1.39656 - 0.000139 * t0) * t0;

       double zeta, z, theta;
       zeta   = (w + ((0.30188 - 0.000344 * t0) + 0.017998 * t) * t) * tas2r;
       z      = (w + ((1.09468 + 0.000066 * t0) + 0.018203 * t) * t) * tas2r;
       theta  = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)
              + ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;

       return ( rotation(-z,3) * rotation(theta,2) * rotation(-zeta,3) );

   }

      // Nutation, IAU 1980 model
   void NutAngles80(const CommonTime& TT, double& dpsi,double& deps)
   {
       const double T = (MJD(TT).mjd - MJD_J2000) / JC;
       const double T2 = T * T;
       const double T3 = T2 * T;

       // Nutation coefficients
       static const double C[106][10] = 
       {
         {   0,   0,   0,   0,   1, -6798.4, -171996, -174.2, 92025,   8.9},
         {   0,   0,   2,  -2,   2,   182.6,  -13187,   -1.6,  5736,  -3.1},
         {   0,   0,   2,   0,   2,    13.7,   -2274,   -0.2,   977,  -0.5},
         {   0,   0,   0,   0,   2, -3399.2,    2062,    0.2,  -895,   0.5},
         {   0,  -1,   0,   0,   0,  -365.3,   -1426,    3.4,    54,  -0.1},
         {   1,   0,   0,   0,   0,    27.6,     712,    0.1,    -7,   0.0},
         {   0,   1,   2,  -2,   2,   121.7,    -517,    1.2,   224,  -0.6},
         {   0,   0,   2,   0,   1,    13.6,    -386,   -0.4,   200,   0.0},
         {   1,   0,   2,   0,   2,     9.1,    -301,    0.0,   129,  -0.1},
         {   0,  -1,   2,  -2,   2,   365.2,     217,   -0.5,   -95,   0.3},
         {  -1,   0,   0,   2,   0,    31.8,     158,    0.0,    -1,   0.0},
         {   0,   0,   2,  -2,   1,   177.8,     129,    0.1,   -70,   0.0},
         {  -1,   0,   2,   0,   2,    27.1,     123,    0.0,   -53,   0.0},
         {   1,   0,   0,   0,   1,    27.7,      63,    0.1,   -33,   0.0},
         {   0,   0,   0,   2,   0,    14.8,      63,    0.0,    -2,   0.0},
         {  -1,   0,   2,   2,   2,     9.6,     -59,    0.0,    26,   0.0},
         {  -1,   0,   0,   0,   1,   -27.4,     -58,   -0.1,    32,   0.0},
         {   1,   0,   2,   0,   1,     9.1,     -51,    0.0,    27,   0.0},
         {  -2,   0,   0,   2,   0,  -205.9,     -48,    0.0,     1,   0.0},
         {  -2,   0,   2,   0,   1,  1305.5,      46,    0.0,   -24,   0.0},
         {   0,   0,   2,   2,   2,     7.1,     -38,    0.0,    16,   0.0},
         {   2,   0,   2,   0,   2,     6.9,     -31,    0.0,    13,   0.0},
         {   2,   0,   0,   0,   0,    13.8,      29,    0.0,    -1,   0.0},
         {   1,   0,   2,  -2,   2,    23.9,      29,    0.0,   -12,   0.0},
         {   0,   0,   2,   0,   0,    13.6,      26,    0.0,    -1,   0.0},
         {   0,   0,   2,  -2,   0,   173.3,     -22,    0.0,     0,   0.0},
         {  -1,   0,   2,   0,   1,    27.0,      21,    0.0,   -10,   0.0},
         {   0,   2,   0,   0,   0,   182.6,      17,   -0.1,     0,   0.0},
         {   0,   2,   2,  -2,   2,    91.3,     -16,    0.1,     7,   0.0},
         {  -1,   0,   0,   2,   1,    32.0,      16,    0.0,    -8,   0.0},
         {   0,   1,   0,   0,   1,   386.0,     -15,    0.0,     9,   0.0},
         {   1,   0,   0,  -2,   1,   -31.7,     -13,    0.0,     7,   0.0},
         {   0,  -1,   0,   0,   1,  -346.6,     -12,    0.0,     6,   0.0},
         {   2,   0,  -2,   0,   0, -1095.2,      11,    0.0,     0,   0.0},
         {  -1,   0,   2,   2,   1,     9.5,     -10,    0.0,     5,   0.0},
         {   1,   0,   2,   2,   2,     5.6,      -8,    0.0,     3,   0.0},
         {   0,  -1,   2,   0,   2,    14.2,      -7,    0.0,     3,   0.0},
         {   0,   0,   2,   2,   1,     7.1,      -7,    0.0,     3,   0.0},
         {   1,   1,   0,  -2,   0,   -34.8,      -7,    0.0,     0,   0.0},
         {   0,   1,   2,   0,   2,    13.2,       7,    0.0,    -3,   0.0},
         {  -2,   0,   0,   2,   1,  -199.8,      -6,    0.0,     3,   0.0},
         {   0,   0,   0,   2,   1,    14.8,      -6,    0.0,     3,   0.0},
         {   2,   0,   2,  -2,   2,    12.8,       6,    0.0,    -3,   0.0},
         {   1,   0,   0,   2,   0,     9.6,       6,    0.0,     0,   0.0},
         {   1,   0,   2,  -2,   1,    23.9,       6,    0.0,    -3,   0.0},
         {   0,   0,   0,  -2,   1,   -14.7,      -5,    0.0,     3,   0.0},
         {   0,  -1,   2,  -2,   1,   346.6,      -5,    0.0,     3,   0.0},
         {   2,   0,   2,   0,   1,     6.9,      -5,    0.0,     3,   0.0},
         {   1,  -1,   0,   0,   0,    29.8,       5,    0.0,     0,   0.0},
         {   1,   0,   0,  -1,   0,   411.8,      -4,    0.0,     0,   0.0},
         {   0,   0,   0,   1,   0,    29.5,      -4,    0.0,     0,   0.0},
         {   0,   1,   0,  -2,   0,   -15.4,      -4,    0.0,     0,   0.0},
         {   1,   0,  -2,   0,   0,   -26.9,       4,    0.0,     0,   0.0},
         {   2,   0,   0,  -2,   1,   212.3,       4,    0.0,    -2,   0.0},
         {   0,   1,   2,  -2,   1,   119.6,       4,    0.0,    -2,   0.0},
         {   1,   1,   0,   0,   0,    25.6,      -3,    0.0,     0,   0.0},
         {   1,  -1,   0,  -1,   0, -3232.9,      -3,    0.0,     0,   0.0},
         {  -1,  -1,   2,   2,   2,     9.8,      -3,    0.0,     1,   0.0},
         {   0,  -1,   2,   2,   2,     7.2,      -3,    0.0,     1,   0.0},
         {   1,  -1,   2,   0,   2,     9.4,      -3,    0.0,     1,   0.0},
         {   3,   0,   2,   0,   2,     5.5,      -3,    0.0,     1,   0.0},
         {  -2,   0,   2,   0,   2,  1615.7,      -3,    0.0,     1,   0.0},
         {   1,   0,   2,   0,   0,     9.1,       3,    0.0,     0,   0.0},
         {  -1,   0,   2,   4,   2,     5.8,      -2,    0.0,     1,   0.0},
         {   1,   0,   0,   0,   2,    27.8,      -2,    0.0,     1,   0.0},
         {  -1,   0,   2,  -2,   1,   -32.6,      -2,    0.0,     1,   0.0},
         {   0,  -2,   2,  -2,   1,  6786.3,      -2,    0.0,     1,   0.0},
         {  -2,   0,   0,   0,   1,   -13.7,      -2,    0.0,     1,   0.0},
         {   2,   0,   0,   0,   1,    13.8,       2,    0.0,    -1,   0.0},
         {   3,   0,   0,   0,   0,     9.2,       2,    0.0,     0,   0.0},
         {   1,   1,   2,   0,   2,     8.9,       2,    0.0,    -1,   0.0},
         {   0,   0,   2,   1,   2,     9.3,       2,    0.0,    -1,   0.0},
         {   1,   0,   0,   2,   1,     9.6,      -1,    0.0,     0,   0.0},
         {   1,   0,   2,   2,   1,     5.6,      -1,    0.0,     1,   0.0},
         {   1,   1,   0,  -2,   1,   -34.7,      -1,    0.0,     0,   0.0},
         {   0,   1,   0,   2,   0,    14.2,      -1,    0.0,     0,   0.0},
         {   0,   1,   2,  -2,   0,   117.5,      -1,    0.0,     0,   0.0},
         {   0,   1,  -2,   2,   0,  -329.8,      -1,    0.0,     0,   0.0},
         {   1,   0,  -2,   2,   0,    23.8,      -1,    0.0,     0,   0.0},
         {   1,   0,  -2,  -2,   0,    -9.5,      -1,    0.0,     0,   0.0},
         {   1,   0,   2,  -2,   0,    32.8,      -1,    0.0,     0,   0.0},
         {   1,   0,   0,  -4,   0,   -10.1,      -1,    0.0,     0,   0.0},
         {   2,   0,   0,  -4,   0,   -15.9,      -1,    0.0,     0,   0.0},
         {   0,   0,   2,   4,   2,     4.8,      -1,    0.0,     0,   0.0},
         {   0,   0,   2,  -1,   2,    25.4,      -1,    0.0,     0,   0.0},
         {  -2,   0,   2,   4,   2,     7.3,      -1,    0.0,     1,   0.0},
         {   2,   0,   2,   2,   2,     4.7,      -1,    0.0,     0,   0.0},
         {   0,  -1,   2,   0,   1,    14.2,      -1,    0.0,     0,   0.0},
         {   0,   0,  -2,   0,   1,   -13.6,      -1,    0.0,     0,   0.0},
         {   0,   0,   4,  -2,   2,    12.7,       1,    0.0,     0,   0.0},
         {   0,   1,   0,   0,   2,   409.2,       1,    0.0,     0,   0.0},
         {   1,   1,   2,  -2,   2,    22.5,       1,    0.0,    -1,   0.0},
         {   3,   0,   2,  -2,   2,     8.7,       1,    0.0,     0,   0.0},
         {  -2,   0,   2,   2,   2,    14.6,       1,    0.0,    -1,   0.0},
         {  -1,   0,   0,   0,   2,   -27.3,       1,    0.0,    -1,   0.0},
         {   0,   0,  -2,   2,   1,  -169.0,       1,    0.0,     0,   0.0},
         {   0,   1,   2,   0,   1,    13.1,       1,    0.0,     0,   0.0},
         {  -1,   0,   4,   0,   2,     9.1,       1,    0.0,     0,   0.0},
         {   2,   1,   0,  -2,   0,   131.7,       1,    0.0,     0,   0.0},
         {   2,   0,   0,   2,   0,     7.1,       1,    0.0,     0,   0.0},
         {   2,   0,   2,  -2,   1,    12.8,       1,    0.0,    -1,   0.0},
         {   2,   0,  -2,   0,   1,  -943.2,       1,    0.0,     0,   0.0},
         {   1,  -1,   0,  -2,   0,   -29.3,       1,    0.0,     0,   0.0},
         {  -1,   0,   0,   1,   1,  -388.3,       1,    0.0,     0,   0.0},
         {  -1,  -1,   0,   2,   1,    35.0,       1,    0.0,     0,   0.0},
         {   0,   1,   0,   1,   0,    27.3,       1,    0.0,     0,   0.0}
      };

      double l, lp, F, D, Om;
      double arg, s, c;

        // Mean arguments of luni-solar motion
        //
        // l    Mean anomaly of the Moon
        // l'   Mean anomaly of the Sun
        // F    Mean argument of latitude
        // D    Mean longitude elongation of the Moon from the Sun
        // Om   Mean longitude of the ascending node
     l  = fmod (  485866.733 + (1325.0*TURNAS +  715922.633)*T
                                + 31.310*T2 + 0.064*T3, TURNAS );
     lp = fmod ( 1287099.804 + (  99.0*TURNAS + 1292581.224)*T
                                -  0.577*T2 - 0.012*T3, TURNAS );
     F  = fmod (  335778.877 + (1342.0*TURNAS +  295263.137)*T
                                - 13.257*T2 + 0.011*T3, TURNAS );
     D  = fmod ( 1072261.307 + (1236.0*TURNAS + 1105601.328)*T
                                -  6.891*T2 + 0.019*T3, TURNAS );
     Om = fmod (  450160.280 - (   5.0*TURNAS +  482890.539)*T
                                +  7.455*T2 + 0.008*T3, TURNAS );

        // Nutation in longitude and obliquity [rad]

     deps = dpsi = 0.0;
     for (int i=0; i<106; i++)
     {
         arg  =  ( C[i][0] * l
                 + C[i][1] * lp
                 + C[i][2] * F
                 + C[i][3] * D
                 + C[i][4] * Om ) * AS_TO_RAD;
         s = C[i][6] + C[i][7] * T;
         c = C[i][8] + C[i][9] * T;
         if( s != 0.0) dpsi += s * std::sin(arg);
         if( c != 0.0) deps += c * std::cos(arg);
     }

     dpsi = dpsi * AS_TO_RAD * 1.0e-4;
     deps = deps * AS_TO_RAD * 1.0e-4;

   }


      // Form the matrix of nutation for a given date, IAU 1980 model
   Matrix<double> NutMatrix80(const CommonTime& TT)
   {
       // UTC
       const CommonTime UTC( TT2UTC(TT) );

       // ddpsi, ddeps
       double ddpsi = NutationDPsi(UTC) * AS_TO_RAD;
       double ddeps = NutationDEps(UTC) * AS_TO_RAD;

       // Mean obliquity of the ecliptic
       double epsa;
       epsa = MeanObliquity80( TT );

       // Nutation in longitude and obliquity
       double dpsi, deps;
       NutAngles80(TT, dpsi, deps);

       // Add adjustments: frame bias, precession-rates, geophysical
       dpsi += ddpsi;
       deps += ddeps;

       return rotation(-epsa-deps,1) * rotation(-dpsi,3) * rotation(+epsa,1);

   }

      // Equation of the equinoxes, IAU 1994 model
   double EqnEquinox94(const CommonTime& TT)
   {
       // Interval between fundamental epoch J2000.0 and given date (JC)
       double t = ( MJD(TT).mjd - MJD_J2000 ) / JC;

       // Longitude of the mean ascending node of the lunar orbit on the
       // ecliptic, measured from the mean equinox of date
       double om = Anpm((450160.280 + (-482890.539
                    + (7.455 + 0.008 * t) * t) * t) * AS_TO_RAD
                    + std::fmod(-5.0 * t, 1.0) * TWO_PI);

       // Nutation components and mean obliquity.
       double dpsi(0.0), deps(0.0);
       NutAngles80(TT, dpsi, deps);
       double eps0 = MeanObliquity80(TT);

       // Equation of the equinoxes.
       double ee = dpsi * std::cos(eps0) + AS_TO_RAD * (0.00264 * std::sin(om) + 0.000063 * std::sin(om + om));

       return ee;

   }

      // Universal Time to Greenwich mean sidereal time (IAU 1982 model)
   double GMST82(const CommonTime& UT1)
   {
      // Coefficients of IAU 1982 GMST-UT1 model
      const double A = 24110.54841;
      const double B = 8640184.812866;
      const double C = 0.093104;
      const double D =  -6.2e-6;

      // Julian centuries since fundamental epoch
      double t = (MJD(UT1).mjd - MJD_J2000) / JC;

      // Fractional part of JD(UT1), in seconds
      double f = DAY_TO_SEC * std::fmod(double(MJD(UT1).mjd), 1.0);

      // GMST at this UT1
      double gmst;
      gmst = Anp(SEC_TO_RAD * ((A + (B + (C + D * t) * t) * t) + f) );

      return gmst;

   }


      // Greenwich apparent sidereal time (consistent with IAU 1982/94
      // resolutions)
   double GAST94(const CommonTime& UT1)
   {
       // UTC
       const CommonTime UTC( UT12UTC(UT1) );

       // TT
       const CommonTime TT ( UTC2TT(UTC)  );

       // Mean obliquity of the ecliptic
       double epsa = MeanObliquity80(TT);

       // Celestial pole offsets in longitude
       double ddpsi = NutationDPsi(UTC) * AS_TO_RAD;

       // GAST at this UT1
       double gast;
       gast = Anp( GMST82(UT1) + EqnEquinox94(TT) + ddpsi * std::cos(epsa) );

       return gast;
   }


      // Greenwich Hour Angle Matrix
   Matrix<double> GHAMatrix(const CommonTime& UT1)
   {
       return rotation( GAST94(UT1), 3 );
   }

   
      // Pole Matrix
   Matrix<double> PoleMatrix(const CommonTime& UTC)
   {
       double xp, yp;
       xp = PolarMotionX(UTC) * AS_TO_RAD;
       yp = PolarMotionY(UTC) * AS_TO_RAD;

       return rotation(-xp, 2) * rotation(-yp, 1);
   }


      // CRS to TRS Matrix
   Matrix<double> C2TMatrix(const CommonTime& UTC)
   {
       // TT
       CommonTime TT( UTC2TT(UTC) );

       // UT1
       CommonTime UT1( UTC2UT1(UTC) );

       // Precession Matrix
       Matrix<double> P( PreMatrix76(TT) );
//       cout << "P: " << endl << P << endl;

       // Nutation Matrix
       Matrix<double> N( NutMatrix80(TT) );
//       cout << "N: " << endl << N << endl;

       // Greenwich Hour Angle Matrix
       Matrix<double> R( GHAMatrix(UT1) );
//       cout << "R: " << endl << R << endl;

       // Pole Matrix
       Matrix<double> W( PoleMatrix(UTC) );
//       cout << "W: " << endl << W << endl;

       return W * R * N * P;
   }


      // TRS to CRS Matrix
   Matrix<double> T2CMatrix(const CommonTime& UTC)
   {
       return transpose( C2TMatrix(UTC) );
   }


      // Earth rotation angle (IAU 2000 model)
   double EarthRotationAngle(const CommonTime& UT1)
   {
       // Days since fundamental epoch
       const double t(MJD(UT1).mjd - MJD_J2000);

       // Fractional part of T (days)
       const double f(std::fmod(double(MJD(UT1).mjd),1.0) + std::fmod(JD_TO_MJD,1.0));

       // Earth rotation angle at this UT1
       double theta = Anp(TWO_PI*(f+ 0.7790572732640 + 0.00273781191135448*t));

       return theta;
   }


      // Earth rotation angle first order rate
   double EarthRotationAngleRate1(const CommonTime& UT1)
   {
       //
       const double t( (MJD(UT1).mjd - MJD_J2000) / JC );

       // 
       double dtheta = (1.002737909350795 + 5.9006e-11*t - 5.9e-15*t*t) * TWO_PI / DAY_TO_SEC;

       return dtheta;
   }


      // Time derivative of transformation matrix from CRS to TRS coordinates
      // for a given date
   Matrix<double> dC2TMatrix(const CommonTime& UTC)
   {
       // TT
       CommonTime TT( UTC2TT(UTC) );

       // UT1
       CommonTime UT1( UTC2UT1(UTC) );

       // Precession Matrix
       Matrix<double> P( PreMatrix76(TT) );

       // Nutation Matrix
       Matrix<double> N( NutMatrix80(TT) );

       // Greenwich apparent sideral time
       double GAST( GAST94(UT1) );

       // Time derivative of greenwich apparent sideral time
       double dGAST( EarthRotationAngleRate1(UT1) );

       // Time derivative of Greenwich Hour Angle Matrix
       Matrix<double> dR(3,3,0.0);
       dR[0][0] = -std::sin(GAST);
       dR[0][1] =  std::cos(GAST);
       dR[1][0] = -std::cos(GAST);
       dR[1][1] = -std::sin(GAST);

       dR *= dGAST;

       // Pole Matrix
       Matrix<double> W( PoleMatrix(UTC) );

       return W * dR * N * P;
   }


      // Time derivative of transformation matrix from TRS to CRS coordinates
      // for a given date
   Matrix<double> dT2CMatrix(const CommonTime& UTC)
   {
       return transpose( dC2TMatrix(UTC) );
   }

      // Doodson fundamental arguments
   void DoodsonArguments(const CommonTime& UT1,
                         const CommonTime& TT,
                         double BETA[6],
                         double FNUT[5]         )
   {
      // GMST, IAU 1980 model
      double THETA = GMST82(UT1);

      // Fundamental Arguments
      //-----------------------------------------------------
      //Julian centuries since J2000
      const double  t = (MJD(TT).mjd - MJD_J2000) / JC;

      // Arcseconds in a full circle
      double TURNAS = 360.0 * 3600.0;


      // Mean anomaly of the Moon.
      double temp = fmod(           485868.249036  +
         t * ( 1717915923.2178 +
         t * (         31.8792 +
         t * (          0.051635 +
         t * (        - 0.00024470 ) ) ) ), TURNAS ) * AS_TO_RAD;

      double F1 = Anpm(temp);           // [-PI, +PI]

      // Mean anomaly of the Sun
      temp = fmod(         1287104.793048 +
         t * ( 129596581.0481 +
         t * (       - 0.5532 +
         t * (         0.000136 +
         t * (       - 0.00001149 ) ) ) ), TURNAS ) * AS_TO_RAD;

      double F2 = Anpm(temp);           // [-PI, +PI]
      
      // Mean longitude of the Moon minus that of the ascending node.
      temp = fmod(           335779.526232 +
         t * ( 1739527262.8478 +
         t * (       - 12.7512 +
         t * (        - 0.001037 +
         t * (          0.00000417 ) ) ) ), TURNAS ) * AS_TO_RAD;
      double F3 = Anpm(temp);           // [-PI, +PI]
      
      // Mean elongation of the Moon from the Sun. 
      temp = fmod(          1072260.703692 +
         t * ( 1602961601.2090 +
         t * (        - 6.3706 +
         t * (          0.006593 +
         t * (        - 0.00003169 ) ) ) ), TURNAS ) * AS_TO_RAD;

      double F4 = Anpm(temp);           // [-PI, +PI]
      
      // Mean longitude of the ascending node of the Moon.
      temp = fmod(          450160.398036 +
         t * ( - 6962890.5431 +
         t * (         7.4722 +
         t * (         0.007702 +
         t * (       - 0.00005939 ) ) ) ), TURNAS ) * AS_TO_RAD;
      double F5 = Anpm(temp);           // [-PI, +PI]

      FNUT[0] = F1;
      FNUT[1] = F2;
      FNUT[2] = F3;
      FNUT[3] = F4;
      FNUT[4] = F5;

      double S = F3+F5;

      BETA[0] = THETA + PI - S;
      BETA[1] = F3 + F5;
      BETA[2] = S - F4;
      BETA[3] = S - F1;
      BETA[4] = -F5;
      BETA[5] = S - F4 - F2;

   }


   // JPL Ephemeris Data Handling
   //-------------------------------------------------------------------------

      // JPL Ephemeris File
   void LoadJPLEphFile(std::string fileName)
   {
       jplEph.clearStorage();

       int rc = jplEph.initializeWithBinaryFile(fileName);
       if(rc != 0)
       {
           GPSTK_THROW( Exception("Failed to load JPL EPH File " + fileName) );
       }
   }

      // Planet position and velocity in J2000, in m and m/s
   Vector<double> J2kPosVel(const CommonTime&   TT,
                            SolarSystem::Planet entity,
                            SolarSystem::Planet center)
   {
       Vector<double> rvJ2000(6,0.0);

       try
       {
           double rvState[6] = {0.0};
           int ret = jplEph.computeState(MJD(TT).mjd+MJD_TO_JD,
                                         entity,
                                         center,
                                         rvState);

           // km -> m
           rvState[0] *= 1e3;
           rvState[1] *= 1e3;
           rvState[2] *= 1e3;
           // km/s -> m/s
           rvState[3] *= 1e3;
           rvState[4] *= 1e3;
           rvState[5] *= 1e3;

           if(ret == 0)
           {
               rvJ2000 = rvState;
               return rvJ2000;
           }
       }
       catch(...)
       {
           GPSTK_THROW(Exception("Fail to compute planet position and velocity"));
       }

       return rvJ2000;
   }


      // Planet position in J2000, in m
   Vector<double> J2kPosition(const CommonTime&   TT,
                              SolarSystem::Planet entity)
   {
       Vector<double> rvJ2k = J2kPosVel(TT, entity);

       Vector<double> rJ2k(3,0.0);
       for(int i=0; i<3; i++)
       {
           rJ2k[i] = rvJ2k[i];
       }

       return rJ2k;
   }


      // Planet velocity in J2000, in m/s
   Vector<double> J2kVelocity(const CommonTime&   TT,
                              SolarSystem::Planet entity)
   {
       Vector<double> rvJ2k = J2kPosVel(TT, entity);

       Vector<double> vJ2k(3,0.0);
       for(int i=0; i<3; i++)
       {
           vJ2k[i] = rvJ2k[i+3];
       }

       return vJ2k;
   }


      // Sun position in J2000
   Vector<double> SunJ2kPosition(const CommonTime& TT)
   {

       // Number of Julian centuries since J2000
       const double T( (MJD(TT).mjd - MJD_J2000) / JC );

       // Appropriate mean orbital elements
       double Oo, M, eps;
       Oo   = 282.9400 * DEG_TO_RAD;
       M    = std::fmod(357.5256 + 35999.049*T, 360.0) * DEG_TO_RAD;
       eps  = 23.43929111 * DEG_TO_RAD;

       // The Sun's ecliptic longitude and distance, the ecliptic latitude
       // vanishes within an accuracy of 1'
       double l = Oo + M + (6892*std::sin(M) + 72*std::sin(M+M)) * AS_TO_RAD;
       double r = (149.619 - 2.499*std::cos(M) - 0.021*std::cos(M+M)) * 1e9;

       Vector<double> res(3,0.0);
       res[0] = r * std::cos(l) * 1.0;
       res[1] = r * std::sin(l) * 1.0;
       res[2] = r * 0.0;

       res = rotation(-eps,1) * res;

       return res;
   }

      // Moon position in J2000
   Vector<double> MoonJ2kPosition(const CommonTime& TT)
   {
       // Number of Julian centuries since J2000
       const double T( (MJD(TT).mjd - MJD_J2000) / JC );

       // Appropriate mean orbital elements
       double L0, l, lp, F, D, eps;
       L0 = std::fmod(218.31617 + 481267.88088*T - 1.3972*T, 360.0) * DEG_TO_RAD;
       l  = std::fmod(134.96292 + 477198.86753*T, 360.0) * DEG_TO_RAD;
       lp = std::fmod(357.52543 + 35999.04944*T, 360.0) * DEG_TO_RAD;
       F  = std::fmod(93.27283 + 483202.01873*T, 360.0) * DEG_TO_RAD;
       D  = std::fmod(297.85027 + 445267.11135*T, 360.0) * DEG_TO_RAD;
       eps  = 23.43929111 * DEG_TO_RAD;

       // The Moon's ecliptic longitude, latitude and distance
       double lon, lat, rad;
       double dlon =  (22640*std::sin(l) + 769*std::sin(l+l)
                    -  4586*std::sin(l-D-D) + 2370*std::sin(D+D)
                    -  668*std::sin(lp) - 412*std::sin(F+F)
                    -  212*std::sin(l+l-D-D) - 206*std::sin(l+lp-D-D)
                    +  192*std::sin(l+D+D) - 165*std::sin(lp-D-D)
                    +  148*std::sin(l-lp) - 125*std::sin(D)
                    -  110*std::sin(l+lp) - 55*std::sin(F+F-D-D)) * AS_TO_RAD;

       lon = L0 + dlon;
       double s = (412*std::sin(F+F) + 541*std::sin(lp)) * AS_TO_RAD;
       lat = (18520*std::sin(F + dlon + s)
            - 526*std::sin(F-D-D) + 44*std::sin(l+F-D-D)
            - 31*std::sin(-l+F-D-D) - 25*std::sin(-l-l+F)
            - 23*std::sin(lp+F-D-D) + 21*std::sin(-l+F)
            + 11*std::sin(-lp+F-D-D)) * AS_TO_RAD;
       rad = (385000 - 20905*std::cos(l) - 3699*std::cos(D+D-l)
            - 2956*std::cos(D+D) - 570*std::cos(l+l) + 246*std::cos(l+l-D-D)
            - 205*std::cos(lp-D-D) - 171*std::cos(l+D+D)
            - 152*std::cos(l+lp-D-D)) * 1e3;

       Vector<double> res(3,0.0);
       res[0] = rad * std::cos(lon) * std::cos(lat);
       res[1] = rad * std::sin(lon) * std::cos(lat);
       res[2] = rad * std::sin(lat);

       res = rotation(-eps,1) * res;

       return res;
   }

 
} // end of namespace gpstk

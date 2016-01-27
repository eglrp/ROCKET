/**
 * @file ReferenceSystem.cpp
 * This class ease handling Reference System transformation.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Last Modified:
//
//  create this file, 2013/05/29
//
//  Shoujian Zhang, Wuhan University
//
//============================================================================

#include "ReferenceSystem.hpp"
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "Epoch.hpp"

using namespace std;

namespace gpstk
{
      // MJD of J2000 epoch
   const double ReferenceSystem::MJD_J2000 = 51544.5;

      /// Method from EOPDataStore     

      /// Get the x pole displacement parameter, in arcseconds.
   double ReferenceSystem::getXPole(const CommonTime& UTC) const
      throw (InvalidRequest)
   {
      double xpole;

      try
      {
            // Get the x pole from EOPDataStore
         if( pEopStore != NULL )
         {
//            xpole = (*pEopStore).getXPole( UTC );
            xpole = (*pEopStore).getEOPData( UTC ).xp;
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return xpole;

   } // End of method 'ReferenceSystem::getXPole()'


      /// Get the y pole displacement parameter, in arcseconds.
   double ReferenceSystem::getYPole(const CommonTime& UTC) const
      throw (InvalidRequest)
   {
      double ypole;

      try
      {
            // Get the x pole from EOPDataStore
         if( pEopStore != NULL )
         {
//            ypole = (*pEopStore).getYPole( UTC );
            ypole = (*pEopStore).getEOPData( UTC ).yp;
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return ypole;

   } // End of method 'ReferenceSystem::getYPole()'


      /// Get the value of UT1mUTC, in seconds.
   double ReferenceSystem::getUT1mUTC(const CommonTime& UTC) const
      throw (InvalidRequest) 
   {
      double UT1mUTC;

      try
      {
            // Get the UT1mUTC from EOPDataStore
         if( pEopStore != NULL )
         {
//            UT1mUTC = (*pEopStore).getUT1mUTC( UTC );
            UT1mUTC = (*pEopStore).getEOPData( UTC ).UT1mUTC;
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return UT1mUTC;

   } // End of method 'ReferenceSystem::getUT1mUTC()'


      /// Get the value of dPsi, in arcseconds.
   double ReferenceSystem::getDPsi(const CommonTime& UTC) const
       throw(InvalidRequest)
   {
       double dPsi;

       try
       {
           // Get the dPsi from EOPDataStore
           if( pEopStore != NULL )
           {
               dPsi = (*pEopStore).getEOPData( UTC ).dPsi;
           }
       }
       catch(InvalidRequest& ire) 
       {
           GPSTK_RETHROW(ire);
       }

       // return
       return dPsi;

   } // End of method 'ReferenceSystem::getDPsi()'


      /// Get the value of dEps, in arcseconds.
   double ReferenceSystem::getDEps(const CommonTime& UTC) const
       throw(InvalidRequest)
   {
       double dEps;

       try
       {
           // Get the dEps from EOPDataStore
           if( pEopStore != NULL )
           {
               dEps = (*pEopStore).getEOPData( UTC ).dEps;
           }
       }
       catch(InvalidRequest& ire) 
       {
           GPSTK_RETHROW(ire);
       }

       // return
       return dEps;

   } // End of method 'ReferenceSystem::getDEps()'


      /// Get the EOP data at the given UTC time.
   EOPDataStore::EOPData ReferenceSystem::getEOPData(const CommonTime& UTC) const
      throw(InvalidRequest)
   {
      EOPDataStore::EOPData eopData;

      try
      {
            // Get the EOP data from EOPDataStore
         if( pEopStore != NULL )
         {
            eopData = (*pEopStore).getEOPData( UTC );
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return eopData;

   } // End of method 'ReferenceSystem::getEOPData()'
   

      /// Method from LeapSecondStore

      /// Get the leap second at the given UTC time.
   double ReferenceSystem::getLeapSec(const CommonTime& UTC) const
      throw(InvalidRequest)
   {
      double leapSec;

      try
      {
            // Get the leap second from EOPDataStore
         if( pLeapSecStore!= NULL )
         {
            leapSec = (*pLeapSecStore).getLeapSec( UTC );
         }

      }
      catch(InvalidRequest& ire)
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return leapSec;

   } // End of method 'ReferenceSystem::getLeapSec()'


      /// Method to get the difference between time systems

      /// Return the value of (TAI - UTC) (= leapsec).
   double ReferenceSystem::getTAImUTC(const CommonTime& UTC)
      throw(InvalidRequest)
   {
          // Leap second = TAI - UTC
       return getLeapSec(UTC);

   } // End of method 'ReferenceSystem::getTAImUTC()


      /// Get the value of (TT - TAI)
   double ReferenceSystem::getTTmTAI( void )
      throw(InvalidRequest)
   {
         // The difference is constant : 32.184 second
      return 32.184;

   } // End of method 'ReferenceSystem::getTTmTAI()


      /// Get the value of (TT - UTC)
   double ReferenceSystem::getTTmUTC(const CommonTime& UTC)
      throw(InvalidRequest)
   {
         // TT - TAI
      double TTmTAI ( getTTmTAI() );

         // TAI - UTC
      double TAImUTC( getTAImUTC(UTC) );

         // TT - UTC
      double TTmUTC( TTmTAI + TAImUTC );

         // return
      return TTmUTC;

   } // End of method 'ReferenceSystem::getTTmTAI()


      /// Get the value of (TAI - GPST)
   double ReferenceSystem::getTAImGPST(void)
      throw(InvalidRequest)
   {
         // The difference is constant : 19.0 second
      return 19.0;

   } // End of method 'ReferenceSystem::getTAImGPST()


      /// Methods to convert between different time systems


      /// Convert GPST to UTC.
   CommonTime ReferenceSystem::convertGPST2UTC(const CommonTime& GPST)
   {

         // The input time of 'getLeapSec' should be 'UTC'
         // if the input is GPST, you should amends it.
      CommonTime GPST1(GPST);
      GPST1.setTimeSystem(TimeSystem::UTC);

      double leapSec = getLeapSec( GPST1 );
//      cout << leapSec << endl;

         // The formula of GPS and UTC equals with:
         // GPS = UTC + ( leapSec - 19.0s );

      CommonTime UTC1;
      UTC1  = GPST1 - (leapSec - 19.0);

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
      leapSec = getLeapSec(UTC1);


      CommonTime UTC;
      UTC = GPST1 -  (leapSec - 19.0);

      // The TimeSystem of UTC is already set to TimeSystem::UTC.
      return UTC;

   } // End of method 'ReferenceSystem::convertGPST2UTC()'


      /// Convert UTC to GPST.
   CommonTime ReferenceSystem::convertUTC2GPST(const CommonTime& UTC)
   {
         // Now, set gpst with utc
      CommonTime TAI;

         // Convert the UTC time to TAI time by correcting the leap second
      TAI = UTC + getTAImUTC(UTC);

      CommonTime GPST;

         // Convert the TAI time to GPST time
      GPST = TAI - getTAImGPST();
      GPST.setTimeSystem(TimeSystem::GPS);

         // return gpst
      return GPST;

   } // End of method 'ReferenceSystem::convertUTC2GPST()'


      /// Convert UT1 to UTC.
   CommonTime ReferenceSystem::convertUT12UTC(const CommonTime& UT1)
   {

         // Warnging the input time should be utc, so
         // you must amends it
      CommonTime UTC1( UT1 );
      UTC1.setTimeSystem(TimeSystem::UTC);
      UTC1 = UT1 -  getUT1mUTC(UTC1);
      UTC1.setTimeSystem(TimeSystem::UTC);

         // Correct utc by computing new UT1mUTC with utc1
      CommonTime UTC2;
      UTC2 = UT1 -  getUT1mUTC(UTC1);
      UTC2.setTimeSystem(TimeSystem::UTC);

         // Corrent utc by computing new UT1mUTC with utc2
      CommonTime UTC;
      UTC  = UT1 -  getUT1mUTC(UTC2);
      UTC.setTimeSystem(TimeSystem::UTC);

      return UTC;

   }  // End of method 'ReferenceSystem::convertUT12UTC()'


      /// Convert UTC to UT1.
   CommonTime ReferenceSystem::convertUTC2UT1(const CommonTime& UTC)
   {

      CommonTime UT1;
      UT1 =  UTC + getUT1mUTC(UTC);
      UT1.setTimeSystem(TimeSystem::UT1);

      return UT1;

   }  // End of method 'ReferenceSystem::convertUTC2UT1()'


      /// Convert TAI to UTC.
   CommonTime ReferenceSystem::convertTAI2UTC(const CommonTime& TAI)
   {
       // Initial UTC1.
       CommonTime UTC1( TAI );
       UTC1.setTimeSystem(TimeSystem::UTC);

       // Get leapSec.
       double leapSec( getLeapSec(UTC1) );

       // Update UTC1.
       UTC1 = UTC1 - leapSec;

       // Update leapSec.
       leapSec = getLeapSec(UTC1);

       // Get UTC.
       CommonTime UTC( TAI - leapSec );
       UTC.setTimeSystem(TimeSystem::UTC);

       // return UTC.
       return UTC;

   }  // End of method 'ReferenceSystem::convertTAI2UTC()'


      /// Convert UTC to TAI.
   CommonTime ReferenceSystem::convertUTC2TAI(const CommonTime& UTC)
   {
       // Get leapSec.
       double leapSec( getLeapSec(UTC) );

       // Get TAI.
       CommonTime TAI( UTC + leapSec );
       TAI.setTimeSystem(TimeSystem::TAI);

       // Return TAI.
       return TAI;

   }  // End of method 'ReferenceSystem::convertUTC2TAI()'


      /// Convert TT to UTC.
   CommonTime ReferenceSystem::convertTT2UTC(const CommonTime& TT)
   {
       // Convert TT to TAI.
       CommonTime TAI( TT - getTTmTAI() );
       TAI.setTimeSystem(TimeSystem::TAI);

       // Convert TAI to UTC.
       CommonTime UTC( convertTAI2UTC(TAI) );


       // Return UTC.
       return UTC;

   }  // End of method 'ReferenceSystem::convertTT2UTC()'


      /// Convert UTC to TT.
   CommonTime ReferenceSystem::convertUTC2TT(const CommonTime& UTC)
   {
       // Convert UTC to TAI.
       CommonTime TAI( convertUTC2TAI(UTC) );
       // Convert TAI to TT.
       CommonTime TT( TAI + getTTmTAI() );
       TT.setTimeSystem(TimeSystem::TT);

       // Return TT.
       return TT;

   }  // End of method 'ReferenceSystem::convertUTC2TT()'



      /**Computes the mean obliquity of the ecliptic.
       * Reference: IERS Conventions 1996, Chapter 5.
       *
       * @param   Mjd_TT    Modified Julian Date (Terrestrial Time)
       * @return            Mean obliquity of the ecliptic
       */
   double ReferenceSystem::MeanObliquity (double Mjd_TT)
   {
     const double T = (Mjd_TT - MJD_J2000) / 36525.0;

     return AS_TO_RAD * ( 84381.448 +
                        (-46.8150   +
                        (-0.00059   +
                        ( 0.001813) * T) * T) * T);

   }  // End of method 'ReferenceSystem::MeanObliquity()'


      /**Transformation of equatorial to ecliptical coordinates.
       *
       *@param Mjd_TT    Modified Julian Date (Terrestrial Time)
       *@return          Transformation matrix
       */
   Matrix<double> ReferenceSystem::EclMatrix (double Mjd_TT)
   {

     return rotation ( MeanObliquity(Mjd_TT), 1 );

   }  // End of method 'ReferenceSystem::EclMatrix()'
   

      /**Precession transformation of equatorial coordinates.
       *
       *@param Mjd_1     Epoch given (Modified Julian Date TT)
       *@param MjD_2     Epoch to precess to (Modified Julian Date TT)
       *@return          Precession transformation matrix
       */
   Matrix<double> ReferenceSystem::PrecMatrix (double Mjd_1, double Mjd_2)
   {
     /// Interval between fundamental epoch J2000.0 and start date (JC).
     const double T = (Mjd_1-MJD_J2000) / 36525.0;
     /// Interval over which precession required (JC).
     const double t = (Mjd_2-Mjd_1) / 36525.0;

     /// Euler angles.
     double tas2r = t * AS_TO_RAD;
     double w = 2306.2181 + (1.39656 - 0.000139 * T) * T;

     double zeta,z,theta;
     zeta   = (w + ((0.30188 - 0.000344 * T) + 0.017998 * t) * t) * tas2r;
     z      = (w + ((1.09468 + 0.000066 * T) + 0.018203 * t) * t) * tas2r;
     theta  = ((2004.3109 + (-0.85330 - 0.000217 * T) * T)
            + ((-0.42665 - 0.000217 * T) - 0.041833 * t) * t) * tas2r;

        // Precession matrix
     return rotation(-z,3) * rotation(theta,2) * rotation(-zeta,3);

   }  // End of method 'ReferenceSystem::PrecMatrix()'


      /**Nutation in longitude and obliquity.
       *
       *@param  Mjd_TT    Modified Julian Date (Terrestrial Time)
       *@return           Nutation angles
       */
   void ReferenceSystem::NutAngles (double Mjd_TT, double& dpsi, double& deps)
   {
        // Constants
     const double T  = (Mjd_TT - MJD_J2000) / 36525.0;
     const double T2 = T * T;
     const double T3 = T2 * T;
     const double rev = 360.0 * 3600.0;

     const int  N_coeff = 106;
     const long C[N_coeff][9] =
     {
         //
         // l  l' F  D Om    dpsi    *T     deps     *T       #
         //
       {  0, 0, 0, 0, 1,-1719960,-1742,  920250,   89 },   //   1
       {  0, 0, 0, 0, 2,   20620,    2,   -8950,    5 },   //   2
       { -2, 0, 2, 0, 1,     460,    0,    -240,    0 },   //   3
       {  2, 0,-2, 0, 0,     110,    0,       0,    0 },   //   4
       { -2, 0, 2, 0, 2,     -30,    0,      10,    0 },   //   5
       {  1,-1, 0,-1, 0,     -30,    0,       0,    0 },   //   6
       {  0,-2, 2,-2, 1,     -20,    0,      10,    0 },   //   7
       {  2, 0,-2, 0, 1,      10,    0,       0,    0 },   //   8
       {  0, 0, 2,-2, 2, -131870,  -16,   57360,  -31 },   //   9
       {  0, 1, 0, 0, 0,   14260,  -34,     540,   -1 },   //  10
       {  0, 1, 2,-2, 2,   -5170,   12,    2240,   -6 },   //  11
       {  0,-1, 2,-2, 2,    2170,   -5,    -950,    3 },   //  12
       {  0, 0, 2,-2, 1,    1290,    1,    -700,    0 },   //  13
       {  2, 0, 0,-2, 0,     480,    0,      10,    0 },   //  14
       {  0, 0, 2,-2, 0,    -220,    0,       0,    0 },   //  15
       {  0, 2, 0, 0, 0,     170,   -1,       0,    0 },   //  16
       {  0, 1, 0, 0, 1,    -150,    0,      90,    0 },   //  17
       {  0, 2, 2,-2, 2,    -160,    1,      70,    0 },   //  18
       {  0,-1, 0, 0, 1,    -120,    0,      60,    0 },   //  19
       { -2, 0, 0, 2, 1,     -60,    0,      30,    0 },   //  20
       {  0,-1, 2,-2, 1,     -50,    0,      30,    0 },   //  21
       {  2, 0, 0,-2, 1,      40,    0,     -20,    0 },   //  22
       {  0, 1, 2,-2, 1,      40,    0,     -20,    0 },   //  23
       {  1, 0, 0,-1, 0,     -40,    0,       0,    0 },   //  24
       {  2, 1, 0,-2, 0,      10,    0,       0,    0 },   //  25
       {  0, 0,-2, 2, 1,      10,    0,       0,    0 },   //  26
       {  0, 1,-2, 2, 0,     -10,    0,       0,    0 },   //  27
       {  0, 1, 0, 0, 2,      10,    0,       0,    0 },   //  28
       { -1, 0, 0, 1, 1,      10,    0,       0,    0 },   //  29
       {  0, 1, 2,-2, 0,     -10,    0,       0,    0 },   //  30
       {  0, 0, 2, 0, 2,  -22740,   -2,    9770,   -5 },   //  31
       {  1, 0, 0, 0, 0,    7120,    1,     -70,    0 },   //  32
       {  0, 0, 2, 0, 1,   -3860,   -4,    2000,    0 },   //  33
       {  1, 0, 2, 0, 2,   -3010,    0,    1290,   -1 },   //  34
       {  1, 0, 0,-2, 0,   -1580,    0,     -10,    0 },   //  35
       { -1, 0, 2, 0, 2,    1230,    0,    -530,    0 },   //  36
       {  0, 0, 0, 2, 0,     630,    0,     -20,    0 },   //  37
       {  1, 0, 0, 0, 1,     630,    1,    -330,    0 },   //  38
       { -1, 0, 0, 0, 1,    -580,   -1,     320,    0 },   //  39
       { -1, 0, 2, 2, 2,    -590,    0,     260,    0 },   //  40
       {  1, 0, 2, 0, 1,    -510,    0,     270,    0 },   //  41
       {  0, 0, 2, 2, 2,    -380,    0,     160,    0 },   //  42
       {  2, 0, 0, 0, 0,     290,    0,     -10,    0 },   //  43
       {  1, 0, 2,-2, 2,     290,    0,    -120,    0 },   //  44
       {  2, 0, 2, 0, 2,    -310,    0,     130,    0 },   //  45
       {  0, 0, 2, 0, 0,     260,    0,     -10,    0 },   //  46
       { -1, 0, 2, 0, 1,     210,    0,    -100,    0 },   //  47
       { -1, 0, 0, 2, 1,     160,    0,     -80,    0 },   //  48
       {  1, 0, 0,-2, 1,    -130,    0,      70,    0 },   //  49
       { -1, 0, 2, 2, 1,    -100,    0,      50,    0 },   //  50
       {  1, 1, 0,-2, 0,     -70,    0,       0,    0 },   //  51
       {  0, 1, 2, 0, 2,      70,    0,     -30,    0 },   //  52
       {  0,-1, 2, 0, 2,     -70,    0,      30,    0 },   //  53
       {  1, 0, 2, 2, 2,     -80,    0,      30,    0 },   //  54
       {  1, 0, 0, 2, 0,      60,    0,       0,    0 },   //  55
       {  2, 0, 2,-2, 2,      60,    0,     -30,    0 },   //  56
       {  0, 0, 0, 2, 1,     -60,    0,      30,    0 },   //  57
       {  0, 0, 2, 2, 1,     -70,    0,      30,    0 },   //  58
       {  1, 0, 2,-2, 1,      60,    0,     -30,    0 },   //  59
       {  0, 0, 0,-2, 1,     -50,    0,      30,    0 },   //  60
       {  1,-1, 0, 0, 0,      50,    0,       0,    0 },   //  61
       {  2, 0, 2, 0, 1,     -50,    0,      30,    0 },   //  62
       {  0, 1, 0,-2, 0,     -40,    0,       0,    0 },   //  63
       {  1, 0,-2, 0, 0,      40,    0,       0,    0 },   //  64
       {  0, 0, 0, 1, 0,     -40,    0,       0,    0 },   //  65
       {  1, 1, 0, 0, 0,     -30,    0,       0,    0 },   //  66
       {  1, 0, 2, 0, 0,      30,    0,       0,    0 },   //  67
       {  1,-1, 2, 0, 2,     -30,    0,      10,    0 },   //  68
       { -1,-1, 2, 2, 2,     -30,    0,      10,    0 },   //  69
       { -2, 0, 0, 0, 1,     -20,    0,      10,    0 },   //  70
       {  3, 0, 2, 0, 2,     -30,    0,      10,    0 },   //  71
       {  0,-1, 2, 2, 2,     -30,    0,      10,    0 },   //  72
       {  1, 1, 2, 0, 2,      20,    0,     -10,    0 },   //  73
       { -1, 0, 2,-2, 1,     -20,    0,      10,    0 },   //  74
       {  2, 0, 0, 0, 1,      20,    0,     -10,    0 },   //  75
       {  1, 0, 0, 0, 2,     -20,    0,      10,    0 },   //  76
       {  3, 0, 0, 0, 0,      20,    0,       0,    0 },   //  77
       {  0, 0, 2, 1, 2,      20,    0,     -10,    0 },   //  78
       { -1, 0, 0, 0, 2,      10,    0,     -10,    0 },   //  79
       {  1, 0, 0,-4, 0,     -10,    0,       0,    0 },   //  80
       { -2, 0, 2, 2, 2,      10,    0,     -10,    0 },   //  81
       { -1, 0, 2, 4, 2,     -20,    0,      10,    0 },   //  82
       {  2, 0, 0,-4, 0,     -10,    0,       0,    0 },   //  83
       {  1, 1, 2,-2, 2,      10,    0,     -10,    0 },   //  84
       {  1, 0, 2, 2, 1,     -10,    0,      10,    0 },   //  85
       { -2, 0, 2, 4, 2,     -10,    0,      10,    0 },   //  86
       { -1, 0, 4, 0, 2,      10,    0,       0,    0 },   //  87
       {  1,-1, 0,-2, 0,      10,    0,       0,    0 },   //  88
       {  2, 0, 2,-2, 1,      10,    0,     -10,    0 },   //  89
       {  2, 0, 2, 2, 2,     -10,    0,       0,    0 },   //  90
       {  1, 0, 0, 2, 1,     -10,    0,       0,    0 },   //  91
       {  0, 0, 4,-2, 2,      10,    0,       0,    0 },   //  92
       {  3, 0, 2,-2, 2,      10,    0,       0,    0 },   //  93
       {  1, 0, 2,-2, 0,     -10,    0,       0,    0 },   //  94
       {  0, 1, 2, 0, 1,      10,    0,       0,    0 },   //  95
       { -1,-1, 0, 2, 1,      10,    0,       0,    0 },   //  96
       {  0, 0,-2, 0, 1,     -10,    0,       0,    0 },   //  97
       {  0, 0, 2,-1, 2,     -10,    0,       0,    0 },   //  98
       {  0, 1, 0, 2, 0,     -10,    0,       0,    0 },   //  99
       {  1, 0,-2,-2, 0,     -10,    0,       0,    0 },   // 100
       {  0,-1, 2, 0, 1,     -10,    0,       0,    0 },   // 101
       {  1, 1, 0,-2, 1,     -10,    0,       0,    0 },   // 102
       {  1, 0,-2, 2, 0,     -10,    0,       0,    0 },   // 103
       {  2, 0, 0, 2, 0,      10,    0,       0,    0 },   // 104
       {  0, 0, 2, 4, 2,     -10,    0,       0,    0 },   // 105
       {  0, 1, 0, 1, 0,      10,    0,       0,    0 }    // 106
      };

        // Variables
     double  l, lp, F, D, Om;
     double  arg, s, c;


        // Mean arguments of luni-solar motion
        //
        //   l   mean anomaly of the Moon
        //   l'  mean anomaly of the Sun
        //   F   mean argument of latitude
        //   D   mean longitude elongation of the Moon from the Sun 
        //   Om  mean longitude of the ascending node
     l  = fmod (  485866.733 + (1325.0*rev +  715922.633)*T
                                + 31.310*T2 + 0.064*T3, rev );
     lp = fmod ( 1287099.804 + (  99.0*rev + 1292581.224)*T
                                -  0.577*T2 - 0.012*T3, rev );
     F  = fmod (  335778.877 + (1342.0*rev +  295263.137)*T
                                - 13.257*T2 + 0.011*T3, rev );
     D  = fmod ( 1072261.307 + (1236.0*rev + 1105601.328)*T
                                -  6.891*T2 + 0.019*T3, rev );
     Om = fmod (  450160.280 - (   5.0*rev +  482890.539)*T
                                +  7.455*T2 + 0.008*T3, rev );

        // Nutation in longitude and obliquity [rad]

     deps = dpsi = 0.0;
     for (int i=0; i<N_coeff; i++)
     {
         arg  =  ( C[i][0] * l
                 + C[i][1] * lp
                 + C[i][2] * F
                 + C[i][3] * D
                 + C[i][4] * Om ) * AS_TO_RAD;
         s = C[i][5] + C[i][6] * T;
         c = C[i][7] + C[i][8] * T;
         if( s != 0.0) dpsi += s * std::sin(arg);
         if( c != 0.0) deps += c * std::cos(arg);
     }

     dpsi = dpsi * AS_TO_RAD * 1.0e-5;
     deps = deps * AS_TO_RAD * 1.0e-5;

   }  // End of method 'ReferenceSystem::NutAngles()'


      /**Transformation from mean to true equator and equinox.
       *
       *@param Mjd_TT    Modified Julian Date (Terrestrial Time)
       *@return          Nutation matrix
       */
   Matrix<double> ReferenceSystem::NutMatrix (double Mjd_TT)
   {

     double dpsi, deps, epsa;

        // convert TT to UTC for getting dPsi and dEps
     CommonTime TT( MJD(Mjd_TT).convertToCommonTime() );
     TT.setTimeSystem(TimeSystem::TT);

     CommonTime UTC( convertTT2UTC(TT) );

        // dPsi, dEps
     double ddpsi = getDPsi(UTC) * AS_TO_RAD;
     double ddeps = getDEps(UTC) * AS_TO_RAD;

        // Mean obliquity of the ecliptic
     epsa = MeanObliquity(Mjd_TT);
//     cout << "epsa = " << epsa << endl;

        // Nutation in longitude and obliquity
     NutAngles (Mjd_TT, dpsi,deps);
     
        // Add adjustments: frame bias, precession-rates, geophysical
     dpsi += ddpsi;
     deps += ddeps;
//     cout << "ddpsi = " << ddpsi << ' ' << "ddeps = " << ddeps << endl;
//     cout << "dpsi  = " << dpsi  << ' ' << "deps  = " << deps  << endl;

        // Transformation from mean to true equator and equinox
     return  rotation(-epsa-deps,1)*rotation(-dpsi,3)*rotation(+epsa,1);

   }


      /**Computation of the equation of the equinoxes.
       *
       *@param  Mjd_TT    Modified Julian Date (Terrestrial Time)
       *@return           Equation of the equinoxes

       * \warning
       * The equation of the equinoxes dpsi*cos(eps) is the right ascension of the
       * mean equinox referred to the true equator and equinox and is equal to the
       * difference between apparent and mean sidereal time.
       */
   double ReferenceSystem::EqnEquinox (double Mjd_TT)
   {
        // Interval between fundamental epoch J2000.0 and given date (JC).
     double t;
     t = (Mjd_TT - MJD_J2000) / 36525.0;

        // Longitude of the mean ascending node of the lunar orbit on the
        // ecliptic, measured from the mean equinox of date.
     double Om;
     Om = Anpm((450160.280 + (-482890.539
             + (7.455 + 0.008 * t) * t) * t) * AS_TO_RAD
             + std::fmod(-5.0 * t, 1.0) * TWO_PI);

        // Nutation components and mean obliquity.
     double dpsi, deps, eps0;
     NutAngles(Mjd_TT, dpsi,deps);
     eps0 = MeanObliquity(Mjd_TT);

     double ee;
     ee = dpsi * std::cos(eps0) + AS_TO_RAD * (0.00264*std::sin(Om) + 0.000063*std::sin(Om+Om));

     return ee;
/*
        // Interval between fundamental epoch J2000.0 and given date (JC).
     double T, T2, T3;
     T  = (Mjd_TT - MJD_J2000) / 36525.0;
     T2 = T * T;
     T3 = T2 * T;

        // Longitude of the mean ascending node of the lunar orbit on the
        // ecliptic, measured from the mean equinox of date.
     double rev = 360.0 * 3600.0;
     double Om;
     Om = fmod (  450160.280 - (   5.0*rev +  482890.539)*T
                                +  7.455*T2 + 0.008*T3, rev );

        // Nutation components and mean obliquity.
     double dpsi, deps, eps0;
     NutAngles(Mjd_TT, dpsi,deps);
     eps0 = MeanObliquity(Mjd_TT);

        // Equation of the equinoxes.
     return  dpsi * std::cos(eps0) + AS_TO_RAD * (0.00264*std::sin(Om) + 0.000063*std::sin(Om+Om));
*/
   }


      /**Greenwich Mean Sidereal Time.
       *
       *@param  Mjd_UT1   Modified Julian Date UT1
       *@return           GMST in [rad]
       */
   double ReferenceSystem::GMST (double Mjd_UT1)
   {
        // Coefficients of IAU 1982 GMST-UT1 model.
     double A, B, C, D;
     A = 24110.54841;
     B = 8640184.812866;
     C = 0.093104;
     D = -6.2e-6;

        // Julian centuries since fundamental epoch.
     double t;
     t = (Mjd_UT1 - MJD_J2000) / 36525.0;

        // Fractional part of JD(UT1), in seconds.
     double f;
     f = 86400.0 * std::fmod(Mjd_UT1, 1.0);

        // GMST at this UT1.
     double gmst;
     gmst = Anp(SEC_TO_RAD * ((A + (B + (C + D * t) * t) * t) + f));

     return gmst;
/*
        // Variables
     double Mjd_0,UT1,T_0,T,gmst;

        // Mean Sidereal Time
     Mjd_0 = floor(Mjd_UT1);
     UT1   = 86400.0*(Mjd_UT1 - Mjd_0);          // [s]
     T_0   = (Mjd_0  -MJD_J2000) / 36525.0;
     T     = (Mjd_UT1-MJD_J2000) / 36525.0;


        // According to the IERS notes 21, the GMST shoule be computed
        // using the formula:
     gmst  = 24110.54841 + 8640184.812866*T_0 + (0.093104-6.2e-6*T_0)*T_0*T_0
             + 1.002737909350795*UT1; // [s]
        // Fraction and integer of gmst
     double Frac, Integer;

        // Get the fraction of gmst
     Frac = modf(gmst/86400.0, &Integer);

     return  TWO_PI*Frac;
*/
   }


      /**Greenwich Apparent Sidereal Time.
       *
       *@param  Mjd_UT1   Modified Julian Date UT1
       *@return           GAST in [rad]
       */
   double ReferenceSystem::GAST (double Mjd_UT1)
   {
         // Conversion from UT1 to TT.
     CommonTime UT1( MJD(Mjd_UT1).convertToCommonTime() );
     UT1.setTimeSystem(TimeSystem::UT1);

     CommonTime UTC( convertUT12UTC(UT1) );

     CommonTime TT( convertUTC2TT(UTC) );

     double Mjd_TT( Epoch(TT).MJD() );

        // Mean obliquity of the ecliptic
     double eps0 = MeanObliquity(Mjd_TT);

     double ddpsi = getDPsi(UTC) * AS_TO_RAD;

         // Greenwich apparent sideral time (IAU 1982/1994).
     double gst = GMST(Mjd_UT1) + EqnEquinox(Mjd_TT) + ddpsi * std::cos(eps0);

     return fmod(gst, TWO_PI);
   }


      /* Transformation from true equator and equinox to Earth equator and
       * Greenwich meridian system.
       *
       *@param Mjd_UT1   Modified Julian Date UT1
       *@return          Greenwich Hour Angle matrix
       */
   Matrix<double> ReferenceSystem::GHAMatrix (double Mjd_UT1)
   {
     return  rotation( GAST(Mjd_UT1),3 );
   }


      /**Transformation from pseudo Earth-fixed to Earth-fixed coordinates
       * for a given date.
       *
       *@param  Mjd_UTC   Modified Julian Date UTC
       *@return           Pole matrix
       */
   Matrix<double> ReferenceSystem::PoleMatrix (double Mjd_UTC)
   {
      CommonTime UTC( MJD(Mjd_UTC).convertToCommonTime() );
      UTC.setTimeSystem(TimeSystem::UTC);

      double xp = getXPole(UTC) * AS_TO_RAD;
      double yp = getYPole(UTC) * AS_TO_RAD;

      return  rotation( -xp, 2) * rotation( -yp, 1);
   }


      /**Transformation from ICRS to ITRS coordinates for a given time.
       *
       *@param  Mjd_UTC   Modified Julian Date UTC
       *@return           Rotation matrix
       */
   Matrix<double> ReferenceSystem::getC2TMatrix(CommonTime& GPST)
   {
      try
      {
            // UTC time
         CommonTime UTC;
         UTC = convertGPST2UTC( GPST );

            // Get the UT1 minus UTC
         double UT1mUTC;
         UT1mUTC = getUT1mUTC( UTC );

            // UT1 time
         CommonTime UT1;
         UT1 = UTC + UT1mUTC;
         UT1.setTimeSystem(TimeSystem::UT1);
         cout << fixed << setprecision(15);
//         cout << "UT1: " << UT1 << endl;

            // Get the TT minus UTC
         double TTmUTC;
         TTmUTC = getTTmUTC( UTC );
         TTmUTC = 32.184 + 33;

            // TT time
         CommonTime TT;
         TT = UTC + TTmUTC;
         TT.setTimeSystem(TimeSystem::TT);
//         cout << "TT: " << TT << endl;

            // Get the MJD of UTC/UT1/TT

         Epoch epoch;

            // MJD of the UTC time
         epoch.set(UTC);
         double MJD_UTC ( epoch.MJD() );
//         cout << "MJD (UTC): " << MJD_UTC << endl;

            // MJD of the UT1 time
         epoch.set(UT1);
         double MJD_UT1 ( epoch.MJD() );
//         cout << "MJD (UT1): " << MJD_UT1 << endl;

            // MJD of the TT time
         epoch.set(TT);
         double MJD_TT  ( epoch.MJD() );
//         cout << "MJD (TT): " << MJD_TT << endl;

            // Now, Compute the transformation Matrix: P/N/Theta/Phi

            // Compute Precession Matrix
            // IAU 1976 Precession 
         P = PrecMatrix( MJD_J2000, MJD(TT).mjd );
//         cout << "P: " << endl << P << endl;

            // Compute Precession Matrix
            // IAU 1980 Nutation
         N = NutMatrix( MJD(TT).mjd );
//         cout << "N: " << endl << N << endl;

            // Rotation matrix about the Celestial Ephemeris Pole
            // Apparent Sidereal Time
            // Note:
            // here we evaluate the equation of the
            // equinoxes with the MJD_UT1 time argument
            // (instead of MJD_TT)
         Theta = GHAMatrix( MJD(UT1).mjd );
//         cout << "Theta: " << endl << Theta << endl;

            // Polar Motion Matrix
            // Transformation from the CEP to the IRP of the ITRS
            // Note:
            // the time argument of polar motion series
            // is not rigorously defined, but any differences
            // are negligible
         Pi = PoleMatrix( MJD(UTC).mjd );
//         cout << "Pi: " << endl << Pi << endl;

            // Get the ICRS to ITRS tranformation matrix
         Matrix<double> C2T = Pi*Theta*N*P;

            // The transformation matrixs are prepared
         isPrepared = true;

            // Return
         return C2T;

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

   } // End of method 'ReferenceSystem::getC2TMatrix ()'


      /**Transformation from ITRS to ICRS coordinates for a given time
       *
       *@param  Mjd_UTC   Modified Julian Date UTC
       *@return           Rotation matrix
       */
   Matrix<double> ReferenceSystem::getT2CMatrix(CommonTime& GPST )
   {

         // Rotation Matrix
      Matrix<double> C2T, T2C;

         // Get rotation from ICRS to ITRS
      C2T = getC2TMatrix(GPST);

         // Get rotation from ICRS to ITRS
      T2C = transpose(C2T);

         // Return
      return T2C;

   } // End of method 'ReferenceSystem::getT2CMatrix()'


      /**Get the precession matrix.
       *
       * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix' 
       * firstly, otherwise, this method will throw 'InvalidRequest'
       */
   Matrix<double> ReferenceSystem::getPrecMatrix( void )
     throw(InvalidRequest)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidRequest("EquationSystem is not prepared"));
      }

         // return
      return P;

   };


      /**Get the Nutation matrix.
       *
       * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix' 
       * firstly, otherwise, this method will throw 'InvalidRequest'
       */
   Matrix<double> ReferenceSystem::getNutMatrix( void )
     throw(InvalidRequest)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidRequest("Transformation matrix are not prepared"));
      }

         // return
      return N;
   }


      /**Get the earth rotation matrix.
       *
       * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix' 
       * firstly, otherwise, this method will throw 'InvalidRequest'
       */
   Matrix<double> ReferenceSystem::getThetaMatrix( void )
     throw(InvalidRequest)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidRequest("Transformation matrix are not prepared"));
      }

         // return
      return Theta;
   }


      /**Get the Polar Motion matrix.
       *
       * \warning You must call 'getC2TMatrix' or, 'getT2CMatrix' 
       * firstly, otherwise, this method will throw 'InvalidRequest'
       */
   Matrix<double> ReferenceSystem::getPiMatrix( void )
     throw(InvalidRequest)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidRequest("Transformation matrix are not prepared"));
      }

         // return
      return Pi;
   }


}  // End of namespace 'gpstk'

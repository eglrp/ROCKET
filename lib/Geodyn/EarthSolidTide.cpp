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

/**
* @file EarthSolidTide.cpp
* Class to do Earth Solid Tide correction
*/

#include <complex>

#include "EarthSolidTide.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{
   // For dC21 and dS21
   // The in-phase (ip) amplitudes and the out-of-phase (op) amplitudes of the
   // corrections for frequency dependence of k21(0), taking the nominal value
   // k21 for the diurnal tides as (0.29830 - i*0.00144). Unitis: 10^-12.
   // see IERS Conventions 2010, Table 6.5a
   const double EarthSolidTide::Argu_C21[48][7]=
   {
//      Amp.    Amp.    l   l'  F   D   Omega
//      (ip)    (op)
      {-0.1,    0,       2,    0,  2,  0,  2},
      {-0.1,    0,       0,    0,  2,  2,  2},
      {-0.1,    0,       1,    0,  2,  0,  1},
      {-0.7,    0.1,     1,    0,  2,  0,  2},
      {-0.1,    0,      -1,    0,  2,  2,  2},
      {-1.3,    0.1,     0,    0,  2,  0,  1},
      {-6.8,    0.6,     0,    0,  2,  0,  2},
      { 0.1,    0,       0,    0,  0,  2,  0},
      { 0.1,    0,       1,    0,  2, -2,  2},
      { 0.1,    0,      -1,    0,  2,  0,  1},
      { 0.4,    0,      -1,    0,  2,  0,  2},
      { 1.3,   -0.1,     1,    0,  0,  0,  0},
      { 0.3,    0,       1,    0,  0,  0,  1},
      { 0.3,    0,      -1,    0,  0,  2,  0},
      { 0.1,    0,      -1,    0,  0,  2,  1},
      {-1.9,    0.1,     0,    1,  2, -2,  2},
      { 0.5,    0,       0,    0,  2, -2,  1},
      {-43.4,   2.9,     0,    0,  2, -2,  2},
      { 0.6,    0,       0,   -1,  2, -2,  2},
      { 1.6,   -0.1,     0,    1,  0,  0,  0},
      { 0.1,    0,      -2,    0,  2,  0,  1},
      { 0.1,    0,       0,    0,  0,  0, -2},
      {-8.8,    0.5,     0,    0,  0,  0, -1},
      {470.9, -30.2,     0,    0,  0,  0,  0},
      { 68.1,  -4.6,     0,    0,  0,  0,  1},
      {-1.6,    0.1,     0,    0,  0,  0,  2},
      { 0.1,    0,      -1,    0,  0,  1,  0},
      {-0.1,    0,       0,   -1,  0,  0, -1},
      {-20.6,  -0.3,     0,   -1,  0,  0,  0},
      { 0.3,    0,       0,    1, -2,  2, -2},
      {-0.3,    0,       0,   -1,  0,  0,  1},
      {-0.2,    0,      -2,    0,  0,  2,  0},
      {-0.1,    0,      -2,    0,  0,  2,  1},
      {-5.0,    0.3,     0,    0, -2,  2, -2},
      { 0.2,    0,       0,    0, -2,  2, -1},
      {-0.2,    0,       0,   -1, -2,  2, -2},
      {-0.5,    0,       1,    0,  0, -2,  0},
      {-0.1,    0,       1,    0,  0, -2,  1},
      { 0.1,    0,      -1,    0,  0,  0, -1},
      {-2.1,    0.1,    -1,    0,  0,  0,  0},
      {-0.4,    0,      -1,    0,  0,  0,  1},
      {-0.2,    0,       0,    0,  0, -2,  0},
      {-0.1,    0,      -2,    0,  0,  0,  0},
      {-0.6,    0,       0,    0, -2,  0, -2},
      {-0.4,    0,       0,    0, -2,  0, -1},
      {-0.1,    0,       0,    0, -2,  0,  0},
      {-0.1,    0,      -1,    0, -2,  0, -2},
      {-0.1,    0,      -1,    0, -2,  0, -1}
   };

   // For dC20
   // Corrections for frequency dependence of k20(0) of the zonal tides due to
   // anelasticity. Units: 10^-12. The nominal value k20 for the zonal tides is
   // taken as 0.30190.
   // see IERS Conventions 2010, Table 6.5b
   const double EarthSolidTide::Argu_C20[21][7]=
   {

//      Amp.  Amp.    l   l'  F   D   Omega
//      (ip)  (op)
      {  16.6,  -6.7,  0,  0,  0,  0,  1 },
      {  -0.1,   0.1,  0,  0,  0,  0,  2 },
      {  -1.2,   0.8,  0, -1,  0,  0,  0 },
      {  -5.5,   4.3,  0,  0, -2,  2, -2 }, 
      {   0.1,  -0.1,  0,  0, -2,  2, -1 },
      {  -0.3,   0.2,  0, -1, -2,  2, -2 },
      {  -0.3,   0.7,  1,  0,  0, -2,  0 },
      {   0.1,  -0.2, -1,  0,  0,  0, -1 },
      {  -1.2,   3.7, -1,  0,  0,  0,  0 },
      {   0.1,  -0.2, -1,  0,  0,  0,  1 },
      {   0.1,  -0.2,  1,  0, -2,  0, -2 },
      {   0.0,   0.6,  0,  0,  0, -2,  0 }, 
      {   0.0,   0.3, -2,  0,  0,  0,  0 },
      {   0.6,   6.3,  0,  0, -2,  0, -2 },
      {   0.2,   2.6,  0,  0, -2,  0, -1 }, 
      {   0.0,   0.2,  0,  0, -2,  0,  0 },
      {   0.1,   0.2,  1,  0, -2, -2, -2 },
      {   0.4,   1.1, -1,  0, -2,  0, -2 },
      {   0.2,   0.5, -1,  0, -2,  0, -1 },
      {   0.1,   0.2,  0,  0, -2, -2, -2 },
      {   0.1,   0.1, -2,  0, -2,  0, -2 }
   };

   // For dC22 and dS22
   // Amplitudes of the corrections for frequency dependence of k22, taking the
   // nominal value k22 for the sectorial tides as (0.30102 - i*0.00130).
   // Units: 10^-12. The corrections are only to the real part.
   // see IERS Conventions 2010, Table 6.5c
   const double EarthSolidTide::Argu_C22[2][6] = 
   {
//        Amp.   l   l'  F   D   Omega
       {  -0.3,  1,  0,  2,  0,  2  },
       {  -1.2,  0,  0,  2,  0,  2  }
   };


      /**
       * Solid tide to normalized earth potential coefficients
       *
       * @param utc     time in UTC
       * @param dC      correction to normalized coefficients dC
       * @param dS      correction to normalized coefficients dS
       */
   void EarthSolidTide::getSolidTide(CommonTime utc, double dC[], double dS[])
   {
       // TT
       CommonTime tt( UTC2TT(utc) );

       // UT1
       CommonTime ut1( UTC2UT1(utc) );

       // C2T Matrix
       Matrix<double> c2t = C2TMatrix(utc);

       // Moon and Sun Position in ECI, unit: m
       Vector<double> rm_eci = J2kPosition(tt, SolarSystem::Moon);
       Vector<double> rs_eci = J2kPosition(tt, SolarSystem::Sun);
//       cout << "rm_eci: " << rm_eci << endl;
//       cout << "rs_eci: " << rs_eci << endl;

       // Moon and Sun Position in ECEF, unit: m
       Vector<double> rm_ecef = c2t * rm_eci;
       Vector<double> rs_ecef = c2t * rs_eci;

       // IERS Conventions 2010, Chapter 6.2
       // The computation of the tidal contributions to the geopotential
       // coefficients is most efficiently done by a three-step procedure.

       /////////////////////////////////////////////////////////////////////////
       //
       //   In step 1, the (2m) part of the tidal potential is evaluated in the
       //   time domain for each m using lunar and solar ephemeris, and the
       //   corresponding changes dC2m and dS2m are computed using frequency
       //   independent nominal values k2m for the respective k2m(0). The
       //   contributions of the degree 3 tides to C3m and S3m through k3m(0)
       //   and also those of the degree 2 tides to C4m and S4m through k2m(+)
       //   may be computed by a similar procedure; they are at the level of
       //   10e-11.
       //
       /////////////////////////////////////////////////////////////////////////

       // love numbers for solid earth tides
       // see IERS Conventions 2010, Table 6.3
       double k20, k21, k22;
       k20  =  0.29525;
       k21  =  0.29470;
       k22  =  0.29801;
       double k20p, k21p, k22p;
       k20p = -0.00087;
       k21p = -0.00079;
       k22p = -0.00057;
       double k30, k31, k32, k33;
       k30  =  0.09300;
       k31  =  0.09300;
       k32  =  0.09300;
       k33  =  0.09400;

       for(int j=2; j<=3; ++j)
       {
          Vector<double> rsm;
          double GSM;

          if(2==j)         // Moon
          {
             GSM = GM_MOON;
             rsm = rm_ecef;
          }
          else if(3==j)    // Sun
          {
             GSM = GM_SUN;
             rsm = rs_ecef;
          }

          // rho, sin(lat), cos(lat), lat
          double rho = norm(rsm);
          double slat = rsm(2)/rho;
          double clat = std::sqrt(1.0-slat*slat);
          double lat = std::atan2(slat, clat);

          // longitude
          double xlon = std::atan2(rsm(1), rsm(0));

          // Pnm
          Vector<double> leg;
          legendre(3, lat, leg);

          // temp
          double temp;
          
          temp = GSM/GM_EARTH * std::pow(RE_EARTH/rho,3);
          // C20, S20
          dC[0] += k20/5.0 * temp * leg(3) * 1.0;
          dS[0] += 0.0;
          // C21, S21
          dC[1] += k21/5.0 * temp * leg(4) * std::cos(xlon);
          dS[1] += k21/5.0 * temp * leg(4) * std::sin(xlon);
          // C22, S22
          dC[2] += k22/5.0 * temp * leg(5) * std::cos(2*xlon);
          dS[2] += k22/5.0 * temp * leg(5) * std::sin(2*xlon);

          temp = GSM/GM_EARTH * std::pow(RE_EARTH/rho,4);
          // C30, S30
          dC[3] += k30/7.0 * temp * leg(6) * 1.0;
          dS[3] += 0.0;
          // C31, S31
          dC[4] += k31/7.0 * temp * leg(7) * std::cos(xlon);
          dS[4] += k31/7.0 * temp * leg(7) * std::sin(xlon);
          // C32, S32
          dC[5] += k32/7.0 * temp * leg(8) * std::cos(2*xlon);
          dS[5] += k32/7.0 * temp * leg(8) * std::sin(2*xlon);
          // C33, S33
          dC[6] += k33/7.0 * temp * leg(9) * std::cos(3*xlon);
          dS[6] += k33/7.0 * temp * leg(9) * std::sin(3*xlon);

          temp = GSM/GM_EARTH * std::pow(RE_EARTH/rho,3);
          // C40, S40
          dC[7] += k20p/5.0 * temp * leg(3) * 1.0;
          dS[7] += 0.0;
          // C41, S41
          dC[8] += k21p/5.0 * temp * leg(4) * std::cos(xlon);
          dS[8] += k21p/5.0 * temp * leg(4) * std::sin(xlon);
          // C42, S42
          dC[9] += k22p/5.0 * temp * leg(5) * std::cos(xlon);
          dS[9] += k22p/5.0 * temp * leg(5) * std::sin(xlon);

       }

       /////////////////////////////////////////////////////////////////////////
       //
       //   Step 2 corrects for the deviations of the k21(0) of several of the
       //   constituent tides of the diurnal band from the constant nominal
       //   value k21 assumed for this band in the first step. Similar
       //   corrections need to be applied to a few of the constituents of the
       //   other two bands also.
       //
       /////////////////////////////////////////////////////////////////////////

       // Doodson arguments
       double BETA[6] = {0.0};
       double FNUT[5] = {0.0};
       DoodsonArguments(ut1, tt, BETA, FNUT);
       double GMST = GMST82(ut1);

       // C20
       // see IERS Conventions 2010, Equation 6.8a
       for(int i=0; i<21; ++i)
       {
          // theta_f
          double theta_f = -(Argu_C20[i][2]*FNUT[0] + Argu_C20[i][3]*FNUT[1]
                           + Argu_C20[i][4]*FNUT[2] + Argu_C20[i][5]*FNUT[3]
                           + Argu_C20[i][6]*FNUT[4]);

          // sine and cosine of theta_f
         double stf = std::sin(theta_f);
         double ctf = std::cos(theta_f);

         // correction
         dC[0] += (Argu_C20[i][0]*ctf - Argu_C20[i][1]*stf)*1e-12;
       }

       // C21, S21
       // see IERS Conventions 2010, Equation 6.8b
       for(int i=0; i<48; ++i)
       {
           // theta_f
           double theta_f = 1*(GMST+PI) - (Argu_C21[i][2]*FNUT[0]
                           + Argu_C21[i][3]*FNUT[1] + Argu_C21[i][4]*FNUT[2]
                           + Argu_C21[i][5]*FNUT[3] + Argu_C21[i][6]*FNUT[4]);

           // sine and cosine of theta_f
           double stf = std::sin(theta_f);
           double ctf = std::cos(theta_f);

           // corrections
           dC[1] += (Argu_C21[i][0]*stf + Argu_C21[i][1]*ctf)*1e-12;
           dS[1] += (Argu_C21[i][0]*ctf - Argu_C21[i][1]*stf)*1e-12;
       }

       // C22, S22
       // see IERS Conventions 2010, Equation 6.8b
       for(int i=0; i<2; ++i)
       {
           // theta_f
           double theta_f = 2*(GMST+PI) - (Argu_C22[i][1]*FNUT[0]
                           + Argu_C22[i][2]*FNUT[1] + Argu_C22[i][3]*FNUT[2]
                           + Argu_C22[i][4]*FNUT[3] + Argu_C22[i][5]*FNUT[4]);

           // sine and cosine of theta_f
           double stf = std::sin(theta_f);
           double ctf = std::cos(theta_f);

           // corrections
           dC[2] += ( Argu_C22[i][0]*ctf)*1e-12;
           dS[2] += (-Argu_C22[i][0]*stf)*1e-12;
       }


       /////////////////////////////////////////////////////////////////////////
       //
       //   Treatment of the permanent tide
       //
       //   It is not needed for tide-free EGM08.
       //
       /////////////////////////////////////////////////////////////////////////

   }

   /** Evaluate the fully normalized associated legendre functions.
    * @param deg     Desired degree
    * @param lat     Geocentric latitude in radians
    * @param leg0    Fully normalized associated legendre functions (fnALF)
    *
    * Ref: E.Fantino, J Geod(2009), Methods of harmonic synthesis for global
    * geopotential models and their first-, second- and third-order gradients
    */ 
   void EarthSolidTide::legendre(const int&      deg,
                                 const double&   lat,
                                 Vector<double>& leg0)
   {
      // sine, cosine and tangent of latitude
      double slat, clat, tlat;
      slat = std::sin(lat);
      clat = std::cos(lat);
      tlat = std::tan(lat);

      int size = index(deg, deg);
      leg0.resize(size, 0.0);

      leg0(0) = 1.0;     // P00

//      cout << "size: " << size << endl;
//      cout << "sin(lat): " << slat << endl;
//      cout << "cos(lat): " << clat << endl;
//      cout << "tan(lat): " << tlat << endl;
      
      // first, the leg0
      for(int n=1; n<=deg; ++n)
      {
         // Kronecker's delta
         double delta = ( (1==n) ? 1.0 : 0.0 );

         for(int m=0; m<=n; ++m)
         {
            // sectorials
            if(m == n)
            {
               double fn = std::sqrt((1.0+delta)*(2*n+1.0)/(2*n));

               // P(n,m) = fn * cos(lat) * P(n-1,m-1)
               leg0( index(n,m)-1 ) = fn*clat*leg0( index(n-1,m-1)-1 );
            }
            // zonals and tesserals 
            else
            {
               double gnm = std::sqrt((2*n+1.0)*(2*n-1.0)/(n+m)/(n-m));
               double hnm = std::sqrt((2*n+1.0)*(n-m-1.0)*(n+m-1.0)/(2*n-3.0)/(n+m)/(n-m));
               if(m == n-1)
               {
                  // P(n,m) = gnm * sin(lat) * P(n-1,m)
                  leg0( index(n,m)-1 ) = gnm*slat*leg0( index(n-1,m)-1 );
               }
               else
               {
                  // P(n,m) = gnm * sin(lat) * P(n-1,m) - hnm * P(n-2,m)
                  leg0( index(n,m)-1 ) = gnm*slat*leg0( index(n-1,m)-1 )
                                       - hnm*leg0( index(n-2,m)-1 );
               }
            }
         }
      }

//      cout << "leg0: " << endl;
//      for(int i=0; i<size; i++)
//      {
//         cout << setw(12) << setprecision(8) << leg0(i) << endl;
//      }

   }   // End of method "EarthSolidTide::legendre()"


}	// End of namespace 'gpstk'

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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
* @file EarthSolidTide.cpp
* Class to do Earth Solid Tide correction.
*
*/

#include "EarthSolidTide.hpp"
#include "GNSSconstants.hpp"
#include "Legendre.hpp"

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


      /** Solid tide to normalized earth potential coefficients
       *
       * @param utc     time in UTC
       * @param CS      normalized earth potential coefficients
       */
   void EarthSolidTide::getSolidTide(CommonTime utc, Matrix<double>& CS)
   {

       // TT
       CommonTime tt( pRefSys->UTC2TT(utc) );

       // UT1
       CommonTime ut1( pRefSys->UTC2UT1(utc) );

       // transformation matrix from ICRS to ITRS
       Matrix<double> c2t( pRefSys->C2TMatrix(utc) );

       // moon and sun position and velocity in ICRS, unit: km, km/day
       double jd = JulianDate(tt).jd;
       double rv_moon[6] = {0.0};
       pSolSys->computeState(jd,
                             SolarSystem::Moon,
                             SolarSystem::Earth,
                             rv_moon);
       double rv_sun[6] = {0.0};
       pSolSys->computeState(jd,
                             SolarSystem::Sun,
                             SolarSystem::Earth,
                             rv_sun);

       // moon and sun position in ICRS, unit: m
       Vector<double> rm_icrs(3,0.0);
       rm_icrs(0) = rv_moon[0];
       rm_icrs(1) = rv_moon[1];
       rm_icrs(2) = rv_moon[2];
       rm_icrs *= 1000.0;
       Vector<double> rs_icrs(3,0.0);
       rs_icrs(0) = rv_sun[0];
       rs_icrs(1) = rv_sun[1];
       rs_icrs(2) = rv_sun[2];
       rs_icrs *= 1000.0;
//       cout << "rm_icrs: " << rm_icrs << endl;
//       cout << "rs_icrs: " << rs_icrs << endl;

       // moon and sun position in ITRS, unit: m
       Vector<double> rm_itrs = c2t * rm_icrs;
       Vector<double> rs_itrs = c2t * rs_icrs;

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

       // indexes for degree = 2
       int id20, id21, id22;
       id20 = indexTranslator(2,0) - 1;
       id21 = indexTranslator(2,1) - 1;
       id22 = indexTranslator(2,2) - 1;

       // indexes for degree = 3
       int id30, id31, id32, id33;
       id30 = indexTranslator(3,0) - 1;
       id31 = indexTranslator(3,1) - 1;
       id32 = indexTranslator(3,2) - 1;
       id33 = indexTranslator(3,3) - 1;

       // indexes for degree = 4
       int id40, id41, id42;
       id40 = indexTranslator(4,0) - 1;
       id41 = indexTranslator(4,1) - 1;
       id42 = indexTranslator(4,2) - 1;


       for(int j=2; j<=3; ++j)
       {
          Vector<double> rsm;
          double GSM;

          if(2==j)         // Moon
          {
             GSM = GM_MOON;
             rsm = rm_itrs;
          }
          else if(3==j)    // Sun
          {
             GSM = GM_SUN;
             rsm = rs_itrs;
          }

          // rho, sin(lat), cos(lat), lat
          double rho = norm(rsm);
          double slat = rsm(2)/rho;
          double clat = std::sqrt(1.0-slat*slat);
          double lat = std::atan2(slat, clat);

          // longitude
          double xlon = std::atan2(rsm(1), rsm(0));

          // Pnm
          Vector<double> leg0, leg1, leg2;
          legendre(3, lat, leg0, leg1, leg2, 0);

          // temp
          double temp;
          temp = GSM/GM_EARTH * std::pow(RE_EARTH/rho,3);

          // C20, S20
          CS(id20, 0) += k20/5.0 * temp * leg0(3) * 1.0;
          CS(id20, 1) += 0.0;
          // C21, S21
          CS(id21, 0) += k21/5.0 * temp * leg0(4) * std::cos(xlon);
          CS(id21, 1) += k21/5.0 * temp * leg0(4) * std::sin(xlon);
          // C22, S22
          CS(id22, 0) += k22/5.0 * temp * leg0(5) * std::cos(2*xlon);
          CS(id22, 1) += k22/5.0 * temp * leg0(5) * std::sin(2*xlon);

          temp = GSM/GM_EARTH * std::pow(RE_EARTH/rho,4);
          // C30, S30
          CS(id30, 0) += k30/7.0 * temp * leg0(6) * 1.0;
          CS(id30, 1) += 0.0;
          // C31, S31
          CS(id31, 0) += k31/7.0 * temp * leg0(7) * std::cos(xlon);
          CS(id31, 1) += k31/7.0 * temp * leg0(7) * std::sin(xlon);
          // C32, S32
          CS(id32, 0) += k32/7.0 * temp * leg0(8) * std::cos(2*xlon);
          CS(id32, 1) += k32/7.0 * temp * leg0(8) * std::sin(2*xlon);
          // C33, S33
          CS(id33, 0) += k33/7.0 * temp * leg0(9) * std::cos(3*xlon);
          CS(id33, 1) += k33/7.0 * temp * leg0(9) * std::sin(3*xlon);

          temp = GSM/GM_EARTH * std::pow(RE_EARTH/rho,3);
          // C40, S40
          CS(id40, 0) += k20p/5.0 * temp * leg0(3) * 1.0;
          CS(id40, 1) += 0.0;
          // C41, S41
          CS(id41, 0) += k21p/5.0 * temp * leg0(4) * std::cos(xlon);
          CS(id41, 1) += k21p/5.0 * temp * leg0(4) * std::sin(xlon);
          // C42, S42
          CS(id42, 0) += k22p/5.0 * temp * leg0(5) * std::cos(xlon);
          CS(id42, 1) += k22p/5.0 * temp * leg0(5) * std::sin(xlon);

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
       pRefSys->DoodsonArguments(ut1, tt, BETA, FNUT);
       double GMST = pRefSys->GMST06(ut1, tt);

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
         
         CS(id20, 0) += (Argu_C20[i][0]*ctf - Argu_C20[i][1]*stf)*1e-12;
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
           CS(id21, 0) += (Argu_C21[i][0]*stf + Argu_C21[i][1]*ctf)*1e-12;
           CS(id21, 1) += (Argu_C21[i][0]*ctf - Argu_C21[i][1]*stf)*1e-12;
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
           CS(id22, 0) += ( Argu_C22[i][0]*ctf)*1e-12;
           CS(id22, 1) += (-Argu_C22[i][0]*stf)*1e-12;
       }


       /////////////////////////////////////////////////////////////////////////
       //
       //   Treatment of the permanent tide
       //
       //   It does not need to do permanent tide correction for tide-free EGM08.
       //
       /////////////////////////////////////////////////////////////////////////

   }  // End of method 'EarthSolidTide::getSolidTide()'


}	// End of namespace 'gpstk'

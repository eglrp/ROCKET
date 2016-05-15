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
       * @param MJD_UTC UTC in MJD
       * @param dC      correction to normalized coefficients dC
       * @param dS      correction to normalized coefficients dS
       */
   void EarthSolidTide::getSolidTide(double MJD_UTC, double dC[], double dS[])
   {
       // UTC
       CommonTime UTC( MJD(MJD_UTC).convertToCommonTime() );
       UTC.setTimeSystem(TimeSystem::UTC);

       // TT
       CommonTime TT( UTC2TT(UTC) );

       // UT1
       CommonTime UT1( UTC2UT1(UTC) );

       // C2T Matrix
       Matrix<double> E = C2TMatrix(UTC);

       // Moon and Sun Position in ECI, m
//       Vector<double> moonReci = J2kPosition(TT, SolarSystem::Moon);
//       Vector<double> sunReci = J2kPosition(TT, SolarSystem::Sun);
       Vector<double> moonReci = MoonJ2kPosition(TT);
       Vector<double> sunReci = SunJ2kPosition(TT);

       // Moon and Sun Position in ECEF, m
       Vector<double> moonR = E * moonReci;
       Vector<double> sunR = E * sunReci;

       Position moonP(moonR(0),moonR(1),moonR(2));
       Position sunP(sunR(0),sunR(1),sunR(2));

       double r_sun, phi_sun, lamda_sun;
       r_sun = norm(sunR);
       phi_sun = sunP.getGeocentricLatitude()*DEG_TO_RAD;
       lamda_sun = sunP.getLongitude()*DEG_TO_RAD;

       double r_lunar, phi_lunar, lamda_lunar;
       r_lunar = norm(moonR);
       phi_lunar = moonP.getGeocentricLatitude()*DEG_TO_RAD;
       lamda_lunar = moonP.getLongitude()*DEG_TO_RAD;

       // reference bern 5 TIDPT2.f
       /*
          PERTURBING ACCELERATION DUE TO TIDES CORRESPONDING TO IERS STANDARDS 2003.
          STEP 1 CORRECTIONS OF SOLID EARTH TIDES INCLUDED,
          STEP 2 ONLY TERM DUE TO K1. SOLID EARTH POLE TIDES INCLUDED
          OCEAN TIDE TERMS UP TO N=M=4 INCLUDED
       */

       /*       IERS2003,  P60
       Elastic Earth           Anelastic Earth
       n m     knm     k+nm    Reknm    Imknm    k+nm
       2 0  0.29525  0.00087  0.30190  0.00000  0.00089
       2 1  0.29470  0.00079  0.29830  0.00144  0.00080
       2 2  0.29801  0.00057  0.30102  0.00130  0.00057
       3 0  0.093
       3 1  0.093
       3 2  0.093
       3 3  0.094
       */
       complex<double> k[10] =      // Anelastic Earth
       {
           complex<double>(0.30190, 0.0),          // 20
           complex<double>(0.29830,-0.00144),      // 21
           complex<double>(0.30102,-0.00130),      // 22
           complex<double>(0.093, 0.0),            // 30
           complex<double>(0.093, 0.0),            // 31
           complex<double>(0.093, 0.0),            // 32
           complex<double>(0.094, 0.0),            // 33
           complex<double>(-0.00089, 0.0),         // k+ 20
           complex<double>(-0.00080, 0.0),         // k+ 21
           complex<double>(-0.00057, 0.0)          // k+ 22
       };

       complex<double> res[7];

       //----------------------------------------------------------------------
       // The first step of the computation ,refer to "IERS conventions 2003" P59
       // Each iteration for dC[n,m] and dS[n,m]
       for(int n=2;n<=3;n++)
       {
           for(int m=0;m<=n;m++)
           {
               int index = n * n - 2 * n + m;   //index in the returning value array

               double Nnm = normFactor( n, m ); //normalization coefficents of degree n and order m

               // Pnm: normalized Legendre polynomials of degress n and order m
               // 0 for sun and 1 for lunar each
               double sunPnm  = Nnm * legendrePoly( n, m, std::sin( phi_sun) );
               double moonPnm  = Nnm * legendrePoly( n, m, std::sin( phi_lunar) );

               double sunTemp = (GM_SUN/GM_EARTH)*std::pow(RE_EARTH/r_sun,n+1) * sunPnm;
               double moonTemp = (GM_MOON/GM_EARTH)*std::pow(RE_EARTH/r_lunar,n+1)*moonPnm;

               // Exp(-m*lamda*i) for sun and lunar each
               complex<double> c_sun   = complex<double>( std::cos( - m * lamda_sun ), std::sin( - m * lamda_sun ) );
               complex<double> c_lunar = complex<double>( std::cos( - m * lamda_lunar ), std::sin( - m * lamda_lunar ) );

               res[index] =  sunTemp * c_sun + moonTemp * c_lunar;

               dC[index]  =  (k[index]*res[index]).real()/(2.0*n+1.0);
               dS[index]  = -(k[index]*res[index]).imag()/(2.0*n+1.0);

           }  // 'for(int m=0;m<=n;m++)'

       }  // 'for(int n=2;n<=3;n++)'

       // The correction of dC[4,i] and dS[4,i](i=0,1,2) produced by degree 2 tide
       // The only difference from the above dC[2,i] and dS[2,i] is value of k replaced by k+
       for(int n = 0; n <= 2; n ++ )
       {
           int index   = 2 * 2 - 2 * 2 + n;                     // liuwk
           complex<double> c_temp   = k[n+7 ] * res[ index ];   // liuwk
           dC[7+n] = c_temp.real() / 5.0;
           dS[7+n] =-c_temp.imag() / 5.0;
       }


       //-------------------------------------------------------------
       // The second step

       //   COMPUTE DOODSON'S FUNDAMENTAL ARGUMENTS (BETA)
       double BETA[6] = {0.0};
       double Dela[5] = {0.0};
       DoodsonArguments(UT1, TT, BETA, Dela);
       double GMST = GMST82(UT1);

       for(int i=0;i<48;i++)
       {
           // Computation of thet_f
           double thet_f = 1*(GMST+PI)-(Argu_C21[i][2]*Dela[0]+Argu_C21[i][3]*Dela[1]+Argu_C21[i][4]*Dela[2]+Argu_C21[i][5]*Dela[3]+Argu_C21[i][6]*Dela[4]);

           double t_s = std::sin(thet_f);
           double t_c = std::cos(thet_f);

           // Resulted from formula 5b in chapter 6.1
           dC[1] += ( (Argu_C21[i][0]*t_s+Argu_C21[i][1]*t_c)*1e-12 );
           dS[1] += ( (Argu_C21[i][0]*t_c-Argu_C21[i][1]*t_s)*1e-12 );
       }

       for(int i=0;i<2;i++)
       {
           // Input the computation of thet_f
           double thet_f = 2*(GMST+PI)-(Argu_C22[i][1]*Dela[0]+Argu_C22[i][2]*Dela[1]+Argu_C22[i][3]*Dela[2]+Argu_C22[i][4]*Dela[3]+Argu_C22[i][5]*Dela[4]);

         double t_s = std::sin(thet_f);
         double t_c = std::cos(thet_f);

         // Resulted from formula 5b in chapter 6.1
         // The corrections are only to the real part.
         dC[2] += ( ( Argu_C22[i][0]*t_c)*1e-12 );
         dS[2] += ( (-Argu_C22[i][0]*t_s)*1e-12 );
      }


      for(int i=0;i<21;i++)
      {
         // Input the computation of thet_f
         double thet_f = -(Argu_C20[i][2]*Dela[0]+Argu_C20[i][3]*Dela[1]+Argu_C20[i][4]*Dela[2]+Argu_C20[i][5]*Dela[3]+Argu_C20[i][6]*Dela[4]);

         double t_s = std::sin(thet_f);
         double t_c = std::cos(thet_f);

         // Resulted from formula 5a in chapter 6.1
         // Modified, 05.12.2009
         //         dC[0] += ( ( Argu_C20[i][0] * t_c - Argu_C20[i][1] * t_s ) * 1e-12 );
         dC[0]+=((Argu_C20[i][0]*t_c+Argu_C20[i][1]*t_s)*1e-12);
      }


      //--------------------------------------------------------------------------------
      // the third step
      // 
      //C REMOVE PREMANENT TIDE FROM C02 (FOR JGM-3 NOT FOR GEM-T3)
      //C ---------------------------------------------------------
      //IF (IZTID.EQ.1) CPOT(4)=CPOT(4)+1.3914129D-8*K20
   
   }

   // Nnm IERS2003 P60
   double EarthSolidTide::normFactor(int n, int m) 
   {
      // The input should be n >= m >= 0

      double fac(1.0);
      for(int i = (n-m+1); i <= (n+m); i++)
      {
         fac = fac * double(i);
      }

      double delta  = (m == 0) ? 1.0 : 0.0;

      double num = (2.0 * n + 1.0) * (2.0 - delta);

      // We should make sure fac!=0, but it won't happen on the case,
      // so we just skip handling it
      double out = std::sqrt(num/fac);
      
      return out;

   }  // End of method 'EarthSolidTide::normFactor'
   
  
      //  Legendre polynomial
   double EarthSolidTide::legendrePoly(int n, int m, double u)
   {
      // reference:Satellite Orbits Montenbruck. P66
      if(0==n && 0==m)
      {
         return 1.0;
      }
      else if(m==n)
      {
         return (2.0*m-1.0)*std::sqrt(1.0-u*u)*legendrePoly(n-1,m-1,u);
      }
      else if(n==m+1)
      {
         return (2.0*m+1)*u*legendrePoly(m,m,u);
      }
      else
      {
         return ((2.0*n-1.0)*u*legendrePoly(n-1,m,u)-(n+m-1.0)*legendrePoly(n-2,m,u))/(n-m);
      }
      
   }  // End of method 'EarthSolidTide::legendrePoly()'


   void EarthSolidTide::test()
   {   
      cout<<"testing solid tide"<<endl;
      // debuging
      double mjdUtc = 2454531 + 0.49983796296296296 - 2400000.5;
      double dc[10]={0.0},ds[10]={0.0};
      getSolidTide(mjdUtc,dc,ds);


   }	// End of method 'EarthSolidTide::test()'


}	// End of namespace 'gpstk'

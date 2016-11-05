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
//  Kaifa Kuang - Wuhan University . 2015
//
//============================================================================

/**
 * @file ROCKPressure.cpp
 * Class to do ROCK Pressure calculation.
 */

#include "ROCKPressure.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{

      /** Compute acceleration (and related partial derivatives) of ROCK
       *  Pressure.
       * @param utc     time in UTC
       * @param rb      earth body
       * @param sc      spacecraft
       */
   void ROCKPressure::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
       // get current satellite block
       const string block = sc.getCurrentBlock();

       // get current satellite mass, unit: kg
       const double mass = sc.getCurrentMass();

       // get srp coefficients
       double Gx(0.0),Gy(0.0),Gz(0.0);
       Gx= srpCoeff(0); Gy = srpCoeff(1); Gz = srpCoeff(2);

       da_dSRP.resize(3,3,0.0);

       // TT
       double tt = JulianDate(pRefSys->UTC2TT(utc)).jd;

       // sun position and velocity in ICRS, unit: km, km/day
       double rv_sun[6] = {0.0};
       pSolSys->computeState(tt,
                             SolarSystem::Sun,
                             SolarSystem::Earth,
                             rv_sun);

       // moon position and velocity in ICRS, unit: km, km/day
       double rv_moon[6] = {0.0};
       pSolSys->computeState(tt,
                             SolarSystem::Moon,
                             SolarSystem::Earth,
                             rv_moon);

       // sun position in ICRS, unit: m
       Vector<double> r_sun(3,0.0);
       r_sun(0) = rv_sun[0];
       r_sun(1) = rv_sun[1];
       r_sun(2) = rv_sun[2];
       r_sun *= 1000.0;

       // moon position in ICRS, unit: m
       Vector<double> r_moon(3,0.0);
       r_moon(0) = rv_moon[0];
       r_moon(1) = rv_moon[1];
       r_moon(2) = rv_moon[2];
       r_moon *= 1000.0;

       // satellite position in ICRS, unit: m
       Vector<double> r_sat = sc.getCurrentPos();

       // satellite velocity in ICRS, unit: m/s
       Vector<double> v_sat = sc.getCurrentVel();

       // satellite position wrt sun, unit: m
       Vector<double> r_sun2sat = r_sat - r_sun;

        /// ROCK-T model, GAMIT 10.5 ertorb.f

       // compute unit vectors
       Vector<double> rvec = r_sat / norm(r_sat);       // +R direction
       Vector<double> vvec = v_sat / norm(v_sat);       // +T direction
       Vector<double> zvec = -rvec;                     // +Z direction
       Vector<double> ssvec = r_sun2sat / norm(r_sun2sat);// -D direction
       Vector<double> esvec = r_sun / norm(r_sun);      //

       Vector<double> hvec = cross(vvec, zvec);         // +N direction
       hvec = normalize(hvec);
       Vector<double> yvec = cross(rvec, ssvec);        // +Y direction
       yvec = normalize(yvec);
       Vector<double> bvec = cross(yvec, ssvec);        // +B direction
       bvec = normalize(bvec);
       Vector<double> xvec = cross(rvec, yvec);         // +X direction
       xvec = normalize(xvec);

       // compute the B-angle between the sun and the SV-BODY +Z axis for ROCK4
       // also same as sun-satellite-earth angle
       double anga = dot(ssvec,rvec);
       double angb = std::acos(anga);
       double ang2b = angb + angb;
       double ang3b = angb + ang2b;
       double ang4b = ang2b + ang2b;
       double ang5b = ang3b + ang2b;
       double ang7b = ang5b + ang2b;

       // compute the distance factor for the radiation force
       double dmag2 = norm(r_sun2sat) * norm(r_sun2sat);
       double au2 = AU * AU;
       double distfct = au2/dmag2;

       double d0=0.0, xt=0.0, zt=0.0;


       if(block == "I")  // BLOCK-I
       {
           d0 = 4.54e-5 / mass;
           xt = (-4.55*std::sin(angb)+0.08*std::sin(ang2b+0.9)-0.06*std::cos(ang4b+0.08)+0.08)*1e-5/mass;
           zt = (-4.54*std::cos(angb)+0.20*std::sin(ang2b-0.3)-0.03*std::sin(ang4b))*1e-5/mass;
       }
       else if(block == "II"
            || block == "IIA")  // BLOCK-II or BLOCK-IIA
       {
           d0 = 8.695e-5 / mass;
           xt = (-8.96*std::sin(angb)+0.16*std::sin(ang3b)+0.10*std::sin(ang5b)-0.07*std::sin(ang7b))*1e-5/mass;
           zt = -8.43e-5*std::cos(angb)/mass;
       }
       else if(block == "IIR"
            || block == "IIR-A"
            || block == "IIR-B"
            || block == "IIR-M")   // BLOCK-IIR
       {
           d0 = 11.15e-5 / mass;
//           xt = (-11.0*std::sin(angb) - 0.2*std::sin(ang3b) + 0.2*std::sin(ang5b))*1e-5 / mass;
//           zt = (-11.3*std::cos(angb) + 0.1*std::cos(ang3b) + 0.2*std::cos(ang5b))*1e-5 / mass;
           xt = (-11.0*std::sin(angb) + 0.2*std::sin(ang3b) - 0.2*std::sin(ang5b))*1e-5 / mass;
           zt = (-11.3*std::cos(angb) - 0.1*std::cos(ang3b) - 0.2*std::cos(ang5b))*1e-5 / mass;
       }
       // For IIF, multiply IIR constant by 1.5 (from MIT IGS runs) to get the
       // direct coefficient ~1.0. Since the mass is 1.416 times larger, this
       // implies that the effective cross-sectional area is 2.12 times larger,
       // Note: no xt or zt terms yet available for IIFs.
       else if(block == "IIF") // BLOCK-IIF
       {
           d0 = 16.7e-5 / mass;
       }
       else
       {
           cout << "Unknown block number for radiation scaling!" << endl;
       }

       // shadow factor
       double lambda(1.0);
       lambda = getShadowFunction(r_sat, r_sun, r_moon, SM_CONICAL);

       // srp acceleration
       a = lambda * (Gx*xt*xvec + Gy*d0*yvec + Gz*zt*zvec) * distfct;
//       a = (lambda*Gx*xt*xvec + Gy*d0*yvec + lambda*Gz*zt*zvec) * distfct;

/*
       // For ROCK4 model: SV-BODY (xyz) parameterization with ROCK4 (x/z)
       // terms added (original ROCK4)
       //   radcon(0): [XRAD]: coeff. of (radiation) in sv (+) x-axis direction
       //   (nominally 0.0 to -1.0)
       //   radcon(1): [YRAD]: coeff. of (radiation) in sv (+) y-axis direction
       //   (nominally 0.0)
       //   radcon(2): [ZRAD]: coeff. of (radiation) in sv (+) z-axis direction
       //   (nominally 0.0 to -1.0)
       Vector<double> radcon(3,0.0);

       Vector<double> radprs(3,0.0);
       for(int i=0; i<3; i++)
       {
           radprs[i] = lambda * (Gx * xt * xvec(i) + Gy * d0 * yvec(i) + Gz * zt * zvec(i)) * distfct;
       }

       a = radprs;
*/

        /// Partials of acceleration wrt satellite position, velocity and SRP
        /// parameters

       // first, get the partials of unit vectors wrt satellite position vector
       // ex = xvec, ey = yvec, ez = zvec, x = r_EarthSat
//       Matrix<double> dez_dr(3,3,0.0), dey_dr(3,3,0.0), dex_dr(3,3,0.0);

//       Matrix<double> I = ident<double>(3);

//       dez_dr = -(I - outer(zvec,zvec)) / norm(r_sat);
//       dey_dr = (I - outer(yvec,yvec)) * skewSymm(r_sun) / norm(cross(r_sat,r_sun));
//       dex_dr = (I - outer(xvec,xvec))
//              * (2.0*outer(r_sun,r_sat) - dot(r_sun,r_sat)*I - outer(r_sat,r_sun))
//              / norm(cross(cross(r_sat,r_sun), r_sat));

       // then, get the partials of acceleration wrt satellite position vector
       da_dr.resize(3,3,0.0);
/*
       for(int i=0; i<3; i++)
           for(int j=0; j<3; j++)
           {
               da_dr(i,j) = lambda * (xt * dex_dr(i,j) + 0.0 * d0 * dey_dr(i,j) + zt * dez_dr(i,j)) * distfct;
           }
*/
       // then, get the partials of acceleration wrt satellite velocity vector
       da_dv.resize(3,3,0.0);


       // last, get the partials of acceleration wrt scale factors of ROCK-T SRP
       // model
       for(int i=0; i<3; ++i)
       {
           da_dSRP(i,0) = lambda * xt * xvec(i) * distfct;
           da_dSRP(i,1) = lambda * d0 * yvec(i) * distfct;
           da_dSRP(i,2) = lambda * zt * zvec(i) * distfct;
       }

   }  // End of method 'ROCKPressure::doCompute()'

}  // End of namespace 'gpstk'

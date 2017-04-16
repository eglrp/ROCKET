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
 * @file CODEPressure.cpp
 * Class to do CODE Pressure calculation.
 */

#include "CODEPressure.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{

      /** Compute acceleration (and related partial derivatives) of CODE
       *  Pressure.
       * @param utc     time in UTC
       * @param rb      earth body
       * @param sc      spacecraft
       */
   void CODEPressure::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
      // get satellite block type
      const string type = sc.getBlockType();

      // get satellite mass, unit: kg
      const double mass = sc.getMass();

      // get srp coefficients
      double D0(0.0), Dc(0.0), Ds(0.0);
      double Y0(0.0), Yc(0.0), Ys(0.0);
      double B0(0.0), Bc(0.0), Bs(0.0);

      if(9 == srpCoeff.size())
      {
         D0 = srpCoeff(0); Dc = srpCoeff(1); Ds = srpCoeff(2);
         Y0 = srpCoeff(3); Yc = srpCoeff(4); Ys = srpCoeff(5);
         B0 = srpCoeff(6); Bc = srpCoeff(7); Bs = srpCoeff(8);

         da_dSRP.resize(3,9,0.0);
      }
      else if(5 == srpCoeff.size())
      {
         D0 = srpCoeff(0);
         Y0 = srpCoeff(1);
         B0 = srpCoeff(2); Bc = srpCoeff(3); Bs = srpCoeff(4);

         da_dSRP.resize(3,5,0.0);
      }

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
      Vector<double> r_sat(3,0.0);
      r_sat = sc.getPosition();

      // satellite velocity in ICRS, unit: m/s
      Vector<double> v_sat(3,0.0);
      v_sat = sc.getVelocity();

      // satellite position wrt sun in ICRS, unit: m
      Vector<double> r_sunsat(3,0.0);
      r_sunsat = r_sat - r_sun;

//      cout << "SV wrt Earth: " << r_sat << endl;
//      cout << "SV wrt Sun: " << r_sunsat << endl;
//      cout << "Earth wrt Sun: " << -r_sun << endl;

      /// CODE model, GAMIT 10.5 ertorb.f

      // compute unit vectors
      //   rvec    points from the earth to the satellite (= -zvec)
      //   vvec    points in the direction of the satellite's motion
      //   zvec    points from the satellite to the earth (= -rvec)
      //   ssvec   points from the sun to the satellite and defines the sun or
      //           direct (D-) axis; the sign is consistent with Fleigel et
      //           al.[1992] but opposite to Springer et al.[1998]
      //   esvec   points from the earth to the sun (= approx. -ssvec)
      //
      //   hvec    is the orbit normal in the direction the angular momentum
      //           (R X V) "up in a RH sense"
      //   yvec    is the satellite y-axis, which completes a
      //           right-hand-system with xvec and zvec (viz zvec = xvec x
      //           yvec); it points along the axis of the solar panels and can
      //           be formed from rvec (or zvec) and ssvec: yvec = rvec x
      //           ssvec or yvec = ssvec x zvec. Note that Springer et
      //           al.[1998] define the y-axis with opposite sign, deriving it
      //           by taking z as the primary ("x") axis.
      //   bvec    completes an orthogonal right-hand-system with D and Y in
      //           the CODE models. Springer et al.[1998] derive the from D x
      //           Y, which is our -ssvec x -yvec, or equivalently, ssvec x
      //           yvec in our derivation. However, we define it as yvec x
      //           ssvec (misunderstanding the sign convention in Beutler et
      //           al.[1994]). With this scheme, out D/Y/B system is
      //           left-handed and all three axes have sign opposite to that
      //           given by Beutler et al.[1994] and Springer et al.[1998].
      //   xvec    is the satellite x-axis, which points toward the half plane
      //           that contains the sun. N.B. Kouba [2009] says that for IIR
      //           satellites x-axis points away from the sun.
      //   beta    is the angle between esvec and spvec, positive up (toward
      //           hvec), and thus the angle between the sun and the orbital
      //           plane, or the sun's latitude in the orbital frame
      //   u       is the angle between the satellite and node in the orbital
      //           plane, thus it's argument of latitude.
      //   u0      is the angle between the sun's projection onto the orbital
      //           plane (spvec) and the satellite's node.
      //   u-u0    is the elongation of the satellite from the sun's
      //           projection in the orbital plane.
      Vector<double> rvec = r_sat / norm(r_sat);       // +R direction
      Vector<double> vvec = v_sat / norm(v_sat);       // +T direction
      Vector<double> zvec = -rvec;                     // +Z direction
      Vector<double> ssvec = r_sunsat / norm(r_sunsat);// -D direction
      Vector<double> esvec = r_sun / norm(r_sun);      //

      Vector<double> hvec = cross(rvec, vvec);         // +N direction
      hvec = normalize(hvec);
      Vector<double> yvec = cross(rvec, ssvec);        // +Y direction
      yvec = normalize(yvec);
      Vector<double> bvec = cross(yvec, ssvec);        // +B direction
      bvec = normalize(bvec);
      Vector<double> xvec = cross(rvec, yvec);         // +X direction
      xvec = normalize(xvec);

      // compute the B-angle between the sun and the orbital plane for CODE

      // beta is the angle ( [-90,+90] ) of the Sun above (+) the orbital
      // plane
      double beta = PI / 2.0 - std::acos(dot(hvec,-esvec));

      // z-axis is (0,0,1) in ICRS
      Vector<double> zaxis(3,0.0); zaxis(2) = 1.0;
      // get the node direction
      Vector<double> nvec = cross(zaxis, hvec);
      nvec = normalize(nvec);
      // then get a perpendicular to the orbit normal and sun (will be in
      // orbit plane)
      Vector<double> savec = cross(hvec, esvec);
      savec = normalize(savec);
      // finally get the projection of the sun in the orbit plane
      Vector<double> spvec = cross(savec, hvec);
      spvec = normalize(spvec);

      // temp Vector
      Vector<double> temp(3,0.0);

      // u is the angle from the node to the satellite direction, thus it's
      // argument of latitude
      double cosu = dot(nvec, rvec);
      temp = cross(nvec, rvec);
      double sinu = (dot(temp,hvec)>0.0) ? norm(temp) : -norm(temp);
      double u = std::atan2(sinu, cosu);

      // compute the distance factor for the radiation force
      double dmag2 = norm(r_sunsat) * norm(r_sunsat);
      double au2 = AU * AU;
      double distfct = au2/dmag2;

      // unit: m/s^2
      double d0=0.0;
      if(type == "I")
      {
         d0 = 4.54e-5 / mass;
      }
      else if(type == "II"
           || type == "IIA")
      {
         d0 = 8.695e-5 / mass;
      }
      else if(type == "IIR"
           || type == "IIR-A"
           || type == "IIR-B"
           || type == "IIR-M")
      {
         d0 = 11.15e-5 / mass;
      }
      else if(type == "IIF")
      {
         d0 = 16.7e-5 / mass;
      }
      else
      {
         cerr << "Unknown block number for radiation scaling!" << endl;
      }

      // shadow factor
      double lambda(1.0);
      lambda = getShadowFunction(r_sat, r_sun, r_moon, SM_CONICAL);

      if(lambda != 1.0) sc.setIsEclipsed(true);

      // acceleration
      double Du = lambda*D0 + Dc*cosu + Ds*sinu;
      double Yu = Y0 + Yc*cosu + Ys*sinu;
      double Bu = B0 + Bc*cosu + Bs*sinu;

      a = (Du*ssvec + Yu*yvec + Bu*bvec) * d0 * distfct;


      /// Partials of acceleration wrt satellite position, velocity and SRP
      /// parameters

      // first, get the partials of acceleration wrt satellite position vector
      da_dr.resize(3,3,0.0);

      // then, get the partials of acceleration wrt satellite velocity vector
      da_dv.resize(3,3,0.0);

      // last, get the partials of acceleration wrt scale factors of CODE SRP model
      for(int i=0; i<3; ++i)
      {
         if(9 == srpCoeff.size())
         {
            da_dSRP(i,0) = lambda * ssvec(i) * d0 * distfct;   // da / dD0
            da_dSRP(i,1) = cosu * ssvec(i) * d0 * distfct;     // da / dDc
            da_dSRP(i,2) = sinu * ssvec(i) * d0 * distfct;     // da / dDs
            da_dSRP(i,3) = yvec(i) * d0 * distfct ;            // da / dY0
            da_dSRP(i,4) = cosu * yvec(i) * d0 * distfct;      // da / dYc
            da_dSRP(i,5) = sinu * yvec(i) * d0 * distfct;      // da / dYs
            da_dSRP(i,6) = bvec(i) * d0 * distfct ;            // da / dB0
            da_dSRP(i,7) = cosu * bvec(i) * d0 * distfct;      // da / dBc
            da_dSRP(i,8) = sinu * bvec(i) * d0 * distfct;      // da / dBs
         }
         else if(5 == srpCoeff.size())
         {
            da_dSRP(i,0) = lambda * ssvec(i) * d0 * distfct;   // da / dD0
            da_dSRP(i,1) = yvec(i) * d0 * distfct ;            // da / dY0
            da_dSRP(i,2) = bvec(i) * d0 * distfct ;            // da / dB0
            da_dSRP(i,3) = cosu * bvec(i) * d0 * distfct;      // da / dBc
            da_dSRP(i,4) = sinu * bvec(i) * d0 * distfct;      // da / dBs
         }
      }

   }  // End of method 'CODEPressure::doCompute()'

}  // End of namespace 'gpstk'

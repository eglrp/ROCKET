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
 * This class provides a CODE model for the influence
 * of solar radiation pressure on a satellite.
 */

#include "CODEPressure.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{

   void CODEPressure::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
       // get satellite block number
       const int iblock = sc.getBlockNum();
       // get satellite mass
       const double mass = sc.getDryMass();

       // check the size of sc.P() !!!

       // force model parameters
       Vector<double> p = sc.P();

       // coefficients of CODE model
       double D0, Y0, B0, Dc, Ds, Yc, Ys, Bc, Bs;
       D0 = p[0]; Y0 = p[1]; B0 = p[2];
       Dc = p[3]; Ds = p[4];
       Yc = p[5]; Ys = p[6];
       Bc = p[7]; Bs = p[8];

       // TT
       CommonTime tt( UTC2TT(utc) );

       // Sun and Moon position in ECI
       Vector<double> r_Sun(3,0.0), r_Moon(3,0.0);
       r_Sun = J2kPosition(tt, SolarSystem::Sun);
       r_Moon = J2kPosition(tt, SolarSystem::Moon);

       // Sat position in ECI
       Vector<double> r_Sat(3,0.0);
       r_Sat = sc.R();

       // Sat velocity in ECI
       Vector<double> v_Sat(3,0.0);
       v_Sat = sc.V();

       // Sat position wrt Sun in ECI
       Vector<double> r_SunSat(3,0.0);
       r_SunSat = r_Sat - r_Sun;

        /// CODE model, GAMIT 10.5 ertorb.f

       // Compute unit vectors
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
       Vector<double> rvec = r_Sat / norm(r_Sat);       // +R direction
       Vector<double> vvec = v_Sat / norm(v_Sat);       // +T direction
       Vector<double> zvec = -rvec;                     // +Z direction
       Vector<double> ssvec = r_SunSat / norm(r_SunSat);// -D direction
       Vector<double> esvec = r_Sun / norm(r_Sun);      // 

       Vector<double> hvec = cross(rvec, vvec);         // +N direction
       hvec = normalize(hvec);
       Vector<double> yvec = cross(rvec, ssvec);        // +Y direction
       yvec = normalize(yvec);
       Vector<double> bvec = cross(yvec, ssvec);        // +B direction
       bvec = normalize(bvec);
       Vector<double> xvec = cross(rvec, yvec);         // +X direction
       xvec = normalize(xvec);

       // Compute the B-angle between the sun and the orbital plane for CODE

       // beta is the angle ( [-90,+90] ) of the Sun above (+) the orbital
       // plane
       double beta = PI / 4.0 - std::acos(dot(hvec,esvec));

       // z-axis is (0,0,1) in ECI
       Vector<double> zaxis(3,0.0); zaxis[2] = 1.0;
       // get the node direction
       Vector<double> nvec = cross(zaxis, hvec);
       nvec = normalize(nvec);
       // then get a perpendicular to the orbit normal and sun (will be in
       // orbit plane)
       Vector<double> savec = cross(hvec, esvec);
       savec = norm(savec);
       // finally get the projection of the sun in the orbit plane
       Vector<double> spvec = cross(savec, hvec);
       spvec = norm(spvec);

       // temp Vector
       Vector<double> temp(3,0.0);

       // u is the angle from the node to the satellite direction, thus the
       double cosu = dot(nvec, rvec);
       temp = cross(nvec, rvec);
       double sinu = (dot(temp,hvec)>0.0) ? norm(temp) : -norm(temp);
       double u = std::atan2(sinu, cosu);

       // u0 is the angle from the satellite node to the sun's projection
       double cosu0 = dot(nvec, spvec);
       temp = cross(nvec, spvec);
       double sinu0 = (dot(temp,hvec)>0.0) ? norm(temp) : -norm(temp);
       double u0 = std::atan2(sinu0, cosu0);

       // precalculate sin and cos
       double sinuu0, sin3uu0, sin2beta, cos2beta, sin4beta, cos4beta;
       sinuu0 = std::sin(u-u0);
       sin3uu0 = std::sin(3*u-u0);
       sin2beta = std::sin(2*beta);
       cos2beta = std::cos(2*beta);
       sin4beta = std::sin(4*beta);
       cos4beta = std::cos(4*beta);

       // compute the distance factor for the radiation force
       double dmag2 = norm(r_SunSat) * norm(r_SunSat);
       double au2 = AU * AU;
       double distfct = au2/dmag2;

       double d0=0.0, dt=0.0, xt=0.0, yt=0.0, zt=0.0, bt=0.0, xt1=0.0, xt3=0.0;

       // units (m/s^2)
       if(iblock == 1)
       {
           d0 = 4.54e-5 / mass;
       }
       else if(iblock == 2 || iblock == 3)
       {
           d0 = 8.695e-5 / mass;
       }
       else if(iblock == 4 || iblock == 5 || iblock == 6)
       {
           d0 = 11.15e-5 / mass;
       }
       else if(iblock == 7)
       {
           d0 = 16.7e-5 / mass;
       }
       else
       {
           cerr << "Unknown block number for radiation scaling!" << endl;
       }
/*
       if(iblock == 2 || iblock == 3)
       {
           dt = 0.813e-9*cos2beta - 0.517e-9*cos4beta;
           yt = 0.067e-9*cos2beta;
           bt = -0.385e-9*cos2beta;
           xt1 = -0.015e-9 - 0.018e-9*cos2beta - 0.033e-9*sin2beta;
           xt3 = 0.004e-9 - 0.046e-9*cos2beta - 0.398e-9*sin2beta;

           if(iblock == 2)
           {
               zt = 1.024e-9 + 0.519e-9*cos2beta + 0.125e-9*sin2beta + 0.047e-9*cos4beta - 0.045e-9*sin4beta;
           }
           else if(iblock == 3)
           {
               zt = 0.979e-9 + 0.519e-9*cos2beta + 0.125e-9*sin2beta + 0.047e-9*cos4beta - 0.045e-9*sin4beta;
           }
           else
           {
               cerr << "Sat block not supported for CODE radiation model!" << endl;
           }
       }
*/
       // shadow factor
       double lambda(1.0);
       lambda = getShadowFunction(r_Sat, r_Sun, r_Moon, SM_CONICAL);

/*
       // D(u), Y(u), B(u)
       double Du, Yu, Bu;
       Du = D0;
       Yu = Y0;
       Bu = B0 + Bc*cosu + Bs*sinu;
*/

       Vector<double> radprs(3,0.0);
//       radprs = Du * (-ssvec) + Yu * yvec + Bu * bvec;

       radprs = radprs + (lambda*(D0*d0+dt)*ssvec + (Y0*d0+yt)*yvec + (B0*d0+bt)*bvec)*distfct;

       radprs = radprs + (Dc*d0*cosu*ssvec + Ds*d0*sinu*ssvec
                        + Yc*d0*cosu*yvec + Ys*d0*sinu*yvec
                        + Bc*d0*cosu*bvec + Bs*d0*sinu*bvec)*distfct;
//       radprs = radprs + (xt1*sinuu0*xvec + xt3*sin3uu0*xvec + zt*sin3uu0*zvec)*distfct;

       // srp acceleration
       a = radprs;

       
        /// Partials of acceleration wrt satellite position, velocity and SRP
        /// parameters
      
       // first, get the partials of acceleration wrt satellite position vector
       da_dr.resize(3,3,0.0);

       // then, get the partials of acceleration wrt satellite velocity vector
       da_dv.resize(3,3,0.0);


       // last, get the partials of acceleration wrt scale factors of CODE SRP model
       da_dD0.resize(3,0.0);
       da_dD0 = lambda * d0 * ssvec * distfct;
       da_dY0.resize(3,0.0);
       da_dY0 = d0 * yvec * distfct;
       da_dB0.resize(3,0.0);
       da_dB0 = d0 * bvec * distfct;

       da_dDc.resize(3,0.0);
       da_dDc = cosu * d0 * ssvec * distfct;
       da_dDs.resize(3,0.0);
       da_dDs = sinu * d0 * ssvec * distfct;

       da_dYc.resize(3,0.0);
       da_dYc = cosu * d0 * yvec * distfct;
       da_dYs.resize(3,0.0);
       da_dYs = sinu * d0 * yvec * distfct;

       da_dBc.resize(3,0.0);
       da_dBc = cosu * d0 * bvec * distfct;
       da_dBs.resize(3,0.0);
       da_dBs = sinu * d0 * bvec * distfct;

   }  // End of method 'CODEPressure::doCompute()'

}  // End of namespace 'gpstk'

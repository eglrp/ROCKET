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
 * This class provides a ROCK model for the influence
 * of solar radiation pressure on a satellite.
 */

#include "ROCKPressure.hpp"
#include "IERSConventions.hpp"

using namespace std;

namespace gpstk
{

   void ROCKPressure::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
       // time in TT
       CommonTime TT( UTC2TT(utc) );

       // get satellite block type
       const string type = sc.getBlock();
       // get satellite mass
       const double mass = sc.getMass();

       /// check the size of sc.P() !!!

       // force model parameters
       Vector<double> p = sc.P();

       // coefficients of ROCK model
       double Gx,Gy,Gz;
       Gx = p[0]; Gy = p[1]; Gz = p[2];

       // sun and moon position in eci, unit: m
       Vector<double> r_Sun(3,0.0), r_Moon(3,0.0);
       r_Sun = J2kPosition(TT, SolarSystem::Sun);
       r_Moon = J2kPosition(TT, SolarSystem::Moon);

       // satellite position in eci, unit: m
       Vector<double> r_Sat = sc.R();

       // satellite velocity in eci, unit: m/s
       Vector<double> v_Sat = sc.V();

       // satellite position wrt sun, unit: m
       Vector<double> r_SunSat = r_Sat - r_Sun;

        /// ROCK-T model, GAMIT 10.5 ertorb.f

       // compute unit vectors
       Vector<double> rvec = r_Sat / norm(r_Sat);       // +R direction
       Vector<double> vvec = v_Sat / norm(v_Sat);       // +T direction
       Vector<double> zvec = -rvec;                     // +Z direction
       Vector<double> ssvec = r_SunSat / norm(r_SunSat);// -D direction
       Vector<double> esvec = r_Sun / norm(r_Sun);      // 

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
       double anga, angb, ang2b, ang3b, ang4b, ang5b, ang6b, ang7b;
       anga = dot(ssvec,rvec);
       angb = std::acos(anga);
       ang2b = angb + angb;
       ang3b = angb + ang2b;
       ang4b = ang2b + ang2b;
       ang5b = ang3b + ang2b;
       ang7b = ang5b + ang2b;

       // compute the distance factor for the radiation force
       double dmag2 = norm(r_SunSat) * norm(r_SunSat);
       double au2 = AU * AU;
       double distfct = au2/dmag2;

       double d0=0.0, xt=0.0, zt=0.0;


            /// SRXYZ ///


       if(type == "I")  // BLOCK-I
       {
           d0 = 4.54e-5 / mass;
           xt = (-4.55e-5*std::sin(angb) + 0.08e-5*std::sin(ang2b+0.9) - 0.06e-5*std::cos(ang4b+0.08) + 0.08e-5) / mass;
           zt = (-4.54e-5*std::cos(angb) + 0.20e-5*std::sin(ang2b-0.3) - 0.03e-5*std::sin(ang4b)) / mass;
       }
       else if(type == "II"
            || type == "IIA")  // BLOCK-II or BLOCK-IIA
       {
           d0 = 8.695e-5 / mass;
           xt = (-8.96e-5*std::sin(angb) + 0.16e-5*std::sin(ang3b) + 0.10e-5*std::sin(ang5b) - 0.07e-5*std::sin(ang7b)) / mass;
           zt = -8.43e-5*std::cos(angb) / mass;
       }
       else if(type == "IIR"
            || type == "IIR-A"
            || type == "IIR-B"
            || type == "IIR-M")   // BLOCK-IIR
       {
           d0 = 11.15e-5 / mass;
//           xt = (-11.0e-5*std::sin(angb) - 0.2e-5*std::sin(ang3b) + 0.2e-5*std::sin(ang5b)) / mass;
//           zt = (-11.3e-5*std::cos(angb) + 0.1e-5*std::cos(ang3b) + 0.2e-5*std::cos(ang5b)) / mass;
           xt = (-11.0e-5*std::sin(angb) + 0.2e-5*std::sin(ang3b) - 0.2e-5*std::sin(ang5b)) / mass;
           zt = (-11.3e-5*std::cos(angb) - 0.1e-5*std::cos(ang3b) - 0.2e-5*std::cos(ang5b)) / mass;
       }
       // For IIF, multiply IIR constant by 1.5 (from MIT IGS runs) to get the
       // direct coefficient ~1.0. Since the mass is 1.416 times larger, this
       // implies that the effective cross-sectional area is 2.12 times larger,
       // Note: no xt or zt terms yet available for IIFs.
       else if(type == "IIF") // BLOCK-IIF
       {
           d0 = 16.7e-5 / mass;
       }
       else
       {
           cout << "Unknown block number for radiation scaling!" << endl;
       }

       // shadow factor
       double lambda(1.0);
       lambda = getShadowFunction(r_Sat, r_Sun, r_Moon, SM_CONICAL);

       // srp acceleration
//       a = lambda * (Gx*xt*xvec + Gy*d0*yvec + Gz*zt*zvec) * distfct;
       a = (lambda*Gx*xt*xvec + Gy*d0*yvec + lambda*Gz*zt*zvec) * distfct;


            /// SRDYZ ///
/*
       if(iblock == 1)
       {
           d0 = 4.54e-5 / mass;
           xt = (+0.01e-5*std::sin(angb) - 0.08e-5*std::sin(ang2b+0.9) + 0.06e-5*std::cos(ang4b+0.08) - 0.08e-5) / mass;
           zt = (-0.20e-5*std::sin(ang2b-0.3) + 0.03e-5*std::sin(ang4b)) / mass;
       }
       else if(iblock == 2 || iblock == 3)
       {
           d0 = 8.695e-5 / mass;
           xt = (+0.265e-5*std::sin(angb) - 0.16e-5*std::sin(ang3b) - 0.10e-5*std::sin(ang5b) + 0.07e-5*std::sin(ang7b)) / mass;
           zt = -0.265e-5*std::cos(angb) / mass;
       }
       else if(iblock == 4 || iblock == 5 || iblock == 6)
       {
           d0 = 11.15e-5 / mass;
           xt = (-0.15e-5*std::sin(angb) + 0.2e-5*std::sin(ang3b) - 0.2e-5*std::sin(ang5b)) / mass;
           zt = (+0.15e-5*std::cos(angb) - 0.1e-5*std::cos(ang3b) - 0.2e-5*std::cos(ang5b)) / mass;
       }
       else if(iblock == 7)
       {
           d0 = 16.7e-5 / mass;
       }
       else
       {
           cout << "Unknown block number for radiation scaling!" << endl;
       }


       double lambda(1.0);
       lambda = getShadowFunction(r_Sat, r_Sun, r_Moon, SM_CONICAL);

       a = lambda * (Gx*d0*ssvec + Gy*d0*yvec + Gz*d0*zvec + (xt*xvec + zt*zvec)) * distfct;
*/

/*
       // For ROCK4 model: SV-BODY (xyz) parameterization with ROCK4 (x/z)
       // terms added (original ROCK4)
       //   radcon(0): [XRAD]: coeff. of (radiation) in sv (+) x-axis direction
       //   (nominally 0.0 to -1.0)
       //   radcon(1): [YRAD]: coeff. of (radiation) in sv (+) y-axis direction
       //   (nominally 0.0)
       //   radcon(2): [ZRAD]: coeff. of (radiation) in sv (+) z-axis direction
       //   (nominally 0.0 to -1.0)
       //   I don't know why it likes this!!!
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
       Matrix<double> dez_dr(3,3,0.0), dey_dr(3,3,0.0), dex_dr(3,3,0.0);
       
       Matrix<double> I = ident<double>(3);

       dez_dr = -(I - outer(zvec,zvec)) / norm(r_Sat);
       dey_dr = (I - outer(yvec,yvec)) * skewSymm(r_Sun) / norm(cross(r_Sat,r_Sun));
       dex_dr = (I - outer(xvec,xvec)) * (2.0*outer(r_Sun,r_Sat) - dot(r_Sun,r_Sat)*I - outer(r_Sat,r_Sun)) / norm(cross(cross(r_Sat,r_Sun), r_Sat));

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
       da_dGx.resize(3,0.0);
       da_dGx = lambda * xt * xvec * distfct;

       da_dGy.resize(3,0.0);
       da_dGy = lambda * d0 * yvec * distfct;

       da_dGz.resize(3,0.0);
       da_dGz = lambda * zt * zvec * distfct;
/*
       cout << "ROCK:" << endl
            << da_dGx << endl
            << da_dGy << endl
            << da_dGz << endl;
*/
   }  // End of method 'ROCKPressure::doCompute()'

}  // End of namespace 'gpstk'

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
//
//  Kaifa Kuang - Wuhan University . 2015
//
//============================================================================


/**
 * @file CODEPressure.cpp
 * Class to do CODE Pressure calculation.
 */

#include "CODEPressure.hpp"
#include "GNSSconstants.hpp"
#include "Epoch.hpp"

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
        // get current satellite block
        const string block = sc.getCurrentBlock();

        // get current satellite mass, unit: kg
        const double mass = sc.getCurrentMass();

        // get srp coefficients
        double D0(0.0), Dc(0.0), Ds(0.0);
        double Y0(0.0), Yc(0.0), Ys(0.0);
        double B0(0.0), Bc(0.0), Bs(0.0);

        if(9 == srpCoeff.size())
        {
            D0 = srpCoeff(0);
            Y0 = srpCoeff(1);
            B0 = srpCoeff(2);
            Dc = srpCoeff(3);
            Ds = srpCoeff(4);
            Yc = srpCoeff(5);
            Ys = srpCoeff(6);
            Bc = srpCoeff(7);
            Bs = srpCoeff(8);

            da_dSRP.resize(3,9,0.0);
        }
        else if(5 == srpCoeff.size())
        {
            D0 = srpCoeff(0);
            Y0 = srpCoeff(1);
            B0 = srpCoeff(2);
            Bc = srpCoeff(3);
            Bs = srpCoeff(4);

            da_dSRP.resize(3,5,0.0);
        }

        // TT
        double tt = JulianDate(pRefSys->UTC2TT(utc)).jd;

        SolarSystem::Planet center(SolarSystem::Earth);
        SolarSystem::Planet target;

        // sun position and velocity in ICRS, unit: km, km/day
        double rv_sun[6] = {0.0};
        target = SolarSystem::Sun;
        pSolSys->computeState(tt, target, center, rv_sun);

        // moon position and velocity in ICRS, unit: km, km/day
        double rv_moon[6] = {0.0};
        target = SolarSystem::Moon;
        pSolSys->computeState(tt, target, center, rv_moon);

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
        r_sat = sc.getCurrentPos();

        // satellite velocity in ICRS, unit: m/s
        Vector<double> v_sat(3,0.0);
        v_sat = sc.getCurrentVel();

        // satellite position wrt sun in ICRS, unit: m
        Vector<double> r_sun2sat(3,0.0);
        r_sun2sat = r_sat - r_sun;


        // compute unit vectors
        Vector<double> rvec = normalize(r_sat);       // +R direction
        Vector<double> vvec = normalize(v_sat);       // +T direction
        Vector<double> zvec = -rvec;                  // +Z direction
        Vector<double> svec = normalize(r_sun2sat);   // -D direction
        Vector<double> esvec = normalize(r_sun);      //

        Vector<double> hvec = cross(vvec, zvec);      // +N direction
        hvec = normalize(hvec);
        Vector<double> yvec = cross(rvec, svec);      // +Y direction
        yvec = normalize(yvec);
        Vector<double> bvec = cross(yvec, svec);      // +B direction
        bvec = normalize(bvec);
        Vector<double> xvec = cross(rvec, yvec);      // +X direction
        xvec = normalize(xvec);


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
        double sinu = norm(temp);
        if(dot(temp,hvec) < 0.0) sinu *= -1;
        double u = std::atan2(sinu, cosu);

        // compute the distance factor for the radiation force
        double dmag2 = norm(r_sun2sat) * norm(r_sun2sat);
        double au2 = AU * AU;
        double distfct = au2/dmag2;

        // unit: m/s^2
        double d0=0.0;
        if(block == "I")
        {
            d0 = 4.54e-5 / mass;
        }
        else if(block == "II"
             || block == "IIA")
        {
            d0 = 8.695e-5 / mass;
        }
        else if(block == "IIR"
             || block == "IIR-A"
             || block == "IIR-B"
             || block == "IIR-M")
        {
            d0 = 11.15e-5 / mass;
        }
        else if(block == "IIF")
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

        if(lambda != 1.0) sc.setEclipse(true);

        // acceleration
        double Du = lambda*D0 + Dc*cosu + Ds*sinu;
        double Yu = Y0 + Yc*cosu + Ys*sinu;
        double Bu = B0 + Bc*cosu + Bs*sinu;

        a = (Du*svec + Yu*yvec + Bu*bvec) * d0 * distfct;


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
                da_dSRP(i,0) = lambda * svec(i) * d0 * distfct; // da / dD0
                da_dSRP(i,1) = yvec(i) * d0 * distfct ;         // da / dY0
                da_dSRP(i,2) = bvec(i) * d0 * distfct ;         // da / dB0
                da_dSRP(i,3) = cosu * svec(i) * d0 * distfct;   // da / dDc
                da_dSRP(i,4) = sinu * svec(i) * d0 * distfct;   // da / dDs
                da_dSRP(i,5) = cosu * yvec(i) * d0 * distfct;   // da / dYc
                da_dSRP(i,6) = sinu * yvec(i) * d0 * distfct;   // da / dYs
                da_dSRP(i,7) = cosu * bvec(i) * d0 * distfct;   // da / dBc
                da_dSRP(i,8) = sinu * bvec(i) * d0 * distfct;   // da / dBs
            }
            else if(5 == srpCoeff.size())
            {
                da_dSRP(i,0) = lambda * svec(i) * d0 * distfct; // da / dD0
                da_dSRP(i,1) = yvec(i) * d0 * distfct ;         // da / dY0
                da_dSRP(i,2) = bvec(i) * d0 * distfct ;         // da / dB0
                da_dSRP(i,3) = cosu * bvec(i) * d0 * distfct;   // da / dBc
                da_dSRP(i,4) = sinu * bvec(i) * d0 * distfct;   // da / dBs
            }
        }

    }  // End of method 'CODEPressure::doCompute()'

}  // End of namespace 'gpstk'

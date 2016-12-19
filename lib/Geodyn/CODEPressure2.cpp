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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================


/**
 * @file CODEPressure2.cpp
 * Class to do CODE Pressure calculation without a prior model.
 */

#include "CODEPressure2.hpp"
#include "GNSSconstants.hpp"
#include "Epoch.hpp"

using namespace std;

namespace gpstk
{

    /** Compute acceleration (and related partial derivatives) of CODE
     *  Pressure without a prior model.
     * @param utc     time in UTC
     * @param rb      earth body
     * @param sc      spacecraft
     */
    void CODEPressure2::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
    {
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


        // unit vector of R direction, Earth to Sat
        Vector<double> r_unit = normalize(r_sat);
        // unit vector of V direction
        Vector<double> v_unit = normalize(v_sat);

        // unit vector of Z direction, Sat to Earth
        Vector<double> z_unit = -r_unit;
        // unit vector of D direction, Sat to Sun
        Vector<double> d_unit = -normalize(r_sun2sat);

        // unit vector of S direction, Earth to Sun
        Vector<double> sun_unit = normalize(r_sun);

        // unit vector of N direction, Orbit Normal
        Vector<double> nop_unit = cross(r_unit, v_unit);

        // unit vector of Y direction, Solar Panels
        Vector<double> y_unit = cross(d_unit, r_unit);
        // unit vector of B direction, B = D X Y
        Vector<double> b_unit = cross(d_unit, y_unit);

        // unit vector of X direction, X = Y X Z
        Vector<double> x_unit = cross(y_unit, z_unit);

        // z-axis in ICRS, (0,0,1)
        Vector<double> zaxis(3,0.0); zaxis(2) = 1.0;
        // node direction in orbit plane
        Vector<double> node_unit = cross(zaxis, nop_unit);

        // u, argument of latitude, from node to sat direction
        double cosu = dot(node_unit, r_unit);
        Vector<double> tmp_unit = cross(node_unit, r_unit);
        double sinu = dot(tmp_unit, nop_unit);
        double u = std::atan2(sinu, cosu);

        // distance factor
        double distfct = std::pow(AU/norm(r_sun2sat),2);

        // shadow factor
        double lambda(1.0);
        lambda = getShadowFunction(r_sat, r_sun, r_moon, SM_CONICAL);
        if(lambda != 1.0) sc.setEclipse(true);

        double lambda1(lambda);
        if(lambda < 0.999) lambda1 = 0.0;


        /// acceleration
        double d0_const = 1e-9;

        double Du(0.0), Yu(0.0), Bu(0.0);
        if(9 == srpCoeff.size())
        {
            Du = D0 + (Dc*cosu + Ds*sinu)*lambda1;
            Yu = Y0 + (Yc*cosu + Ys*sinu)*lambda1;
            Bu = B0 + (Bc*cosu + Bs*sinu)*lambda1;
        }
        else if(5 == srpCoeff.size())
        {
            Du = D0;
            Yu = Y0;
            Bu = B0 + (Bc*cosu + Bs*sinu)*lambda1;
        }

        a = d0_const*lambda*(Du*d_unit + Yu*y_unit + Bu*b_unit)*distfct;


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
                da_dSRP(i,0) = d_unit(i);           // da / dD0
                da_dSRP(i,1) = d_unit(i) * cosu;    // da / dDc
                da_dSRP(i,2) = d_unit(i) * sinu;    // da / dDs
                da_dSRP(i,3) = y_unit(i);           // da / dY0
                da_dSRP(i,4) = y_unit(i) * cosu;    // da / dYc
                da_dSRP(i,5) = y_unit(i) * sinu;    // da / dYs
                da_dSRP(i,6) = b_unit(i);           // da / dB0
                da_dSRP(i,7) = b_unit(i) * cosu;    // da / dBc
                da_dSRP(i,8) = b_unit(i) * sinu;    // da / dBs
            }
            else if(5 == srpCoeff.size())
            {
                da_dSRP(i,0) = d_unit(i);           // da / dD0
                da_dSRP(i,1) = y_unit(i);           // da / dY0
                da_dSRP(i,2) = b_unit(i);           // da / dB0
                da_dSRP(i,3) = b_unit(i) * cosu;    // da / dBc
                da_dSRP(i,4) = b_unit(i) * sinu;    // da / dBs
            }
        }

        da_dSRP = d0_const * lambda * distfct * da_dSRP;

    }  // End of method 'CODEPressure2::doCompute()'

}  // End of namespace 'gpstk'

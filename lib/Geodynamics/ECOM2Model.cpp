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
 * @file ECOM2Model.cpp
 * Class to do SRP calculation with ECOM2 model.
 */

#include "ECOM2Model.hpp"
#include "constants.hpp"
#include "Epoch.hpp"


using namespace std;


namespace gpstk
{

    /** Compute acceleration (and related partial derivatives) of SRP.
     * @param tt        TT
     * @param orbits    orbits
     */
    void ECOM2Model::Compute( const CommonTime&   tt,
                              const satVectorMap& orbits )
    {

        // TT
        double jd_tt = JulianDate(tt).jd;

        SolarSystem::Planet center(SolarSystem::Earth);
        SolarSystem::Planet target;

        // sun position and velocity in ICRS, unit: km, km/day
        double rv_sun[6] = {0.0};
        target = SolarSystem::Sun;
        pSolSys->computeState(jd_tt, target, center, rv_sun);

        // moon position and velocity in ICRS, unit: km, km/day
        double rv_moon[6] = {0.0};
        target = SolarSystem::Moon;
        pSolSys->computeState(jd_tt, target, center, rv_moon);

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


        SatID sat;
        Vector<double> orbit(96,0.0);
        Vector<double> coeff(9,0.0);

        double D0(0.0), DC2(0.0), DS2(0.0), DC4(0.0), DS4(0.0);
        double Y0(0.0);
        double B0(0.0), BC(0.0), BS(0.0);

        // satellite position in ICRS, unit: m
        Vector<double> r_sat(3,0.0);

        // satellite velocity in ICRS, unit: m/s
        Vector<double> v_sat(3,0.0);

        // satellite position wrt sun in ICRS, unit: m
        Vector<double> r_sun2sat(3,0.0);

        // unit vector of R direction, Earth to Sat
        Vector<double> r_unit(3,0.0);
        // unit vector of V direction
        Vector<double> v_unit(3,0.0);

        // unit vector of Z direction, Sat to Earth
        Vector<double> z_unit(3,0.0);
        // unit vector of D direction, Sat to Sun
        Vector<double> d_unit(3,0.0);

        // unit vector of S direction, Earth to Sun
        Vector<double> sun_unit(3,0.0);

        // unit vector of N direction, Orbit Normal
        Vector<double> nop_unit(3,0.0);

        // unit vector of Y direction, Solar Panels
        Vector<double> y_unit(3,0.0);
        // unit vector of B direction, B = D X Y
        Vector<double> b_unit(3,0.0);

        // unit vector of X direction, X = Y X Z
        Vector<double> x_unit(3,0.0);

        // z-axis in ICRS, (0,0,1)
        Vector<double> zaxis(3,0.0);
        zaxis(2) = 1.0;
        // node direction in orbit plane
        Vector<double> node_unit(3,0.0);

        // unit vector, an auxilliary direction perpendicular
        // to nop_unit and sun_unit, in the orbit plane
        Vector<double> sa_unit(3,0.0);

        // unit vector, the projection of sun in the orbit plane
        Vector<double> sp_unit(3,0.0);

        Vector<double> tmp_unit(3,0.0);

        // u, argument of latitude, from node to sat direction
        double cosu_sat(0.0), sinu_sat(0.0), u_sat(0.0);

        // u, argument of latitude, from node to sun direction
        double cosu_sun(0.0), sinu_sun(0.0), u_sun(0.0);

        double du(0.0);
        double cos1du(0.0), sin1du(0.0);
        double cos2du(0.0), sin2du(0.0);
        double cos4du(0.0), sin4du(0.0);

        // distance factor
        double distfct(0.0);

        // shadow factor
        double lambda(1.0);
        double lambda1(lambda);

        /// acceleration
        double d0_const = 1e-9;

        double Du(0.0), Yu(0.0), Bu(0.0);

        Vector<double> a(3,0.0);

        // the partials of acceleration wrt satellite position vector
        Matrix<double> da_dr(3,3,0.0);

        // the partials of acceleration wrt satellite velocity vector
        Matrix<double> da_dv(3,3,0.0);

        // the partials of acceleration wrt scale factors of CODE SRP model
        Matrix<double> da_dSRP(3,9,0.0);

        for( satVectorMap::const_iterator it = orbits.begin();
             it != orbits.end();
             ++it )
        {
            sat = it->first;
            orbit = it->second;

            satVectorMap::const_iterator itSRP( satSRPCoeff.find(sat) );

            if( itSRP != satSRPCoeff.end() )
            {
                coeff = itSRP->second;
            }
            else
            {
                coeff.resize(9,0.0);
            }

            D0   = coeff(0); DC2  = coeff(1);
            DS2  = coeff(2);
            DC4  = coeff(3); DS4  = coeff(4);
            Y0   = coeff(5);
            B0   = coeff(6);
            BC   = coeff(7); BS   = coeff(8);

            r_sat(0) = orbit(0);
            r_sat(1) = orbit(1);
            r_sat(2) = orbit(2);

            v_sat(0) = orbit(3);
            v_sat(1) = orbit(4);
            v_sat(2) = orbit(5);

            r_sun2sat = r_sat - r_sun;


            // unit vector of R direction, Earth to Sat
            r_unit = normalize(r_sat);
            // unit vector of V direction
            v_unit = normalize(v_sat);

            // unit vector of Z direction, Sat to Earth
            z_unit = -r_unit;
            // unit vector of D direction, Sat to Sun
            d_unit = -normalize(r_sun2sat);

            // unit vector of S direction, Earth to Sun
            sun_unit = normalize(r_sun);

            // unit vector of N direction, Orbit Normal
            nop_unit = cross(r_unit, v_unit);

            // unit vector of Y direction, Solar Panels
            y_unit = cross(d_unit, r_unit);
            // unit vector of B direction, B = D X Y
            b_unit = cross(d_unit, y_unit);

            // unit vector of X direction, X = Y X Z
            x_unit = cross(y_unit, z_unit);

            // node direction in orbit plane
            node_unit = cross(zaxis, nop_unit);

            // unit vector, an auxilliary direction perpendicular
            // to nop_unit and sun_unit, in the orbit plane
            sa_unit = cross(nop_unit, sun_unit);

            // unit vector, the projection of sun in the orbit plane
            sp_unit = cross(sa_unit, nop_unit);

            // u, argument of latitude, from node to sat direction
            cosu_sat = dot(node_unit, r_unit);
            tmp_unit = cross(node_unit, r_unit);
            sinu_sat = dot(tmp_unit, nop_unit);
            u_sat = std::atan2(sinu_sat, cosu_sat);

            // u, argument of latitude, from node to sun direction
            cosu_sun = dot(node_unit, sp_unit);
            tmp_unit = cross(node_unit, sp_unit);
            sinu_sun = dot(tmp_unit, nop_unit);
            u_sun = std::atan2(sinu_sun, cosu_sun);

            du = u_sat - u_sun;
            cos1du = std::cos(1*du);
            sin1du = std::sin(1*du);
            cos2du = std::cos(2*du);
            sin2du = std::sin(2*du);
            cos4du = std::cos(4*du);
            sin4du = std::sin(4*du);

            // distance factor
            distfct = std::pow(AU/norm(r_sun2sat),2);

            // shadow factor
            lambda = getShadowFunction(r_sat, r_sun, r_moon, SM_CONICAL);

            if(lambda < 0.999) lambda1 = 0.0;


            /// acceleration
            Du = D0 + (DC2*cos2du + DS2*sin2du + DC4*cos4du + DS4*sin4du)*lambda1;
            Yu = Y0;
            Bu = B0 + (BC *cos1du + BS *sin1du)*lambda1;

            a = d0_const*lambda*(Du*d_unit + Yu*y_unit + Bu*b_unit)*distfct;
            satAcc[sat] = a;


            /// Partials of acceleration wrt satellite position, velocity and SRP
            /// parameters

            for(int i=0; i<3; ++i)
            {
                da_dSRP(i,0) = d_unit(i);               // da / dD0
                da_dSRP(i,1) = d_unit(i) * cos2du;      // da / dDC2
                da_dSRP(i,2) = d_unit(i) * sin2du;      // da / dDS2
                da_dSRP(i,3) = d_unit(i) * cos4du;      // da / dDC4
                da_dSRP(i,4) = d_unit(i) * sin4du;      // da / dDS4
                da_dSRP(i,5) = y_unit(i);               // da / dY0
                da_dSRP(i,6) = b_unit(i);               // da / dB0
                da_dSRP(i,7) = b_unit(i) * cos1du;      // da / dBc
                da_dSRP(i,8) = b_unit(i) * sin1du;      // da / dBs
            }

            da_dSRP = d0_const * lambda * distfct * da_dSRP;
            satPartialSRP[sat] = da_dSRP;

        } // End of 'for(satVectorMap::const_iterator...)'

    }  // End of method 'ECOM2Model::Compute(...)'


}  // End of namespace 'gpstk'

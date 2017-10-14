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
 * @file ThirdBody.cpp
 * Class to do ThirdBody Gravitation calculation.
 */

#include "ThirdBody.hpp"
#include "constants.hpp"


using namespace std;


namespace gpstk
{

    /** Compute acceleration (and related partial derivatives) of ThirdBody.
     * @param tt        TT
     * @param orbits    orbits
     */
    void ThirdBody::Compute( const CommonTime&   tt,
                             const satVectorMap& orbits )
    {

        double jd_tt = JulianDate(tt).jd;

//        Matrix<double> c2t( pRefSys->C2TMatrix(time) );

        bool bPlanets[10] = {false};
        bPlanets[0] = bSun;
        bPlanets[1] = bMoon;
        bPlanets[2] = bMercury;
        bPlanets[3] = bVenus;
        bPlanets[4] = bMars;
        bPlanets[5] = bJupiter;
        bPlanets[6] = bSaturn;
        bPlanets[7] = bUranus;
        bPlanets[8] = bNeptune;
        bPlanets[9] = bPluto;

        SolarSystem::Planet targets[10];
        targets[0] = SolarSystem::Sun;
        targets[1] = SolarSystem::Moon;
        targets[2] = SolarSystem::Mercury;
        targets[3] = SolarSystem::Venus;
        targets[4] = SolarSystem::Mars;
        targets[5] = SolarSystem::Jupiter;
        targets[6] = SolarSystem::Saturn;
        targets[7] = SolarSystem::Uranus;
        targets[8] = SolarSystem::Neptune;
        targets[9] = SolarSystem::Pluto;

        SolarSystem::Planet target;

        double factors[10] = {0.0};
        factors[0] = GM_SUN;
        factors[1] = GM_MOON;
        factors[2] = GM_MERCURY;
        factors[3] = GM_VENUS;
        factors[4] = GM_MARS;
        factors[5] = GM_JUPITER;
        factors[6] = GM_SATURN;
        factors[7] = GM_URANUS;
        factors[8] = GM_NEPTUNE;
        factors[9] = GM_PLUTO;

        double factor;

        SolarSystem::Planet center(SolarSystem::Earth);


        // Distance from planet to satellite
        Vector<double> r_p2s(3,0.0);

        double p(0.0),p3(0.0);
        double d(0.0),d3(0.0),d5(0.0);

        Matrix<double> I = ident<double>(3);

        Vector<double> r_sat(3,0.0);


        // Geocentric position and velocity of planet, unit: km, km/day
        double rv_planet[6] = {0.0};

        // Geocentric position of planet, unit: m
        Vector<double> position(3,0.0);

        // Geocentric position of planets, unit: m
        Vector<double> positions[10] = {0.0};

        for(int i=0; i<10; ++i)
        {
            if( bPlanets[i] )
            {
                pSolSys->computeState(jd_tt, targets[i], center, rv_planet);

                position(0) = rv_planet[0]*1e3;
                position(1) = rv_planet[1]*1e3;
                position(2) = rv_planet[2]*1e3;
            }

            positions[i] = position;
        }


        SatID sat;
        Vector<double> orbit;

        Vector<double> ap(3,0.0);
        Matrix<double> dap_dr(3,3,0.0);

        Vector<double> a(3,0.0);
        Matrix<double> da_dr(3,3,0.0);
        Matrix<double> da_dv(3,3,0.0);

        for( satVectorMap::const_iterator it = orbits.begin();
             it != orbits.end();
             ++it )
        {
            sat = it->first;
            orbit = it->second;

            r_sat(0) = orbit(0);
            r_sat(1) = orbit(1);
            r_sat(2) = orbit(2);

            ap.resize(3,0.0);
            dap_dr.resize(3,3,0.0);

            // Loop
            for(int i=0; i<10; ++i)
            {
                if( !bPlanets[i]) continue;

                factor = factors[i];
                position = positions[i];

                r_p2s = r_sat - position;

                p = norm(position);
                p3 = p*p*p;

                d = norm(r_p2s);
                d3 = d*d*d;
                d5 = d3*d*d;

                // a
                ap += -factor * (r_p2s/d3 + position/p3);

                // da_dr
                dap_dr += -factor * (I/d3 - 3.0*outer(r_p2s,r_p2s)/d5);

            }

//            if( bPlanets[1] )
//            {
//                Vector<double> a_j2(3,0.0);
//                factor = 1.5*J2_EARTH*GM_MOON*RE_EARTH*RE_EARTH;
//
//                Vector<double> r_itrs( c2t*r_moon );
//
//                double rho = norm(r_itrs);
//                double rho4 = std::pow(rho,4);
//                double slat = r_itrs(2)/rho;
//                double clat = std::sqrt(1.0-slat*slat);
//                double lon = std::atan2(r_itrs(1),r_itrs(0));
//                double slon = std::sin(lon);
//                double clon = std::cos(lon);
//
//                Vector<double> temp(3,0.0);
//                temp(0) = (5*slat*slat-1)*clat*clon;
//                temp(1) = (5*slat*slat-1)*clat*slon;
//                temp(2) = (5*slat*slat-3)*slat;
//
//                a_j2 = factor/rho4*temp;
//
//                ap += a_j2;
//            }

            a = ap;
            satAcc[sat] = a;

            da_dr = dap_dr;
            satPartialR[sat] = da_dr;

        }

    }  // End of method 'ThirdBody::Compute(...)'

}  // End of namespace 'gpstk'

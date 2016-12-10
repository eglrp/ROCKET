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
#include "GNSSconstants.hpp"


using namespace std;


namespace gpstk
{

    /** Compute acceleration (and related partial derivatives) of ThirdBody.
     * @param utc     time in UTC
     * @param eb      earth body
     * @param sc      spacecraft
     */
    void ThirdBody::doCompute(CommonTime utc, EarthBody& eb, Spacecraft& sc)
    {
        /* Oliver P69 and P248
         * a = GM*( (s-r)/norm(s-r)^3 - s/norm(s)^3 )
         *
         * da/dr = -GM*( I/norm(r-s)^3 - 3(r-s)transpose(r-s)/norm(r-s)^5)
         */

        double tt = JulianDate( pRefSys->UTC2TT(utc) ).jd;

        Vector<double> r_sat = sc.getCurrentPos();

        Vector<bool> bPlanets(10,false);
        bPlanets(0) = bSun;
        bPlanets(1) = bMoon;
        bPlanets(2) = bMercury;
        bPlanets(3) = bVenus;
        bPlanets(4) = bMars;
        bPlanets(5) = bJupiter;
        bPlanets(6) = bSaturn;
        bPlanets(7) = bUranus;
        bPlanets(8) = bNeptune;
        bPlanets(9) = bPluto;

        double GMP;
        SolarSystem::Planet target;
        SolarSystem::Planet center(SolarSystem::Earth);

        // Geocentric position and velocity of planet, unit: km, km/day
        double rv_planet[6] = {0.0};

        // Geocentric position of planet, unit: m
        Vector<double> r_planet(3,0.0);

        // Distance from planet to satellite
        Vector<double> r_p2s(3,0.0);

        double p(0.0),p3(0.0);
        double d(0.0),d3(0.0),d5(0.0);

        Matrix<double> I = ident<double>(3);

        Vector<double> ap(3,0.0);
        Matrix<double> dap_dr(3,3,0.0);

        // Loop
        for(int i=0; i<10; ++i)
        {
            if( !bPlanets(i) ) continue;

            if(0 == i)          // Sun
            {
                GMP = GM_SUN;
                target = SolarSystem::Sun;
            }
            else if(1 == i)     // Moon
            {
                GMP = GM_MOON;
                target = SolarSystem::Moon;
            }
            else if(2 == i)     // Mercury
            {
                GMP = GM_MERCURY;
                target = SolarSystem::Mercury;
            }
            else if(3 == i)     // Venus
            {
                GMP = GM_VENUS;
                target = SolarSystem::Venus;
            }
            else if(4 == i)     // Mars
            {
                GMP = GM_MARS;
                target = SolarSystem::Mars;
            }
            else if(5 == i)     // Jupiter
            {
                GMP = GM_JUPITER;
                target = SolarSystem::Jupiter;
            }
            else if(6 == i)     // Saturn
            {
                GMP = GM_SATURN;
                target = SolarSystem::Saturn;
            }
            else if(7 == i)     // Uranus
            {
                GMP = GM_URANUS;
                target = SolarSystem::Uranus;
            }
            else if(8 == i)     // Neptune
            {
                GMP = GM_NEPTUNE;
                target = SolarSystem::Neptune;
            }
            else if(9 == i)     // Pluto
            {
                GMP = GM_PLUTO;
                target = SolarSystem::Pluto;
            }

            pSolSys->computeState(tt, target, center, rv_planet);

            r_planet(0) = rv_planet[0]*1e3;
            r_planet(1) = rv_planet[1]*1e3;
            r_planet(2) = rv_planet[2]*1e3;

            r_p2s = r_sat - r_planet;

            p = norm(r_planet);
            p3 = p*p*p;

            d = norm(r_p2s);
            d3 = d*d*d;
            d5 = d3*d*d;

            // a
            ap += -GMP * (r_p2s/d3 + r_planet/p3);

            // da_dr
            dap_dr += -GMP * (I/d3 - 3.0*outer(r_p2s,r_p2s)/d5);

            // da_dv ...

            // da_dp ...

        }

        a = ap;
        da_dr = dap_dr;

    }  // End of method 'ThirdBody::doCompute()'

}  // End of namespace 'gpstk'

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

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file MoonForce.cpp
 * This class calculate the gravitational effect of the moon.
 */

#include "MoonForce.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{

   MoonForce::MoonForce()
      : mu(GM_MOON)
   {}


      /* Call the relevant methods to compute the acceleration.
      * @param utc  Time reference class
      * @param rb   Body reference class
      * @param sc   Spacecraft parameters and state
      * @return the acceleration [m/s^s]
      */
   void MoonForce::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
      /* Oliver P69 and P248
       * a = GM*( (s-r)/norm(s-r)^3 - s/norm(s)^3 )
       *
       * da/dr = -GM*( I/norm(r-s)^3 - 3(r-s)transpose(r-s)/norm(r-s)^5)
       */

      Vector<double> r_sat = sc.R();

      // Geocentric position of moon
      Vector<double> r_moon = J2kPosition(UTC2TT(utc), SolarSystem::Moon);
      double m( norm(r_moon) );
      double m3( m*m*m );

      // Distance from satellite to moon
      Vector<double> dist(r_moon-r_sat);
      double d( norm(dist) );
      double d3( d*d*d );
      double d5( d3*d*d );

      // a
      a = mu * (dist/d3 - r_moon/m3);

      // da_dr
      Matrix<double> I = ident<double>(3);
      da_dr = -mu * (I/d3 - 3.0*outer(dist,dist)/d5);

      // da_dv
      da_dv.resize(3,3,0.0);

      // da_dp
      
   }  // End of method 'MoonForce::doCompute()'


}  // End of namespace 'gpstk'

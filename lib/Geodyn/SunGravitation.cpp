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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
 * @file SunGravitation.cpp
 * Class to do Sun Gravitation calculation.
 */

#include "SunGravitation.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{
   
      /** Compute acceleration (and related partial derivatives) of Sun
       *  Gravitation.
       * @param utc     time in UTC
       * @param rb      earth body
       * @param sc      spacecraft
       */
   void SunGravitation::doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
      /* Oliver P69 and P248
       * a = GM*( (s-r)/norm(s-r)^3 - s/norm(s)^3 )
       *
       * da/dr = -GM*( I/norm(r-s)^3 - 3(r-s)transpose(r-s)/norm(r-s)^5)
       */

      Vector<double> r_sat = sc.R();

      // Geocentric position of sun
      double rv_sun[6] = {0.0};     // unit: km, km/day
      double tt = JulianDate( pRefSys->UTC2TT(utc) ).jd;
      pSolSys->computeState(tt,
                            SolarSystem::Sun,
                            SolarSystem::Earth,
                            rv_sun);
      Vector<double> r_sun(3,0.0);  // unit: km
      r_sun(0) = rv_sun[0];
      r_sun(1) = rv_sun[1];
      r_sun(2) = rv_sun[2];
      r_sun *= 1000.0;              // unit: m

      double s( norm(r_sun) );
      double s3( s*s*s );

      // Distance from satellite to sun
      Vector<double> dist(r_sun-r_sat);
      double d( norm(dist) );
      double d3( d*d*d );
      double d5( d3*d*d );

      // a
      a = GM_SUN * (dist/d3 - r_sun/s3);

      // da_dr
      Matrix<double> I = ident<double>(3);
      da_dr = -GM_SUN * (I/d3 - 3.0*outer(dist,dist)/d5);

//      cout << "df_sun/dr: " << endl;
//      cout << setprecision(15) << da_dr << endl;

      // da_dv
      da_dv.resize(3,3,0.0);

      // da_dp
      
   }  // End of method 'SunGravitation::doCompute()'

}  // End of namespace 'gpstk'

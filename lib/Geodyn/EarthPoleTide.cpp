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


/**
* @file EarthPoleTide.cpp
* 
*/

#include "EarthPoleTide.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{

      /* Solid pole tide to normalized earth potential coefficients
       *
       * @param MJD_UTC UTC in MJD
       * @param dC21    correction to normalized coefficients dC21
       * @param dS21    correction to normalized coefficients dS21
       */
   void EarthPoleTide::getPoleTide(double MJD_UTC, double& dC21, double& dS21)
   {
      // See IERS Conventions 2003 section 7.1.4, P84

      CommonTime UTC( MJD(MJD_UTC).convertToCommonTime() );
      UTC.setTimeSystem(TimeSystem::UTC);

      // x offset and it's rate in 2000
      const double xp0 = 0.054;      // in arcsec
      const double dxp0 = 0.00083;   // in arcsec/year

      // y offset and it's rate in 2000
      const double yp0 = 0.357;      // in arcsec
      const double dyp0 = 0.00395;   // in arcsec/year

      // UTC
      double leapYear = (MJD_UTC - MJD_J2000) / 365.25;

      double xpm = xp0 + leapYear * dxp0;
      double ypm = yp0 + leapYear * dyp0;

      double xp = PolarMotionX(UTC);  // in arcsec
      double yp = PolarMotionY(UTC);  // in arcsec

      double m1 =  xp - xpm;
      double m2 = -yp + ypm;

      // See IERS Conventions 2003 section 6.2, P65
      // Correction to normalized earth potential coefficients
      // C21 and S21
      dC21 = -1.333e-9 * ( m1 - 0.0115 * m2 );
      dS21 = -1.333e-9 * ( m2 + 0.0115 * m1 );

   }  // End of method 'EarthPoleTide::getPoleTide()'

}  // End of namespace 'gpstk'

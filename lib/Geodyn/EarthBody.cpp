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
* @file EarthBody.cpp
* Class to handle earth planet, it'll be taken as the central
* body of the spacecraft.
*/

#include "EarthBody.hpp"
#include "Epoch.hpp"

namespace gpstk
{
    // Returnts the dynamic Earth rotation rate.
    double EarthBody::getSpinRate(CommonTime utc)
    {
        // UT1
        CommonTime ut1( pRefSys->UTC2UT1(utc) );

        // UT1 in MJD
        double MJD_UT1( MJD(ut1).mjd );

        double MJD_T0 = std::floor(MJD_UT1);

        double Tu   = (MJD_T0  - MJD_J2000) / 36525.0;

        return (7292115.8553e-11 + 4.3e-15 * Tu);

    }  // End of method 'EarthBody::getSpinRate()'

}  // End of namespace 'gpstk'

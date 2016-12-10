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
 * @file GNSSOrbit2.cpp
 * This Class handles GNSS Orbit.
 */

#include "GNSSOrbit2.hpp"
#include "Epoch.hpp"


using namespace std;


namespace gpstk
{
    // get derivative dy/dt
    Vector<double> GNSSOrbit2::getDerivatives(const double&         t,
                                              const Vector<double>& y)
    {
        // current epoch
        CommonTime utc(utc0+t);
        sc.setCurrentTime(utc);

        // current state
        sc.setCurrentState(y);

        // current acceleration and partial derivatives
        Vector<double> a(3,0.0);

        Vector<double> a_egm(3,0.0);
        Vector<double> a_thd(3,0.0);
        Vector<double> a_srp(3,0.0);

        if(pEGM != NULL)
        {
            pEGM->doCompute(utc,eb,sc);
            a += pEGM->getAcceleration();

            a_egm = pEGM->getAcceleration();
        }

        if(pThd != NULL)
        {
            pThd->doCompute(utc,eb,sc);
            a += pThd->getAcceleration();

            a_thd = pThd->getAcceleration();
        }

        if(pSRP != NULL)
        {
            pSRP->doCompute(utc,eb,sc);
            a += pSRP->getAcceleration();

            a_srp = pSRP->getAcceleration();
        }

        // v
        Vector<double> v( sc.getCurrentVel() );

        Vector<double> dy(6,0.0);

        for(int i=0; i<3; ++i)
        {
            // v
            dy(i+0) = v(i);

            // a
            dy(i+3) = a(i);
        }

        return dy;
    }

}  // End of namespace 'gpstk'

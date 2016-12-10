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
 * @file OrbitPrediction.cpp
 * This Class handles Orbit Prediction.
 */

#include "OrbitPrediction.hpp"


using namespace std;


namespace gpstk
{

    /* Orbit Integration.
     *
     * @return  predicted orbit and partials
     *
     */
    Matrix<double> OrbitPrediction::OrbitInt()
    {
        // Number of Paramemters
        int np( sc.getNumOfParam() );

        // Matrix of Prediction
        int point( int(arcLen*3600/arcInt) );
        Matrix<double> orbit(3*point,1+6+np, 0.0);


        // Current Time
        double t(0.0);

        // Current State
        Vector<double> y = sc.getCurrentState();

        vector< double > t_curr;
        vector< Vector<double> > y_curr;

        t_curr.push_back(t);
        y_curr.push_back(y);

        // Step Size
        double size_rkf78( pRKF78->getStepSize() );
        double size_adams( pAdams->getStepSize() );

        int count(0);

        // RKF78
        for(int i=0; i<8; ++i)
        {
            // OI
            pRKF78->integrateTo(t, y, pOrbit, t+size_adams);

            t_curr.push_back(t);
            y_curr.push_back(y);

            // y = (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
            if(int(t)%int(arcInt) == 0)
            {
                for(int j=0; j<3; ++j)
                {
                    // r
                    orbit(3*count+j,0) = y(j);

                    // dr/dr0, dr/dv0
                    for(int k=0; k<3; ++k)
                    {
                        orbit(3*count+j,1+k) = y( 6+3*j+k);
                        orbit(3*count+j,4+k) = y(15+3*j+k);
                    }

                    // dr/dp0
                    for(int k=0; k<np; ++k)
                    {
                        orbit(3*count+j,7+k) = y(42+np*i+k);
                    }
                }

                count += 1;
            }
        }


        // Adams
        while(t < arcLen*3600)
        {
            // OI
            pAdams->integrateTo(t_curr, y_curr, pOrbit, t+size_adams);

            t = t_curr[8];
            y = y_curr[8];

            // y = (r, v, dr/dr0, dr/dv0, dv/dr0, dv/dv0, dr/dp0, dv/dp0)
            if(int(t)%int(arcInt) == 0)
            {
                for(int j=0; j<3; ++j)
                {
                    // r
                    orbit(3*count+j,0) = y(j);

                    // dr/dr0, dr/dv0
                    for(int k=0; k<3; ++k)
                    {
                        orbit(3*count+j,1+k) = y( 6+3*j+k);
                        orbit(3*count+j,4+k) = y(15+3*j+k);
                    }

                    // dr/dp0
                    for(int k=0; k<np; ++k)
                    {
                        orbit(3*count+j,7+k) = y(42+np*j+k);
                    }
                }

                count += 1;

            }
        }

        // Return
        return orbit;

    }   // End of method 'OrbitPrediction::OrbitInt()'


}  // End of namespace 'gpstk'

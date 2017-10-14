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
 * @file AdamsIntegrator.cpp
 * This class implements Adams Bashforth Moulton 8-th order algorithm.
 */

#include "AdamsIntegrator.hpp"
#include "Epoch.hpp"

using namespace std;

namespace gpstk
{

    // coefficients, cbi
    const double AdamsIntegrator::cb[9] =
    {
        14097247.0,
      - 43125206.0,
        95476786.0,
      -139855262.0,
       137968480.0,
      - 91172642.0,
        38833486.0,
      -  9664106.0,
         1070017.0,
    };

    // coefficients, cmi
    const double AdamsIntegrator::cm[9] =
    {
       1070017.0,
       4467094.0,
      -4604594.0,
       5595358.0,
      -5033120.0,
       3146338.0,
      -1291214.0,
        312874.0,
      -  33953.0,
    };


    /// Real implementation of Adams
    satVectorMap AdamsIntegrator::integrateTo( const CommonTime& t_next )
    {

        satVectorMap y_next;

        // forward or backward
        double forward(1.0);
        if(t_curr[8] > t_next) forward = -1.0;

        double dt( std::abs(t_next - t_curr[8]) );

        if(dt != stepSize)
        {
            stepSize = std::abs(t_next - t_curr[8]);
            cerr << "Warning in AdamsIntegrator: the default stepSize has been changed !"
                 << endl;
        }

        // Derivatives at t_curr[i] and y_curr[i]
        vector<satVectorMap> dys;

        satVectorMap dy_temp;
        for(int i=0; i<9; ++i)
        {
            dy_temp = pEOM->getDerivatives(t_curr[i],y_curr[i]);
            dys.push_back(dy_temp);
        }


        SatID sat;

        // Prediction
        satVectorMap yp(y_curr[8]);
        for(int i=0; i<9; ++i)
        {
            for(satVectorMap::iterator it = yp.begin();
                it != yp.end();
                ++it)
            {
                sat = it->first;

                it->second += stepSize/3628800*cb[i]*(dys[8-i])[sat]*forward;
            }
        }


        // Derivatives at t(n+1), computed with yp
        CommonTime t_np1;
        t_np1 = t_curr[8] + stepSize*forward;

        satVectorMap dy_np1;
        dy_np1 = pEOM->getDerivatives(t_np1, yp);

        dys.erase( dys.begin() );
        dys.push_back( dy_np1 );

        // Correction
        satVectorMap yc(y_curr[8]);
        for(int i=0; i<9; ++i)
        {
            for(satVectorMap::iterator it = yc.begin();
                it != yc.end();
                ++it)
            {
                sat = it->first;

                it->second += stepSize/3628800*cm[i]*(dys[8-i])[sat]*forward;
            }
        }

        y_next = yc;


        Vector<double> diff(3,0.0);
        Vector<double> pred, corr;

        invalidSats.clear();

        for(satVectorMap::iterator it = yp.begin();
            it != yp.end();
            ++it)
        {
            sat = it->first;
            pred = it->second;

            corr = yc[sat];

            diff(0) = corr(0) - pred(0);
            diff(1) = corr(1) - pred(1);
            diff(2) = corr(2) - pred(2);

            if(norm(diff) > 1e-5)
            {
                invalidSats.push_back(sat);
            }
        }

        return y_next;
    }


}  // End of 'namespace gpstk'

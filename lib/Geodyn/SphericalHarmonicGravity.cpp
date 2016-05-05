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
//  Kaifa Kuang - Wuhan University . 2015
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
* @file SphericalHarmonicGravity.cpp
* 
*/

#include "SphericalHarmonicGravity.hpp"

using namespace std;

namespace gpstk
{

    /** Evaluate the fully normalized associated legendre functions and its gradients.
     * @param degree    Desired degree
     * @param lat       Geocentric latitude in radians
     * @param leg       Fully normalized associated legendre functions (fnALF)
     * @param leg1      1st-order derivatives of fnALF
     * @param leg2      2nd-order derivatives of fnALF
     *
     * Ref: E.Fantino, J Geod(2009), Methods of harmonic synthesis for global
     * geopotential models and their first-, second- and third-order gradients
     *
     */
    void SphericalHarmonicGravity::legendre(int degree, double lat, Vector<double>& leg, Vector<double>& leg1, Vector<double>& leg2)
    {
        // sine, cosine and tangent of latitude
        double slat, clat, tlat;
        slat = std::sin(lat);
        clat = std::cos(lat);
        tlat = std::tan(lat);

        int size = index(degree,degree);
        leg.resize(size, 0.0);
        leg1.resize(size, 0.0);
        leg2.resize(size, 0.0);

        leg(0) = 1.0;     // P00
/*
        cout << "size: " << size << endl;
        cout << "sin(lat): " << slat << endl;
        cout << "cos(lat): " << clat << endl;
        cout << "tan(lat): " << tlat << endl;
*/
        // first, the leg
        for(int n=1; n<=degree; n++)
        {
            // Kronecker's delta
            double delta = ( (1==n) ? 1.0 : 0.0 );

            for(int m=0; m<=n; m++)
            {
                // sectorials
                if(m == n)
                {
                    double fn = std::sqrt((1.0+delta)*(2*n+1.0)/(2*n));

                    leg( index(n,m)-1 ) = fn*clat*leg( index(n-1,m-1)-1 );
                }
                // zonals and tesserals
                else
                {
                    double gnm = std::sqrt((2*n+1.0)*(2*n-1.0)/(n+m)/(n-m));
                    double hnm = std::sqrt((2*n+1.0)*(n-m-1.0)*(n+m-1.0)/(2*n-3.0)/(n+m)/(n-m));
                    if(m == n-1)
                    {
                        leg( index(n,m)-1 ) = gnm*slat*leg( index(n-1,m)-1 );
                    }
                    else
                    {
                        leg( index(n,m)-1 ) = gnm*slat*leg( index(n-1,m)-1 ) - hnm*leg( index(n-2,m)-1 );
                    }
                }
            }
        }
/*
        cout << "leg: " << endl;
        for(int i=0; i<size; i++)
        {
            cout << setw(12) << setprecision(8) << leg(i) << endl;
        }
*/

        // then, the leg1
        for(int n=0; n<=degree; n++)
        {
            for(int m=0; m<=n; m++)
            {
                // Kronecker's delta
                double delta = ( (0 == m) ? 1.0 : 0.0 );

                double knm = std::sqrt((2.0-delta)*(n-m)*(n+m+1.0)/2);

                if(m == n)
                {
                    leg1( index(n,m)-1 ) = -m*tlat*leg( index(n,m)-1 );
                }
                else
                {
                    leg1( index(n,m)-1 ) = knm*leg( index(n,m+1)-1 ) - m*tlat*leg( index(n,m)-1 );
                }
            }
        }
/*
        cout << "leg1: " << endl;
        for(int i=0; i<size; i++)
        {
            cout << setw(12) << setprecision(8) << leg1(i) << endl;
        }
*/

        // then, the leg2
        for(int n=0; n<=degree; n++)
        {
            for(int m=0; m<=n; m++)
            {
                leg2( index(n,m)-1 ) = (m*m/clat/clat-n*(n+1))*leg( index(n,m)-1 ) + tlat*leg1( index(n,m)-1 );
            }
        }
/*
        cout << "leg2: " << endl;
        for(int i=0; i<size; i++)
        {
            cout << setw(12) << setprecision(8) << leg2(i) << endl;
        }
*/

    }   // End of method "SphericalHarmonicGravity::legendre()"

}  // End of namespace 'gpstk'

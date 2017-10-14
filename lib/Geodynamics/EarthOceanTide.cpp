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
//  kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
* @file EarthOceanTide.cpp
*
*/

#include "EarthOceanTide.hpp"
#include "constants.hpp"
#include "StringUtils.hpp"
#include "Legendre.hpp"
#include "Epoch.hpp"


using namespace std;
using namespace gpstk::StringUtils;


namespace gpstk
{

    // Load Ocean Tide file
    void EarthOceanTide::loadFile(const string& file)
        throw(FileMissingException)
    {
        ifstream inpf(file.c_str());

        if(!inpf)
        {
            FileMissingException fme("Could not open Ocean Tide file " + file);
            GPSTK_THROW(fme);
        }

        // First, file header
        string temp;
        for(int i=0; i<4; ++i)
            getline(inpf,temp);

        bool ok(true);

        // Then, file data
        string line;
        while( !inpf.eof() && inpf.good() )
        {
            getline(inpf,line);
            stripTrailing(line,'\r');

            if( inpf.eof() ) break;

            if( inpf.bad() ) { ok = false; break; }

            OceanTideData data;

            // Doodson number, coded into Doodson variable multipliers
            data.n[0]   =  asInt( line.substr(0,1) );
            data.n[1]   =  asInt( line.substr(1,1) ) - 5;
            data.n[2]   =  asInt( line.substr(2,1) ) - 5;
            data.n[3]   =  asInt( line.substr(4,1) ) - 5;
            data.n[4]   =  asInt( line.substr(5,1) ) - 5;
            data.n[5]   =  asInt( line.substr(6,1) ) - 5;
            // Darwin's symbol
            data.Darw   =  line.substr(8,3);
            // Degree, Order
            data.l      =  asInt( line.substr(12,3) );
            data.m      =  asInt( line.substr(16,3) );

            // Coefficients
            data.DelCp  =  asDouble( line.substr(21,8) )*1e-12;
            data.DelSp  =  asDouble( line.substr(31,8) )*1e-12;
            data.DelCm  =  asDouble( line.substr(43,8) )*1e-12;
            data.DelSm  =  asDouble( line.substr(53,8) )*1e-12;

            if(data.l <= desiredDegree)
            {
                otDataVec.push_back(data);
            }
            else
            {
                continue;
            }
        }

        inpf.close();

        if( !ok )
        {
            FileMissingException fme("Ocean Tide file "
                                    + file
                                    + " is corrupted or in wrong format.");
            GPSTK_THROW(fme);
        }

    }  // End of method "EarthOceanTide::loadFile()"


    /* Ocean pole tide to normalized earth potential coefficients
     *
     * @param tt    TT
     * @return      correction to normalized Cnm and Snm
     */
    Matrix<double> EarthOceanTide::getOceanTide(CommonTime tt)
    {
        // resize dCS
        int size = indexTranslator(desiredDegree,desiredOrder);
        Matrix<double> dCS(size,2, 0.0);

        // UTC
        CommonTime utc( pRefSys->TT2UTC(tt) );

        // UT1
        CommonTime ut1( pRefSys->UTC2UT1(utc) );

        // Doodson arguments
        double BETA[6] = {0.0};
        double FNUT[5] = {0.0};

        DoodsonArguments(ut1, tt, BETA, FNUT);

        vector<OceanTideData>::const_iterator itr;
        for(itr=otDataVec.begin(); itr!=otDataVec.end(); ++itr)
        {
            // theta_f
            double theta_f = (*itr).n[0]*BETA[0] + (*itr).n[1]*BETA[1]
                           + (*itr).n[2]*BETA[2] + (*itr).n[3]*BETA[3]
                           + (*itr).n[4]*BETA[4] + (*itr).n[5]*BETA[5];

            // sine and cosine of theta_f
            double stf = std::sin(theta_f);
            double ctf = std::cos(theta_f);

            // index
            int id = indexTranslator((*itr).l, (*itr).m) - 1;

            // coefficients
            double DelCp = (*itr).DelCp;
            double DelSp = (*itr).DelSp;
            double DelCm = (*itr).DelCm;
            double DelSm = (*itr).DelSm;

            // corrections
            dCS(id, 0) += (DelCp + DelCm)*ctf + (DelSp + DelSm)*stf;
            dCS(id, 1) += (DelSp - DelSm)*ctf - (DelCp - DelCm)*stf;

        }  // End of 'for()'

        return dCS;

    }  // End of method 'EarthOceanTide::getOceanTide()'

}  // End of namespace 'gpstk'

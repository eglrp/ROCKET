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
 * @file EGM08Model.hpp
 */

#ifndef EGM08_MODEL_HPP
#define EGM08_MODEL_HPP

#include "EGMModel.hpp"

namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** EGM08 Model.
     *
     */
    class EGM08Model : public EGMModel
    {
    public:
        /** Constructor
         * @param n    Desired degree
         * @param m    Desired order
         */
        EGM08Model (int n = 0, int m = 0)
            : EGMModel(n, m)
        {
            // model name
            egmData.modelName = "EGM08";

            // earth gravitation constant
            egmData.GM = 3.9860044150e+14;

            // radius
            egmData.ae = 6378136.3;

            // tide free
            egmData.includesPermTide = false;

            // reference epoch
            egmData.refMJD =  51544.5;

        };  // End of constructor


        /// Default destructor
        virtual ~EGM08Model() {};


        /// Load file
        void loadFile(std::string file)
            throw(FileMissingException);


        /** Compute acceleration (and related partial derivatives) of Earth
         *  Gravitation.
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits );


        /// Return the force model name
        inline virtual std::string modelName() const
        { return egmData.modelName; }


    }; // End of class 'EGM08Model'

    // @}

}  // End of namespace 'gpstk'

#endif   // EGM08_MODEL_HPP

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

/**
 * @file CODEPressure2.hpp
 * Class to do CODE Pressure calculation without a prior model.
 */

#ifndef CODE_PRESSURE2_HPP
#define CODE_PRESSURE2_HPP

#include "SolarPressure.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do CODE Pressure calculation without a prior model.
     */
    class CODEPressure2 : public SolarPressure
    {
    public:

        /// Default constructor
        CODEPressure2() {}

        /// Default destructor
        virtual ~CODEPressure2() {}


        /** Compute acceleration (and related partial derivatives) of CODE
         *  Pressure without a prior model.
         * @param utc     time in UTC
         * @param rb      earth body
         * @param sc      spacecraft
         */
        virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc);

        /// Return the force model name
        inline virtual std::string modelName() const
        { return "CODEPressure2"; }

    }; // End of class 'CODEPressure2'

    // @}

}  // End of namespace 'gpstk'

#endif   // CODE_PRESSURE2_HPP

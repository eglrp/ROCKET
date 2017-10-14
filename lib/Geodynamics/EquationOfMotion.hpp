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
//  Kaifa Kuang - Wuhan University . 2017
//
//============================================================================


/**
 * @file EquationOfMotion.hpp
 *
 * Class provides the mechanism for passing a method
 * that computes the derivatives to an integrator.
 */

#ifndef EQUATION_OF_MOTION_HPP
#define EQUATION_OF_MOTION_HPP

#include "CommonTime.hpp"
#include "DataStructures.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /**
     * The EquationsOfMotion provides the mechanism for passing a method
     * that computes the derivatives to an integrator.
     *
     */
    class EquationOfMotion
    {
    public:
        /// Default constructor
        EquationOfMotion() {};

        /// Default deconstructor
        virtual ~EquationOfMotion() {};

        /** Compute the derivatives.
         * @params t    time or the independent variable.
         * @params y    the required data.
         * @return      the derivatives.
         */
        virtual satVectorMap getDerivatives( const CommonTime&   time,
                                             const satVectorMap& states ) = 0;

    }; // End of class 'EquationOfMotion'

    // @}

}  // End of namespace 'gpstk'

#endif  // EQUATION_OF_MOTION_HPP

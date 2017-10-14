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

/**
 * @file Relativity.hpp
 * Class to do Relativity calculation.
 */

#ifndef RELATIVITY_HPP
#define RELATIVITY_HPP

#include "ForceModel.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Relativity calculation.
     * see IERS Conventions 2010 Section 10.3 for more details.
     *
     */
    class Relativity : public ForceModel
    {
    public:
        /// Default constructor
        Relativity() {};

        /// Default destructor
        virtual ~Relativity() {};

        /** Compute acceleration (and related partial derivatives).
         * @param tt        TT
         * @param orbits    orbits
         */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits );

        /// Return the force model name
        inline virtual std::string modelName() const
        { return "Relativity"; }

    }; // End of class 'Relativity'

    // @}

}  // End of namespace 'gpstk'

#endif  // RELATIVITY_HPP

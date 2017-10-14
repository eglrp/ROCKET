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
 * @file ERPModel.hpp
 * Class to do ERP calculation.
 */

#ifndef ERP_MODEL_HPP
#define ERP_MODEL_HPP

#include "ForceModel.hpp"
#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do ERP calculation.
     */
    class ERPModel : public ForceModel
    {
    public:

        /// Default constructor
        ERPModel() {};

        /// Default destructor
        virtual ~ERPModel() {};


        /// Set reference system
        inline ERPModel& setReferenceSystem(ReferenceSystem& ref)
        { pRefSys = &ref; return (*this); };

        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        { return pRefSys; };


        /// Set solar system
        inline ERPModel& setSolarSystem(SolarSystem& sol)
        { pSolSys = &sol; return (*this); };

        /// Get solar system
        inline SolarSystem* getSolarSystem() const
        { return pSolSys; };


        /// Return the force model name
        inline virtual std::string modelName() const
        { return "ERPModel"; }

        /** Compute acceleration (and related partial derivatives) of ERP.
         * @param tt        TT
         * @param orbits    orbits
          */
        virtual void Compute( const CommonTime&   tt,
                              const satVectorMap& orbits ) = 0;


    protected:

        /// Reference System
        ReferenceSystem* pRefSys;

        /// Solar System
        SolarSystem* pSolSys;

        /// SRP Coefficients
        satVectorMap satSRPCoeff;

    }; // End of class 'ERPModel'

    // @}

}  // End of namespace 'gpstk'

#endif   // ERP_MODEL_HPP

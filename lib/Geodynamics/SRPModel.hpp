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

/**
 * @file SRPModel.hpp
 * Class to do SRP calculation.
 */

#ifndef SRP_MODEL_HPP
#define SRP_MODEL_HPP

#include "ForceModel.hpp"
#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do SRP calculation.
     */
    class SRPModel : public ForceModel
    {
    public:

        enum ShadowModel
        {
            SM_CYLINDRICAL,      // cylindrical
            SM_CONICAL           // conical
        };

    public:

        /// Default constructor
        SRPModel() {};

        /// Default destructor
        virtual ~SRPModel() {};


        /** Determines if the satellite is in sunlight or shadow.
         * Taken from Montenbruck and Gill p. 80-83
         * @param r       position of spacecraft [m]
         * @param r_Sun   position of Sun (wrt Earth) [m]
         * @param r_Moon  position of Moon (wrt Earth) [m]
         * @return        0.0 if in shadow
         *                1.0 if in sunlight
         *                0.0 to 1.0 if in partial shadow
         */
        double getShadowFunction(Vector<double> r,
                                 Vector<double> r_Sun,
                                 Vector<double> r_Moon,
                                 SRPModel::ShadowModel sm = SM_CONICAL);


        /// Set reference system
        inline SRPModel& setReferenceSystem(ReferenceSystem& ref)
        { pRefSys = &ref; return (*this); };

        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        { return pRefSys; };


        /// Set solar system
        inline SRPModel& setSolarSystem(SolarSystem& sol)
        { pSolSys = &sol; return (*this); };

        /// Get solar system
        inline SolarSystem* getSolarSystem() const
        { return pSolSys; };


        /// Set SRP coefficients
        inline SRPModel& setSRPCoeff(const satVectorMap& coeffs)
        { satSRPCoeff = coeffs; return (*this); };


        /// Get SRP coefficients
        Vector<double> getSRPCoeff(const SatID& sat) const
            throw(SatIDNotFound)
        {
            satVectorMap::const_iterator it( satSRPCoeff.find(sat) );

            if( it != satSRPCoeff.end() )
            {
                return (*it).second;
            }
            else
            {
                GPSTK_THROW(SatIDNotFound("SatID not found in map"));
            }
        };


        /// Return the force model name
        inline virtual std::string modelName() const
        { return "SRPModel"; }

        /** Compute acceleration (and related partial derivatives) of SRP.
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

    }; // End of class 'SRPModel'

    // @}

}  // End of namespace 'gpstk'

#endif   // SRP_MODEL_HPP

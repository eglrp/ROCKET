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
 * @file GlobalParameters.hpp
 * Class to do Global Parameters calculation.
 */

#ifndef GLOBAL_PARAMETERS_HPP
#define GLOBAL_PARAMETERS_HPP

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Global Parameters calculation.
     */
    class GlobalParameters
    {
    public:

        /// Default constructor
        GlobalParameters() {}

        /// Default destructor
        virtual ~GlobalParameters() {}


        /// Set reference epoch
        inline GlobalParameters& setRefEpoch(const CommonTime& utc)
        {
            utc0 = utc;

            return (*this);
        }

        /// Get reference epoch
        inline CommonTime getRefEpoch() const
        {
            return utc0;
        }


        /// Set reference system
        inline GlobalParameters& setReferenceSystem(ReferenceSystem& ref)
        {
            pRefSys = &ref;

            return (*this);
        }

        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        {
            return pRefSys;
        }


        /// Set solar system
        inline GlobalParameters& setSolarSystem(SolarSystem& sol)
        {
            pSolSys = &sol;

            return (*this);
        }

        /// Get solar system
        inline SolarSystem* getSolarSystem() const
        {
            return pSolSys;
        }


        /// Get dR_dERP
        inline Matrix<double> dR_dERP() const
        {
            return dr_derp;
        }

        /// Get dR_dHP
        inline Matrix<double> dR_dHP() const
        {
            return dr_dhp;
        }


        /** Compute partial derivatives of Satellites Position in ICRS
         *  wrt Global Parameters.
         *
         * @param utc     time in utc
         * @param eb      earth body
         * @param sc      spacecraft
         *
         */
        void Process(const CommonTime&     utc,
                     const EarthBody&      eb,
                     const Spacecraft&     sc  );


        /** Compute partial derivatives of Satellite Position in ICRS
         *  wrt Earth Rotation Parameters.
         *
         * @param utc   time in utc
         * @param eb    earth body
         * @param sc    spacecraft
         *
         */
        void PartialsOfERP(const CommonTime&     utc,
                           const EarthBody&      eb,
                           const Spacecraft&     sc  );


        /** Compute partial derivatives of Satellite Position in ICRS
         *  wrt Helmert Parameters.
         *
         * @param utc   time in utc
         * @param eb    earth body
         * @param sc    spacecraft
         *
         */
        void PartialsOfHP (const CommonTime&     utc,
                           const EarthBody&      eb,
                           const Spacecraft&     sc  );


    private:

        /// Reference Epoch
        CommonTime utc0;

        /// Reference System
        ReferenceSystem* pRefSys;

        /// Solar System
        SolarSystem* pSolSys;

        /// Partial Derivatives of Satellite Position in ICRS
        /// wrt Earth Rotation Parameters
        Matrix<double> dr_derp;     // 3*6

        /// Partial Derivatives of Satellite Position in ICRS
        /// wrt Helmert Parameters
        Matrix<double> dr_dhp;      // 3*7


    };  // End of class 'GlobalParameters'

    // @}

}  // End of namespace 'gpstk'

#endif  // GLOBAL_PARAMETERS_HPP

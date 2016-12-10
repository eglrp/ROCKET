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
//  Copyright
//
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
 * @file GNSSOrbit.hpp
 * Class to do GNSS Orbit configuration.
 */

#ifndef GNSS_ORBIT_HPP
#define GNSS_ORBIT_HPP

#include "EquationOfMotion.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"

#include "EGM08GravityModel.hpp"
#include "ThirdBody.hpp"
#include "CODEPressure.hpp"
#include "RelativityEffect.hpp"

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do GNSS Orbit configuration.
     *
     */
    class GNSSOrbit : public EquationOfMotion
    {
    public:

        /// Default constructor
        GNSSOrbit()
        {}

        /// Default destructor
        virtual ~GNSSOrbit()
        {}

        /// Set reference epoch
        inline GNSSOrbit& setRefEpoch(CommonTime utc)
        {
            utc0 = utc;

            return (*this);
        }


        /// Get reference epoch
        inline CommonTime getRefEpoch() const
        {
            return utc0;
        }

        /// Set spacecraft
        inline GNSSOrbit& setSpacecraft(const Spacecraft& s)
        {
            sc = s;

            return *this;
        }

        /// Get spacecraft
        inline Spacecraft getSpacecraft() const
        {
            return sc;
        }


        /// Set earth gravitation
        inline GNSSOrbit& setEarthGravitation(EarthGravitation& egm)
        {
            pEGM = &egm;

            return (*this);
        }

        /// Get earth gravitation
        inline EarthGravitation* getEarthGravitation() const
        {
            return pEGM;
        }


        /// Set third body
        inline GNSSOrbit& setThirdBody(ThirdBody& thd)
        {
            pThd = &thd;

            return (*this);
        }

        /// Get third body
        inline ThirdBody* getThirdBody() const
        {
            return pThd;
        }


        /// Set solar pressure
        inline GNSSOrbit& setSolarPressure(SolarPressure& srp)
        {
            pSRP = &srp;

            return (*this);
        }

        /// Get solar pressure
        inline SolarPressure* getSolarPressure() const
        {
            return pSRP;
        }


        /// Set relativity effect
        inline GNSSOrbit& setRelativityEffect(RelativityEffect& rel)
        {
            pRel = &rel;

            return (*this);
        }

        /// Get relativity effect
        inline RelativityEffect* getRelativityEffect() const
        {
            return pRel;
        }


        /// Get derivatives
        virtual Vector<double> getDerivatives(const double&         t,
                                              const Vector<double>& y);

    private:

        /// Reference epoch
        CommonTime utc0;

        /// Spacecraft object
        Spacecraft sc;

        /// EarthBody object
        EarthBody eb;

        /// Force models
        EarthGravitation*   pEGM;
        ThirdBody*          pThd;
        SolarPressure*      pSRP;
        RelativityEffect*   pRel;

    }; // End of class 'GNSSOrbit'

}  // End of namespace 'gpstk'

#endif   // GNSS_ORBIT_HPP

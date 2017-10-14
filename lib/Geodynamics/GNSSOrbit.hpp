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

#include "EGM08Model.hpp"
#include "ThirdBody.hpp"
#include "ECOMModel.hpp"
#include "ECOM1Model.hpp"
#include "ECOM2Model.hpp"
#include "Relativity.hpp"

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
            : pEGM(NULL), pThd(NULL),
              pSRP(NULL), pRel(NULL)
        {};

        /// Default destructor
        virtual ~GNSSOrbit() {};


        /// Set EGM Model
        inline GNSSOrbit& setEGMModel(EGMModel& egm)
        { pEGM = &egm; return (*this); };

        /// Get EGM Model
        inline EGMModel* getEGMModel() const
        { return pEGM; };


        /// Set third body
        inline GNSSOrbit& setThirdBody(ThirdBody& thd)
        { pThd = &thd; return (*this); };

        /// Get third body
        inline ThirdBody* getThirdBody() const
        { return pThd; };


        /// Set SRP Model
        inline GNSSOrbit& setSRPModel(SRPModel& srp)
        { pSRP = &srp; return (*this); };

        /// Get SRP Model
        inline SRPModel* getSRPModel() const
        { return pSRP; };


        /// Set relativity
        inline GNSSOrbit& setRelativity(Relativity& rel)
        { pRel = &rel; return (*this); };

        /// Get relativity
        inline Relativity* getRelativity() const
        { return pRel; };


        /// Get derivatives
        virtual satVectorMap getDerivatives( const CommonTime&   tt,
                                             const satVectorMap& states );

    private:

        /// Force models
        EGMModel*       pEGM;
        ThirdBody*      pThd;
        SRPModel*       pSRP;
        Relativity*     pRel;

    }; // End of class 'GNSSOrbit'

}  // End of namespace 'gpstk'

#endif   // GNSS_ORBIT_HPP

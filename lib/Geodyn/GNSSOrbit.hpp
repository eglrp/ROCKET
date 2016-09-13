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

#ifndef GNSSORBIT_HPP
#define GNSSORBIT_HPP

#include "EquationOfMotion.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"

#include "EGM08GravityModel.hpp"
#include "MoonGravitation.hpp"
#include "SunGravitation.hpp"
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
      {
      }

         /// Default destructor
      virtual ~GNSSOrbit()
      {
      }

         /// Set reference epoch
      inline GNSSOrbit& setRefEpoch(CommonTime utc)
      {
         utc0 = utc;

         return (*this);
      }

         /// get reference epoch
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


         /// Set sun gravitation
      inline GNSSOrbit& setSunGravitation(SunGravitation& sun)
      {
         pSun = &sun;

         return (*this);
      }

         /// Get sun gravitation
      inline SunGravitation* getSunGravitation() const
      {
         return pSun;
      }


         /// Set moon gravitation
      inline GNSSOrbit& setMoonGravitation(MoonGravitation& moon)
      {
         pMoon = &moon;

         return (*this);
      }

         /// Get moon gravitation
      inline MoonGravitation* getMoonGravitation() const
      {
         return pMoon;
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
      EarthGravitation* pEGM;
      SunGravitation*   pSun;
      MoonGravitation*  pMoon;
      SolarPressure*    pSRP;
      RelativityEffect* pRel;

      // Force models configuration
      bool bEGM;
      bool bSun;
      bool bMoon;
      bool bSRP;
      bool bRel;

   }; // End of class 'GNSSOrbit'

}  // End of namespace 'gpstk'

#endif   // GNSSORBIT_HPP

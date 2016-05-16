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
 * This Class handles GNSS orbit.
 */

#ifndef GNSSORBIT_HPP
#define GNSSORBIT_HPP

#include "EquationOfMotion.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"
#include "ForceModelList.hpp"

#include "EGM08GravityModel.hpp"
#include "SunForce.hpp"
#include "MoonForce.hpp"
#include "SolarPressure.hpp"
#include "ROCKPressure.hpp"
#include "CODEPressure.hpp"
#include "RelativityEffect.hpp"

namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

  
      /** This Class handles GNSS orbit.
       * 
       */
   class GNSSOrbit : public EquationOfMotion
   {
   public:

         /// Default constructor
      GNSSOrbit() {}

         /// Default destructor
      virtual ~GNSSOrbit() {}

         /// Set reference epoch
      GNSSOrbit& setRefEpoch(CommonTime utc) { utc0 = utc; return (*this); }

         /// get reference epoch
      CommonTime getRefEpoch() const { return utc0; }

         /// Set spacecraft
      GNSSOrbit& setSpacecraft(const Spacecraft& s) { sc = s; return *this; }

         /// Get spacecraft
      Spacecraft getSpacecraft() const { return sc; }

         /// Get derivatives
      virtual Vector<double> getDerivatives(const double&         t,
                                            const Vector<double>& y);

   private:

         /// Reference epoch
      CommonTime utc0;

         /// Spacecraft object
      Spacecraft sc;

         /// Earth Body
      EarthBody eb;

         /// Force models
      EGM08GravityModel egm;
      SunForce          sun;
      MoonForce         moon;
      CODEPressure      srp;
      RelativityEffect  rel;

      // Force models configuration
      bool bGeoEarth;
      bool bGeoSun;
      bool bGeoMoon;
      bool bSRP;
      bool bREL;

   }; // End of class 'GNSSOrbit'

}  // End of namespace 'gpstk'

#endif   // GNSSORBIT_HPP

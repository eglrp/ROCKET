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
//  Kaifa Kuang - Wuhan University . 2015
//
//============================================================================

/**
 * @file ROCKPressure.hpp
 * This class provides a ROCK model for the influence
 * of solar radiation pressure on a satellite.
 */

#ifndef GPSTK_ROCK_PRESSURE_HPP
#define GPSTK_ROCK_PRESSURE_HPP

#include "SolarPressure.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * This class provides a ROCK model for the influence
       * of solar radiation pressure on a satellite.
       * 
       */
   class ROCKPressure : public SolarPressure
   {
   public:
         /// Default constructor
      ROCKPressure() {}

         /// Default destructor
      virtual ~ROCKPressure() {}

         /// This is the real one do computation
      virtual void doCompute(CommonTime t, EarthBody& bRef, Spacecraft& sc);

         /// Return the force model name
      virtual std::string modelName() const
      { return "ROCKPressure"; }

   }; // End of class 'ROCKPressure'

      // @}

}  // End of namespace 'gsptk'

#endif   // GPSTK_ROCK_PRESSURE_HPP

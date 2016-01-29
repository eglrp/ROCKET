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
 * @file SimpleSRP.hpp
 * This class provides a simple model for the influence
 * of solar radiation pressure on a satellite.
 */

#ifndef GPSTK_SIMPLE_PRESSURE_HPP
#define GPSTK_SIMPLE_PRESSURE_HPP

#include "SolarPressure.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * This class provides a simple model for the influence
       * of solar radiation pressure on a satellite.
       * 
       */
   class SimplePressure : public SolarPressure
   {
   public:
         /// Default constructor
      SimplePressure() {}

         /// Default destructor
      virtual ~SimplePressure() {}

         // this is the real one
      virtual void doCompute(CommonTime t, EarthBody& bRef, Spacecraft& sc);

   }; // End of class 'SimplePressure'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SIMPLE_SRP_HPP

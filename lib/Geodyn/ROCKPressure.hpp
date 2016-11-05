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
 * Class to do ROCK Pressure calculation.
 */

#ifndef ROCK_PRESSURE_HPP
#define ROCK_PRESSURE_HPP

#include "SolarPressure.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** Class to do ROCK Pressure calculation.
       */
   class ROCKPressure : public SolarPressure
   {
   public:
         /// Default constructor
      ROCKPressure() {}

         /// Default destructor
      virtual ~ROCKPressure() {}

         /** Compute acceleration (and related partial derivatives) of ROCK
          *  Pressure.
          * @param utc      time in UTC
          * @param rb       earth body
          * @param sc       spacecraft
          */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc);

         /// Return the force model name
      virtual std::string modelName() const
      { return "ROCKPressure"; }

   }; // End of class 'ROCKPressure'

      // @}

}  // End of namespace 'gsptk'

#endif   // ROCK_PRESSURE_HPP

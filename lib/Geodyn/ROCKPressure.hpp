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
//  Kaifa Kuang - Wuhan University . 2015
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

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

         /// Return the force model index
      virtual int forceIndex() const
      { return FMI_SRP; }

   }; // End of class 'ROCKPressure'

      // @}

}  // End of namespace 'gsptk'

#endif   // GPSTK_ROCK_PRESSURE_HPP

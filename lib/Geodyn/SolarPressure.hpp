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

/**
 * @file AtmosphericDrag.hpp
 * This class computes the acceleration due to solar pressure on a satellite.
 */

#ifndef GPSTK_SOLAR_PRESSURE_HPP
#define GPSTK_SOLAR_PRESSURE_HPP

#include "ForceModel.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{
   
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * This class computes the acceleration due to solar pressure on a
       * satellite.
       *
       */
   class SolarPressure : public ForceModel
   {
   public:
       enum ShadowModel
       {
           SM_CYLINDRICAL,      // cylindrical
           SM_CONICAL           // conical
       };

   public:
         /** Determines if the satellite is in sunlight or shadow.
          * Taken from Montenbruck and Gill p. 80-83
          * @param r ECI position vector of spacecraft [m].
          * @param r_Sun position vector of Sun (geocentric) [m].
          * @param r_Moon position vector of Moon (geocentric) [m].
          * @return 0.0 if in shadow, 1.0 if in sunlight, 0.0 to 1.0 if in
          * partial shadow.
          */
       double getShadowFunction(Vector<double> r,
                                Vector<double> r_Sun,
                                Vector<double> r_Moon,
                                SolarPressure::ShadowModel sm = SM_CONICAL);

         /// Default constructor
      SolarPressure()
      {};

         /// Default destructor
      virtual ~SolarPressure() {};


         /// Return force model name
      virtual std::string modelName() const
      {return "SolarPressure";}

         /// return the force model index
      virtual int forceIndex() const
      { return FMI_SolarRadiationPressure; }
       
         /// this is the real one
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc) = 0;

   }; // End of class 'SolarPressure'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SOLAR_PRESSURE_HPP

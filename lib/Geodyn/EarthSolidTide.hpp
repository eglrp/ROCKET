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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
* @file EarthSolidTide.hpp
* Class to do Earth Solid Tide correction.
*/

#ifndef GPSTK_EARTH_SOLID_TIDE_HPP
#define GPSTK_EARTH_SOLID_TIDE_HPP

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** Class to do Earth Solid Tide correction
       * see IERS Conventions 2010 Section 6.2 for more details.
       */
   class EarthSolidTide
   {
   public:

         /// Default constructor
      EarthSolidTide()
         : pRefSys(NULL),
           pSolSys(NULL)
      {}

         /// Default destructor
      ~EarthSolidTide() {}


         /// Set reference system
      inline EarthSolidTide& setReferenceSystem(ReferenceSystem& ref)
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
      inline EarthSolidTide& setSolarSystem(SolarSystem& sol)
      {
         pSolSys = &sol;

         return (*this);
      }


         /// Get solar system
      inline SolarSystem* getSolarSystem() const
      {
         return pSolSys;
      }


         /** Solid tide to normalized earth potential coefficients
          *
          * @param utc     time in UTC
          * @param CS      normalized earth potential coefficients
          */
      void getSolidTide(CommonTime utc, Matrix<double>& CS);


   protected:

         /// Parameters
      static const double Argu_C20[21][7];
      static const double Argu_C21[48][7];
      static const double Argu_C22[2][6];

         /// Reference System
      ReferenceSystem* pRefSys;

         /// Solar System
      SolarSystem* pSolSys;
      
   }; // End of class 'EarthSolidTide'

      // @}

}  // End of namespace 'gpstk'

#endif   //GPSTK_EARTH_SOLID_TIDE_HPP

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
* @file EarthPoleTide.hpp
* Class to do Earth Pole Tide correction.
* 
*/

#ifndef GPSTK_EARTH_POLE_TIDE_HPP
#define GPSTK_EARTH_POLE_TIDE_HPP

#include "ReferenceSystem.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * Class to do Earth Pole Tide correction
       * see IERS Conventions 2010 Section 6.4 and 6.5 for more details.
       */
   class EarthPoleTide
   {
   public:
         /// Default constructor
      EarthPoleTide()
         : pRefSys(NULL)
      {}

         /// Default destructor
      ~EarthPoleTide() {}


         /// Set reference system
      inline EarthPoleTide& setReferenceSystem(ReferenceSystem& ref)
      {
         pRefSys = &ref;

         return (*this);
      }


         /// Get reference system
      inline ReferenceSystem* getReferenceSystem() const
      {
         return pRefSys;
      }


         /** Pole tide to normalized earth potential coefficients
          *
          * @param utc     time in UTC
          * @param CS      normalized earth potential coefficients
          */
      void getPoleTide(CommonTime utc, Matrix<double>& CS);

   protected:

         /// Reference System
      ReferenceSystem* pRefSys;


   }; // End of class 'EarthPoleTide'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EARTH_POLE_TIDE_HPP

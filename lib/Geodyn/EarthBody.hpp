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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
* @file EarthBody.hpp
* Class to handle earth planet, it'll be taken as the central
* body of the spacecraft.
*/

#ifndef GPSTK_EARTH_BODY_HPP
#define GPSTK_EARTH_BODY_HPP

#include "CommonTime.hpp"
#include "ReferenceSystem.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** Class to handle earth planet, it'll be taken as the central
       * body of the spacecraft.
       */
   class EarthBody
   {
   public:
         /// Default constructor
      EarthBody() {}

         /// Default destructor
      virtual ~EarthBody() {}

         /// Set reference system
      inline EarthBody& setReferenceSystem(ReferenceSystem& ref)
      {
         pRefSys = &ref;

         return (*this);
      }

      inline ReferenceSystem* getReferenceSystem() const
      {
         return pRefSys;
      }
      
         /**
          * Returnts the dynamic Earth rotation rate. 
          * @param utc  Time in UTC
          */
      virtual double getSpinRate(CommonTime utc);

   private:

      ReferenceSystem* pRefSys;

   }; // End of class 'EarthBody'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EARTH_BODY_HPP

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

/**
* @file EarthSolidTide.hpp
* Class to do Earth Solid Tide correction
*/

#ifndef GPSTK_EARTH_SOLID_TIDE_HPP
#define GPSTK_EARTH_SOLID_TIDE_HPP

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**Class to do Earth Solid Tide correction
       * see IERS Conventions 2010
       */
   class EarthSolidTide
   {
   public:

         /// Default constructor
      EarthSolidTide(){}

         /// Default destructor
      ~EarthSolidTide(){}


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

         /**
          * Solid tide to normalized earth potential coefficients
          *
          * @param utc     time in UTC
          * @param dC      correction to normalized coefficients dC
          * @param dS      correction to normalized coefficients dS
          */
      void getSolidTide(CommonTime utc, double dC[], double dS[]);


         /** Translate degree and order (n,m) from 2-D to 1-D.
          * @param n    Degree
          * @param m    Order
          *
          * For example:
          * (n,m) = (0,0) <===> return = 1
          * (n,m) = (1,0) <===>        = 2
          * (n,m) = (1,1) <===>        = 3
          * (n,m) = (2,0) <===>        = 4
          *          ...                ...
          */
      inline int index(int n, int m)
      {
         return n*(n+1)/2 + (m+1);
      }

         /** Evaluate the fnALF.
          * @param deg  Desired degree
          * @param lat  Geocentric latitude in radians
          * @param leg  Fully nomalized associated legendre functions
          */
      void legendre(const int& deg, const double& lat, Vector<double>& leg);
      

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

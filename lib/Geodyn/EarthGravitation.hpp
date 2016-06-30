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
* @file EarthGravitation.hpp
* Class to do Earth Gravitation calculation.
* 
*/

#ifndef GPSTK_EARTH_GRAVITATION_HPP
#define GPSTK_EARTH_GRAVITATION_HPP

#include "ForceModel.hpp"
#include "ReferenceSystem.hpp"
#include "EarthSolidTide.hpp"
#include "EarthOceanTide.hpp"
#include "EarthPoleTide.hpp"


namespace gpstk
{
   /** @addtogroup GeoDynamics */
   //@{

       /** Class to do Earth Gravitation calculation.
       */
   class EarthGravitation : public ForceModel
   {
   public:

         /// Struct to hold Gravity Model Data
      struct GravityModelData
      {
         std::string modelName;

         double GM;
         double ae;

         bool includesPermTide;

         double refMJD;

         int maxDegree;

         Matrix<double> normalizedCS;

      };

   public:

         /** Constructor.
          * @param n   Desired degree
          * @param m   Desired order
          */
      EarthGravitation(int n=0, int m=0)
         : desiredDegree(n),
           desiredOrder(m),
           pRefSys(NULL),
           pSolidTide(NULL),
           pOceanTide(NULL),
           pPoleTide(NULL)
      {
         int size = desiredDegree*(desiredDegree+1)/2 + (desiredOrder+1);

         gmData.normalizedCS.resize(size,4,0.0);
      }


         /// Default destructor
      virtual ~EarthGravitation() {}


         /// Load file
      virtual void loadFile(const std::string& file)
         throw(FileMissingException)
      {}


         /// Set desired degree and order
      inline EarthGravitation& setDesiredDegreeOrder(const int& n, const int& m)
      {
         if(n >= m)
         {
            desiredDegree  =  n;
            desiredOrder   =  m;
         }
         else
         {
            desiredDegree  =  n;
            desiredOrder   =  n;
         }

         int size = desiredDegree*(desiredDegree+1)/2 + (desiredOrder+1);
         gmData.normalizedCS.resize(size,4,0.0);

         return (*this);
      }


         /// Get desired degree and order
      inline void getDesiredDegreeOrder(int& n, int& m) const
      {
         n = desiredDegree;
         m = desiredOrder;
      }


         /// Set reference system
      inline EarthGravitation& setReferenceSystem(ReferenceSystem& ref)
      {
         pRefSys = &ref;

         return (*this);
      }

         /// Get reference system
      inline ReferenceSystem* getReferenceSystem() const
      {
         return pRefSys;
      }

         /// Set earth solid tide
      inline EarthGravitation& setEarthSolidTide(EarthSolidTide& solidTide)
      {
         pSolidTide = &solidTide;

         return (*this);
      }

         /// Get earth solid tide
      inline EarthSolidTide* getEarthSolidTide() const
      {
         return pSolidTide;
      }


         /// Set earth ocean tide
      inline EarthGravitation& setEarthOceanTide(EarthOceanTide& oceanTide)
      {
         pOceanTide = &oceanTide;

         return (*this);
      }


         /// Get earth ocean tide
      inline EarthOceanTide* getEarthOceanTide() const
      {
         return pOceanTide;
      }


         /// Set earth pole tide
      inline EarthGravitation setEarthPoleTide(EarthPoleTide& poleTide)
      {
         pPoleTide = &poleTide;

         return (*this);
      }

         /// Get earth pole tide
      inline EarthPoleTide* getEarthPoleTide() const
      {
         return pPoleTide;
      }


         /** Compute acceleration (and related partial derivatives) of Earth
          *  Gravitation.
          * @param utc     time in UTC
          * @param rb      earth body
          * @param sc      spacecraft
          */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
      {}


         /// Return the force model name
      inline virtual std::string forceModelName() const
      { return "EarthGravitation"; }


         /// Return the force model index
      inline virtual int forceModelIndex() const
      { return FMI_EarthGravitation; }


   protected:
  
         /// Desired degree and order
      int desiredDegree;
      int desiredOrder;

         /// Reference system
      ReferenceSystem*  pRefSys;

         /// Earth tides
      EarthSolidTide*   pSolidTide;
      EarthOceanTide*   pOceanTide;
      EarthPoleTide*    pPoleTide;

         /// Gravity Model Data
      GravityModelData gmData;


   }; // End of class 'EarthGravitation'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EARTH_GRAVITATION_HPP

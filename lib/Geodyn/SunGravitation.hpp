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
 * @file SunGravitation.hpp
 * This class calculate the gravitational effect of the Sun.
 */

#ifndef GPSTK_SUN_GRAVITATION_HPP
#define GPSTK_SUN_GRAVITATION_HPP

#include "ForceModel.hpp"
#include "SolarSystem.hpp"
#include "ReferenceSystem.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * This class calculates the gravitational effect of the Sun.
       *
       */
   class SunGravitation : public ForceModel
   {
   public:
 
      /// Default constructor
      SunGravitation()
         : pSolSys(NULL), pRefSys(NULL), isPrepared(NULL)
      {}


      /// Constructor
      SunGravitation(SolarSystem&      sol,
                     ReferenceSystem&  ref)
         : isPrepared(false)
      {
         pSolSys = &sol;
         pRefSys = &ref;
      }


      /// Default destructor
      virtual ~SunGravitation() {};


      /// Set solar system
      inline SunGravitation& setSolarSystem(SolarSystem& sol)
      {
         pSolSys = &sol;

         return (*this);
      }


      /// Get solar system
      inline SolarSystem* getSolarSystem() const
      {
         return pSolSys;
      }


      /// Set reference system
      inline SunGravitation& setReferenceSystem(ReferenceSystem& ref)
      {
         pRefSys = &ref;

         return (*this);
      }


      /// Get reference system
      inline ReferenceSystem* getReferenceSystem() const
      {
         return pRefSys;
      }


      /** Call the relevant methods to compute the acceleration.
       * @param utc  Time reference class
       * @param rb   Reference Body class
       * @param sc   Spacecraft parameters and state
       * @return the acceleration [m/s^s]
       */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc);


      /// Return force model name
      inline virtual std::string modelName()  const
      { return "SunForce"; }

      /// return the force model index
      inline virtual int forceIndex() const
      { return FMI_SunGravitation; }

   private:

      /// Solar System
      SolarSystem* pSolSys;

      /// Reference System
      ReferenceSystem* pRefSys;

      /// Is Prepared ?
      bool isPrepared;


   }; // End of class 'SunGravitation'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SUN_FORCE_HPP

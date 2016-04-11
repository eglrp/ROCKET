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
 * @file SatOrbit.hpp
 * This Class handle general orbit for different satellite orbit type.
 */

#ifndef GPSTK_SAT_ORBIT_HPP
#define GPSTK_SAT_ORBIT_HPP

#include "EquationOfMotion.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"
#include "ForceModelList.hpp"

#include "SphericalHarmonicGravity.hpp"
#include "SunForce.hpp"
#include "MoonForce.hpp"
#include "SolarPressure.hpp"
#include "RelativityEffect.hpp"

namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

  
      /** This Class handle general orbit for different satellite orbit type.
       * 
       */
   class SatOrbit : public EquationOfMotion
   {
   public:
         /// Valid gravity models
      enum GravityModel
      {
         GM_EGM96,
         GM_EGM08
      };

         /// Valid solar radiation pressure models, add by kfkuang, 2015/10/07
      enum SRPModel
      {
          SRPM_ROCK,
          SRPM_CODE
      };

         /// Struct to hold force model setting data
      struct FMCData
      {
         bool geoEarth;
         bool geoSun;
         bool geoMoon;
         bool solarPressure;
         bool relEffect;

         GravityModel grvModel;
         int grvDegree;
         int grvOrder;

         bool solidTide;
         bool oceanTide;
         bool poleTide;

         SRPModel srpModel;

         // We'll allocate memory in the heap for some of the models are memory
         // consuming
         
         SphericalHarmonicGravity* pGeoEarth;
         
         SunForce* pGeoSun;
         
         MoonForce* pGeoMoon;
         
         SolarPressure* pSolarPressure;

         RelativityEffect* pRelEffect;

         FMCData()
         {
            geoEarth = true;
            geoSun = geoMoon = false;
            solarPressure = false;
            relEffect = false;

            grvModel = GM_EGM96;
            grvDegree = 1;
            grvOrder = 1;

            solidTide = oceanTide = poleTide = false;

            srpModel = SRPM_CODE;

            pGeoEarth = NULL;
            pGeoSun   = NULL;
            pGeoMoon  = NULL;
            pSolarPressure  = NULL;
            pRelEffect      = NULL;
         }
      };

   public:

         /// Default constructor
      SatOrbit() : fmlPrepared(false)
      { reset(); }

         /// Default destructor
      virtual ~SatOrbit()
      { deleteFMObjects(fmcData); }

      virtual Vector<double> getDerivatives(const double&         t,
                                            const Vector<double>& y );

         /// Restore the default setting
      SatOrbit& reset()
      { deleteFMObjects(fmcData); fmlPrepared = false;init(); return(*this); }

         /// set reference epoch
      SatOrbit& setRefEpoch(CommonTime utc)
      { utc0 = utc; return (*this); }

         /// get reference epoch
      CommonTime getRefEpoch() const
      { return utc0; }

         /// Set spacecraft
      SatOrbit& setSpacecraft(const Spacecraft& s) { sc = s; return *this; }


         // Methods to configure the orbit perturbation force models
         // call 'reset()' before call these methods

      SatOrbit& enableGeopotential(SatOrbit::GravityModel model = SatOrbit::GM_EGM96,
                                   const int& maxDegree = 1,
                                   const int& maxOrder = 1,
                                   const bool& solidTide = false,
                                   const bool& oceanTide = false,
                                   const bool& poleTide = false);

      SatOrbit& enableThirdBodyPerturbation(const bool& bsun = false,
                                            const bool& bmoon = false);

      SatOrbit& enableSolarRadiationPressure(SatOrbit::SRPModel model
                                                 = SatOrbit::SRPM_CODE,
                                             const bool& bsrp = false);

      SatOrbit& enableRelativeEffect(const bool& brel = false);

      SatOrbit& setForceModelListPrepared()
      {
          createFMObjects(fmcData);
      }


         /// For POD, and it will be improved later
      void setForceModelType(std::set<ForceModel::ForceModelType> fmt)
      {
          fml.setForceModelType(fmt);
      }

         /// get ForceModelList object
      ForceModelList getForceModelList()
      {
          return fml;
      }

   private:

      virtual void init();

      void createFMObjects(FMCData& fmc);

      void deleteFMObjects(FMCData& fmc);

         /**
          * Adds a generic force to the list
          */
      void addForce(ForceModel* pForce)
      { fml.addForce(pForce); }

         /// Reference epoch
      CommonTime utc0;

         /// Spacecraft object
      Spacecraft sc;

         /// Earth Body
      EarthBody earthBody;

         /// Object holding force model configuration data
      FMCData fmcData;
  
         /// Flag indicate if the ForceModelList has been prepared
         /// 'fmcData' can't be change when 'fmlPrepared' is true
      bool fmlPrepared;

         /// Force Model List
      ForceModelList fml;

         // Objected

   }; // End of class 'SatOrbit'

}  // End of namespace 'gpstk'

#endif   // GPSTK_SAT_ORBIT_HPP

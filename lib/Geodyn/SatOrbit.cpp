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
 * @file SatOrbit.cpp
 * This Class handle general orbit for different satellite orbit type.
 */

#include "SatOrbit.hpp"

#include "EGM08GravityModel.hpp"

#include "ROCKPressure.hpp"
#include "CODEPressure.hpp"


using namespace std;

namespace gpstk
{
   // get derivative dy/dt
   Vector<double> SatOrbit::getDerivatives(const double&         t,
                                           const Vector<double>& y)
   {
//      cout << "SatOrbit::getDerivatives() " << getForceModelType() << endl;

      if(fmlPrepared == false)
      {
         createFMObjects(fmcData);
      }

//      cout << "SatOrbit::getDerivatives() " << getForceModelType() << endl;

      // import the state vector to sc
      sc.setStateVector(y);

//      cout << sc.P() << endl;

      CommonTime utc = utc0;
      utc += t;

      return fml.getDerivatives(utc, earthBody, sc);
   }

   void SatOrbit::init()
   {
      enableGeopotential(GM_EGM08,1,1,false,false,false);
      enableThirdBodyPerturbation(false,false);
      enableSolarRadiationPressure(SRPM_CODE,false);
      enableRelativeEffect(false);

   }  // End of method 'SatOrbit::init()'


   SatOrbit& SatOrbit::enableGeopotential(SatOrbit::GravityModel model,
                                          const int& maxDegree,
                                          const int& maxOrder,
                                          const bool& solidTide,
                                          const bool& oceanTide,
                                          const bool& poleTide)
   {
      // DO NOT allow to change the configuration 
      if(fmlPrepared) return (*this);

      fmcData.geoEarth = true;

      fmcData.grvModel = model;
      fmcData.grvDegree = maxDegree;
      fmcData.grvOrder = maxOrder;

      fmcData.solidTide = solidTide;
      fmcData.oceanTide = oceanTide;
      fmcData.poleTide = poleTide;

      return (*this);

   }  // End of method 'SatOrbit::enableGeopotential()'


   SatOrbit& SatOrbit::enableThirdBodyPerturbation(const bool& bsun,
                                                   const bool& bmoon)
   {
      // DO NOT allow to change the configuration 
      if(fmlPrepared) return (*this);

      fmcData.geoSun = bsun;
      fmcData.geoMoon = bmoon;

      return (*this);

   }  // End of method 'SatOrbit::enableThirdBodyPerturbation()'


   SatOrbit& SatOrbit::enableSolarRadiationPressure(SatOrbit::SRPModel model,
                                                    const bool& bsrp)
   {
       // DO NOT allow to change the configuration
       if(fmlPrepared) return (*this);

       fmcData.srpModel = model;
       fmcData.solarPressure = bsrp;

       return (*this);

   }  // End of method 'SatOrbit::enableSolarRadiationPressure()'


   SatOrbit& SatOrbit::enableRelativeEffect(const bool& brel)
   {
      // DO NOT allow to change the configuration
      if(fmlPrepared) return (*this);

      fmcData.relEffect = brel;

      return (*this);

   }  // End of method 'SatOrbit::enableRelativeEffect()'


   void SatOrbit::createFMObjects(FMCData& fmc)
   {
      // First, we release the memory
      deleteFMObjects(fmc);

      //

      // GeoEarth
      if(fmc.grvModel == GM_EGM08)
      {
         fmc.pGeoEarth = new EGM08GravityModel();
      }
      else
      {
         // Unexpected, never go here
      }

      // GeoMoon
      fmc.pGeoMoon = new MoonGravitation();

      // GeoSun
      fmc.pGeoSun = new SunGravitation();

      // SRP
      if(fmc.srpModel == SRPM_ROCK)
      {
          fmc.pSolarPressure = new ROCKPressure();
      }
      else if(fmc.srpModel == SRPM_CODE)
      {
          fmc.pSolarPressure = new CODEPressure();
      }
      else
      {
          // Unexpected, never go here
      }

      // Rel
      fmc.pRelEffect = new RelativityEffect();

      // Now, it's time to check if we create these objects successfully
      if( !fmc.pGeoEarth || !fmc.pGeoSun || !fmc.pGeoMoon ||
          !fmc.pSolarPressure || !fmc.pRelEffect )
      {
         // deallocate allocated memory
         deleteFMObjects(fmc);

         Exception e("Failed to allocate memory for force models !");
         GPSTK_THROW(e);
      }

      // At last, prepare the force model list
      fmc.pGeoEarth->setDesiredDegreeOrder(fmc.grvDegree,fmc.grvOrder);
      fmc.pGeoEarth->setCorrectSolidTide(fmc.solidTide);
      fmc.pGeoEarth->setCorrectOceanTide(fmc.oceanTide);
      fmc.pGeoEarth->setCorrectPoleTide(fmc.poleTide);

      fml.clear();

      if(fmc.geoEarth) fml.addForce(fmc.pGeoEarth);
      if(fmc.geoSun) fml.addForce(fmc.pGeoSun);
      if(fmc.geoMoon) fml.addForce(fmc.pGeoMoon);
      if(fmc.solarPressure) fml.addForce(fmc.pSolarPressure);
      if(fmc.relEffect) fml.addForce(fmc.pRelEffect);

//      cout << fml.getFMPList().size() << endl;
//      cout << fml.getFMTSet().size() << endl;

      // set the flag
      fmlPrepared = true;

   }  // End of method 'SatOrbit::createFMObjects()'


   void SatOrbit::deleteFMObjects(FMCData& fmc)
   {
      // GeoEarth
      if(fmc.pGeoEarth)
      {
         if(fmc.grvModel == GM_EGM08)
         {
            delete (EGM08GravityModel*) fmc.pGeoEarth;
            fmc.pGeoEarth = NULL;
         }
         else
         {
            delete fmc.pGeoEarth;
            fmc.pGeoEarth = NULL;
         }
      }
      
      // GeoSun
      if(fmc.pGeoSun)
      {
         delete fmc.pGeoSun;
         fmc.pGeoSun = NULL;
      }
      
      // GeoMoon
      if(fmc.pGeoMoon)
      {
         delete fmc.pGeoMoon;
         fmc.pGeoMoon = NULL;
      }

      // SRP
      if(fmc.pSolarPressure)
      {
          if(fmc.srpModel == SRPM_ROCK)
          {
              delete (ROCKPressure*) fmc.pSolarPressure;
              fmc.pSolarPressure = NULL;
          }
          else if(fmc.srpModel == SRPM_CODE)
          {
              delete (CODEPressure*) fmc.pSolarPressure;
              fmc.pSolarPressure = NULL;
          }
          else
          {
              delete fmc.pSolarPressure;
              fmc.pSolarPressure = NULL;
          }
      }

      // Rel
      if(fmc.pRelEffect)
      {
         delete fmc.pRelEffect;
         fmc.pRelEffect = NULL;
      }


      // set the flag
      fmlPrepared = false;

   }  // End of method 'SatOrbit::deleteForceModelList()'


}  // End of namespace 'gpstk'

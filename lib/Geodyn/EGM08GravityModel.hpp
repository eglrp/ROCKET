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
 * @file EGM08GravityModel.hpp
 */

#ifndef GPSTK_EGM08_GRAVITY_MODEL_HPP
#define GPSTK_EGM08_GRAVITY_MODEL_HPP

#include "EarthGravitation.hpp"

namespace gpstk
{
   /** @addtogroup GeoDynamics */
   //@{

   /** EGM08 Gravity Model.
    *
    */
   class EGM08GravityModel : public EarthGravitation
   {
   public:  
      /** Constructor.
       * @param n    Desired degree
       * @param m    Desired order
       */ 
      EGM08GravityModel (int n = 0, int m = 0)
         : EarthGravitation(n, m)
      {
         // model name
         gmData.modelName = "EGM08";

         // earth gravitation constant
         gmData.GM = 3.9860044150E+14;

         // radius
         gmData.ae = 6378136.300000;

         // tide free
         gmData.includesPermTide = false;

         // reference time
         gmData.refMJD =  51544.0;

         // max degree
         gmData.maxDegree = 200;

         // coefficients and errors
         int size = 200*(200+1)/2 + (200+1);
         gmData.normalizedCS.resize(size,4);

         for(int i=0; i<size; i++)
         {
            gmData.normalizedCS(i,0) = 0.0;
            gmData.normalizedCS(i,1) = 0.0;
            gmData.normalizedCS(i,2) = 0.0;
            gmData.normalizedCS(i,3) = 0.0;
         }

       } // End of constructor
 

      /// Load EGM file
      void loadEGMFile(std::string file)
         throw(FileMissingException);

      /** Compute the acceleration due to earth gravitation.
       * @param utc   Time reference class
       * @param rb    Earth body class
       * @param sc    Spacecraft parameters and state
       * @return the accelerations [m/s^2]
       */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc);

      virtual std::string modelName() const
      { return gmData.modelName; }

      virtual ~EGM08GravityModel(){};

   private:
 
      struct GravityModelData
      {
         std::string modelName;
         
         double GM;
         double ae;

         bool includesPermTide;

         double refMJD;

         int maxDegree;

         Matrix<double> normalizedCS;

      } gmData;

   }; // End of class 'EGM08GravityModel'

   // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EGM08_GRAVITY_MODEL_HPP

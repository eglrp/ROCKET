<<<<<<< Updated upstream
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
 * @file EGM08GravityModel.hpp
 * EGM08 gravity model
 */

#ifndef GPSTK_EGM08_GRAVITY_MODEL_HPP
#define GPSTK_EGM08_GRAVITY_MODEL_HPP

#include "SphericalHarmonicGravity.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** EGM08 gravity model
       *
       */
   class EGM08GravityModel : public SphericalHarmonicGravity
   {
   public:  
       /** Constructor.
        * @param n    Desired degree
        * @param m    Desired order
        */
       EGM08GravityModel (int n = 12, int m = 12)
           : SphericalHarmonicGravity( n, m )
       {
           gmData.modelName = "EGM08";

           gmData.GM = 3.9860044150E+14;
           gmData.ae = 6378136.300000;

           gmData.includesPermTide = false;

           gmData.refMJD =  51544.0;

           gmData.dotC20 = 11.6e-12;
           gmData.dotC30 =  4.9e-12;
           gmData.dotC40 =  4.7e-12;

           gmData.maxDegree = 12;
           gmData.maxOrder  = 12;

           gmData.xp.resize(3,2,0.0);
           gmData.yp.resize(3,2,0.0);

           // until 2010.0, unit: mas/year
           gmData.xp(0,0) = 55.974;   gmData.yp(0,0) = 346.346;
           gmData.xp(1,0) = 1.8243;   gmData.yp(1,0) = 1.7896;
           gmData.xp(2,0) = 0.18413;  gmData.yp(2,0) = -0.10729;
           gmData.xp(3,0) = 0.007024; gmData.yp(3,0) = -0.000908;

           // after 2010.0, unit: mas/year
           gmData.xp(0,1) = 23.513;   gmData.yp(0,1) = 358.891;
           gmData.xp(1,1) = 7.6141;   gmData.yp(1,1) = -0.6287;

           int size = n*(n+1)/2 + (m+1) - 3;
           gmData.normalizedCS.resize(size,2);

           for(int i=0; i<size; i++)
           {
               gmData.normalizedCS(i,0) = gmcs[i][0];
               gmData.normalizedCS(i,1) = gmcs[i][1];
           }

       } // End of constructor
 
       /** Compute the acceleration due to earth gravitation.
         * @param utc   Time reference class
         * @param rb    Reference body class
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

           double dotC20;
           double dotC30;
           double dotC40;

           // Coefficients of the IERS (2010) mean pole model
           // see IERS Conventions 2010, Table 7.7
           Matrix<double> xp;
           Matrix<double> yp;

           int maxDegree;
           int maxOrder;

           Matrix<double> normalizedCS;

       } gmData;


       // normalized gravity model coefficients, not include order 0 and 1
       //    (n+1)(n+2)/2 - 3 = 13*14/2 - 3 = 88
       static const double gmcs[88][2];

   }; // End of class 'EGM08GravityModel'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EGM08_GRAVITY_MODEL_HPP
=======
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
 * @file EGM08GravityModel.hpp
 * EGM08 gravity model
 */

#ifndef GPSTK_EGM08_GRAVITY_MODEL_HPP
#define GPSTK_EGM08_GRAVITY_MODEL_HPP

#include "SphericalHarmonicGravity.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** EGM08 gravity model
       *
       */
   class EGM08GravityModel : public SphericalHarmonicGravity
   {
   public:  
       /** Constructor.
        * @param n    Desired degree
        * @param m    Desired order
        */
       EGM08GravityModel (int n = 12, int m = 12)
           : SphericalHarmonicGravity( n, m )
       {
           gmData.modelName = "EGM08";

           gmData.GM = 3.9860044150E+14;
           gmData.ae = 6378136.300000;

           gmData.includesPermTide = false;

           gmData.refMJD =  51544.0;

           gmData.dotC20 = 11.6e-12;
           gmData.dotC30 =  4.9e-12;
           gmData.dotC40 =  4.7e-12;

           gmData.maxDegree = 12;
           gmData.maxOrder  = 12;

           gmData.xp.resize(3,2,0.0);
           gmData.yp.resize(3,2,0.0);

           // until 2010.0, unit: mas/year
           gmData.xp(0,0) = 55.974;   gmData.yp(0,0) = 346.346;
           gmData.xp(1,0) = 1.8243;   gmData.yp(1,0) = 1.7896;
           gmData.xp(2,0) = 0.18413;  gmData.yp(2,0) = -0.10729;
           gmData.xp(3,0) = 0.007024; gmData.yp(3,0) = -0.000908;

           // after 2010.0, unit: mas/year
           gmData.xp(0,1) = 23.513;   gmData.yp(0,1) = 358.891;
           gmData.xp(1,1) = 7.6141;   gmData.yp(1,1) = -0.6287;

           int size = n*(n+1)/2 + (m+1) - 3;
           gmData.normalizedCS.resize(size,2);

           for(int i=0; i<size; i++)
           {
               gmData.normalizedCS(i,0) = gmcs[i][0];
               gmData.normalizedCS(i,1) = gmcs[i][1];
           }

       } // End of constructor
 
       /** Compute the acceleration due to earth gravitation.
         * @param utc   Time reference class
         * @param rb    Reference body class
         * @param sc    Spacecraft parameters and state
         * @return the accelerations [m/s^2]
         */
       virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc);

       virtual std::string modelName() const
       { return gmData.modelName; }

       virtual ~EGM08GravityModel() {};

   private:

       struct GravityModelData
       {
           std::string modelName;

           double GM;
           double ae;

           bool includesPermTide;

           double refMJD;

           double dotC20;
           double dotC30;
           double dotC40;

           // Coefficients of the IERS (2010) mean pole model
           // see IERS Conventions 2010, Table 7.7
           Matrix<double> xp;
           Matrix<double> yp;

           int maxDegree;
           int maxOrder;

           Matrix<double> normalizedCS;

       } gmData;


       // normalized gravity model coefficients, not include order 0 and 1
       //    (n+1)(n+2)/2 - 3 = 13*14/2 - 3 = 88
       static const double gmcs[88][2];

   }; // End of class 'EGM08GravityModel'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EGM08_GRAVITY_MODEL_HPP
>>>>>>> Stashed changes

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
* @file SphericalHarmonicGravity.hpp
* 
*/

#ifndef GPSTK_SPHERICAL_HARMONIC_GRAVITY_HPP
#define GPSTK_SPHERICAL_HARMONIC_GRAVITY_HPP

#include "ForceModel.hpp"
#include "EarthSolidTide.hpp"
#include "EarthOceanTide.hpp"
#include "EarthPoleTide.hpp"
#include "StringUtils.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{


      /** This class computes the body fixed acceleration due to the harmonic 
       *  gravity field of the central body
       */
   class SphericalHarmonicGravity : public ForceModel
   {
   public:

         /** Constructor.
          * @param n Desired degree.
          * @param m Desired order.
          */
      SphericalHarmonicGravity(int n, int m)
          : desiredDegree(n),
            desiredOrder(m),
            correctSolidTide(false),
            correctPoleTide(false),
            correctOceanTide(false)
       {}


         /// Default destructor
      virtual ~SphericalHarmonicGravity() {};


         /** Call the relevant methods to compute the acceleration.
          * @param utc Time reference class
          * @param rb  Reference body class
          * @param sc  Spacecraft parameters and state
          * @return the acceleration [m/s^2]
          */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc)
      {}


      SphericalHarmonicGravity& setDesiredDegree(const int& n, const int& m)
      { desiredDegree = n; desiredOrder = m; return (*this); }


      /// Methods to enable earth tide correction

      SphericalHarmonicGravity& enableSolidTide(bool b = true)
      { correctSolidTide = b; return (*this); }

      SphericalHarmonicGravity& setOceanTideFile(std::string fileName)
      { oceanTide.setOceanTideFile(fileName); return (*this); }

      SphericalHarmonicGravity& enableOceanTide(bool b = true)
      { correctOceanTide = b; return (*this); }
      
      SphericalHarmonicGravity& enablePoleTide(bool b = true)
      { correctPoleTide = b; return (*this); }

         /// Return the force model name
      virtual std::string modelName() const
      { return "SphericalHarmonicGravity"; }

         /// return the force model index
      virtual int forceIndex() const
      { return FMI_GEOEARTH; }

   protected:

      /** Translate degree and order (n,m) from 2-D to 1-D.
       * @param n   Degree
       * @param m   Order
       *
       * For example:
       * (n,m) = (0,0) <===> return = 1
       *         (1,0) <===>          2
       *         (1,1) <===>          3
       *         (2,0) <===>          4
       *          ...                ...
       */
      inline int index(int n, int m)
      {
          return n*(n+1)/2 + (m+1);
      }

       /** Evaluate the fnALF and its gradients.
        * @param degree     Desired degree
        * @param lat        Geocentric latitude in radians
        * @param leg        Fully normalized associated legendre functions
        * @param leg1       First-order derivatives of leg
        * @param leg2       Second-order derivatives of leg
        */
       void legendre(int degree, double lat, Vector<double>& leg, Vector<double>& leg1, Vector<double>& leg2);


   protected:

         /// Degree and Order of gravity model desired.
      int desiredDegree, desiredOrder;

         /// Flags to indicate earth tides correction
      bool   correctSolidTide;
      bool   correctPoleTide;
      bool   correctOceanTide;

         /// Objects to do earth tides correction
      EarthSolidTide  solidTide;
      EarthPoleTide   poleTide;
      EarthOceanTide  oceanTide;


   }; // End of namespace 'gpstk'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SPHERICAL_HARMONIC_GRAVITY_HPP

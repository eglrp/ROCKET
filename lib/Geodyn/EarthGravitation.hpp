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
* 
*/

#ifndef GPSTK_EARTH_GRAVITATION_HPP
#define GPSTK_EARTH_GRAVITATION_HPP

#include "ForceModel.hpp"
#include "EarthSolidTide.hpp"
#include "EarthOceanTide.hpp"
#include "EarthPoleTide.hpp"
#include "ReferenceSystem.hpp"


namespace gpstk
{
   /** @addtogroup GeoDynamics */
   //@{

   /** Earth gravitation.
    */
   class EarthGravitation : public ForceModel
   {
   public:

      /** Constructor.
       * @param n Desired degree.
       * @param m Desired order.
       */
      EarthGravitation(int n, int m)
         : desiredDegree(n),
           desiredOrder(m),
           correctSolidTide(false),
           correctOceanTide(false),
           correctPoleTide(false)
      {}

      /// Default destructor
      virtual ~EarthGravitation() {}


      /// Load EGM file
      virtual void loadEGMFile(const std::string& file)
         throw(FileMissingException)
      {}


      /// Set desired degree and order
      inline EarthGravitation& setDesiredDegreeOrder(const int& n, const int& m)
      {
         desiredDegree = n;
         desiredOrder = m;

         // check if order <= degree
         if(desiredOrder >= desiredDegree)
            desiredOrder = desiredDegree;

         return (*this);
      }

      /// Get desired degree and order
      inline void getDesiredDegreeOrder(int& n, int& m) const
      {
         n = desiredDegree;
         m = desiredOrder;
      }


      /// Set correct solid tide
      inline EarthGravitation& setCorrectSolidTide(const bool& b)
      {
         correctSolidTide = b;

         return (*this);
      }

      /// Get correct solid tide
      inline bool getCorrectSolidTide()
      {
         return correctSolidTide;
      }

      /// Set correct ocean tide
      inline EarthGravitation& setCorrectOceanTide(const bool& b)
      {
         correctOceanTide = b;

         return (*this);
      }

      /// Get correct ocean tide
      inline bool getCorrectOceanTide()
      {
         return correctOceanTide;
      }
      
      /// Set ocean tide file
      inline EarthGravitation& setOceanTideFile(std::string file)
      {
         oceanTide.setOceanTideFile(file);

         return (*this);
      }

      /// Set correct pole tide
      inline EarthGravitation& setCorrectPoleTide(const bool& b)
      {
         correctPoleTide = b;

         return (*this);
      }

      /// Get correct pole tide
      inline bool getCorrectPoleTide()
      {
         return correctPoleTide;
      }

      /// Set reference system
      inline EarthGravitation& setReferenceSystem(ReferenceSystem& refSys)
      {
         pRefSys = &refSys;

         return (*this);
      }

      /// Get reference system
      inline ReferenceSystem* getReferenceSystem() const
      {
         return pRefSys;
      }


      /** Call the relevant methods to compute the acceleration.
       * @param utc Time reference class
       * @param rb  Earth body class
       * @param sc  Spacecraft parameters and state
       * @return the acceleration [m/s^2]
       */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc) {};

      /// Return the force model name
      inline virtual std::string forceModelName() const
      { return "EarthGravitation"; }

      /// Return the force model index
      inline virtual int forceModelIndex() const
      { return FMI_EarthGravitation; }


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
       * @param deg    Desired degree
       * @param lat    Geocentric latitude in radians
       * @param leg    Fully normalized associated legendre functions
       * @param leg1   First-order derivatives of leg
       * @param leg2   Second-order derivatives of leg
       */
      void legendre(const int&      deg,
                    const double&   lat,
                    Vector<double>& leg,
                    Vector<double>& leg1,
                    Vector<double>& leg2);

   protected:
  
      /// Degree and Order of gravity model desired
      int desiredDegree;
      int desiredOrder;
 
      /// Correct tides or not
      bool   correctSolidTide;
      bool   correctPoleTide;
      bool   correctOceanTide;

      /// Objects to do earth tides correction
      EarthSolidTide  solidTide;
      EarthPoleTide   poleTide;
      EarthOceanTide  oceanTide;

      /// Reference System
      ReferenceSystem* pRefSys;


   }; // End of namespace 'gpstk'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EARTH_GRAVITATION_HPP

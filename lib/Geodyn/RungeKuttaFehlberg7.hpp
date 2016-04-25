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
* @file RungeKuttaFehlberg7.hpp
* This class does numerical integrations with Runge Kutta Fehlberg 7th-8th
* Order Integration Algorithm.
*/


#ifndef GPSTK_RUNGE_KUTTA_FEHLBERG7_HPP
#define GPSTK_RUNGE_KUTTA_FEHLBERG7_HPP

#include "EquationOfMotion.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class does numerical integrations with Runge Kutta Fehlberg 7th-8th
       *  Order Integration Algorithm.
       *
       *  References:
       *
       *     NASA Technical Report TR R-352
       *     Some Experimental Results Concerning The Error Propagation in
       *     Runge-Kutta type integration formulas
       *     by Erwin Fehlberg
       *     October 1970
       *
       */
   class RungeKuttaFehlberg7
   {
   public:
         /// Default constructor
      RungeKuttaFehlberg7(const Vector<double>& initialState,
                          double initialTime = 0,
                          double timeEpsilon = 1e-18)
          : currentTime(initialTime), currentState(initialState),
            teps(timeEpsilon), stateSize(initialState.size()), k0(stateSize),
            k1(stateSize), k2(stateSize), k3(stateSize), k4(stateSize),
            k5(stateSize), k6(stateSize), k7(stateSize), k8(stateSize),
            k9(stateSize), k10(stateSize), k11(stateSize), k12(stateSize),
            tempy(stateSize)
      {}

         /// Default destructor
      virtual ~RungeKuttaFehlberg7()
      {}

         /** The Runge Kutta Fehlberg 7th-8th Order Integration Algorithm.
          * @param nextTime the time to integrate to
          * @param error    the Vector of estimated integration error
          *                 (one for each element)
          * @param peom     the derivatives of the state
          * @param stepSize the amount time between internal integration steps
          */
      virtual void integrateTo(double nextTime,
                               Vector<double>& error,
                               EquationOfMotion* peom,
                               double stepSize = 0);

         /// Return the current time of the system.
      double getTime(void)
      { return currentTime; }


         /// Return the current state of the system.
      const Vector<double>& getState(void)
      { return currentState; }

   private:
         /// Current time of the system
      double currentTime;

         /// State of the system at the current time
      Vector<double> currentState;

         /// Step size of the system
//      double stepSize;

         /// Precision for time calculations and comparisons
      double teps;

         /// State size of the system
      int stateSize;


       // rkf78 coefficients
       const static double a[13];
       const static double b[13][12];
       const static double c1[13],c2[13];

         /// Disallow copy constructor
      RungeKuttaFehlberg7(const RungeKuttaFehlberg7& cloneDonor);

         /// Disallow the assignment operator
      RungeKuttaFehlberg7& operator= (const RungeKuttaFehlberg7& right);

         /** These values are only used in the integrateTo method(s).
          *  Declaring them here keeps them from being constructed every
          *  time the integrateTo method(s) are called.
          */
      Vector<double> k0, k1, k2, k3, k4, k5, k6, k7, k8, k9, k10, k11, k12, tempy;

   }; // End of class 'RungeKuttaFehlberg7'

      // @}

}  // End of namespace 'gpstk'


#endif // GPSTK_RUNGE_KUTTA_FEHLBERG_HPP

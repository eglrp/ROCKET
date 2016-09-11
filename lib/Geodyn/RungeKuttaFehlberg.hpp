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
* @file RungeKuttaFehlberg.hpp
* This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
*/


#ifndef GPSTK_RUNGE_KUTTA_FEHLBERG_HPP
#define GPSTK_RUNGE_KUTTA_FEHLBERG_HPP

#include "EquationOfMotion.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
       *
       */
   class RungeKuttaFehlberg
   {
   public:

      /// Default constructor
      RungeKuttaFehlberg()
         : stepSize(60.0),
           errorTol(1.0e-5)
      {}


      /// Default destructor
      ~RungeKuttaFehlberg() {}


      /// Set step size
      inline RungeKuttaFehlberg& setStepSize(const double& size)
      {
         stepSize = size;

         return (*this);
      }


      /// Get step size
      inline double getStepSize() const
      {
         return stepSize;
      }


      /// Set error tolerance
      inline RungeKuttaFehlberg& setErrorTolerance(const double& tol)
      {
         errorTol = tol;

         return (*this);
      }


      /// Get error tolerance
      inline double getErrorTolerance() const
      {
         return errorTol;
      }


      /// Set equation of motion
//      inline RungeKuttaFehlberg& setEquationOfMotion(EquationOfMotion& eom)
//      {
//         peom = &eom;

//         return (*this);
//      }


      /// Get equation of motion
//      inline EquationOfMotion* getEquationOfMotion() const
//      {
//         return peom;
//      }


      /// Set current time
//      inline RungeKuttaFehlberg& setCurrentTime(const double& time)
//      {
//         currentTime = time;

//         return (*this);
//      }


      /// Get current time
//      inline double getCurrentTime() const
//      {
//         return currentTime;
//      }


      /// Set current state
//      inline RungeKuttaFehlberg& setCurrentState(const Vector<double>& state)
//      {
//         currentState = state;

//         return (*this);
//      }


      /// Get current state
//      inline Vector<double> getCurrentState() const
//      {
//         return currentState;
//      }


      /// Real implementation of rkf78
      void integrateTo(double& currentTime,
                       Vector<double>& currentState,
                       EquationOfMotion* peom,
                       double nextTime);


   private:

      /// Coefficients of rkf78
      const static double a[13];
      const static double b[13][12];
      const static double c1[11], c2[13];


      /// Step size
      double stepSize;

      /// Error tolerance
      double errorTol;

      /// Pointer to EquationOfMotion
//      EquationOfMotion* peom;

      /// Current time
//      double currentTime;

      /// Current state
//      Vector<double> currentState;


   }; // End of class 'RungeKuttaFehlberg'

      // @}

}  // End of namespace 'gpstk'


#endif // GPSTK_RUNGE_KUTTA_FEHLBERG_HPP

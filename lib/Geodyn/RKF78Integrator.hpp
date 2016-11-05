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
* @file RKF78Integrator.hpp
* This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
*/


#ifndef RKF78_INTEGRATOR_HPP
#define RKF78_INTEGRATOR_HPP

#include "EquationOfMotion.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
       *
       */
   class RKF78Integrator
   {
   public:

      /// Default constructor
      RKF78Integrator()
         : stepSize(60.0),
           errorTol(1.0e-5)
      {}


      /// Default destructor
      ~RKF78Integrator() {}


      /// Set step size
      inline RKF78Integrator& setStepSize(const double& size)
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
      inline RKF78Integrator& setErrorTolerance(const double& tol)
      {
         errorTol = tol;

         return (*this);
      }


      /// Get error tolerance
      inline double getErrorTolerance() const
      {
         return errorTol;
      }


      /// Real implementation of RKF78
      void integrateTo(double&              t_curr,
                       Vector<double>&      y_curr,
                       EquationOfMotion*    peom,
                       double               t_next);


   private:

      /// Coefficients of rkf78
      const static double a[13];
      const static double b[13][12];
      const static double c1[11], c2[13];


      /// Step size
      double stepSize;

      /// Error tolerance
      double errorTol;

   }; // End of class 'RKF78Integrator'

      // @}

}  // End of namespace 'gpstk'


#endif // RKF78_INTEGRATOR_HPP

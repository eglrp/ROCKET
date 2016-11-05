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
* @file AdamsIntegrator.hpp
* This class implements Adams Bashforth Moulton 8-th order algorithm.
*/


#ifndef ADAMS_INTEGRATOR_HPP
#define ADAMS_INTEGRATOR_HPP

#include "EquationOfMotion.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class implements Adams Bashforth Moulton 8-th order algorithm.
       *
       */
   class AdamsIntegrator
   {
   public:

      /// Default constructor
      AdamsIntegrator()
         : stepSize(300.0)
      {}


      /// Default destructor
      virtual ~AdamsIntegrator() {}


      /// Set step size
      inline AdamsIntegrator& setStepSize(const double& size)
      {
         stepSize = size;

         return (*this);
      }


      /// Get step size
      inline double getStepSize() const
      {
         return stepSize;
      }


      /// Real implementation of Adams
      void integrateTo(std::vector< double >&           t_curr,
                       std::vector< Vector<double> >&   y_curr,
                       EquationOfMotion*                peom,
                       double                           t_next);


   private:

      /// Coefficients of Adams-Bashforth
      const static double cb[9];

      /// Coefficients of Adams-Moulton
      const static double cm[9];

      /// Step size
      double stepSize;

   }; // End of class 'AdamsIntegrator'

      // @}

}  // End of namespace 'gpstk'


#endif // ADAMS_INTEGRATOR_HPP

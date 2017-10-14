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
//
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
* @file RKF78Integrator.hpp
* This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
*/


#ifndef RKF78_INTEGRATOR_HPP
#define RKF78_INTEGRATOR_HPP

#include "Integrator.hpp"


namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** This class implements Runge Kutta Fehlberg 7(8)-th order algorithm.
     *
     */
    class RKF78Integrator : public Integrator
    {
    public:
        /// Default constructor
        RKF78Integrator(double step=60.0,
                        double error=1e-7)
            : Integrator(step), errorTol(error)
        {};

        /// Default destructor
        ~RKF78Integrator() {};


        /// Set error tolerance
        inline RKF78Integrator& setErrorTolerance(const double& tol)
        { errorTol = tol; };

        /// Get error tolerance
        inline double getErrorTolerance() const
        { return errorTol; };


        /// Set current time
        inline RKF78Integrator& setCurrentTime(const CommonTime& time)
        { t_curr = time; return (*this); };

        /// Get current time
        inline CommonTime getCurrentTime() const
        { return t_curr; };


        /// Set current state
        inline RKF78Integrator& setCurrentState(const satVectorMap& state)
        { y_curr = state; return (*this); };

        /// Get current state
        inline satVectorMap getCurrentState() const
        { return y_curr; };


        /// Real implementation of RKF78
        virtual satVectorMap integrateTo( const CommonTime& t_next );


    private:
        /// Coefficients of RKF78
        const static double a[13];
        const static double b[13][12];
        const static double c1[13], c2[13];

        /// Error Tolerance
        double errorTol;

        /// Current Time
        CommonTime t_curr;

        /// Current State
        satVectorMap y_curr;

    };  // End of class 'RKF78Integrator'

    // @}

}  // End of namespace 'gpstk'

#endif // RKF78_INTEGRATOR_HPP

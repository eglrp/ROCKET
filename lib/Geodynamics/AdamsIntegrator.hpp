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
* @file AdamsIntegrator.hpp
* This class implements Adams Bashforth Moulton 8-th order algorithm.
*/


#ifndef ADAMS_INTEGRATOR_HPP
#define ADAMS_INTEGRATOR_HPP

#include "Integrator.hpp"


namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** This class implements Adams Bashforth Moulton 8-th order algorithm.
     *
     */
    class AdamsIntegrator : public Integrator
    {
    public:

        /// Default constructor
        AdamsIntegrator(double step = 300.0)
            : Integrator(step)
        {};

        /// Default destructor
        virtual ~AdamsIntegrator() {};


        /// Set current time
        inline AdamsIntegrator& setCurrentTime(const std::vector<CommonTime>& time)
        { t_curr = time; return (*this); };

        /// Get current time
        inline std::vector<CommonTime> getCurrentTime() const
        { return t_curr; };


        /// Set current state
        inline AdamsIntegrator& setCurrentState(const std::vector<satVectorMap>& state)
        { y_curr = state; return (*this); };

        /// Get current state
        inline std::vector<satVectorMap> getCurrentState() const
        { return y_curr; };


        /// Get invalid sats
        inline std::vector<SatID> getInvalidSats() const
        { return invalidSats; };


        /// Real implementation of Adams
        virtual satVectorMap integrateTo(const CommonTime& t_next);


    private:

        /// Coefficients of Adams-Bashforth
        const static double cb[9];

        /// Coefficients of Adams-Moulton
        const static double cm[9];

        /// Current Time
        std::vector<CommonTime> t_curr;

        /// Current State
        std::vector<satVectorMap> y_curr;

        /// Invalid Satellites
        std::vector<SatID> invalidSats;

    }; // End of class 'AdamsIntegrator'

    // @}

}  // End of namespace 'gpstk'

#endif // ADAMS_INTEGRATOR_HPP

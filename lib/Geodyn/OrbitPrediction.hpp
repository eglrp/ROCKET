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
//  Copyright
//
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
 * @file OrbitPrediction.hpp
 * Class to do Orbit Prediction.
 */

#ifndef ORBIT_PREDICTION_HPP
#define ORBIT_PREDICTION_HPP

#include "EquationOfMotion.hpp"

#include "Spacecraft.hpp"
#include "GNSSOrbit.hpp"
#include "RKF78Integrator.hpp"
#include "AdamsIntegrator.hpp"
#include "SatDataReader.hpp"

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Orbit Prediction.
     *
     */
    class OrbitPrediction
    {
    public:

        /// Default constructor
        OrbitPrediction() {}

        /// Default destructor
        virtual ~OrbitPrediction() {}


        /// Set initial time
        inline OrbitPrediction& setInitialTime(const CommonTime& utc)
        {
            utc0 = utc;
            return (*this);
        }

        /// Get initial time
        inline CommonTime getInitialTime() const
        {
            return utc0;
        }


        /// Set Spacecraft
        inline OrbitPrediction& setSpacecraft(const Spacecraft& s)
        {
            sc = s;
            return (*this);
        }

        /// Get Spacecraft
        inline Spacecraft getSpacecraft() const
        {
            return sc;
        }


        /// Set arc length
        inline OrbitPrediction& setArcLength(const double& l)
        {
            arcLen = l;
            return (*this);
        }

        /// Get arc length
        inline double getArcLength() const
        {
            return arcLen;
        }


        /// Set arc interval
        inline OrbitPrediction& setArcInterval(const double& i)
        {
            arcInt = i;
            return (*this);
        }

        /// Get arc interval
        inline double getArcInterval() const
        {
            return arcInt;
        }


        /// Set GNSS Orbit
        inline OrbitPrediction& setSatOrbit(GNSSOrbit& o)
        {
            pOrbit = &o;
            return (*this);
        }

        /// Get GNSS Orbit
        inline GNSSOrbit* getSatOrbit() const
        {
            return pOrbit;
        }


        /// Set RKF78 Integrator
        inline OrbitPrediction& setRKF78Integrator(RKF78Integrator& integrator)
        {
            pRKF78 = &integrator;
            return (*this);
        }

        /// Get RKF78 Integrator
        inline RKF78Integrator* getRKF78Integrator() const
        {
            return pRKF78;
        }

        /// Set Adams Integrator
        inline OrbitPrediction& setAdamsIntegrator(AdamsIntegrator& integrator)
        {
            pAdams = &integrator;
            return (*this);
        }

        /// Get Adams Integrator
        inline AdamsIntegrator* getAdamsIntegrator() const
        {
            return pAdams;
        }


        /* Orbit Integration.
         *
         * @return  predicted orbit and partials
         *
         */
        Matrix<double> OrbitInt();


    private:

        /// Initial Time in UTC
        CommonTime utc0;

        /// Spacecraft
        Spacecraft sc;

        /// Arc Length, unit: hour
        double arcLen;

        /// Arc Interval, unit: second
        double arcInt;

        /// GNSSOrbit
        GNSSOrbit* pOrbit;

        /// RKF78Integrator
        RKF78Integrator* pRKF78;

        /// AdamsIntegrator
        AdamsIntegrator* pAdams;

   }; // End of class 'OrbitPrediction'


}  // End of namespace 'gpstk'

#endif   // ORBIT_PREDICTION_HPP

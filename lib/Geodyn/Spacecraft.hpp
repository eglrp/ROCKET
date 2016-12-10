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
* @file Spacecraft.hpp
* Class to encapsulate Spacecraft related paramenters.
*/

#ifndef SPACECRAFT_HPP
#define SPACECRAFT_HPP

#include <iostream>
#include <string>

#include "Vector.hpp"
#include "Matrix.hpp"

#include "CommonTime.hpp"
#include "SatID.hpp"
#include "SatDataReader.hpp"

using namespace std;

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to encapsulate Spacecraft related parameters.
     */
    class Spacecraft
    {
    public:

        /// Constructor
        Spacecraft()
        {
            resetState();
        }

        /// Default destructor
        ~Spacecraft() {}


        /// Set current time of Spacecraft
        inline Spacecraft& setCurrentTime(const CommonTime& t)
        {
            utc = t;
            return (*this);
        }

        /// Get current time of Spacecraft
        inline CommonTime getCurrentTime() const
        {
            return utc;
        }


        /// Set SatID of Spacecraft
        inline Spacecraft& setSatID(const SatID& si)
        {
            sat = si;
            return (*this);
        }

        /// Get SatID of Spacecraft
        inline SatID getSatID() const
        {
            return sat;
        }


        /// Set SatData
        inline Spacecraft& setSatData(SatDataReader& sd)
        {
            pSatData = &sd;
            return (*this);
        }

        /// Get SatData
        inline SatDataReader* getSatData() const
        {
            return pSatData;
        }


        /// Get current block of Spacecraft
        inline std::string getCurrentBlock() const
        {
            string block;

            if(pSatData != NULL)
            {
                block = pSatData->getBlock(sat,utc);
            }
            else
            {
                cerr << "Call Spacecraft::getCurrentBlock() error."
                     << "The SatDataReader is not ready."
                     << endl;
            }

            return block;
        }


        /// Get current mass of Spacecraft
        inline double getCurrentMass() const
        {
            double mass;

            if(pSatData != NULL)
            {
                mass = pSatData->getMass(sat,utc);
            }
            else
            {
                cerr << "Call Spacecraft::getCurrentMass() error."
                     << "The SatDataReader is not ready."
                     << endl;
            }

            return mass;
        }


        /// Set current position of Spacecraft
        inline Spacecraft& setCurrentPos(const Vector<double>& pos)
        {
            r = pos;
            return (*this);
        }

        /// Get current position of Spacecraft
        inline Vector<double> getCurrentPos() const
        {
            return r;
        }


        /// Set current velocity of Spacecraft
        inline Spacecraft& setCurrentVel(const Vector<double>& vel)
        {
            v = vel;
            return (*this);
        }

        /// Get current velocity of Spacecraft
        inline Vector<double> getCurrentVel() const
        {
            return v;
        }


        /// Set current position and velocity of Spacecraft
        inline Spacecraft& setCurrentPosVel(const Vector<double>& rv)
        {
            r(0) = rv(0); r(1) = rv(1); r(2) = rv(2);
            v(0) = rv(3); v(1) = rv(4); v(2) = rv(5);

            return (*this);
        }

         /// Get current position and velocity of Spacecraft
        inline Vector<double> getCurrentPosVel() const
        {
            Vector<double> rv(6,0.0);
            rv(0) = r(0); rv(1) = r(1); rv(2) = r(2);
            rv(3) = v(0); rv(4) = v(1); rv(5) = v(2);

            return rv;
        }


        /// Set number of force model parameters to be estimated
        inline Spacecraft& setNumOfParam(const int& n)
        {
            np = n;

            dr_dp0.resize(3,np,0.0);
            dv_dp0.resize(3,np,0.0);

            return (*this);
        }

        /// Get number of force model parameters to be estimated
        inline int getNumOfParam() const
        {
            return np;
        }


        /// Set eclipse of Spacecraft
        inline Spacecraft& setEclipse(const bool& b)
        {
            eclipse = b;
            return (*this);
        }

        /// Get eclipsed of Spacecraft
        inline bool getEclipsed() const
        {
            return eclipse;
        }


        /// Get partial derivatives of currrent position to initial position
        inline Matrix<double> dR_dR0() const
        {
            return dr_dr0;
        }


        /// Get partial derivatives of current position to initial velocity
        inline Matrix<double> dR_dV0() const
        {
            return dr_dv0;
        }


        /// Get partial derivatives of current position to initial force model
        /// parameters
        inline Matrix<double> dR_dP0() const
        {
            return dr_dp0;
        }


        /// Get partial derivatives of current velocity to initial position
        inline Matrix<double> dV_dR0() const
        {
            return dv_dr0;
        }


        /// Get partial derivatives of current velocity to initial velocity
        inline Matrix<double> dV_dV0() const
        {
            return dv_dv0;
        }


        /// Get partial derivatives of current velocity to initial force model
        /// parameters
        inline Matrix<double> dV_dP0() const
        {
            return dv_dp0;
        }


        /// Set current state of Spacecraft
        Spacecraft& setCurrentState(const Vector<double>& state);


        /// Get current state of Spacecraft
        Vector<double> getCurrentState() const;


        /// Get transition matrix of Spacecraft
        Matrix<double> getTransitionMatrix() const;


        /// Get state transition matrix of Spacecraft
        Matrix<double> getStateTransitionMatrix() const;


        /// Get sensitivity matrix of Spacecraft
        Matrix<double> getSensitivityMatrix() const;


        /// Convert kepler orbit elements to position and velocity
        Spacecraft& convertFromKepler(const Vector<double>& kepler);

        /// Convert position and velocity to kepler orbit elements
        Vector<double> convertToKepler() const;


        /// Reset state of Spacecraft
        void resetState();


    private:

        /// Current time
        CommonTime utc;

        /// SatID
        SatID sat;

        /// SatData
        SatDataReader* pSatData;

        /// Current position
        Vector<double> r;       // 3*1

        /// Current velocity
        Vector<double> v;       // 3*1

        /// Number of parameters
        int np;

        /// Eclipse
        bool eclipse;

        /// Current partial derivatives
        Matrix<double> dr_dr0;    // 3*3
        Matrix<double> dr_dv0;    // 3*3
        Matrix<double> dv_dr0;    // 3*3
        Matrix<double> dv_dv0;    // 3*3
        Matrix<double> dr_dp0;    // 3*np
        Matrix<double> dv_dp0;    // 3*np

    }; // End of class 'Spacecraft'


    /**
     * Stream output for Spacecraft objects.  Typically used for debugging.
     * @param s stream to append formatted Spacecraft to.
     * @param t Spacecraft to append to stream \c s.
     * @return reference to \c s.
     */
    std::ostream& operator<<( std::ostream& s,
                              const Spacecraft& sc );
    // @}

}  // End of namespace 'gpstk'

#endif   // SPACECRAFT_HPP

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

#ifndef GPSTK_SPACECRAFT_HPP
#define GPSTK_SPACECRAFT_HPP

#include <iostream>
#include <string>

#include "Vector.hpp"
#include "Matrix.hpp"

#include "CommonTime.hpp"
#include "SatID.hpp"


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
      ~Spacecraft() {};


         /// Set current time of Spacecraft
      inline Spacecraft& setCurrentTime(const CommonTime& utc)
      {
         curTime = utc;

         return (*this);
      }

         /// Get current time of Spacecraft
      inline CommonTime getCurrentTime() const
      {
         return curTime;
      }

         /// Set SatID of Spacecraft
      inline Spacecraft& setSatID(const SatID& si)
      {
         satID = si;

         return (*this);
      }

         /// Get SatID of Spacecraft
      inline SatID getSatID() const
      {
         return satID;
      }


         /// Set block type of Spacecraft
      inline Spacecraft& setBlockType(const std::string& bt)
      {
         blockType = bt;

         return (*this);
      }

         /// Get block type of Spacecraft
      inline std::string getBlockType() const
      {
         return blockType;
      }

         /// Set mass of Spacecraft
      inline Spacecraft& setMass(const double& m)
      {
         mass = m;

         return (*this);
      }


         /// Get mass of Spacecraft
      inline double getMass() const
      {
         return mass;
      }


         /// Set position of Spacecraft
      inline Spacecraft& setPosition(const Vector<double>& pos)
      {
         r = pos;

         return (*this);
      }


         /// Get position of Spacecraft
      inline Vector<double> getPosition() const
      {
         return r;
      }


         /// Set velocity of Spacecraft
      inline Spacecraft& setVelocity(const Vector<double>& vel)
      {
         v = vel;

         return (*this);
      }


         /// Get velocity of Spacecraft
      inline Vector<double> getVelocity() const
      {
         return v;
      }


         /// Set number of force model parameters to be estimated
      inline Spacecraft& setNumOfParam(const int& n)
      {
         numOfParam = n;

         dr_dp0.resize(3*numOfParam, 0.0);
         dv_dp0.resize(3*numOfParam, 0.0);

         return (*this);
      }

         /// Get number of force model parameters to be estimated
      inline int getNumOfParam() const
      {
         return numOfParam;
      }


         /// Get partial derivatives of currrent position to initial position
      inline Vector<double> dR_dR0() const
      {
         return dr_dr0;
      }


         /// Get partial derivatives of current position to initial velocity
      inline Vector<double> dR_V0() const
      {
         return dr_dv0;
      }


         /// Get partial derivatives of current position to initial force model
         /// parameters
      inline Vector<double> dR_dP0() const
      {
         return dr_dp0;
      }


         /// Get partial derivatives of current velocity to initial position
      inline Vector<double> dV_dR0() const
      {
         return dv_dr0;
      }


         /// Get partial derivatives of current velocity to initial velocity
      inline Vector<double> dV_dV0() const
      {
         return dv_dr0;
      }


         /// Get partial derivatives of current velocity to initial force model
         /// parameters
      inline Vector<double> dV_dP0() const
      {
         return dv_dp0;
      }


         /// Set state vector of Spacecraft
      Spacecraft& setStateVector(const Vector<double>& sv);


         /// Get state vector of Spacecraft
      Vector<double> getStateVector() const;


         /// Get transition matrix of Spacecraft
      Matrix<double> getTransitionMatrix() const;


         /// Get state transition matrix of Spacecraft
      Matrix<double> getStateTransitionMatrix() const;


         /// Get sensitivity matrix of Spacecraft
      Matrix<double> getSensitivityMatrix() const;


   private:

         /// Reset state of Spacecraft
      void resetState();


         /// Current time of Spacecraft
      CommonTime curTime;

         /// SatID of Spacecraft
      SatID satID;

         /// Block type of spacecraft
      std::string blockType;

         /// Mass of spacecraft
      double mass;


         /// State vector     6*n + 42
      Vector<double> r;       // 3*1
      Vector<double> v;       // 3*1

         /// Number of parameters
      int numOfParam;

         /// Partial derivatives
      Vector<double> dr_dr0;  // 3*3
      Vector<double> dr_dv0;  // 3*3
      Vector<double> dr_dp0;  // 3*n
      Vector<double> dv_dr0;  // 3*3
      Vector<double> dv_dv0;  // 3*3
      Vector<double> dv_dp0;  // 3*n

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

#endif   // GPSTK_SPACECRAFT_HPP

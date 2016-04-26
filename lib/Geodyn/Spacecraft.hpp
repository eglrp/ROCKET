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

/**
* @file Spacecraft.hpp
* The Spacecraft class encapsulates physical parameters.
*/

#ifndef GPSTK_SPACECRAFT_HPP
#define GPSTK_SPACECRAFT_HPP

#include <iostream>
#include <string>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /**
       * The Spacecraft class encapsulates physical parameters.
       * 
       */
   class Spacecraft
   {
   public:

         /// Default constructor
      Spacecraft() { resetState(); }

         /// Default destructor
      ~Spacecraft() {};


         /// Set satid of the spacecraft
      Spacecraft& setSatID(const SatID& s) { satid = s; return *this; }

         /// Get satid of the spacecraft
      SatID getSatID() const { return satid; }

         /// Set epoch of the spacecraft
      Spacecraft& setEpoch(const CommonTime& t) { epoch = t; return *this; }

         /// Get epoch of the spacecraft
      CommonTime getEpoch() const { return epoch; }

         /// Set block of the spacecraft
      Spacecraft& setBlock(const std::string& b) { block = b; return *this; }

         /// Get block of the spacecraft
      std::string getBlock() const { return block; };

         /// Set mass of the spacecraft
      Spacecraft& setMass(const double& m) { mass = m; return *this; }

         /// Get mass of the spacecraft
      double getMass() const { return mass; };


         /// SC position(R), velocity(V) and force model parameters(P)
      Vector<double> R() {return r;}
      Vector<double> V() {return v;}
      Vector<double> P() {return p;}

         /// SC derivatives
      Vector<double> dR_dR0() {return dr_dr0;}
      Vector<double> dR_dV0() {return dr_dv0;}
      Vector<double> dR_dP0() {return dr_dp0;}
      Vector<double> dV_dR0() {return dv_dr0;}
      Vector<double> dV_dV0() {return dv_dv0;}
      Vector<double> dV_dP0() {return dv_dp0;}

         /// Get number of force model parameters
      int getNumOfP()
      { return p.size(); }

         /// initialize the state vector with position, velocity and force model parameters
      void initStateVector(const Vector<double>& rv, const Vector<double>& dp);

         /// Methods to handle SC state vector 6 + 6*(6+np)
      Vector<double> getStateVector();
      void setStateVector(Vector<double> y);

         /// Methods to handle SC transition matrix 6*(6+np)
      Matrix<double> getTransitionMatrix();
      void setTransitionMatrix(Matrix<double> phiMatrix);

         /// Method to get SC state transition matrix 6*6
      Matrix<double> getStateTransitionMatrix();

         /// Method to get SC sensitivity matrix 6*np
      Matrix<double> getSensitivityMatrix();


   private:

         /// Reset state of the spacecraft
      void resetState();


         /// SatID of the spacecraft
      SatID satid;
         /// Epoch of the spacecraft
      CommonTime epoch;
         /// Block of the spacecraft
      std::string block;
         /// Mass of the spacecraft
      double mass;

         /// State vector     6*n + 42
      Vector<double> r;       // 3 Position
      Vector<double> v;       // 3 Velocity
      Vector<double> p;       // n Force Model Parameters

         /// Partial derivatives
      Vector<double> dr_dr0;      // 3*3  I
      Vector<double> dr_dv0;      // 3*3  0
      Vector<double> dr_dp0;      // 3*n  0
      Vector<double> dv_dr0;      // 3*3  0
      Vector<double> dv_dv0;      // 3*3  I
      Vector<double> dv_dp0;      // 3*n  0

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

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
* @file Spacecraft.hpp
* The Spacecraft class encapsulates physical parameters.
*/

#ifndef GPSTK_SPACECRAFT_HPP
#define GPSTK_SPACECRAFT_HPP

#include <iostream>
#include <string>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "CommonTime.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /**
       * The Spacecraft class encapsulates physical parameters
       * 
       */
   class Spacecraft
   {
   public:

         /// Default constructor
      Spacecraft(std::string name = "spacecraft");

         /// Default destructor
      ~Spacecraft() {};

         /// SC position(R), velocity(V) and dynamic parameters(P)
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
      
         /// initialize the state vector with position and velocity and force model parameters
      void initStateVector(Vector<double> rv, Vector<double> dp = Vector<double>(0,0.0));

         /// Methods to handle SC state vector
      Vector<double> getStateVector();
      void setStateVector(Vector<double> y);

         /// Methods to handle SC transition matrix
      Matrix<double> getTransitionMatrix();
      void setTransitionMatrix(Matrix<double> phiMatrix);
      
         /// Method to get SC state transition matrix 6*6
      Matrix<double> getStateTransitionMatrix();

         /// Method to get SC sensitivity matrix 6*np
      Matrix<double> getSensitivityMatrix();


         // Methods to handle SC physical parameters
      
         /// Get spacecraft name
      std::string getName()
      { return scName; }

         /// Set spacecraft name
      Spacecraft& setName(std::string satName)
      { scName = satName; return (*this); }

         /// Get reflect coefficient
      double getReflectCoeff()
      { return reflectCoeff; }

         /// Set reflect coefficient
      Spacecraft& setReflectCoeff(double Cr)
      { reflectCoeff = Cr; return (*this); }

         /// Get drag coefficient
      double getDragCoeff()
      { return dragCoeff; }

         /// Set drag coefficient
      Spacecraft& setDragCoeff(double Cd)
      { dragCoeff = Cd; return (*this); }

         /// Get drag area
      double getDragArea()
      { return crossArea; }

         /// Set drag area
      Spacecraft& setDragArea(double satArea)
      { crossArea = satArea; return (*this); }

         /// Get SRP area
      double getSRPArea()
      { return crossArea; }

         /// Set SRP area
      Spacecraft& setSRPArea(double satArea)
      { crossArea = satArea; return (*this); }

         /// Get dry mass
      double getDryMass()
      { return dryMass; }

         /// Set dry mass
      Spacecraft& setDryMass(double satMass)
      { dryMass = satMass; return (*this); }

         /// Get block number
      int getBlockNum()
      { return blockNum; }

         /// Set block number
      Spacecraft& setBlockNum(int satBlock)
      { blockNum = satBlock; return (*this); }


         /// some basic test
      void test();


   protected:

         /// Reset state of the spacecraft
      void resetState();
      
         /// The name of the spacecraft (e.g. "NCC-1701-D") 
      std::string scName;
      
         /// Object to hold epoch in UTC
      CommonTime utc;

         /// State vector     6*n + 42
      Vector<double> r;         // 3 Position
      Vector<double> v;         // 3 Velocity
      Vector<double> p;         // n dynamical parameters


         /// Change the Vector to Matrix ???
      Vector<double> dr_dr0;      // 3*3  I
      Vector<double> dr_dv0;      // 3*3  0
      Vector<double> dr_dp0;      // 3*n  0
      Vector<double> dv_dr0;      // 3*3  0
      Vector<double> dv_dv0;      // 3*3  I
      Vector<double> dv_dp0;      // 3*n  0

         /// Coefficient of Reflectivity
      double reflectCoeff;

         /// Coefficient of drag
      double dragCoeff;

         /// Cross sectional (reflective) area [m^2]
      double crossArea;

         /// Mass [kg]
      double dryMass;

         /// Block number for GNSS satellites, added by kfkuang, 2015/09/09
         // BLOCK NUMBER (GPS):
         //     1: BLOCK I
         //     2: BLOCK II
         //     3: BLOCK IIA
         //     4: BLOCK IIR
         //     5: BLOCK IIR-A
         //     6: BLOCK IIR-B
         //     7: BLOCK IIR-M
         //     8: BLOCK IIF
         // BLOCK NUMBER (GLONASS):
         //     101: GLONASS
         //     102: GLONASS-M
         //     103: GLONASS-K1
      int blockNum;

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

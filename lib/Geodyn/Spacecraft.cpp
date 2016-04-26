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
* @file Spacecraft.cpp
* The Spacecraft class encapsulates physical parameters.
*/

#include "Spacecraft.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"

using namespace std;

namespace gpstk
{
   // reset State
   void Spacecraft::resetState()
   {
       // reset state vector
       r.resize(3,0.0); v.resize(3,0.0); p.resize(0,0.0);

       // reset partial derivatives
       dr_dr0.resize(9,0.0); dr_dv0.resize(9,0.0); dr_dp0.resize(0,0.0);
       dv_dr0.resize(9,0.0); dv_dv0.resize(9,0.0); dv_dp0.resize(0,0.0);

       dr_dr0(0) = 1.0; dr_dr0(4) = 1.0; dr_dr0(8) = 1.0;
       dv_dv0(0) = 1.0; dv_dv0(4) = 1.0; dv_dv0(8) = 1.0;
   }


   // init State Vector
   void Spacecraft::initStateVector(const Vector<double>& rv, const Vector<double>& dp)
   {
      // first the size of input vector should be checked
      if(rv.size()!=6)
      {
         Exception e("Error in Spacecraft::initStateVector(): the size of rv should be 6.");
         GPSTK_THROW(e);
      }

      resetState();
      
      // set position
      r(0) = rv(0); r(1) = rv(1); r(2) = rv(2);

      // set velocity
      v(0) = rv(3); v(1) = rv(4); v(2) = rv(5);
      
      // set force model parameters
      p = dp;

      // set dr_dp0 and dv_dp0
      const int np = p.size();

      dr_dp0.resize(3*np,0.0);
      dv_dp0.resize(3*np,0.0);

   }  // End of method 'Spacecraft::initStateVector()'


   // get State Vector 6 + 6*(6+np)
   Vector<double> Spacecraft::getStateVector()
   {
      const int np=p.size();
      Vector<double> y(6*np+42,0.0);

      // position
      y(0) = r(0); y(1) = r(1); y(2) = r(2);

      // position related partial derivatives
      for(int i=0; i<3*3; i++)
      {
          y( 3+i) = dr_dr0(i);
          y(12+i) = dr_dv0(i);
      }
      for(int i=0; i<3*np; i++)
      {
          y(21+i) = dr_dp0(i);
      }

      // velocity
      y(21+3*np) = v(0); y(22+3*np) = v(1); y(23+3*np) = v(2);

      // velocity related partial derivatives
      for(int i=0; i<3*3; i++)
      {
          y(24+3*np+i) = dv_dr0(i);
          y(33+3*np+i) = dv_dv0(i);
      }
      for(int i=0; i<3*np; i++)
      {
          y(42+3*np+i) = dv_dp0(i);
      }

      // return
      return y;

   }  // End of method 'Spacecraft::getStateVector()'


   // set State Vector 
   void Spacecraft::setStateVector(Vector<double> y)
   {
      // check the size of y !!!

      const int dim = y.size();
      const int np = (dim-42)/6;
      
      // resize the force model parameters
      p.resize(np,0.0);
      dr_dp0.resize(3*np,0.0);
      dv_dp0.resize(3*np,0.0);
      
      // set position
      r(0) = y(0); r(1) = y(1); r(2) = y(2);

      // set position related partial derivatives
      for(int i=0; i<3*3; i++)
      {
          dr_dr0(i) = y( 3+i);
          dr_dv0(i) = y(12+i);
      }
      for(int i=0; i<3*np; i++)
      {
          dr_dp0(i) = y(21+i);
      }

      // set velocity
      v(0) = y(21+3*np); v(1) = y(22+3*np); v(2) = y(23+3*np);

      // set velocity related partial derivatives
      for(int i=0; i<3*3; i++)
      {
          dv_dr0(i) = y(24+3*np+i);
          dv_dv0(i) = y(33+3*np+i);
      }
      for(int i=0; i<3*np; i++)
      {
          dv_dp0(i) = y(42+3*np+i);
      }

   }  // End of method 'Spacecraft::setStateVector(Vector<double> y)'


   // get Transition Matrix
   Matrix<double> Spacecraft::getTransitionMatrix()
   {
      /* Transition Matrix
          |                          |
          | dr_dr0   dr_dv0   dr_dp0 |
          |                          |
      tra=| dv_dr0   dv_dv0   dv_dp0 |
          |                          |
          | 0        0        I      |
          |                          |
      */
      
      const int np = p.size();
      Matrix<double> traMatrix(6+np,6+np,0.0);

      // state transition matrix
      for(int i=0; i<3; i++)
          for(int j=0; j<3; j++)
          {
              traMatrix(i,j)        =   dr_dr0(i*3+j);
              traMatrix(i,j+3)      =   dr_dv0(i*3+j);

              traMatrix(i+3,j)      =   dv_dr0(i*3+j);
              traMatrix(i+3,j+3)    =   dv_dv0(i*3+j);
          }

      // sensitivity matrix
      for(int i=0; i<3; i++)
          for(int j=0; j<np; j++)
          {
              traMatrix(i,j+6)      =   dr_dp0(i*np+j);

              traMatrix(i+3,j+6)    =   dv_dp0(i*np+j);
          }

      // I
      for(int i=6; i<6+np; i++)
      {  
          traMatrix(i,i) = 1.0;
      }

      return traMatrix;

   }  // End of method 'Spacecraft::getTransitionMatrix()'


   // set Transition Matrix
   void Spacecraft::setTransitionMatrix(Matrix<double> traMatrix)
   {
      // check the size of tranMatrix !!!

      /* Transition Matrix
          |                          |
          | dr_dr0   dr_dv0   dr_dp0 |
          |                          |
      tra=| dv_dr0   dv_dv0   dv_dp0 |
          |                          |
          | 0        0        I      |
          |                          |
      */

      const int np = traMatrix.rows()-6;
      p.resize(np,0.0);
      dr_dp0.resize(3*np,0.0);
      dv_dp0.resize(3*np,0.0);

      // state transition matrix
      for(int i=0; i<3; i++)
          for(int j=0; j<3; j++)
          {
              dr_dr0(i*3+j) = traMatrix(i,j);
              dr_dv0(i*3+j) = traMatrix(i,j+3);

              dv_dr0(i*3+j) = traMatrix(i+3,j);
              dv_dv0(i*3+j) = traMatrix(i+3,j+3);
          }

      // sensitivity matrix
      for(int i=0; i<3; i++)
          for(int j=0; j<np; j++)
          {
              dr_dp0(i*np+j) = traMatrix(i,j+6);

              dv_dp0(i*np+j) = traMatrix(i+3,j+6);
          }

   }  // End of method 'Spacecraft::setTransitionMatrix()'


   // get State Transition Matrix 6*6
   Matrix<double> Spacecraft::getStateTransitionMatrix()
   {
      /* State Transition Matrix
          |                  |
          | dr_dr0   dr_dv0  |
      phi=|                  |
          | dv_dr0   dv_dv0  |
          |                  |
      */

      Matrix<double> phiMatrix(6,6,0.0);

      // state transition matrix
      for(int i=0; i<3; i++)
          for(int j=0; j<3; j++)
          {
              phiMatrix(i,j)      =   dr_dr0(i*3+j);
              phiMatrix(i,j+3)    =   dr_dv0(i*3+j);

              phiMatrix(i+3,j)    =   dv_dr0(i*3+j);
              phiMatrix(i+3,j+3)  =   dv_dv0(i*3+j);
          }

      return phiMatrix;

   }  // End of method 'Spacecraft::getStateTransitionMatrix()'


   // get Sensitivity Matrix 6*np
   Matrix<double> Spacecraft::getSensitivityMatrix()
   {
      /* Sensitivity Matrix
          |        |
          | dr_dp0 |
      s = |        |
          | dv_dp0 |
          |        |
      */
      const int np = p.size();
      Matrix<double> senMatrix(6,np,0.0);

      // sensitivity matrix
      for(int i=0; i<3; i++)
          for(int j=0; j<np; j++)
          {
              senMatrix(i,j)      =   dr_dp0(i*np+j);

              senMatrix(i+3,j)    =   dv_dp0(i*np+j);
          }

      return senMatrix;

   }  // End of method 'Spacecraft::getSensitivityMatrix()'


      // Stream output for Spacecraft objects.  Typically used for debugging.
      // @param s stream to append formatted Spacecraft to.
      // @param sc Spacecraft to append to stream \c s.
      // @return reference to \c s.
   ostream& operator<<( ostream& s, 
                        const gpstk::Spacecraft& sc )
   {
       s << sc.getSatID() << ' ' << CivilTime(sc.getEpoch()) << ' '
         << sc.getBlock() << ' ' << sc.getMass();

       return s;

   }  // End of 'ostream& operator<<()'

}  // End of namespace 'gpstk'

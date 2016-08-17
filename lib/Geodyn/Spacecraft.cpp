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
//  Kaifa Kuang - Kaifa Kuang . 2016
//
//============================================================================

/**
* @file Spacecraft.cpp
* Class to encapsulate Spacecraft related parameters.
*/

#include "Spacecraft.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"


using namespace std;


namespace gpstk
{

      /// Reset state
   void Spacecraft::resetState()
   {
      // reset state vector
      r.resize(3,0.0); v.resize(3,0.0);

      // reset partial derivatives
      dr_dr0.resize(9,0.0); dr_dv0.resize(9,0.0); dr_dp0.resize(0,0.0);
      dv_dr0.resize(9,0.0); dv_dv0.resize(9,0.0); dv_dp0.resize(0,0.0);
      dr_dr0(0) = 1.0; dr_dr0(4) = 1.0; dr_dr0(8) = 1.0;
      dv_dv0(0) = 1.0; dv_dv0(4) = 1.0; dv_dv0(8) = 1.0;

   }  // End of method 'Spacecraft::resetState()'


      /// Set state vector
   Spacecraft& Spacecraft::setStateVector(const Vector<double>& sv)
   {
      // check the size of sv
      if(sv.size() == (42+6*numOfParam))
      {
         // resize dr_dp0 and dv_dp0
         dr_dp0.resize(3*numOfParam, 0.0);
         dv_dp0.resize(3*numOfParam, 0.0);

         // set position
         r(0) = sv(0); r(1) = sv(1); r(2) = sv(2);

         // set velocity
         v(0) = sv(3); v(1) = sv(4); v(2) = sv(5);


         // set position related partial derivatives
         for(int i=0; i<3*3; ++i)
         {
             dr_dr0(i) = sv( 6+i);
             dr_dv0(i) = sv(15+i);
         }

         for(int i=0; i<3*numOfParam; ++i)
         {
             dr_dp0(i) = sv(24+i);
         }

         // set velocity related partial derivatives
         for(int i=0; i<3*3; ++i)
         {
             dv_dr0(i) = sv(24+3*numOfParam+i);
             dv_dv0(i) = sv(33+3*numOfParam+i);
         }

         for(int i=0; i<3*numOfParam; ++i)
         {
             dv_dp0(i) = sv(42+3*numOfParam+i);
         }

      }
      else
      {
         cerr << "Spacecraft::setStateVector() error, the size does not match." << endl;
      }

      return (*this);

   }  // End of method 'Spacecraft::setStateVector()'


      /// Get state vector of Spacecraft
   Vector<double> Spacecraft::getStateVector() const
   {

      Vector<double> sv(42+6*numOfParam, 0.0);


      //// Parameters related to equation of motion ////

      // position
      sv(0) = r(0); sv(1) = r(1); sv(2) = r(2);

      // velocity
      sv(3) = v(0); sv(4) = v(1); sv(5) = v(2);


      //// parameters related to equation of variation ////

      // position related partial derivatives

      // dr_dr0, dr_dv0
      for(int i=0; i<3*3; ++i)
      {
          sv( 6+i) = dr_dr0(i);
          sv(15+i) = dr_dv0(i);
      }

      // dr_dp0
      for(int i=0; i<3*numOfParam; ++i)
      {
          sv(24+i) = dr_dp0(i);
      }

      // velocity related partial derivatives

      // dv_dr0, dv_dv0
      for(int i=0; i<3*3; ++i)
      {
          sv(24+3*numOfParam+i) = dv_dr0(i);
          sv(33+3*numOfParam+i) = dv_dv0(i);
      }

      // dv_dp0
      for(int i=0; i<3*numOfParam; ++i)
      {
          sv(42+3*numOfParam+i) = dv_dp0(i);
      }


      return sv;

   }  // End of method 'Spacecraft::getStateVector()'



      /// Get transition matrix
   Matrix<double> Spacecraft::getTransitionMatrix() const
   {
      ////////////////// Transition Matrix ////////////////
      //                                                 //
      //             |                          |        //
      //             | dr_dr0   dr_dv0   dr_dp0 |        //
      //             |                          |        //
      //    tMat  =  | dv_dr0   dv_dv0   dv_dp0 |        //
      //             |                          |        //
      //             | 0        0        I      |        //
      //             |                          |        //
      //                                                 //
      /////////////////////////////////////////////////////

      Matrix<double> tMat(6+numOfParam,6+numOfParam, 0.0);

      // state transition matrix
      for(int i=0; i<3; ++i)
      {
          for(int j=0; j<3; ++j)
          {
              tMat(i+0,j+0)   =   dr_dr0(i*3+j);
              tMat(i+0,j+3)   =   dr_dv0(i*3+j);

              tMat(i+3,j+0)   =   dv_dr0(i*3+j);
              tMat(i+3,j+3)   =   dv_dv0(i*3+j);
          }
      }

      // sensitivity matrix
      for(int i=0; i<3; ++i)
      {
          for(int j=0; j<numOfParam; ++j)
          {
              tMat(i+0,j+6)   =   dr_dp0(i*numOfParam+j);

              tMat(i+3,j+6)   =   dv_dp0(i*numOfParam+j);
          }
      }

      // identity part
      for(int i=6; i<6+numOfParam; ++i)
      {
          tMat(i,i) = 1.0;
      }

      return tMat;

   }  // End of method 'Spacecraft::getTransitionMatrix()'


      // Get state transition matrix
   Matrix<double> Spacecraft::getStateTransitionMatrix() const
   {
      ////////////// State Transition Matrix //////////////
      //                                                 //
      //             |                  |                //
      //             | dr_dr0   dr_dv0  |                //
      //    stMat =  |                  |                //
      //             | dv_dr0   dv_dv0  |                //
      //             |                  |                //
      //                                                 //
      /////////////////////////////////////////////////////

      Matrix<double> stMat(6,6,0.0);

      // state transition matrix
      for(int i=0; i<3; ++i)
      {
          for(int j=0; j<3; ++j)
          {
              stMat(i+0,j+0)  =  dr_dr0(i*3+j);
              stMat(i+0,j+3)  =  dr_dv0(i*3+j);

              stMat(i+3,j+0)  =  dv_dr0(i*3+j);
              stMat(i+3,j+3)  =  dv_dv0(i*3+j);
          }
      }

      return stMat;

   }  // End of method 'Spacecraft::getStateTransitionMatrix()'


   // Get sensitivity matrix
   Matrix<double> Spacecraft::getSensitivityMatrix() const
   {
      ////////// Sensitivity Matrix //////////////
      //                                        //
      //                |        |              //
      //                | dr_dp0 |              //
      //       sMat  =  |        |              //
      //                | dv_dp0 |              //
      //                |        |              //
      //                                        //
      ////////////////////////////////////////////

      Matrix<double> sMat(6,numOfParam, 0.0);

      // sensitivity matrix
      for(int i=0; i<3; ++i)
      {
          for(int j=0; j<numOfParam; ++j)
          {
              sMat(i+0,j)  =  dr_dp0(i*numOfParam+j);
              sMat(i+3,j)  =  dv_dp0(i*numOfParam+j);
          }
      }

      return sMat;

   }  // End of method 'Spacecraft::getSensitivityMatrix()'


      /** Stream output for Spacecraft objects. Typically used for debugging.
       * @param s    stream to append formatted Spacecraft to
       * @param sc   Spacecraft to append to stream \c s
       */
   ostream& operator<<( ostream& s,
                        const gpstk::Spacecraft& sc )
   {
       s << sc.getSatID() << ' ' << CivilTime(sc.getCurrentTime()) << ' '
         << sc.getBlockType() << ' ' << sc.getMass();

       return s;

   }  // End of 'ostream& operator<<()'


}  // End of namespace 'gpstk'

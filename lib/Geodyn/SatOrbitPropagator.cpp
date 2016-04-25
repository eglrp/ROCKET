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
* @file SatOrbitPropagator.cpp
* 
*/

#include "SatOrbitPropagator.hpp"
#include "KeplerOrbit.hpp"
#include "IERSConventions.hpp"
#include "GNSSconstants.hpp"

using namespace std;

namespace gpstk
{

      // Constructor
   SatOrbitPropagator::SatOrbitPropagator()
      : pIntegrator(NULL),
        curT(0.0)
   {
      setDefaultIntegrator();
      setDefaultOrbit();

      setStepSize(1.0);

      setFMT.clear();
      pOrbit->setForceModelType(setFMT);
    
   }  // End of constructor 'SatOrbitPropagator::SatOrbitPropagator()'
   

      // Default destructor
   SatOrbitPropagator::~SatOrbitPropagator()
   {
      pIntegrator = NULL;
      pOrbit = NULL;
   }
   
      /* Take a single integration step.
       *
       * @param t     time or independent variable
       * @param y     containing needed inputs (usually the state)
       * @param tf    next time
       * @return      containing the new state
       */
   Vector<double> SatOrbitPropagator::integrateTo(double t,Vector<double> y,double tf)
   {
      try
      {
         curT = tf;

         cout << "SatOrbitPropagator::integrateTo()" << endl;
         curState = pIntegrator->integrateTo(t,y,pOrbit,tf);
         
         updateMatrix();

         return curState;
      }
      catch(...)
      {
         Exception e("Error in OrbitPropagator::integrateTo()");
         GPSTK_THROW(e);
      }

   }  // End of method 'SatOrbitPropagator::integrateTo()'


   bool SatOrbitPropagator::integrateTo(double tf)
   {
      try
      {
         double t = curT;
         Vector<double> y = curState;

         curT = tf;

         cout << "before pIntegrator->integrateTo()" << endl;
         curState = pIntegrator->integrateTo(t, y, pOrbit, tf);
         cout << "after pIntegrator->integrateTo()" << endl;
         
         updateMatrix();

         return true;
      }
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }

      catch(...)
      {
         Exception e("Unknown error in SatOrbitPropagator::integrateTo()");
         GPSTK_THROW(e);
      }

      return false;

   }  // End of method 'SatOrbitPropagator::integrateTo()'

      /*
       * set init state
       * utc0   init epoch
       * rv0    init state
       */
   SatOrbitPropagator& SatOrbitPropagator::setInitState(CommonTime utc0, Vector<double> rv0)
   {
      const int np = setFMT.size();
      
      curT = double(0.0);
      curState.resize(42+6*np,0.0);
      
      // position and velocity
      curState(0) = rv0(0);
      curState(1) = rv0(1);
      curState(2) = rv0(2);
      curState(3) = rv0(3);
      curState(4) = rv0(4);
      curState(5) = rv0(5);

      double I[9] = {1.0, 0.0 ,0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
      
      for(int i = 0; i < 9; i++)
      {
         curState(6+i) = I[i];
         curState(33+3*np+i) = I[i];
      }
      
      updateMatrix();
      
      // set reference epoch
      setRefEpoch(utc0);


      return (*this);

   }  // End of method 'SatOrbitPropagator::setInitState()'

   
      /// update phiMatrix sMatrix and rvState from curState
   void SatOrbitPropagator::updateMatrix()
   {
      const int np = getNP();

      Vector<double> dr_dr0(9,0.0);
      Vector<double> dr_dv0(9,0.0);
      Vector<double> dr_dp0(3*np,0.0);
      Vector<double> dv_dr0(9,0.0);
      Vector<double> dv_dv0(9,0.0);
      Vector<double> dv_dp0(3*np,0.0);

      for(int i = 0; i < 9; i++)
      {
         dr_dr0(i) = curState(6+i);
         dr_dv0(i) = curState(15+i);

         dv_dr0(i) = curState(24+3*np+i);
         dv_dv0(i) = curState(33+3*np+i);
      }
      for(int i = 0;i < 3*np; i++)
      {
         dr_dp0 = curState(24+i);
         dv_dp0 = curState(42+3*np+i);
      }

      // update phiMatrix
      phiMatrix.resize(6,6,0.0);

      // dr/dr0
      phiMatrix(0,0) = dr_dr0(0);
      phiMatrix(0,1) = dr_dr0(1);
      phiMatrix(0,2) = dr_dr0(2);
      phiMatrix(1,0) = dr_dr0(3);
      phiMatrix(1,1) = dr_dr0(4);
      phiMatrix(1,2) = dr_dr0(5);
      phiMatrix(2,0) = dr_dr0(6);
      phiMatrix(2,1) = dr_dr0(7);
      phiMatrix(2,2) = dr_dr0(8);
      // dr/dv0
      phiMatrix(0,3) = dr_dv0(0);
      phiMatrix(0,4) = dr_dv0(1);
      phiMatrix(0,5) = dr_dv0(2);
      phiMatrix(1,3) = dr_dv0(3);
      phiMatrix(1,4) = dr_dv0(4);
      phiMatrix(1,5) = dr_dv0(5);
      phiMatrix(2,3) = dr_dv0(6);
      phiMatrix(2,4) = dr_dv0(7);
      phiMatrix(2,5) = dr_dv0(8);
      // dv/dr0
      phiMatrix(3,0) = dv_dr0(0);
      phiMatrix(3,1) = dv_dr0(1);
      phiMatrix(3,2) = dv_dr0(2);
      phiMatrix(4,0) = dv_dr0(3);
      phiMatrix(4,1) = dv_dr0(4);
      phiMatrix(4,2) = dv_dr0(5);
      phiMatrix(5,0) = dv_dr0(6);
      phiMatrix(5,1) = dv_dr0(7);
      phiMatrix(5,2) = dv_dr0(8);
      // dv/dv0
      phiMatrix(3,3) = dv_dv0(0);
      phiMatrix(3,4) = dv_dv0(1);
      phiMatrix(3,5) = dv_dv0(2);
      phiMatrix(4,3) = dv_dv0(3);
      phiMatrix(4,4) = dv_dv0(4);
      phiMatrix(4,5) = dv_dv0(5);
      phiMatrix(5,3) = dv_dv0(6);
      phiMatrix(5,4) = dv_dv0(7);
      phiMatrix(5,5) = dv_dv0(8);

      // update sMatrix 6*np
      sMatrix.resize(6,np,0.0);
      for(int i = 0; i<np; i++)
      {
         sMatrix(0,i) = dr_dp0(0*np+i);
         sMatrix(1,i) = dr_dp0(1*np+i);
         sMatrix(2,i) = dr_dp0(2*np+i);

         sMatrix(3,i) = dv_dp0(0*np+i);
         sMatrix(4,i) = dv_dp0(1*np+i);
         sMatrix(5,i) = dv_dp0(2*np+i);
      }
      
      // update rvVector
      rvVector.resize(6,0.0);
      for(int i = 0; i < 6; i++) 
      { 
         rvVector(i) = curState(i);
      }

   }  // End of method 'SatOrbitPropagator::updateMatrix()'
   

      /* set initial state of the the integrator
       *
       *  v      3
       * dr_dr0   3*3
       * dr_dv0   3*3
       * dr_dp0   3*np
       * dv_dr0   3*3
       * dv_dv0   3*3
       * dv_dp0   3*np
       */
   void SatOrbitPropagator::setState(Vector<double> state)
   {
      int np = (state.size()-42)/6;
      if(np<0)
      {
         Exception e("The size of the imput state is not valid");
         GPSTK_THROW(e);
      }
      curT = 0;
      curState.resize(state.size(),0.0);
      for(size_t i=0;i<state.size();i++)
      {
         curState(i) = state(i);
      }

      updateMatrix();

   }  // End of method 'SatOrbitPropagator::setState()'


   Vector<double> SatOrbitPropagator::rvState(bool bJ2k)
   {
      if(bJ2k == true)      // state ICRF
      {
         return rvVector;
      }
      else                 // state in ITRF
      {
         CommonTime utc = getCurTime();
         Matrix<double> E = C2TMatrix(utc);

         Vector<double> rVector(3,0.0), vVector(3,0.0);
         for(int i=0; i<3; i++)
         {
             rVector[i] = rvVector[i];
             vVector[i] = rvVector[i+3];
         }

         rVector = E * rVector;
         vVector = E * vVector; // need to add the term dE * rVector

         for(int i=0; i<3; i++)
         {
             rvVector[i] = rVector[i];
             rvVector[i+3] = vVector[i];
         }

         return rvVector;
      }
      
   }  // End of method 'SatOrbitPropagator::rvState()'


      /// write curT curState to a file
   void SatOrbitPropagator::writeToFile(ostream& s)
   {
      CommonTime utcRef = pOrbit->getRefEpoch();
      CommonTime utc = utcRef;
      utc += curT;

      const int np = getNP();

      s << fixed;
      s << "#" << utc << " "
        << setprecision(12) << MJD(utc).mjd << endl;

      for(int i=0;i<6;i++)
      {
         s << setw(20) << setprecision(12) << rvVector(i) << " ";
      }
      s << endl;

      // [phi s]
      for(int i=0;i<6;i++)
      {
         for(int j=0;j<6;j++)
         {
            s << setw(20) << setprecision(12) << phiMatrix(i,j) << " ";
         }
         for(int j=0;j<np;j++)
         {
            s << setw(20) << setprecision(12) << sMatrix(i,j) << " ";
         }

         s << endl;
      }
   }

      /*
       * Stream output for SatOrbitPropagator objects.  Typically used for debugging.
       * @param s stream to append formatted SatOrbitPropagator to.
       * @param op SatOrbitPropagator to append to stream  s.
       * @return reference to  s.
       */
   ostream& operator<<(ostream& s,SatOrbitPropagator& op)
   {
      op.writeToFile(s);

      return s;
   }


   /*
   void OrbitPropagator::setForceModel(ForceModelSetting& fms)
   {
      if(pOrbit)
      {
         pOrbit->setForceModel(fms);
      }
   }*/


}  // End of namespace 'gpstk'

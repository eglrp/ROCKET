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
* @file ForceModelList.cpp
* ForceModelList is a countainer for force models.
*/

#include "ForceModelList.hpp"

using namespace std;

namespace gpstk
{

      // Interface implementation for the 'ForceModel'
   Vector<double> ForceModelList::getDerivatives(CommonTime utc, EarthBody& rb, Spacecraft& sc)
   {
//      cout << sc.P() << endl;
//      cout << "the size of fmList " << fmList.size() << endl;
//      cout << "the size of fmtSet " << fmtSet.size() << endl;
      const int np = fmtSet.size();

      a.resize(3,0.0);
      da_dr.resize(3,3,0.0); da_dv.resize(3,3,0.0); da_dp.resize(3,np,0.0);

      da_dCd.resize(3,0.0);
      da_dCr.resize(3,0.0);

      da_dGx.resize(3,0.0); da_dGy.resize(3,0.0); da_dGz.resize(3,0.0);

      da_dD0.resize(3,0.0); da_dDc.resize(3,0.0); da_dDs.resize(3,0.0);
      da_dY0.resize(3,0.0); da_dYc.resize(3,0.0); da_dYs.resize(3,0.0);
      da_dB0.resize(3,0.0); da_dBc.resize(3,0.0); da_dBs.resize(3,0.0);

      da_dar.resize(3,0.0); da_dat.resize(3,0.0); da_dan.resize(3,0.0);

      for(list<ForceModel*>::iterator it = fmList.begin();
          it != fmList.end();
          ++it)
      {
          (*it)->doCompute(utc,rb,sc);

          a       += (*it)->getAccel();
          da_dr   += (*it)->partialR();
          da_dv   += (*it)->partialV();
          da_dp   += (*it)->partialP();

          da_dCd  += (*it)->partialCd();
          da_dCr  += (*it)->partialCr();

          da_dGx  += (*it)->partialGx();
          da_dGy  += (*it)->partialGy();
          da_dGz  += (*it)->partialGz();

          da_dD0  += (*it)->partialD0();
          da_dDc  += (*it)->partialDc();
          da_dDs  += (*it)->partialDs();
          da_dY0  += (*it)->partialY0();
          da_dYc  += (*it)->partialYc();
          da_dYs  += (*it)->partialYs();
          da_dB0  += (*it)->partialB0();
          da_dBc  += (*it)->partialBc();
          da_dBs  += (*it)->partialBs();

          da_dar  += (*it)->partialAr();
          da_dat  += (*it)->partialAt();
          da_dan  += (*it)->partialAn();
      }
      
      // declare a counter
      int i = 0;

      for(set<ForceModel::ForceModelType>::iterator it = fmtSet.begin();
         it!=fmtSet.end();
         it++)
      {
         if((*it) == ForceModel::Cd)
         {
             da_dp(0,i) = da_dCd(0);
             da_dp(1,i) = da_dCd(1);
             da_dp(2,i) = da_dCd(2);
         }
         else if((*it) == ForceModel::Cr)
         {
             da_dp(0,i) = da_dCr(0);
             da_dp(1,i) = da_dCr(1);
             da_dp(2,i) = da_dCr(2);
         }
         else if((*it) == ForceModel::Gx)
         {
             da_dp(0,i) = da_dGx(0);
             da_dp(1,i) = da_dGx(1);
             da_dp(2,i) = da_dGx(2);
         }
         else if((*it) == ForceModel::Gy)
         {
             da_dp(0,i) = da_dGy(0);
             da_dp(1,i) = da_dGy(1);
             da_dp(2,i) = da_dGy(2);
         }
         else if((*it) == ForceModel::Gz)
         {
             da_dp(0,i) = da_dGz(0);
             da_dp(1,i) = da_dGz(1);
             da_dp(2,i) = da_dGz(2);
         }
         else if((*it) == ForceModel::D0)
         {
             da_dp(0,i) = da_dD0(0);
             da_dp(1,i) = da_dD0(1);
             da_dp(2,i) = da_dD0(2);
         }
         else if((*it) == ForceModel::Dc)
         {
             da_dp(0,i) = da_dDc(0);
             da_dp(1,i) = da_dDc(1);
             da_dp(2,i) = da_dDc(2);
         }
         else if((*it) == ForceModel::Ds)
         {
             da_dp(0,i) = da_dDs(0);
             da_dp(1,i) = da_dDs(1);
             da_dp(2,i) = da_dDs(2);
         }
         else if((*it) == ForceModel::Y0)
         {
             da_dp(0,i) = da_dY0(0);
             da_dp(1,i) = da_dY0(1);
             da_dp(2,i) = da_dY0(2);
         }
         else if((*it) == ForceModel::Yc)
         {
             da_dp(0,i) = da_dYc(0);
             da_dp(1,i) = da_dYc(1);
             da_dp(2,i) = da_dYc(2);
         }
         else if((*it) == ForceModel::Ys)
         {
             da_dp(0,i) = da_dYs(0);
             da_dp(1,i) = da_dYs(1);
             da_dp(2,i) = da_dYs(2);
         }
         else if((*it) == ForceModel::B0)
         {
             da_dp(0,i) = da_dB0(0);
             da_dp(1,i) = da_dB0(1);
             da_dp(2,i) = da_dB0(2);
         }
         else if((*it) == ForceModel::Bc)
         {
             da_dp(0,i) = da_dBc(0);
             da_dp(1,i) = da_dBc(1);
             da_dp(2,i) = da_dBc(2);
         }
         else if((*it) == ForceModel::Bs)
         {
             da_dp(0,i) = da_dBs(0);
             da_dp(1,i) = da_dBs(1);
             da_dp(2,i) = da_dBs(2);
         }
         else if((*it) == ForceModel::ar)
         {
             da_dp(0,i) = da_dar(0);
             da_dp(1,i) = da_dar(1);
             da_dp(2,i) = da_dar(2);
         }
         else if((*it) == ForceModel::at)
         {
             da_dp(0,i) = da_dat(0);
             da_dp(1,i) = da_dat(1);
             da_dp(2,i) = da_dat(2);
         }
         else if((*it) == ForceModel::an)
         {
             da_dp(0,i) = da_dan(0);
             da_dp(1,i) = da_dan(1);
             da_dp(2,i) = da_dan(2);
         }
         else
         {
            Exception e("Error in ForceModelList::getDerivatives():Unexpected ForceModelType");
            GPSTK_THROW(e);
         }

         i++;
      }

      /* Transition Matrix (6+np)*(6+np)
           |                          |
           | dr_dr0   dr_dv0   dr_dp0 |
           |                          |
      phi= | dv_dr0   dv_dv0   dv_dp0 |
           |                          |
           | 0        0        I      |
           |                          |
      */
      Matrix<double> phi = sc.getTransitionMatrix();

//      cout << phi << endl;

      /* A Matrix (6+np)*(6+np)
          |                          |
          | 0         I        0     |
          |                          |
      A = | da_dr     da_dv    da_dp |
          |                          |
          | 0         0        0     |
          |                          |
      */
      Matrix<double> A = getAMatrix();

//      cout << A << endl;

      /* dphi Matrix
             |                          |
             | dv_dr0   dv_dv0   dv_dp0 |
             |                          |
      dphi = | da_dr0   da_dv0   da_dp0 |
             |                          |
             | 0        0        0      |
             |                          |

      da_dr0 = da_dr*dr_dr0 + da_dv*dv_dr0

      da_dv0 = da_dr*dr_dv0 + da_dv*dv_dv0

      da_dp0 = da_dr*dr_dp0 + da_dv*dv_dp0 + da_dp0;
      */
      Matrix<double> dphi = A * phi;

//      cout << "dphi: " << endl << dphi << endl << endl;

      Vector<double> r = sc.R();
      Vector<double> v = sc.V();

      //        |   v    |
      //        | dv_dr0 |
      //        | dv_dv0 |
      //        | dv_dp0 |
      // dydt = |   a    |
      //        | da_dr0 |
      //        | da_dv0 |
      //        | da_dp0 |
      Vector<double> dydt(42+6*np,0.0);

      // v
      dydt(0) = v(0); dydt(1) = v(1); dydt(2) = v(2);
      // a
      dydt(21+3*np) = a(0); dydt(22+3*np) = a(1); dydt(23+3*np) = a(2);

      // dv_dr0, dv_dv0, da_dr0, da_dv0
      for(int i=0; i<3; i++)
          for(int j=0; j<3; j++)
          {
              dydt( 3+i*3+j)    =   dphi(i,j);
              dydt(12+i*3+j)    =   dphi(i,j+3);

              dydt(21+3*np+i*3+j)   =   dphi(i+3,j);
              dydt(30+3*np+i*3+j)   =   dphi(i+3,j+3);
          }

      // dv_dp0, da_dp0
      for(int i=0; i<3; i++)
          for(int j=0; j<np; j++)
          {
              dydt(21+i*np+j)   =   dphi(i,j+6);
              dydt(42+i*np+j)   =   dphi(i+3,j+6);
          }

//      cout << "dydt: " << endl << dydt << endl << endl;

      return dydt;

   }  // End of method 'ForceModelList::getDerivatives()'


      // Show the existing force model
   void ForceModelList::printForceModel(ostream& s)
   {
      // a counter
      int i(1);

      for(list<ForceModel*>::iterator it = fmList.begin();
         it != fmList.end();
         ++it)
      {
         s << setw(3) << i << ' '
           << (*it)->forceModelIndex() << ' '
           << (*it)->forceModelName() << endl;

         i++;
      }

   }  // End of method 'ForceModelList::printForceModel()'


      // Set force model type
   void ForceModelList::setForceModelType(set<ForceModel::ForceModelType> fmt)
   {
      fmtSet.clear();
      for(set<ForceModel::ForceModelType>::iterator it = fmt.begin();
         it != fmt.end();
         ++it)
      {
         fmtSet.insert(*it);
      }

//      cout << fmtSet.size() << endl;

   }  // End of method 'ForceModelList::setForceModelType()'

      // Set force model
   void ForceModelList::setForceModel(list<ForceModel*> fm)
   {
       fmList.clear();
       for(list<ForceModel*>::iterator it = fm.begin();
           it != fm.end();
           ++it)
       {
           fmList.push_back(*it);
       }

   }  // End of method 'ForceModelList::setForceModel()'


}  // End of namespace 'gpstk'

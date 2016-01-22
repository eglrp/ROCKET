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
* @file ForceModel.hpp
* Force Model is a simple interface which allows uniformity among the various force
* models 
*/

#ifndef GPSTK_FORCE_MODEL_HPP
#define GPSTK_FORCE_MODEL_HPP

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Spacecraft.hpp"
#include "EarthBody.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * Force Model is a simple interface which allows uniformity among the various force
       * models 
       */
   class ForceModel
   {
   public:

         /// To identify every force model and make sure one type of
         /// force exist only one instance in the force model list.
         /// class indexed with FMI_BASE and FMI_LIST can't be added
         /// to the force model list
      enum ForceModelIndex
      {
         FMI_BASE       = 1000,  ///< For This class 'ForceModel'

         FMI_GEOEARTH,           ///< Geopotential of Earth
         FMI_GEOSUN,             ///< Geopotential of Sun
         FMI_GEOMOON,            ///< Geopotential of Moon
         FMI_DRAG,               ///< Atmospheric Drag
         FMI_SRP,                ///< Solar Radiation Pressure
         FMI_RELATIVE,           ///< Relativistic Effect
         FMT_EMPIRICAL,          ///< Empirical Force
         
         //... add more here

         FMI_LIST   = 2000      ///< For class 'ForceModelList'
      };

         /// The force model parameters to be estimated
      enum ForceModelType
      {
         Cd,            // Coefficient of Drag
         Cr,            // Coefficient of Reflectivity

         // added by kfkuang, 2015/09/10
         Gx,            // Scale factor of ROCK-T SRP model, x axis
         Gy,            // Scale factor of ROCK-T SRP model, y axis
         Gz,            // Sacle factor of ROCK-T SRP model, z axis

         // added by kfkuang, 2015/10/10
         D0,            // Scale factor of CODE SRP model, D direction
         Y0,            // Scale factor of CODE SRP model, Y direction
         B0,            // Scale factor of CODE SRP model, B direction
         Dc,            // Scale factor of CODE SRP model, D direction, 1/revolution
         Ds,            // Scale factor of CODE SRP model, D direction, 1/revolution
         Yc,            // Scale factor of CODE SRP model, Y direciton, 1/revolution
         Ys,            // Scale factor of CODE SRP model, Y direciton, 1/revolution
         Bc,            // Scale factor of CODE SRP model, B direction, 1/revolution
         Bs,            // Scale factor of CODE SRP model, B direction, 1/revolution

         // added by kfkuang, 2015/09/10
         ar,            // Empirical acceleration, radial
         at,            // Empirical acceleration, along-track (transverse)
         an             // Empirical acceleration, cross-track (normal)
      };

         /// Default constructor
      ForceModel()
      {
         a.resize(3,0.0);
         da_dr.resize(3,3,0.0);
         da_dv.resize(3,3,0.0);
         da_dp.resize(3,0,0.0);      // default np = 0;

         // atmospheric drag
         da_dCd.resize(3,0.0);

         // simple srp model
         da_dCr.resize(3,0.0);

         // rock srp model
         da_dGx.resize(3,0.0);
         da_dGy.resize(3,0.0);
         da_dGz.resize(3,0.0);

         // code srp model
         da_dD0.resize(3,0.0);
         da_dY0.resize(3,0.0);
         da_dB0.resize(3,0.0);
         da_dDc.resize(3,0.0);
         da_dDs.resize(3,0.0);
         da_dYc.resize(3,0.0);
         da_dYs.resize(3,0.0);
         da_dBc.resize(3,0.0);
         da_dBs.resize(3,0.0);

         // empirical force
         da_dar.resize(3,0.0);
         da_dat.resize(3,0.0);
         da_dan.resize(3,0.0);

      }

         /// Default destructor
      virtual ~ForceModel(){}


         /// this is the real one to do computation
      virtual void doCompute(CommonTime t, EarthBody& bRef, Spacecraft& sc)
      {
         a.resize(3,0.0);
         da_dr.resize(3,3,0.0);
         da_dv.resize(3,3,0.0);
         da_dp.resize(3,0,0.0);      // default np = 0;

         // atmospheric drag
         da_dCd.resize(3,0.0);

         // simple srp model
         da_dCr.resize(3,0.0);

         // rock srp model
         da_dGx.resize(3,0.0);
         da_dGy.resize(3,0.0);
         da_dGz.resize(3,0.0);

         // code srp model
         da_dD0.resize(3,0.0);
         da_dY0.resize(3,0.0);
         da_dB0.resize(3,0.0);
         da_dDc.resize(3,0.0);
         da_dDs.resize(3,0.0);
         da_dYc.resize(3,0.0);
         da_dYs.resize(3,0.0);
         da_dBc.resize(3,0.0);
         da_dBs.resize(3,0.0);

         // empirical force
         da_dar.resize(3,0.0);
         da_dat.resize(3,0.0);
         da_dan.resize(3,0.0);

      }


         /// return the force model name
      virtual std::string modelName() const
      { return "ForceModel"; }


         /// return the force model index
      virtual int forceIndex() const
      { return FMI_BASE; }


         /**
          * Return the acceleration
          * @return  acceleration
          */
      virtual Vector<double> getAccel() const
      { return a; }

         /**
          * Return the partial derivative of acceleration wrt position
          * @return Matrix containing the partial derivative of acceleration wrt r
          */
      virtual Matrix<double> partialR() const
      { return da_dr; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt v
          */
      virtual Matrix<double> partialV() const
      { return da_dv; }

         /**
          * Return the partial derivative of acceleration wrt danymic
          * parameters
          * @return Matrix containing the partial derivative of acceleration wrt p
          */
      virtual Matrix<double> partialP() const
      { return da_dp; }

         /**
          * Return the partial derivative of acceleration wrt Cd
          * @return Vector containing the partial derivative of acceleration wrt Cd
          */
      virtual Vector<double> partialCd() const
      { return da_dCd; } 

         /**
          * Return the partial derivative of acceleration wrt Cr
          * @return Vector containing the partial derivative of acceleration wrt Cr
          */
      virtual Vector<double> partialCr() const
      { return da_dCr; }


         /**
          * Return the partial derivative of acceleration wrt Gx
          * @return Vector containing the partial derivative of acceleration
          * wrt Gx
          */
      virtual Vector<double> partialGx() const
      { return da_dGx; }


         /**
          * Return the partial derivative of acceleration wrt Gy
          * @return Vector containing the partial derivative of acceleration
          * wrt Gy
          */
      virtual Vector<double> partialGy() const
      { return da_dGy; }

         /**
          * Return the partial derivative of acceleration wrt Gz
          * @return Vector containing the partial derivative of acceleration
          * wrt Gz
          */
      virtual Vector<double> partialGz() const
      { return da_dGz; }

         /**
          * Return the partial derivative of acceleration wrt D0
          * @return Vector containing the partial derivative of acceleration
          * wrt D0
          */
      virtual Vector<double> partialD0() const
      { return da_dD0; }

         /**
          * Return the partial derivative of acceleration wrt Y0
          * @return Vector containing the partial derivative of acceleration
          * wrt Y0
          */
      virtual Vector<double> partialY0() const
      { return da_dY0; }

         /**
          * Return the partial derivative of acceleration wrt B0
          * @return Vector containing the partial derivative of acceleration
          * wrt B0
          */
      virtual Vector<double> partialB0() const
      { return da_dB0; }

         /**
          * Return the partial derivative of acceleration wrt Dc
          * @return Vector containing the partial derivative of acceleration
          * wrt Dc
          */
      virtual Vector<double> partialDc() const
      { return da_dDc; }

         /**
          * Return the partial derivative of acceleration wrt Ds
          * @return Vector containing the partial derivative of acceleration
          * wrt Ds
          */
      virtual Vector<double> partialDs() const
      { return da_dDs; }

         /**
          * Return the partial derivative of acceleration wrt Yc
          * @return Vector containing the partial derivative of acceleration
          * wrt Yc
          */
      virtual Vector<double> partialYc() const
      { return da_dYc; }

         /**
          * Return the partial derivative of acceleration wrt Ys
          * @return Vector containing the partial derivative of acceleration
          * wrt Ys
          */
      virtual Vector<double> partialYs() const
      { return da_dYs; }

         /**
          * Return the partial derivative of acceleration wrt Bc
          * @return Vector containing the partial derivative of acceleration
          * wrt Bc
          */
      virtual Vector<double> partialBc() const
      { return da_dBc; }

         /**
          * Return the partial derivative of acceleration wrt Bs
          * @return Vector containing the partial derivative of acceleration
          * wrt Bs
          */
      virtual Vector<double> partialBs() const
      { return da_dBs; }

         /**
          * Return the partial derivative of acceleration wrt ar
          * @return Vector containing the partial derivative of acceleration
          * wrt ar
          */
      virtual Vector<double> partialAr() const
      { return da_dar; }

         /**
          * Return the partial derivative of acceleration wrt at
          * @return Vector containing the partial derivative of acceleration
          * wrt at
          */
      virtual Vector<double> partialAt() const
      { return da_dat;}

         /**
          * Return the partial derivative of acceleration wrt an
          * @return Vector containing the partial derivative of acceleration
          * wrt an
          */
      virtual Vector<double> partialAn() const
      { return da_dan; }


         /** Return the number of np
          */
      int getNumOfP() const
      { return da_dp.cols(); }

         /// get A Matrix
      Matrix<double> getAMatrix() const
      {
            /* A Matrix
            |                          |
            | 0         I        0     |
            |                          |
         A =| da_dr     da_dv    da_dp |
            |                          |
            | 0         0        0     |
            |                          |
            */

         const int np = da_dp.cols();

         Matrix<double> A(6+np,6+np,0.0);

         // I
         A(0,3) = 1.0;
         A(1,4) = 1.0;
         A(2,5) = 1.0;

         // da_dr
         A(3,0) = da_dr(0,0);
         A(3,1) = da_dr(0,1);
         A(3,2) = da_dr(0,2);
         A(4,0) = da_dr(1,0);
         A(4,1) = da_dr(1,1);
         A(4,2) = da_dr(1,2);
         A(5,0) = da_dr(2,0);
         A(5,1) = da_dr(2,1);
         A(5,2) = da_dr(2,2);

         // da_dv
         A(3,3) = da_dv(0,0);
         A(3,4) = da_dv(0,1);
         A(3,5) = da_dv(0,2);
         A(4,3) = da_dv(1,0);
         A(4,4) = da_dv(1,1);
         A(4,5) = da_dv(1,2);
         A(5,3) = da_dv(2,0);
         A(5,4) = da_dv(2,1);
         A(5,5) = da_dv(2,2);

         // da_dp
         for(int i=0;i<np;i++)
         {
            A(3,6+i) = da_dp(0,i);
            A(4,6+i) = da_dp(1,i);
            A(5,6+i) = da_dp(2,i);
         }

         return A;

      }  // End of method 'getAMatrix()'

      void test()
      {
         /*
         cout<<"test Force Model"<<endl;

         a.resize(3,2.0);
         da_dr.resize(3,3,3.0);
         da_dv.resize(3,3,4.0);
         da_dp.resize(3,2,5.0);
         writeToFile("default.fm");

         // it work well
         */
      }

   protected:

         /// Acceleration
      Vector<double> a;         // 3*1
      
         /// Partial derivative of acceleration wrt position
      Matrix<double> da_dr;     // 3*3
      
         /// Partial derivative of acceleration wrt velocity
      Matrix<double> da_dv;     // 3*3
      
         /// Partial derivative of acceleration wrt dynamic parameters
      Matrix<double> da_dp;     // 3*np
         
         /// Partial derivative of acceleration wrt Cd, atmospheric drag
      Vector<double> da_dCd;    // 3*1
         
         /// Partial derivative of acceleration wrt Cr, simple srp model
      Vector<double> da_dCr;    // 3*1

         /// Partial derivative of acceleration wrt Gx, ROCK srp model
      Vector<double> da_dGx;    // 3*1

         /// Partial derivative of acceleration wrt Gy, ROCK srp model
      Vector<double> da_dGy;    // 3*1

         /// Partial derivative of acceleration wrt Gy, ROCK srp model
      Vector<double> da_dGz;    // 3*1

         /// Partial derivative of acceleration wrt D0, CODE srp model
      Vector<double> da_dD0;    // 3*1

         /// Partial derivative of acceleration wrt Y0, CODE srp model
      Vector<double> da_dY0;    // 3*1

         /// Partial derivative of acceleration wrt B0, CODE srp model
      Vector<double> da_dB0;    // 3*1

         /// Partial derivative of acceleration wrt Dc, CODE srp model
      Vector<double> da_dDc;    // 3*1

         /// Partial derivative of acceleration wrt Ds, CODE srp model
      Vector<double> da_dDs;    // 3*1

         /// Partial derivative of acceleration wrt Yc, CODE srp model
      Vector<double> da_dYc;    // 3*1

         /// Partial derivative of acceleration wrt Ys, CODE srp model
      Vector<double> da_dYs;    // 3*1

         /// Partial derivative of acceleration wrt Bc, CODE srp model
      Vector<double> da_dBc;    // 3*1

         /// Partial derivative of acceleration wrt Bs, CODE srp model
      Vector<double> da_dBs;    // 3*1

         /// Partial derivative of acceleration wrt ar, empirical force
      Vector<double> da_dar;    // 3*1

         /// Partial derivative of acceleration wrt at, empirical force
      Vector<double> da_dat;    // 3*1

         /// Partial derivative of acceleration wrt an, empirical force
      Vector<double> da_dan;    // 3*1


   }; // End of class 'ForceModel'

      /**
       * Stream output for CommonTime objects.  Typically used for debugging.
       * @param s stream to append formatted CommonTime to.
       * @param t CommonTime to append to stream \c s.
       * @return reference to \c s.
       */
   inline std::ostream& operator<<( std::ostream& s,
                                    const ForceModel& fm )
   {
      Vector<double> a = fm.getAccel();
      Matrix<double> da_dr = fm.partialR();
      Matrix<double> da_dv = fm.partialV();
      Matrix<double> da_dp = fm.partialP();

      s<<"a ["<<a.size()<<"]\n{\n"
         <<a<<std::endl<<"}\n\n";

      s<<"da/dr ["<<da_dr.rows()<<","<<da_dr.cols()<<"]\n{\n"
         <<da_dr<<std::endl<<"}\n\n";

      s<<"da/dv ["<<da_dv.rows()<<","<<da_dv.cols()<<"]\n{\n"
         <<da_dv<<std::endl<<"}\n\n";

      s<<"da/dp ["<<da_dp.rows()<<","<<da_dp.cols()<<"]\n{\n"
         <<da_dp<<std::endl<<"}\n\n";

      Matrix<double> A = fm.getAMatrix();

      s<<"A = ["<<A.rows()<<","<<A.cols()<<"]\n{\n"
         <<A<<std::endl<<"}\n\n";

      return s;

   }

      // @}

}  // End of namespace 'gpstk'


#endif  // GPSTK_FORCE_MODEL_HPP

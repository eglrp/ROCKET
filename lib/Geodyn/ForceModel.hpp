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
* models.
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
       * models.
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
         FMI_BASE       = 1000,     ///< For This class 'ForceModel'

         FMI_EarthGravitation,      ///< Geopotential of Earth
         FMI_SunGravitation,        ///< Geopotential of Sun
         FMI_MoonGravitation,       ///< Geopotential of Moon
         FMI_AtmosphericDrag,       ///< Atmospheric Drag
         FMI_SolarRadiationPressure,///< Solar Radiation Pressure
         FMI_RelativisticEffect,    ///< Relativistic Effect
         FMI_EmpiricalForce,        ///< Empirical Force

         //... add more here

         FMI_LIST       = 2000      ///< For class 'ForceModelList'
      };

         /// The force model parameters to be estimated
      enum ForceModelType
      {
         Cnm,           // Spherical harmonic coefficients
         Snm,           // Spherical harmonic coefficients

         Cd,            // Coefficient of Atomspheric Drag
         Cr,            // Coefficient of Solar Radiation Pressure

         // added by kfkuang, 2015/09/10
         Gx,            // Scale factor of ROCK-T SRP model, x axis
         Gy,            // Scale factor of ROCK-T SRP model, y axis
         Gz,            // Sacle factor of ROCK-T SRP model, z axis

         // added by kfkuang, 2015/10/10
         D0,            // Scale factor of CODE SRP model, D direction
         Dc,            // Scale factor of CODE SRP model, D direction, 1/revolution
         Ds,            // Scale factor of CODE SRP model, D direction, 1/revolution
         Y0,            // Scale factor of CODE SRP model, Y direction
         Yc,            // Scale factor of CODE SRP model, Y direciton, 1/revolution
         Ys,            // Scale factor of CODE SRP model, Y direciton, 1/revolution
         B0,            // Scale factor of CODE SRP model, B direction
         Bc,            // Scale factor of CODE SRP model, B direction, 1/revolution
         Bs,            // Scale factor of CODE SRP model, B direction, 1/revolution

         // added by kfkuang, 2015/09/10
         ar,            // Empirical acceleration, radial
         at,            // Empirical acceleration, along-track (transverse)
         an             // Empirical acceleration, cross-track (normal)

      };

   public:

         /// Constructor
      ForceModel()
      {
         a.resize(3, 0.0);
         da_dr.resize(3,3, 0.0);
         da_dv.resize(3,3, 0.0);
         da_dp.resize(3,0, 0.0);
         da_dEGM.resize(3,0, 0.0);
         da_dSRP.resize(3,0, 0.0);
      }

         /// Default destructor
      virtual ~ForceModel() {}


         /** Compute acceleration (and related partial derivatives).
          * @param utc     time in UTC
          * @param rb      earth body
          * @param sc      spacecraft
          */
      virtual void doCompute(CommonTime utc, EarthBody& rb, Spacecraft& sc) {}


         /// Return the force model name
      inline virtual std::string forceModelName() const
      { return "ForceModel"; }


         /// Return the force model index
      inline virtual int forceModelIndex() const
      { return FMI_BASE; }


         /// Get acceleration, a
      inline virtual Vector<double> getAcceleration() const
      {
         return a;
      }

         /// Get partials of acceleration to position, da_dr
      inline virtual Matrix<double> dA_dR() const
      {
         return da_dr;
      }

         /// Get partials of acceleration to velocity, da_dv
      inline virtual Matrix<double> dA_dV() const
      {
         return da_dv;
      }


         /// Get partials of acceleration to force model parameters, da_dp
      inline virtual Matrix<double> dA_dP() const
      {
         return da_dp;
      }


         /// Get partials of acceleration to EGM coefficients, da_dEGM
      inline virtual Matrix<double> dA_dEGM() const
      {
         return da_dEGM;
      }


         /// Get partials of acceleration to SRP coefficients, da_dSRP
      inline virtual Matrix<double> dA_dSRP() const
      {
         return da_dSRP;
      }



         /// Get coefficient matrix of equation of variation
      Matrix<double> getCoeffMatOfEOV() const
      {

         //////////////////////////////////////////////////
         //             |                     |          //
         //             | 0      I      0     |          //
         //             |                     |          //
         //    cMat  =  | da/dr  da/dv  da/dp |          //
         //             |                     |          //
         //             | 0      0      0     |          //
         //             |                     |          //
         //////////////////////////////////////////////////

         int numOfParam = da_dp.cols();
         Matrix<double> cMat(6+numOfParam,6+numOfParam, 0.0);

         // identify part
         cMat(0,3) = 1.0; cMat(1,4) = 1.0; cMat(2,5) = 1.0;

         // da_dr
         for(int i=0; i<3; ++i)
         {
             for(int j=0; j<3; ++j)
             {
                 cMat(i+3,j+0) = da_dr(i,j);
             }
         }

         // da_dv
         for(int i=0; i<3; ++i)
         {
             for(int j=0; j<3; ++j)
             {
                 cMat(i+3,j+3) = da_dv(i,j);
             }
         }

         // da_dp
         for(int i=0; i<3; ++i)
         {
             for(int j=0; j<numOfParam; ++j)
             {
                 cMat(i+3,j+6) = da_dp(i,j);
             }
         }

         return cMat;

      }  // End of method 'ForceModel::getCoeffMatOfEOV()'


   protected:

         /// Acceleration
      Vector<double> a;          // 3*1

         /// Partial derivative of acceleration wrt position
      Matrix<double> da_dr;      // 3*3

         /// Partial derivative of acceleration wrt velocity
      Matrix<double> da_dv;      // 3*3

         /// Partial derivative of acceleration wrt dynamic parameters
      Matrix<double> da_dp;      // 3*np

         /// Partial derivatives of acceleration wrt EGM coefficients
      Matrix<double> da_dEGM;    // 3*n_egm

         /// Partial derivatives of acceleration wrt SRP coefficients
      Matrix<double> da_dSRP;    // 3*n_srp



   }; // End of class 'ForceModel'

      /**
       * Stream output for CommonTime objects. Typically used for debugging.
       * @param s stream to append formatted CommonTime to.
       * @param t CommonTime to append to stream \c s.
       * @return reference to \c s.
       */
   inline std::ostream& operator<<( std::ostream& s,
                                    const ForceModel& fm )
   {
      Vector<double> a = fm.getAcceleration();
      Matrix<double> da_dr = fm.dA_dR();
      Matrix<double> da_dv = fm.dA_dV();
      Matrix<double> da_dp = fm.dA_dP();

      s<<"a ["<<a.size()<<"]\n{\n"
         <<a<<std::endl<<"}\n\n";

      s<<"da/dr ["<<da_dr.rows()<<","<<da_dr.cols()<<"]\n{\n"
         <<da_dr<<std::endl<<"}\n\n";

      s<<"da/dv ["<<da_dv.rows()<<","<<da_dv.cols()<<"]\n{\n"
         <<da_dv<<std::endl<<"}\n\n";

      s<<"da/dp ["<<da_dp.rows()<<","<<da_dp.cols()<<"]\n{\n"
         <<da_dp<<std::endl<<"}\n\n";

      Matrix<double> A = fm.getCoeffMatOfEOV();

      s<<"A = ["<<A.rows()<<","<<A.cols()<<"]\n{\n"
         <<A<<std::endl<<"}\n\n";

      return s;

   }

      // @}

}  // End of namespace 'gpstk'


#endif  // GPSTK_FORCE_MODEL_HPP

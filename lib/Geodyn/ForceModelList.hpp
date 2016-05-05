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
* @file ForceModelList.hpp
* ForceModelList is a countainer for force models.
*/

#ifndef GPSTK_FORCE_MODEL_LIST_HPP
#define GPSTK_FORCE_MODEL_LIST_HPP

#include "ForceModel.hpp"

#include <list>
#include <set>

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{
      
      /**
       * ForceModelList is a countainer for force models. It allows the simulation to encapsulate
       * a number of different force models and obtain their combined effect on a spacecraft. 
       */
   class ForceModelList : public ForceModel
   {
   public:

         /// Default constructor
      ForceModelList()
      {
          clear();
      }

         /// Default destructor
      virtual ~ForceModelList() {}

         /// Clear the ForceModelList
      void clear()
      {
          fmList.clear();
//          fmtSet.clear();
      }
      

         /**
         * Add a generic force to the list
         * @param fm    Object which implements the ForceModel interface
         */
      void addForce(ForceModel* fm)
      { fmList.push_back(fm); }

         /**
         * Remove a generic force to the list
         * @param fm    Object which implements the ForceModel interface
         */
      void removeForce(ForceModel* fm)
      { fmList.remove(fm); }


         /// Interface implementation for the 'ForceModel'
      virtual Vector<double> getDerivatives(CommonTime utc, EarthBody& rb, Spacecraft& sc);


         /// Get force model type
      std::set<ForceModel::ForceModelType> getForceModelType()
      { return fmtSet; }

         /// Set force model type
      void setForceModelType(std::set<ForceModel::ForceModelType> fmt);


         /// Get force model
      std::list<ForceModel*> getForceModel()
      { return fmList; }

         /// Set force model
      void setForceModel(std::list<ForceModel*> fm);

         /// Return the force model name
      virtual std::string forceModelName() const
      { return "ForceModelList"; };

         /// Return the force model index
      virtual int forceModelIndex() const
      { return FMI_LIST; }

         /// Show the existing force model
      void printForceModel(std::ostream& s);

   protected:

         /// List of force model
      std::list<ForceModel*> fmList;

         /// Set of force model type
      std::set<ForceModel::ForceModelType> fmtSet;

   }; // End of class 'ForceModelList'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_FORCE_MODEL_LIST_HPP

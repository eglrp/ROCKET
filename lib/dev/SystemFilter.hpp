#pragma ident "$Id: SystemFilter.hpp$"

/**
 * @file SystemFilter.hpp 
 * filter observation data 
 */

#ifndef GPSTK_SYSTEM_FILTER_HPP
#define GPSTK_SYSTEM_FILTER_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Modifications
//  -------------
//
//  - Create this subroutine, 2017/02/22.
//
//  Copyright
//  ---------
//  
//  Gao Kang, Wuhan University, 2016
//
//============================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include "ProcessingClass.hpp"
#include "DataStructures.hpp"

namespace gpstk
{

      /** @addtogroup SystemFilter */

   class SystemFilter : public ProcessingClass
   {

      public:

         // default constructor
         SystemFilter()
         { }
         
         // one parameter constructor
         // @param satSys satellite system
         SystemFilter( SatID::SatelliteSystem satSys )
         { satSysSet.insert(satSys); }
         
         /// add SatelliteSystem to vector
         SystemFilter& addFilterSatSys(SatID::SatelliteSystem satSys)
         { satSysSet.insert(satSys); return (*this); }
         
         ///  TODO: filter satellite system excluded in 'satSysVec'
         ///  @param: gData   satTypeValueMap
         ///  @return satTypeValueMap
         virtual satTypeValueMap& Process(satTypeValueMap& gData);
         
         ///  TODO: filter satellite system excluded in 'satSysVec'
         //   @param: gData   gnssSatTypeValueMap
         //   @param: gData   gnssSatTypeValueMap
         virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         {
            Process( gData.body );
            return gData;
         }

         ///  TODO: filter satellite system exclued in 'satSysVec'
         //   @param: gData   gnssRinex
         //   @param: gData   gnssRinex
         virtual gnssRinex& Process(gnssRinex& gData)
         {
            Process( gData.body );
            return gData;
         }
         

         ///  TODO: filter satellite system exclued in 'satSysVec'
         //   @param: gData   gnssDataMap
         //   @param: gData   gnssDataMap
         virtual gnssDataMap& Process(gnssDataMap& gData);

         /// return the name of ProcessingClass Type
         virtual std::string getClassName(void) const
         { return "SystemFilter"; }
         
         // destructor
         ~SystemFilter(){};

      private:
         std::set<SatID::SatelliteSystem> satSysSet;
   };

}  // End of namespace gpstk
#endif   // GPSTK_SYSTEM_FILTER_HPP

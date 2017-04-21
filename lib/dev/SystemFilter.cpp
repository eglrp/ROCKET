#pragma ident "$Id: SystemFilter.cpp $"

/**
 * @file SystemFilter.cpp
 * filter satellite system type
 */

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
//  - Create this subroutine, 2012/06/15.
//
//  Copyright
//  ---------
//  
//  Gao Kang, Wuhan University, 2016
//
//============================================================================


#include "SystemFilter.hpp"

namespace gpstk
{

   /// TODO: filter satellite system excluded in 'satSysVec'
   /// paramter is 'satTypeValueMap'
   satTypeValueMap& SystemFilter::Process(satTypeValueMap& gData)
   {
      for( satTypeValueMap::iterator stvm = gData.begin();
            stvm != gData.end(); )
      {
         std::set<SatID::SatelliteSystem>::iterator iter =
            satSysSet.find( ((*stvm).first).system );

         if( satSysSet.end() == iter )
         {
            gData.erase(stvm++);
         }
         else
         {
            stvm++;
         }  
      }

      return gData;
   }

   /// TODO: filter satellite system excluded in 'satSysVec'
   /// parameter is 'gnssDataMap'
   gnssDataMap& SystemFilter::Process(gnssDataMap& gData)
   {
      
      for( gnssDataMap::iterator gdmIter = gData.begin();
            gdmIter != gData.end(); gdmIter++ )
      {
         for( sourceDataMap::iterator sdmIter = gdmIter->second.begin();
               sdmIter != gdmIter->second.end(); sdmIter++ )
         {
            Process( sdmIter->second );
         }
      }

      return gData;
   }

}  // End of namespace gpstk

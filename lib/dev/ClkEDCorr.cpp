#pragma ident "$Id$"

/**
 * @file ClkEDCorr.cpp
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "ClkEDCorr.hpp"


namespace gpstk
{


   std::string ClkEDCorr::getClassName(void) const
   { return "ClkEDCorr"; }

	
   satTypeValueMap& ClkEDCorr::Process( const CommonTime& time, satTypeValueMap& gData )
     throw(ProcessingException)
   {
      /// obtain satTypeValueMap for last epoch data
      sourceDataMap::iterator sdmIter = mLastEpochData.find( m_targetSource );
      
      if( mLastEpochData.end() == sdmIter ) { return gData; }
      satTypeValueMap lastSatValueMap = sdmIter->second;

      satValueMap svMap;
      while( gData.size() >= 4)
      {
	      svMap.clear();

      	double cdt=0;
	      double sigma=0.0;
      	int  num = 0;
      	for( satTypeValueMap::iterator stvm = gData.begin();
	    	      stvm != gData.end(); stvm++ )
       	{
	         try
	         {
	            satTypeValueMap::iterator lastSTVM = lastSatValueMap.find(stvm->first);	
	            if( lastSatValueMap.end() == lastSTVM ) continue;
	            if( stvm->second(TypeID::satArc) != lastSTVM->second(TypeID::satArc) ) continue;
	     
	            double lc = stvm->second(TypeID::prefitL) - lastSTVM->second(TypeID::prefitL);
	            double clkBias = ( m_pNavEphStore->getXvt( (*stvm).first, time ) ).getClockBias() ;
	            double  cdtn = lc + clkBias * C_MPS + m_targetSource.recClkBias;
	            svMap[(*stvm).first] = cdtn ;
	            num++;
	            sigma = ( (num-1) * sigma ) / num + std::pow(cdtn-cdt,2)*(num-1)/(num*num); 
	            cdt = cdtn/num + ( (num-1)*cdt )/num;
	          } 
	          catch(InvalidRequest& e)
	          {
	            continue;
	          }
         }
	
	      if( sigma >= 0.1 * 0.1 )
	      {
	         SatID maxRMSSat;
	         double maxRMS = 0.0;
	         for( satValueMap::iterator svmIter = svMap.begin();
		         svmIter != svMap.end(); svmIter++ )
	         {
		         double rms = std::fabs(svmIter->second - cdt );
		         if( svmIter == svMap.begin() )
		         {
		            maxRMS = rms;
		            maxRMSSat = svmIter->first;
		            continue;
		         } else {
		            if( maxRMS < rms )
		            {
			            maxRMS = rms;
			            maxRMSSat = svmIter->first;
		            }
		         }
	         }   
	    
	         gData.removeSatID(maxRMSSat);
	      } else {
	         break;
	      }
       }
      return gData;
   }

   gnssDataMap& ClkEDCorr::Process( gnssDataMap& gData )
      throw(ProcessingException)
   {
     try 
     {
	      sourceDataMap lastEpochData;
	
	      for( gnssDataMap::iterator gdsIter = gData.begin();
              gdsIter != gData.end(); gdsIter++ )
      	{
            for( sourceDataMap::iterator sdmIter = gdsIter->second.begin();
                 sdmIter != gdsIter->second.end();)
             {
		          m_targetSource = sdmIter->first;
		          Process( gdsIter->first, sdmIter->second );
		          if( sdmIter->second.numSats() <= 0 )
		          {
		               gdsIter->second.erase(sdmIter++);
		          } else {

		               /// copy satTypeValueMap
		               satTypeValueMap lastSatValueMap;
	                  for( satTypeValueMap::iterator stvm = sdmIter->second.begin();
		                     stvm != sdmIter->second.end(); stvm++ )
	                  {
			              try {
	                	      typeValueMap tvm;
	    			            double clkBias = m_pNavEphStore->getXvt( (*stvm).first, (*gdsIter).first ).getClockBias();
				               tvm[TypeID::satArc] = stvm->second(TypeID::satArc);
	    			            tvm[TypeID::prefitC] = stvm->second(TypeID::prefitC) + clkBias * C_MPS + m_targetSource.recClkBias ;
	    			            tvm[TypeID::prefitL] = stvm->second(TypeID::prefitL) + clkBias * C_MPS + m_targetSource.recClkBias ;
            			      lastSatValueMap[stvm->first] = tvm;
	 	     	            }catch(Exception& u ){  }
	                  }
		     
		               lastEpochData[sdmIter->first] = lastSatValueMap;
  		     
		               sdmIter++;
		            }
	            }
            }

	         mLastEpochData = lastEpochData;
         } 
         catch(Exception& u ) 
         {
	         ProcessingException e( getClassName()+u.what() );
	         GPSTK_THROW(e);
         }
     
      return gData;
   }

}  // End of namespace gpstk

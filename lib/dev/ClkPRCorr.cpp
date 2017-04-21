#pragma ident "$Id$"

/**
 * @file ClkPRCorr.cpp
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


#include "ClkPRCorr.hpp"


namespace gpstk
{


   std::string ClkPRCorr::getClassName(void) const
   { return "ClkPRCorr"; }

	
   satTypeValueMap& ClkPRCorr::Process( const CommonTime& time, satTypeValueMap& gData )
     throw(ProcessingException)
   {
      double interval = m_pStateStore->getDecimateInterval();
      
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
	         double pc = stvm->second(TypeID::prefitC);
	         double clkBias = ( m_pNavEphStore->getXvt( (*stvm).first, time ) ).getClockBias()  ;
	         double trop = 0.0;
	         try{
	    	      trop =  m_pStateStore->getSolution(TypeID::wetMap,m_targetSource);
		         SourceTropData data;
		         data.wetValue = trop;
		         data.validTime = CommonTime( m_pStateStore->getStateEpoch() );
		         mWetTrop[m_targetSource] = data;
	         } catch(InvalidRequest& e) 
	         {
		         std::map<SourceID, SourceTropData>::iterator iter = mWetTrop.find(m_targetSource);
		         if( iter != mWetTrop.end() ) {
		            CommonTime epoch = m_pStateStore->getStateEpoch();
		            if( fabs( epoch - (*iter).second.validTime ) <= 7200 ) {
		               trop = (*iter).second.wetValue;
		            } else {
		               continue;
		            }
		         } else {
		            continue;
		         }
	         }
	         double  cdtn = pc + clkBias * C_MPS - trop;
	         svMap[(*stvm).first] = cdtn ;
	         num++;
	         sigma = ( (num-1) * sigma ) / num + std::pow(cdtn-cdt,2)*(num-1)/(num*num); 
	         cdt = cdtn/num + ( (num-1)*cdt )/num;
             //std::cout<<"count:"<<num<<",cdt:"<<cdt<<",sigma:"<<sigma<<std::endl;
         }
	
	      if( sigma >= 4 )
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
		         } 
		          else 
		         {
		            if( maxRMS < rms )
		            {
			            maxRMS = rms;
			            maxRMSSat = svmIter->first;
		            }
		          }
	           }
	    
	         std::cout<<"prefitCode source:"<<m_targetSource<<",remove sat:"<<maxRMSSat<<","<<
	         "MaxRMS:"<<maxRMS<<",sigma:"<<sigma<<std::endl;
	         gData.removeSatID(maxRMSSat);
	      }
	   else
	   {
	      for( satTypeValueMap::iterator stvm = gData.begin();
		      stvm != gData.end(); stvm++ )
	       {
	         try
	         {
	     	      double recClkBias = m_pStateStore->getSolution(TypeID::cdt,m_targetSource);
	            m_pStateStore->setSolution(TypeID::cdt,m_targetSource,recClkBias-cdt);
		         stvm->second(TypeID::prefitC) -= cdt;
		         stvm->second(TypeID::prefitL) -= cdt;
	         } catch (InvalidRequest& e){ }
	       }
	       m_targetSource.recClkBias = cdt;
	       break;
	     }
 
       }

      return gData;
   }

   gnssDataMap& ClkPRCorr::Process( gnssDataMap& gData )
      throw(ProcessingException)
   {
     try 
     {
	
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
		     //std::cout<<"remove source:"<<m_targetSource<<std::endl;
		     gdsIter->second.erase(sdmIter++);
		  }
		  else
		  {
		    SourceID& ref = (SourceID&)((*sdmIter).first);
		    ref.recClkBias =  m_targetSource.recClkBias;
		    sdmIter++;
		  }
	     }
         }
      } 
      catch(Exception& u ) 
      {
	ProcessingException e( getClassName()+u.what() );
	GPSTK_THROW(e);
      }
     
      return gData;
   }

}  // End of namespace gpstk

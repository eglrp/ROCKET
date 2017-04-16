#pragma ident "$Id$"

/**
 * @file ValidOp.cpp
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


#include "TimeUpdateCorr.hpp"


namespace gpstk
{


   std::string TimeUpdateCorr::getClassName(void) const
   { return "TimeUpdateCorr"; }

	
   satTypeValueMap& TimeUpdateCorr::Process( satTypeValueMap& gData )
     throw(ProcessingException)
   {
	
      for( satTypeValueMap::iterator stvm = gData.begin();
	    stvm != gData.end(); stvm++ )
       {
	  
	  for( std::list<Equation>::iterator iter = m_Equations.begin();
		iter != m_Equations.end(); iter++ )
	   {
	      TypeID indType = (*iter).header.indTerm.getType();
	      double indValue = stvm->second(indType);
	      double corr = 0;
	      for( VarCoeffMap::const_iterator vcmIter = (*iter).body.begin();
		    vcmIter != (*iter).body.end(); vcmIter++ )
	       {
		  Variable var( (*vcmIter).first );
		  Coefficient coef( (*vcmIter).second );
		  
		  double coefValue = coef.defaultCoefficient;
		  double val = 0.0;
		  try
		  {
		     if( var.getSatIndexed() && 
		         var.getSourceIndexed() )
		      {
		      	  val = m_pStateStore->getSolution( 
						var.getType(), 
						m_targetSource,
						(*stvm).first );
			  
		      } else if ( var.getSatIndexed() )
		      {
			  val = m_pStateStore->getSolution(
						var.getType(),
					        (*stvm).first );
		      } else if( var.getSourceIndexed() )
		      {
			  val = m_pStateStore->getSolution(
						var.getType(),
					 	m_targetSource );
		      }
		  } 
		  catch (InvalidRequest& e )
	          { }
		 corr = corr + coefValue * val; 
	       }
	     std::cout<<"source:"<<m_targetSource<<",sat:"<<(*stvm).first
		<<" "<<indValue<<" "<<corr<<" "<<indValue-corr<<std::endl;
	   }
       }
	
      return gData;
   }

   gnssDataMap& TimeUpdateCorr::Process( gnssDataMap& gData )
      throw(ProcessingException)
   {
     try 
     {

	for( gnssDataMap::iterator gdsIter = gData.begin();
              gdsIter != gData.end(); gdsIter++ )
      	{
            for( sourceDataMap::iterator sdmIter = gdsIter->second.begin();
                 sdmIter != gdsIter->second.end(); sdmIter++)
             {
		  m_targetSource = sdmIter->first;
		  Process( sdmIter->second );
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

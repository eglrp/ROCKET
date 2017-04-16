#pragma ident "$Id: StateStore.cpp $"

/**
 * @file StateStore.cpp
 * Store State for Kalman filter
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


#include "StateStore.hpp"

namespace gpstk
{

      /** update receiver position
       *
       * @param gData   the data to process
       *
       * @return this object.
       */
   StateStore& StateStore::updateNominalPos( gnssDataMap& gData )
   {
      
      for( gnssDataMap::iterator gdsIter = gData.begin();
           gdsIter != gData.end(); gdsIter++ )
      {
         CommonTime workEpoch(gdsIter->first);
         workEpoch.setTimeSystem(TimeSystem::Unknown);

         for( sourceDataMap::iterator sdmIter = gdsIter->second.begin();
              sdmIter != gdsIter->second.end();)
         {

            try
            {
               MSCData data = m_pMSCStore->findMSC( sdmIter->first.sourceName, workEpoch );
               Position nominalPos = data.coordinates;
               SourceID& source = (SourceID&)(sdmIter->first);
               source.nominalPos = nominalPos;
               
               if( sdmIter->first.sourceName == m_MasterName )
               {
                  m_MasterSource = SourceID( sdmIter->first );
               }
            
               sdmIter++;
            }
            catch( InvalidRequest& exec )
            {
               std::cerr << "The station "<< sdmIter->first.sourceName 
                  << " isn't included in MSC file." << std::endl;
               
               // remove the sourceData
               gdsIter->second.erase( sdmIter++ ); 
            }
            
         }
      }

      return (*this);
   }

      /** get State Vector from sub VariableSet
       *
       * @param subVariableSet the sub variable set
       *
       * @return sub state vector.
       *
       * \warning If the variable in subset is not included in current
       *  variable set, the value will be set zero.
       */
   Vector<double> StateStore::getStateVector( const VariableSet& subVariableSet )
   {
      Vector<double> subStateVec( subVariableSet.size() );

      for( VariableSet::const_iterator varIter = subVariableSet.begin();
           varIter != subVariableSet.end(); varIter++ )
      {
         int nowIndex = varIter->getNowIndex();
         int preIndex = varIter->getPreIndex();
         if( -1 == preIndex ) { subStateVec[nowIndex] = 0.0; continue;}
         subStateVec[nowIndex] = m_StateVec[preIndex];
      }

      return subStateVec;
   }
  
   
      /** get State Vector from sub VariableSet
       *
       * @param subVariableSet the sub variable set
       *
       * @return sub state vector.
       *
       * \warning If the variable in subset is not included in current
       *  variable set, the value will be set zero.
       */
   Vector<double> StateStore::getStateVector( const VariableSet& subVariableSet,
					      gnssDataMap& gData,
					      const TypeID& type )
   {
      Vector<double> subStateVec( subVariableSet.size() );

      for( VariableSet::const_iterator varIter = subVariableSet.begin();
           varIter != subVariableSet.end(); varIter++ )
      {
         int nowIndex = varIter->getNowIndex();
         int preIndex = varIter->getPreIndex();
         double csflags = 0.0;
	 try
	 {
	    csflags = gData.getValue( (*varIter).getSource(),
			              (*varIter).getSatellite(),
			    	      type );
	 } 
	 catch(ValueNotFound& exc) { csflags = -1.0; }
	 
	 if( -1 == preIndex || csflags >= 1.0 )
	 {
	    gData.insertValue( (*varIter).getSource(),
			       (*varIter).getSatellite(),
			       type, 0.0 );
	    subStateVec[nowIndex] = 0.0; continue;
	 }
         subStateVec[nowIndex] = m_StateVec[preIndex];
      }

      return subStateVec;
   }


 
      /** get Covariance Matrix from subVariableSet
       *
       * @param subVariableSet the sub variable set
       *
       * @return sub covariance matrix.
       *
       */
   Matrix<double> StateStore::getCovarMatrix( const VariableSet& subVariableSet )
   {
      int size = subVariableSet.size();
      Matrix<double> subCovarMatrix(size,size);
      VariableSet tempSet(subVariableSet);
      for( VariableSet::const_iterator varIter1 = subVariableSet.begin();
           varIter1 != subVariableSet.end(); varIter1++ )
      {
         
         int preIndex1 = varIter1->getPreIndex();
         int nowIndex1 = varIter1->getNowIndex();
         
         if( -1 == preIndex1 )
         {
            subCovarMatrix( nowIndex1, nowIndex1 ) = varIter1->getInitialVariance();
         }
         else
         {
            subCovarMatrix( nowIndex1, nowIndex1 ) = m_CovarMatrix( preIndex1, preIndex1 );
         }
         
            // remove from tempSet
         tempSet.erase( (*varIter1) );

         for( VariableSet::const_iterator varIter2 = tempSet.begin();
              varIter2 != tempSet.end(); varIter2++ )
         {
            int preIndex2 = varIter2->getPreIndex();
            int nowIndex2 = varIter2->getNowIndex();

            if( -1 == preIndex1 ||
                -1 == preIndex2 )
            {
               subCovarMatrix( nowIndex1, nowIndex2 ) = 
                  subCovarMatrix( nowIndex2, nowIndex1 ) = 0.0;
               continue;
            }
            
            subCovarMatrix( nowIndex1, nowIndex2 ) =
               subCovarMatrix( nowIndex2, nowIndex1 ) = m_CovarMatrix( preIndex1, preIndex2 ); 
         }

      }

      return subCovarMatrix;
   }


      /** get Covariance Matrix from subVariableSet
       *
       * @param subVariableSet the sub variable set
       *
       * @return sub covariance matrix.
       *
       */
   Matrix<double> StateStore::getCovarMatrix( const VariableSet& subVariableSet,
					      gnssDataMap& gData,
					      const TypeID& type )
   {
      int size = subVariableSet.size();
      Matrix<double> subCovarMatrix(size,size);
      VariableSet tempSet(subVariableSet);
      for( VariableSet::const_iterator varIter1 = subVariableSet.begin();
           varIter1 != subVariableSet.end(); varIter1++ )
      {
         
         int preIndex1 = varIter1->getPreIndex();
         int nowIndex1 = varIter1->getNowIndex();
	 double csflags = 0.0;

	try
	{
	  csflags = gData.getValue( (*varIter1).getSource(),
				    (*varIter1).getSatellite(),
				    type );
	} 
	catch( ValueNotFound& exc ) { csflags=-1.0; }         



         if( -1 == preIndex1 || csflags >= 1.0 )
         {
            subCovarMatrix( nowIndex1, nowIndex1 ) = varIter1->getInitialVariance();
         }
         else
         {
            subCovarMatrix( nowIndex1, nowIndex1 ) = m_CovarMatrix( preIndex1, preIndex1 );
         }
         
            // remove from tempSet
         tempSet.erase( (*varIter1) );

         for( VariableSet::const_iterator varIter2 = tempSet.begin();
              varIter2 != tempSet.end(); varIter2++ )
         {
            int preIndex2 = varIter2->getPreIndex();
            int nowIndex2 = varIter2->getNowIndex();

            if( -1 == preIndex1 ||
                -1 == preIndex2 || csflags >= 1.0 )
            {
               subCovarMatrix( nowIndex1, nowIndex2 ) = 
                  subCovarMatrix( nowIndex2, nowIndex1 ) = 0.0;

	       gData.insertValue( (*varIter1).getSource(),
				  (*varIter1).getSatellite(),
				  type, 0.0 );	
               continue;
            }
            
            subCovarMatrix( nowIndex1, nowIndex2 ) =
               subCovarMatrix( nowIndex2, nowIndex1 ) = m_CovarMatrix( preIndex1, preIndex2 ); 
         }

      }

      return subCovarMatrix;
   }


  StateStore& StateStore::setSolution( const Variable& variable,
				       double value )
  throw(InvalidRequest)
  {
      VariableSet::const_iterator varIter = m_VariableSet.find( variable );
       
      if( m_VariableSet.end() != varIter )
      {
	 int index = varIter->getNowIndex();
	 if( -1 != index ) m_StateVec[index] = value;
	 else {
	   InvalidRequest e("variable index is -1!");
	   GPSTK_THROW(e);
	 }
      } else {
	InvalidRequest e("variable is not found!");
        GPSTK_THROW(e);
      }
     
     return (*this);
  }
	
      /** Returns the solution associated to a given Variable.
       *
       * @param variable      Variable object solution we are looking for.
       */
   double StateStore::getSolution( const Variable& variable ) const
      throw(InvalidRequest)
   {
      VariableSet::const_iterator varIter = m_VariableSet.find( variable );

      if( m_VariableSet.end() != varIter )
      {
         int index = varIter->getNowIndex();
         if( -1 != index ) return m_StateVec[index];
         return 0.0;
      }
      else
      {
         InvalidRequest e("Variable not found in State vector.");
         GPSTK_THROW(e);
         return 0.0;
      }
   }

      /** Returns the solution associated to a give TypeID.
       *
       * @param type      TypeID of the solution we are looking for.
       *
       * \warning In the case the solution cotains more than one variable
       * of this type, only the first one will be returned.
       */
   double StateStore::getSolution( const TypeID& type ) const
      throw(InvalidRequest)
   {
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         if( varIter->getType() == type )
         {
            int index = varIter->getNowIndex();
            if( -1 != index ) return m_StateVec[index];
            return 0.0;
         }
      }

      InvalidRequest e("TypeID not found in State vector.");
      GPSTK_THROW(e);
      return 0.0;
   }


      /** set the solution value associated to a given TypeID.
       *
       *  @param type   TypeID of the solution we are looking for.
       *  @param value  TypeID value.
       *
       * \warning In the case the solutin contains more than one variable
       * of this type, we will set the same value.
       */ 	
   StateStore& StateStore::setSolution( const TypeID& type,
			 		double value )
	throw(InvalidRequest)
   {
	for( VariableSet::const_iterator varIter = m_VariableSet.begin();
	     varIter != m_VariableSet.end(); varIter++ )
        {
            if( varIter->getType() == type )
	    {
		int index = varIter->getNowIndex();
		if( -1 != index )
		{
		  m_StateVec[index] = value;
		}
		else
		{
		  InvalidRequest e("index cannot be -1 !");
		  GPSTK_THROW(e);
		}
	    }
	}
	
	return *this;
   }



      /** Returns the solution associated to a give TypeID and SourceID.
       *
       * @param type      TypeID of the solution we are looking for.
       * @param source    SourceID of the solution we are looking for.
       *
       * \warning In the case the solution cotains more than one variable
       * of this type, only the first one will be returned.
       */
   double StateStore::getSolution( const TypeID& type,
                                   const SourceID& source ) const
      throw(InvalidRequest)
   {
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         if( varIter->getType() == type &&
             varIter->getSource() == source )
         {
            int index = varIter->getNowIndex();
            if( -1 != 0 ) return m_StateVec[index];
            return 0.0;
         }
      }   

      InvalidRequest e("TypeID or SourceID not found in State vector.");
      GPSTK_THROW(e);
      return 0.0;
   }


      /** set the solution value associated to a given TypeID.
       *
       *  @param type        TypeID of the solution we are looking for.
       *  @param SourceID    SourceID 
       *  @param value       TypeID value.
       *
       * \warning In the case the solutin contains more than one variable
       * of this type, we will set the same value.
       */ 	
   StateStore& StateStore::setSolution( const TypeID& type,
					const SourceID& source,
					double value )
	throw(InvalidRequest)

   {
       for( VariableSet::const_iterator varIter = m_VariableSet.begin();
	      varIter != m_VariableSet.end(); varIter++ )

	{
	   if( varIter->getSource() == source &&
	       varIter->getType() == type )
	    {
		int index = varIter->getNowIndex();
		if( -1 != index ) 
		{
		  m_StateVec[index]=value;
		}
		else
		{
		  InvalidRequest e("index cannot be -1");
		  GPSTK_THROW(e);	
		}
	     break;
	    }	
	}	
	
	return (*this);
   }

      /** Returns the solution associated to a give TypeID and SatID.
       *
       * @param type      TypeID of the solution we are looking for.
       * @param sat       SatID of the solution we are looking for.
       *
       * \warning In the case the solution cotains more than one variable
       * of this type, only the first one will be returned.
       */
   double StateStore::getSolution( const TypeID& type,
                                   const SatID& sat ) const
      throw(InvalidRequest)
   {
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         if( varIter->getType() == type &&
             varIter->getSatellite() == sat )
         {
            int index = varIter->getNowIndex();
            if( -1 != index ) return m_StateVec[index];
            return 0.0;
         }
      }

      InvalidRequest e("TypeID or SatID not found in State vector.");
      GPSTK_THROW(e);
      return 0.0;
   }

	

      /** set the solution value associated to a give TypeID and SatID.
       *
       * @param type      TypeID of the solution we are looking for.
       * @param sat       SatID of the solution we are looking for.
       *
       * \warning In the case the solution cotains more than one variable
       * of this type, only the first one will be returned.
       */
   StateStore& StateStore::setSolution( const TypeID& type,
					const SatID& sat,
					double value )
      throw(InvalidRequest)
   {
       for( VariableSet::const_iterator varIter = m_VariableSet.begin();
	     varIter != m_VariableSet.end(); varIter++ )
	{
	    if( varIter->getType() == type &&
		varIter->getSatellite() == sat )
	      {
		 int index = varIter->getNowIndex();
		 if( -1 != index )
		  {
		    m_StateVec[index] = value;
                  }
		  else
		  {
		    InvalidRequest e("index cannot be -1!");
		    GPSTK_THROW(e);
		  }
		break;
	      }
	}
	
      return (*this);
   }

      /** Returns the solution associated to a give TypeID SourceID and SatID.
       *
       * @param type      TypeID of the solution we are looking for.
       * @param source    SourceID of the solution we are looking for.
       * @param sat       SatID of the solution we are looking for.
       *
       * \warning In the case the solution cotains more than one variable
       * of this type, only the first one will be returned.
       */
   double StateStore::getSolution( const TypeID& type,
                                   const SourceID& source,
                                   const SatID& sat ) const
      throw(InvalidRequest)
   {
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         if( varIter->getType() == type &&
             varIter->getSource() == source &&
             varIter->getSatellite() == sat )
         {
            int index = varIter->getNowIndex();
            if( -1 != index ) return m_StateVec[index];
            return 0.0;
         }
      }

      InvalidRequest e("TypeID SourceID or SatID not found in State vector.");
      GPSTK_THROW(e);
      return 0.0;
   }
    
   StateStore& StateStore::setSolution( const TypeID& type,
			     		const SourceID& source,
			    		const SatID& sat,
			    		double value )
	throw(InvalidRequest)
   	{
           for( VariableSet::const_iterator varIter = m_VariableSet.begin();
		varIter != m_VariableSet.end(); varIter++ )
	    {
		if( varIter->getSource() == source &&
		    varIter->getSatellite() == sat &&
		    varIter->getType() == type )
		{
		   int index = varIter->getNowIndex();
		   if( -1 != index )
		    {
			m_StateVec[index] = value;
		    }
		    else 
		    {
			InvalidRequest e("index cannoy be -1!");
			GPSTK_THROW(e);
		    }
		   break;
		}
	    }
	   return (*this);
   	}


      /** get all SourceID contained in variable set
       *
       * @param sourceSet  reference set of SourceIDSet as input and output
       */
   void StateStore::getSourceSet(SourceIDSet& sourceSet)
   {
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         if( varIter->getSourceIndexed() )
         {
            sourceSet.insert( varIter->getSource() );
         }
      }
   }

      /** get all SatID contained in variable set
       *
       * @param satSet reference set of SatIDSet as input and output.
       */
   void StateStore::getSatIDSet(SatIDSet& satSet)
   {  
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         if( varIter->getSatIndexed() )
         {
            satSet.insert(  varIter->getSatellite() );
         }
      }
   }
	
   double StateStore::getDecimateInterval()
   {	
      if( CommonTime::BEGINNING_OF_TIME == m_prevEpoch )
      {
	return m_decimateInterval;
      } else {
	return m_StateEpoch - m_prevEpoch;		
      }
   }	

      /** Simplely output the information of StateStore
       *  include the state value and variable information.
       */
   std::ostream& StateStore::dump(std::ostream& s) const
   {
      for( VariableSet::const_iterator varIter = m_VariableSet.begin();
           varIter != m_VariableSet.end(); varIter++ )
      {
         int index = varIter->getNowIndex();
         double val = 0;
         if( -1 != index ) val = m_StateVec[index];
         s << StringUtils::asString(*varIter)<<" "<<val<<std::endl;
      }

      return s;
   }

}  // End of namespace gpstk

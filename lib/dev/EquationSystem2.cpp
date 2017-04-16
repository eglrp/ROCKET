#pragma ident "$Id: EquationSystem2.cpp 2939 2012-04-06 19:55:11Z shjzhang $"

/**
 * @file EquationSystem2.cpp
 * Class to define and handle complex equation systems for solvers.
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
//  Copyright 
//  ---------
//
//  shjzhang - Wuhan University. 2015
//
//============================================================================
//
//  History 
//
//  - Modify of the Method "getPhiQ()". The gnssRinex is needed to 'Prepare' 
//    the stochastic model. When the variable is source-indexed, the gnssRinex 
//    can be given by finding the source in gdsMap, but when the variable is 
//    only satellite-indexed, the gnssRinex is the first gnssRinex which "SEE"
//    this satellite. (2012.06.19)
//
//  - Change 
//    "varUnknowns.insert( currentUnknowns.begin(), currentUnknowns.end() );"
//    to
//    "varUnknowns = currentUnknowns;"
//
//  - Version 2.0 (majar modification)
//
//    The EquationSystem2 is totoally revised to compute the related matrix/vector
//    very fast. The new version can improve the speed by a fator of about 100.
//    In the new equation version, the major revision includes:
//    (1) The typeValueMap data will be stored in each equation header, then
//        in the following Phi/Q, Prefit/Geometry/Weight computation, the data
//        will be directly feeded instead of extracting from 'gdsDataMap'.
//    (2) The 'getPrefit' and 'getGeometryWeights' are merged into 
//        'getPrefitGeometryWeights', which will compute prefit/geometry/weights
//        in one loop. This will save time in further.
//    shjzhang, 2016.6.26
//
//============================================================================


#include "SystemTime.hpp"
#include "EquationSystem2.hpp"
#include <iterator>
#include "Epoch.hpp"
#include "TimeString.hpp"

namespace gpstk
{



      // General white noise stochastic model
   WhiteNoiseModel EquationSystem2::whiteNoiseModel;



      /* Add a new equation to be managed.
       *
       * @param equation   Equation object to be added.
       *
       */
   EquationSystem2& EquationSystem2::addEquation( const Equation& equation )
   {

         // Add "equation" to "equDescripList"
      equDescripList.push_back(equation);

         // We must "Prepare()" this EquationSystem2
      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem2::addEquation()'



      /* Remove an Equation being managed. In this case the equation is
       * identified by its independent term.
       *
       * @param indterm  Variable object of the equation independent term
       *                 (measurement type).
       *
       * \warning All Equations with the same independent term will be
       *          erased.
       */
   EquationSystem2& EquationSystem2::removeEquation( const Variable& indterm )
   {

         // Create a backup list
      std::list<Equation> backupList;

         // Visit each "Equation" in "equDescripList"
      for( std::list<Equation>::const_iterator itEq =
                                                equDescripList.begin();
           itEq != equDescripList.end();
           ++itEq )
      {

            // If current equation has a different independent term, save it
         if ( (*itEq).getIndependentTerm() != indterm )
         {
            backupList.push_back(*itEq);
         }

      }

         // Clear the full contents of this object
      clearEquations();

         // Add each "Equation" in the backup equation list
      for( std::list<Equation>::const_iterator itEq = backupList.begin();
           itEq != backupList.end();
           ++itEq )
      {
         addEquation(*itEq);
      }

         // We must "Prepare()" this EquationSystem2 again
      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem2::removeEquation()'



      // Remove all Equation objects from this EquationSystem2.
   EquationSystem2& EquationSystem2::clearEquations()
   {
         // First, clear the "equDescripList"
      equDescripList.clear();

      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem2::clearEquations()'



      /* Prepare this object to carry out its work.
       *
       * @param gData   GNSS data structure (GDS).
       *
       */
   EquationSystem2& EquationSystem2::Prepare( gnssRinex& gData )
   {

         // First, create a temporary gnssDataMap
      gnssDataMap myGDSMap;

         // Get gData into myGDSMap
      myGDSMap.addGnssRinex( gData );

         // Call the map-enabled method, and return the result
      return (Prepare(myGDSMap));

   }  // End of method 'EquationSystem2::Prepare()'



      /* Prepare this object to carry out its work.
       *
       * @param gdsMap     Map of GNSS data structures (GDS), indexed
       *                   by SourceID.
       *
       */
   EquationSystem2& EquationSystem2::Prepare( gnssDataMap& gdsMap )
   {

         // Let's prepare current sources and satellites
      prepareCurrentSourceSat(gdsMap); 

      // store old unknowns parameters
      VariableSet temp_old_unknowns(currentUnknowns);
         
       // Prepare set of current unknowns and list of current equations
      currentUnknowns = prepareUnknownsAndEquations(gdsMap);
      
      // Set up index for Variables now
      setUpEquationIndex( temp_old_unknowns );

         // Set this object as "prepared"
      isPrepared = true;

      return (*this);

   }  // End of method 'EquationSystem2::Prepare()'
	
	std::ostream& EquationSystem2::printEquationInfo( std::ostream& s,
																	  int mode ) const
		throw( InvalidEquationSystem2 )
	{
		if(!isPrepared)
		{
			s << "Equation system is not prepared !!! " << std::endl;
			return s;
		}

		CivilTime ct(epoch);

		s << "vvvvvvvvvvvvvvvvvvvv Equation info vvvvvvvvvvvvvvvvvvvv " << endl;
			// Time
		s << ct.year << " " << ct.month << " " << ct.day << " "
			<< ct.hour << " " << ct.minute << " " << ct.second << endl;

			// stations and satellites
		for( std::map<SourceID, SatIDSet>::const_iterator it = sourceSatSet.begin(); 
			  it != sourceSatSet.end(); 
			  ++it )
		{
			s << "station: " << it -> first << " observed Sats: ";
				
				// SatIDSet
			for( SatIDSet::iterator ite = (it -> second).begin();
				  ite != (it -> second).end(); 
				  ++ite )
			{
				s << *ite << " ";

			}  // End of ' for( SatIDSet::iterator::iterator ite  ... '

			s << endl;

		}  // End of ' for( std::map<SourceID, SatIDSet>::iterator ... '

			// Now equations
//		for()
//		{
//		}


		s << "^^^^^^^^^^^^^^^^^^^^ Equation info ^^^^^^^^^^^^^^^^^^^^^ " << endl;

		return s; 

	}  // End of ' std::ostream& EquationSystem2::printEquationInfo( std::ostream& s, ... '



	TypeID EquationSystem2::getTypeOfFirstEqu()
	{
		TypeID type;
		type = (*(equDescripList.begin())).header.indTerm.getType();

		return type;
	}
   
   void EquationSystem2::setUpEquationIndex( VariableSet& old_variable_set){

      /// Setup variable list
      int now_index  = 0;
      for( VariableSet::const_iterator now_it = currentUnknowns.begin();
            now_it != currentUnknowns.end(); now_it++ ){
         
         /// set current index
         ( (Variable&)(*now_it) ).setNowIndex( now_index++ );
         
         /// find previous index in old variable set
         VariableSet::const_iterator pre_it = old_variable_set.find(*now_it);
         if( old_variable_set.end() != pre_it){
            Variable preVar( *pre_it );
            ( (Variable&)(*now_it) ).setPreIndex( preVar.getNowIndex() );
         }
      }

    /// Setup Variable index in EquationList  
     
      for( std::list<Equation>::const_iterator itEq = currentEquationsList.begin();
            itEq != currentEquationsList.end(); itEq++ ){

         for( VarCoeffMap::const_iterator varIt = itEq->body.begin();
               varIt != itEq->body.end(); varIt++ ){
            
            VariableSet::const_iterator it = currentUnknowns.find( varIt->first );

            if( it != currentUnknowns.end() ){
               
               ( (Variable&)( varIt->first ) ).setNowIndex( ( (Variable&)(*it) ).getNowIndex() );
               ( (Variable&)( varIt->first ) ).setPreIndex( ( (Variable&)(*it) ).getPreIndex() );

            }

         }

      }

   }

      // Get current sources (SourceID's) and satellites (SatID's)
   void EquationSystem2::prepareCurrentSourceSat( gnssDataMap& gdsMap )
   {

	   // Recored the present epoch
	   epoch = ( gdsMap.begin() ) -> first;

	   // Clear "currentSatSet" and "currentSourceSet"
	   currentSatSet.clear();
	   currentSourceSet.clear();

	   // Insert the corresponding SatID's in "currentSatSet"
	   currentSatSet = gdsMap.getSatIDSet();// all stellites in this network

	   // Insert the corresponding SourceID's in "currentSourceSet"
	   currentSourceSet = gdsMap.getSourceIDSet();// all source in this network

	   // source -- satSet
	   for( SourceIDSet::iterator itSource = currentSourceSet.begin();
		                          itSource != currentSourceSet.end(); 
		                          ++itSource )
	   {
		   SourceID source(*itSource);
		   gnssDataMap tempgds( gdsMap.extractSourceID(source) );
		   sourceSatSet[ source ] = tempgds.getSatIDSet();

	   }  // End of ' for( SourceIDSet::iterator itSource = ... '



	   // Let's return
	   return;

   }  // End of method 'EquationSystem2::prepareCurrentSourceSat()'



      // Prepare set of current unknowns and list of current equations
   VariableSet EquationSystem2::prepareUnknownsAndEquations( gnssDataMap& gdsMap )
   {

         // Let's clear the current equations list
      currentEquationsList.clear();

         // Let's create 'currentUnkSet' set
      VariableSet currentUnkSet;

         // Let's retrieve the unknowns according to the 
         // equation descriptions and 'gdsMap'

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
                                       it != gdsMap.end();
                                       ++it )
      {
            // Then, iterate through corresponding 'sourceDataMap'
         for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
                                            sdmIter != (*it).second.end();
                                            ++sdmIter )
         {
            
				// Debug code vvv
				//SourceID sid( (*sdmIter).first );
				//cout << "station: " << sid <<  endl; 
				// Debug code ^^^

               // Visit each "Equation" in "equDescripList"
            for( std::list<Equation>::const_iterator itEq = equDescripList.begin();
                                                     itEq != equDescripList.end();
                                                     ++itEq )
            {

                  // Bool indicating whether current source is attributed to
                  // current equation.
               bool found(false);


                  // Check if current equation description is valid for all sources
               if ( (*itEq).getEquationSource() == Variable::allSources )
               {
                   found = true;
               }
                 // Check if equation description is valid for some sources
               else if ( (*itEq).getEquationSource() == Variable::someSources )
               {
                     // First, get the SourceID set for this equation description
                  SourceIDSet equSourceSet =  (*itEq).getSourceSet();

                     // Judge whether current source in inclued in 'equSourceSet'?
                  SourceIDSet::iterator itsrc = equSourceSet.find( (*sdmIter).first );
                  if(itsrc != equSourceSet.end() )
                  {
                     found = true;
                  }
               }
               else
               {
                     // In this case, we take directly the source as 'equSource' 
                  SourceID equSource = (*itEq).getEquationSource() ;

                  if( (equSource) == (*sdmIter).first )
                  {
                     found = true;
                  }

               } // End of 'if ( (*itEq).getEquationSource() == ...'

               if(found)
               {
                     // We need a copy of current Equation object description
                  Equation tempEquation( (*itEq) );

                     // Update equation independent term with SourceID information
                  tempEquation.header.equationSource = (*sdmIter).first;

						// Debug code vvv
//						std::cout << "Observable: " 
//										<< tempEquation.header.indTerm.getType()
//										<< endl
//										<< "Variables: "
//										<< endl;
						// Debug code ^^^

                     // Iterate the satellite and create the equations
                  for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                       stvmIter != (*sdmIter).second.end();
                       stvmIter++ )
                  {
                        // Set equation satellite
                     tempEquation.header.equationSat = (*stvmIter).first;

                        // Remove all the variables from this equation
                     tempEquation.clear();

                        // Set the type value data 
                     tempEquation.header.typeValueData = (*stvmIter).second;

                        // Now, let's visit all Variables and the corresponding 
                        // coefficient in this equation description
                     for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
                          vcmIter != (*itEq).body.end();
                          ++vcmIter )
                     { 
                           // We will work with a copy of current Variable
                        Variable var( (*vcmIter).first );
                          
                           // Take out the coefficient information from equation
                           // description
                        Coefficient coef( (*vcmIter).second);
                        
                           // Check what type of variable we are working on
                     
                           // If variable is source-indexed, set SourceID
                        if( var.getSourceIndexed() )
                        {
                           var.setSource( (*sdmIter).first );
                        }

                           // If variable is not satellite-indexed, we just need to
                           // add it to "currentUnkSet
                        if( !var.getSatIndexed() )
                        {
                              // Insert the result in "currentUnkSet" 
                           currentUnkSet.insert(var);

                              // Add this variable and related coefficient information to 
                              // current equation description. 
                           tempEquation.addVariable(var, coef);
                        }
                        else
                        {
                              // Set satellite
                           var.setSatellite( (*stvmIter).first );

                              // Insert the result in "currentUnkSet" 
                           currentUnkSet.insert(var);

                              // Add this variable and related coefficient information to 
                              // current equation description. 
                           tempEquation.addVariable(var, coef);

                        }  // End of 'if( !var.getSatIndexed() )...'

								// Debug code vvv
							   //cout << var.getType() << " " << coef.defaultCoefficient << " " << endl;
								// Debug code ^^^

                     }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'

							// Debug code vvv
							//cout << endl;
							// Debug code ^^^ 

                        // New equation is complete: Add it to 'currentEquationsList'
                     currentEquationsList.push_back( tempEquation );

                  }  // End of 'for( satTypeValueMap::const_iterator ...'

               }  // End of 'if(found)'

            }  // End of 'for(std::list<Equation>::const_iterator)'

         }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      }  // End of 'for( gnssDataMap::const_iterator it = ...'
        
         // Return set of current unknowns
		
		// Debug code vvv
//		CommonTime time((gdsMap.begin())->first);
//
//		std::cout << "Present unknowns: " << currentUnkSet.size()  << std::endl;
//
//		int count(1);
//		
//		for( VariableSet::iterator iter = currentUnkSet.begin(); 
//			  iter != currentUnkSet.end();
//			  ++iter )
//		{
//
//			std::cout << "Variable index: " << count << std::endl;
//			
//			Variable varTest(*iter);
//			std::cout << varTest.getType() << std::endl;
//
//			
//
//			count++;
//		}  // End of 'for( VariableSet::iterator iter ... '
		// Debug code ^^^
      return currentUnkSet;

   }  // End of method 'EquationSystem2::prepareUnknownsAndEquations()'


      /* Return the CURRENT number of variables, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem2 exception.
       */
   int EquationSystem2::getCurrentNumVariables() const
      throw(InvalidEquationSystem2)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem2("EquationSystem2 is not prepared"));
      }

      return currentUnknowns.size();

   }  // End of method 'EquationSystem2::getCurrentNumVariables()'



      /* Return the set containing variables being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem2 exception.
       */
   VariableSet EquationSystem2::getCurrentUnknowns() const
      throw(InvalidEquationSystem2)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem2("EquationSystem2 is not prepared"));
      }

      return currentUnknowns;

   }  // End of method 'EquationSystem2::getCurrentUnknowns()'



      /* Return the CURRENT number of sources, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem2 exception.
       */
   int EquationSystem2::getCurrentNumSources() const
      throw(InvalidEquationSystem2)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem2("EquationSystem2 is not prepared"));
      }

      return currentSourceSet.size();

   }  // End of method 'EquationSystem2::getCurrentNumSources()'



      /* Return the set containing sources being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem2 exception.
       */
   SourceIDSet EquationSystem2::getCurrentSources() const
      throw(InvalidEquationSystem2)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem2("EquationSystem2 is not prepared"));
      }

      return currentSourceSet;

   }  // End of method 'EquationSystem2::getCurrentSources()'



      /* Return the CURRENT number of satellites, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem2 exception.
       */
   int EquationSystem2::getCurrentNumSats() const
      throw(InvalidEquationSystem2)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem2("EquationSystem2 is not prepared"));
      }

      return currentSatSet.size();

   }  // End of method 'EquationSystem2::getCurrentNumSats()'



      /* Return the set containing satellites being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem2 exception.
       */
   SatIDSet EquationSystem2::getCurrentSats() const
      throw(InvalidEquationSystem2)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem2("EquationSystem2 is not prepared"));
      }

      return currentSatSet;

   }  // End of method 'EquationSystem2::getCurrentSats()'

}  // End of namespace gpstk

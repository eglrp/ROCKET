#pragma ident "$Id: SolverGenUpd.cpp 2346 2012-05-23 15:55:58Z shjzhang $"

/**
 * @file SolverGenUpd.cpp
 * General solver with ambiguity constraints.
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
//  Shoujian Zhang, Wuhan University (2011-2015)
//
//============================================================================
//
//  Modifications:
//
//  2015/07/13  Design a new TimeUpdate and MeasUpdate.
//  2015/07/13  The update for currentState and currentErrorCov is divided
//              into two parts, i.e. the update for the fixed unknowns and
//              the update for the float unknowns. This revision will 
//              improve the computational speed evidently.
//  2015/07/13  In MeasUpdate, the PG^T is not computed directly with
//              P matrix and G^T vector. Considering that the G is very 
//              sparse, therefore, the PG^T is computed only using the 
//              the non-zero element in G^T.
//
//  2015/07/23  Major revision
//              In 'SolverZapNL', all the fixed ambiguities will be hold and
//              transffered to the next epoch. However, if any ambiguities are
//              fixed to wrong values, the solutions in the following epochs
//              will be biased.
//              Thus, in the 'SolverGenUpd' class, only the independent 
//              ambiguities are treated as the constraints of the observation
//              equations, and only the independent ambiguities will be 
//              transferred to the next epoch. However, in order to get
//              fixed solutions in each step, in the 'AmbiguityFixing', 
//              the ambiguities will be fixed one by one, and the fixed solutions
//              and their covariance will be stored in 'stateMapFixed/covMapFixed'.
//
//  2015/08/28  Major revision
//              In the previous version, the fixed independent ambiguites will
//              be removed from the right to the left of the equations. 
//              However, the results showed that they are not equal with the
//              solutions in 'SolverUpdWL', in which the independent ambiguites
//              are treated as constraint equations.
//
//  2015/08/31  Major revision
//              Only the independent ambiguity datum are introduced into 
//              the equations directly, then after measurement update, 
//              the ambiguities will be fixed in 'AmbiguityFixing'.
//
//  2016/10/16
//  Modify Predict function. A new matrix mCoVarMatrix is declared to 
//  replace covarianceMap. Use index of Variable. 
//  I donot follow the modification of measUpdate function as these steps
//  does little contribution to the computational effeciency. 
//  Lei Zhao, WHU
//============================================================================


#include "ARRound.hpp"
#include "SolverGenUpd.hpp"
#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"

// Added by Lei Zhao, 2016/10/05 vvv (^_^)
#include "Counter.hpp"

#ifdef USE_OPENMP
#include <omp.h>
#endif
// ^^^ (^_^)

#include <time.h>

using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverGenUpd::classIndex = 9510000;


      // Returns an index identifying this object.
   int SolverGenUpd::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverGenUpd::getClassName() const
   { return "SolverGenUpd"; }


		/**  Single measurement correction  
		 * 
		 * @param z					Measurement
		 * @param weight			Weight of measurement
		 * @param G					Vector holding the coefficients of variables
		 * @param index			Vector holding index of above variables in 
		 *								the unknown vector
		 */ 
   int SolverGenUpd::singleMeasCorrect( const double& z,
					  					 const double& weight,
					  					 const Vector<double>& G,
					  					 const Vector<int>& index )
   throw(InvalidSolver)
   {
			// Inverse weight
		double inv_W(1.0/weight);

			// Num of var in xhat
		int numVar( xhat.size() );

			// Size of G and index vector
		int numUnks( index.size() );

			// M to store the value of  P*G
		Vector<double> M(numVar,0.0);

			// Now, let's compute M=P*G.
		for(int i=0; i<numVar; i++)
		{
			for(int j=0; j<numUnks; j++)
			{
				M(i) = M(i) + P(i,index(j)) * G(j);
			}  // End of ' for(int j=0; j<numUnks; j++) ' 
		}  // End of ' for(int i=0; i<numVar; i++) '

		double dotGM(0.0);
		for(int i=0; i<numUnks; i++)
		{
			dotGM = dotGM + G(i)*M(index(i));
		}  // End of ' for(int i=0; i<numUnks; i++) '

			// Compute the Kalman gain
		Vector<double> K(numVar,0.0);

		double beta(inv_W + dotGM);
		K = M/beta; 

		double dotGX(0.0);
		for(int i=0; i<numUnks; i++)
		{
			dotGX = dotGX + G(i)*xhat(index(i));
		}  // End of ' for(int i=0; i<numUnks; i++) '

			// State update
		xhat = xhat + K*( z - dotGX );

				// Covariance update
#ifdef USE_OPENMP
	#pragma omp parallel for
#endif
		for(int i=0;i<numVar;i++)
		{
				// The diagonal element
			P(i,i) = P(i,i) -  K(i)*M(i);

				// The upper/lower triangular element
			for(int j=(i+1);j<numVar;j++)
			{
				P(j,i) = P(i,j) = P(i,j) - K(i)*M(j);
			}  // End of ' for(int j=(i+1);j<numVar;j++) '
		}  // End of ' for(int i=0;i<numVar;i++) '

		return 0;

   }


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverGenUpd::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

            // Build a gnssRinex object and fill it with data
         gnssRinex g1;
         g1.header = gData.header;
         g1.body = gData.body;

            // Call the Process() method with the appropriate input object
         Process(g1);

            // Update the original gnssSatTypeValue object with the results
         gData.body = g1.body;

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGenUpd::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverGenUpd::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

            // Build a gnssDataMap object and fill it with data
         gnssDataMap gdsMap;
         SourceID source( gData.header.source );
         gdsMap.addGnssRinex( gData );

            // Call the Process() method with the appropriate input object,
            // and update the original gnssRinex object with the results
         Process(gdsMap);
         gData = gdsMap.getGnssRinex( source );

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGenUpd::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gdsMap     Map object holding the gnss data.
       */
   gnssDataMap& SolverGenUpd::Process(gnssDataMap& gdsMap)
      throw(ProcessingException)
   {

      try
      {
				
            // Prepare everything before computing
         Predict( gdsMap );

					
            // Call the Compute() method with the defined equation model.
         Correct( gdsMap );

            // return
         return gdsMap;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGenUpd::Process()'


      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGenUpd::Predict( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {
      clock_t start,finish;
      double totaltime;
      start=clock();

         oldUnknowns = currentUnknowns;

            // Prepare the equation system with current data
         equSystem.Prepare(gdsMap);

            // Get current unknowns
         currentUnknowns = equSystem.getCurrentUnknowns(); 

            // Get the number of unknowns being processed
         int numUnknowns( currentUnknowns.size() );

            /*
             * Now, Let's get the state and covariance for current 'currentUnknowns'
             */

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );
               // Fill the initialErrorCovariance matrix
            int i(0);      // Reset an index
            for( VariableSet::const_iterator itVar = currentUnknowns.begin();
                 itVar != currentUnknowns.end();
                 ++itVar )
            {
               initialErrorCovariance(i,i) = (*itVar).getInitialVariance();
               ++i;
            }

               // Reset Kalman filter
            xhat = initialState; 
            P = initialErrorCovariance;

//          firstTime = false;
         }
         else
         {
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);

               // Fill the state vector
            int i(0);      // Set an index
            for( VariableSet::const_iterator itVar = currentUnknowns.begin();
                 itVar != currentUnknowns.end();
                 ++itVar )
            {
               currentState(i) = stateMap[ (*itVar) ];
               ++i;
            }

               // Fill the covariance matrix
               // We need a copy of 'currentUnknowns'
            VariableSet tempSet( currentUnknowns );
            i = 0;         // Reset 'i' index
            for( VariableSet::const_iterator itVar1 = currentUnknowns.begin();
                 itVar1 != currentUnknowns.end();
                 ++itVar1 )
            {
//                  // Fill the diagonal element
					// Test code vvv
					int now_index = itVar1->getNowIndex();
					int pre_index = itVar1->getPreIndex();
					if( -1 == pre_index )
					{
						currentErrorCov( now_index, now_index ) = 
							(*itVar1).getInitialVariance();
					}
					else
					{
						currentErrorCov( now_index, now_index ) =
							mCoVarMatrix( pre_index, pre_index );
					}  // End of 'if( -1 == pre_index )' 

					// Test code ^^^
                  // Remove current Variable from 'tempSet'
               tempSet.erase( (*itVar1) );

					for( VariableSet::const_iterator itVar2 = tempSet.begin();
                     itVar2 != tempSet.end() ; itVar2++ )
					{
                  
                  int now_index_2 = itVar2->getNowIndex();
                  int pre_index_2 = itVar2->getPreIndex();

                  if( -1 == pre_index || -1 == pre_index_2 )
						{
                     currentErrorCov( now_index, now_index_2 ) =
                        currentErrorCov( now_index_2, now_index ) = 0;
                  }
						else
						{
                     currentErrorCov( now_index, now_index_2 ) =
                        currentErrorCov( now_index_2, now_index ) =
                           mCoVarMatrix( pre_index, pre_index_2 );
                  }

               }  // End of 'for( VariableSet::const_iterator itVar2 = ...'

            }  // End of for( VariableSet::const_iterator itVar1 = currentUnknowns...'

               // Reset Kalman filter to current state and covariance matrix
            xhat = currentState;
            P = currentErrorCov;

         }  // End of 'if(firstTime)'


            /*
             * Now, do 'TimeUpdate' based on block multiplication
             */

         std::list<Equation> equList;
         equList = equSystem.getCurrentEquationsList();

            // Set to store the variables that already prepared
         VariableSet varUnkPrepared;

            // Resize qMatrix
         qMatrix.resize( numUnknowns, numUnknowns, 0.0);

            // Let's get current time
         gnssDataMap::const_iterator it=gdsMap.begin();
         CommonTime epoch((*it).first);

            // Visit each Equation in "equList"
         for( std::list<Equation>::const_iterator itEq = equList.begin();
              itEq != equList.end();
              ++itEq )
         {

               // Now, let's visit all Variables and the corresponding 
               // coefficient in this equation description
            for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
                 vcmIter != (*itEq).body.end();
                 ++vcmIter )
            {
                  // We will work with a copy of current Variable
               Variable var( (*vcmIter).first );

                  // find this current varialbe in 'varUnkPrepared'
               VariableSet::const_iterator itVar=varUnkPrepared.find( var );

                  // If not found, then you should 'prepare' the stochastic model
                  // for this variable
               if( itVar == varUnkPrepared.end() )
               {
                  SatID varSat(var.getSatellite());
                  SourceID varSource(var.getSource());
                  typeValueMap tData( (*itEq).header.typeValueData );


                     // Prepare variable's stochastic model
                  var.getModel()->Prepare(epoch,
                                          varSource, 
                                          varSat, 
                                          tData);

						// Test code vvv
						int index( var.getNowIndex() );
						// Test code ^^^

                     // phi/q for current model
                  double phiValue, qValue;

                     // Now, check if this is an 'old' variable
                  if( oldUnknowns.find( var ) != oldUnknowns.end() )
                  {
                        // This variable is 'old'; compute its phi and q values
                     phiValue= var.getModel()->getPhi();
                     qValue = var.getModel()->getQ();

                        // Warning, here we will directly update the xhat/P 
                        // according the phi and its index. 

////////////////////////// //////////////////////////
//
//       P(i,:) = phiMatrix * P(i,:)
//       P(:,i) = P(:,i) * phiMatrixT
//
////////////////////////// //////////////////////////


			
//                        // Now, Let's update the state and covariance
                     if(phiValue != 1.0)
                     {
                           // Update the state
                        xhat(index) = phiValue*xhat(index);

                           // Update the diagonal element
                        P(index,index) = phiValue*P(index,index)*phiValue;

                           // Update the row/column of the covariance matrix
                        for(int i=0; i<numUnknowns; i++)
                        {
                           if(i != index)
                           {
                              P(index,i) = 
                                 P(i,index) = phiValue*P(index,i); 
                           }
                        }
                     }

                        // Update the qMatrix 
                     qMatrix(index,index) = var.getModel()->getQ();

                  }
                  else
                  {
                        // This variable is 'new', reset the xhatminus 
                     xhat(index) = 0.0;

                        // Reset the diagonal element
                     P(index,index) = 0.0;

                        // Reset the covariance element
                     for(int i=0;i<numUnknowns;i++)
                     {
                        if(i != index)
                        {
                           P(index,i) = 
                              P(i,index) = 0.0;
                        }
                     }
                        // let's use its initial variance instead of its 
                        // stochastic model
                     qMatrix(index,index) = var.getInitialVariance();

                  }

                     // At last, insert current variable into 'varUnkPrepared'
                  varUnkPrepared.insert(var);

               }  // End of 'if(itVar==varUnkPrepared.end())'
                 
            }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'

         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

            // Now, Let's copy xhat/P to 'predicted' xhatminus/Pminus
         xhatminus = xhat;
         Pminus = P;

            // Now, Let's add the 'qMatrix' to 'Pminus'
         for(int i=0;i<numUnknowns;i++)
         {
            Pminus(i,i) = Pminus(i,i) + qMatrix(i,i); 
         }

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

      return gdsMap;

   }  // End of method 'SolverGenUpd::preCompute()'



      // Compute the solution of the given equations set.
      //
      // @param gData    Data object holding the data.
      //
      // \warning A typical kalman filter will be devided into 
      // two different phase, the prediction and the correction.
      // for some special application, for example, the 'integer'
      // satellite and receiver clock/bias estimation, the ambiguity
      // constraints are generated using the 'predicted' ambiguity 
      // values and its variances. 
      //
   gnssDataMap& SolverGenUpd::Correct( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {

				// Ambiguity Constraints
         MeasUpdate( gdsMap );


				// Ambiguity Constraints
         AmbiguityFixing( gdsMap );



             // Post compute
         postCompute(gdsMap);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }


         // Return  
      return gdsMap;

   }  // End of method 'SolverGenUpd::Compute()'


      /** Code to be executed before 'MeasUpdate()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGenUpd::MeasUpdate( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {
      try
      {
				// Judge WL or BLC equation system
			TypeID equType( equSystem.getTypeOfFirstEqu() );
			if( equType == TypeID::MWubbena)
			{
				ambiguityType = TypeID::BWL;
				waveLength = WL_WAVELENGTH_GPS;
				//waveLength = 0.861918400;
			}
			else
			{
				ambiguityType = TypeID::BL1;
				waveLength = NL_WAVELENGTH_GPS;
			}


            /**
             * Firstly, Let's get the predicted state and covariance 
             */
      clock_t start,finish;
      double totaltime;
      start=clock();

         VariableSet currentUnknowns( equSystem.getCurrentUnknowns() );

            // Fill the initialState vector  
         if( firstTime )
         {

            int i(0);      // Set an index
            for( VariableSet::const_iterator itVar = currentUnknowns.begin();
                 itVar != currentUnknowns.end();
                 ++itVar )
            {

					                  // If the type of (*itVar) is 'TypeID::BWL'
               if( (*itVar).getType() == ambiguityType )
               {
							// Searched type 
						TypeID searchedType = ambiguityType;
						if( ambiguityType == TypeID::BL1 )
						{
							searchedType = TypeID::BLC;
						}
		
                     // Get the MWubbena value
                  double amb = gdsMap.getValue( (*itVar).getSource(), 
                                                (*itVar).getSatellite(), 
                                                searchedType );

  
                     // Compute the initial widelan ambiguity
                  xhatminus(i) = std::floor( amb/(-1*waveLength) + 0.5 );
  
               } // End of 'if( (*itVar).getType() == ... )'
  
                  // Index
               ++i;
            }
               // No longer first time
            firstTime = false;
         }
         
         VariableDataMap ambMap;
         std::map<Variable, VariableDataMap > ambVarMap;

            // Store values of current state
         int i(0);      // Set an index
         for( VariableSet::const_iterator itVar = currentUnknowns.begin();
              itVar != currentUnknowns.end();
              ++itVar )
         {
            if( (*itVar).getType() == ambiguityType )
            {
               ambMap[(*itVar)] = xhatminus(i);
               ambVarMap[ (*itVar) ][ (*itVar) ] = Pminus(i, i);
            }
            ++i;
         }

            /**
             * Now, Let's get the ambiguity datum 
             */
         ambFixedMap.clear();

            // Set apriori state solution to the ambiguityDatum 
         indepAmbDatum.Reset(ambMap, ambVarMap);
         indepAmbDatum.Prepare(gdsMap);

            // Now, get the fixed ambiguity sats and their values
         ambFixedMap = indepAmbDatum.getIndepAmbMap();

            /**
             * Define prefit/geometry/weight matrix for ambiguity candidate 
             */

            // Number of ambiguities that can be fixed
         int numFix( ambFixedMap.size() );

            // Throw exception
         if( numFix == 0 )
         {
               // Throw an exception if something unexpected happens
            ProcessingException e("The ambiguity constraint equation number is 0.");

               // Throw the exception
            GPSTK_THROW(e);

         }  // End of 'If(...)'

            //
            // Now, Let's preform the measurment udpate
            //


         xhat = xhatminus;
         P = Pminus;

            //
            // Measurement update using single observables each time.
            //
         int numVar(currentUnknowns.size());

            //
            // Let's apply the independent ambiguity constraints
            //
            // To be modified
            // apply the ambiguity constraint equations
            //


         for( VariableDataMap::const_iterator itamb=ambFixedMap.begin();
              itamb !=ambFixedMap.end();
              ++itamb )
         {

               // Now, prefit = fixed ambiguity
            double tempPrefit( (*itamb).second );

               // Weight
            double weight( 1.0e+9 );
            double tempCoef(1.0);

               // Second, fill geometry matrix: Look for equation coefficients
            VariableDataMap tempCoeffMap;
               
               // Now, Let's get the position of this variable in 
               // 'currentUnknowns'
            Variable var( (*itamb).first );
            VariableSet::const_iterator itVar1=currentUnknowns.find( (var) );

               // Get the coefficent map
            if( itVar1 != currentUnknowns.end() )
            {
               tempCoeffMap[(*itVar1)] = tempCoef ;
            }

               // Now, Let's create the index for current float unks

               // Float unks number for current equation
            int numUnks(tempCoeffMap.size());
            Vector<int> index(numUnks);
            Vector<double> G(numUnks);

               // Loop the tempCoeffMap
            int i(0);
            for( VariableDataMap::const_iterator itvdm=tempCoeffMap.begin();
                 itvdm != tempCoeffMap.end();
                 ++itvdm )
            {
               int col(0);
               VariableSet::const_iterator itVar2=currentUnknowns.begin();
               while( (*itVar2) != (*itvdm).first )
               {
                  col++;
                  itVar2++;
               }
               index(i) = col; 
               G(i) = (*itvdm).second;

                  // Incrment of the float variable 
               i++;
            }

					// True measurement update
				singleMeasCorrect( tempPrefit, 
										 weight,
									    G,
									    index );

         }  // End of 'for( VariableDataMap::const_iterator ... '

			
			   //
            // Let's perform the measurement update one observable by one
            //
         std::list<Equation> equList;
         equList = equSystem.getCurrentEquationsList();
         int numEqu(equList.size());

         prefitResiduals.resize(numEqu,0.0);
         hMatrix.resize( numEqu, numVar, 0.0 );

         int row(0);
         for( std::list<Equation>::const_iterator itEq = equList.begin();
              itEq != equList.end();
              ++itEq )
         {
               // Get the type value data from the header of the equation
            typeValueMap tData( (*itEq).header.typeValueData );

               // Get the independent type of this equation
            TypeID indepType( (*itEq).header.indTerm.getType() );

               // Now, Let's get current prefit
            double tempPrefit(tData(indepType));

               // Weight
            double weight;

               // First, fill weights matrix
               // Check if current 'tData' has weight info. If you don't want those
               // weights to get into equations, please don't put them in GDS
            if( tData.find(TypeID::weight) != tData.end() )
            {
                  // Weights matrix = Equation weight * observation weight
               weight = (*itEq).header.constWeight * tData(TypeID::weight);
            }
            else
            {
               weight = (*itEq).header.constWeight; // Weights matrix = Equation weight
            }

               // Second, fill geometry matrix: Look for equation coefficients
            VariableDataMap tempCoeffMap;
               
               // Now, let's visit all Variables and the corresponding 
               // coefficient in this equation description
            for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
                 vcmIter != (*itEq).body.end();
                 ++vcmIter, i++)
            {
                  // We will work with a copy of current Variable
               Variable var( (*vcmIter).first );

                  // Coefficient Struct
               Coefficient coef( (*vcmIter).second);

                  // Coefficient values
               double tempCoef(0.0);

                  // Check if '(*itCol)' unknown variable enforces a specific
                  // coefficient, according the coefficient information from
                  // the equation
               if( coef.forceDefault )
               {
                     // Use default coefficient
                  tempCoef = coef.defaultCoefficient;
               }
               else
               {
                     // Look the coefficient in 'tdata'

                     // Get type of current varUnknown
                  TypeID type( var.getType() );

                     // Check if this type has an entry in current GDS type set
                  if( tData.find(type) != tData.end() )
                  {
                        // If type was found, insert value into hMatrix
                     tempCoef = tData(type);
                  }
                  else
                  {
                        // If value for current type is not in gdsMap, then
                        // insert default coefficient for this variable
                     tempCoef = coef.defaultCoefficient;
                  }

               }  // End of 'if( (*itCol).isDefaultForced() ) ...'

                  // Now, Let's get the position of this variable in 
                  // 'currentUnknowns'
               VariableSet::const_iterator itVar1=currentUnknowns.find( (var) );

                  // Get the coefficent map
               if( itVar1 != currentUnknowns.end() )
               {
                  tempCoeffMap[(*itVar1)] = tempCoef ;
               }

            }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'

               // Now, Let's create the index for current float unks

               // Float unks number for current equation
            int numUnks(tempCoeffMap.size());
            Vector<int> index(numUnks);
            Vector<double> G(numUnks);

               // Loop the tempCoeffMap
            int i(0);
            for( VariableDataMap::const_iterator itvdm=tempCoeffMap.begin();
                 itvdm != tempCoeffMap.end();
                 ++itvdm )
            {
               int col(0);
               VariableSet::const_iterator itVar2=currentUnknowns.begin();
               while( (*itVar2) != (*itvdm).first )
               {
                  col++;
                  itVar2++;
               }
               index(i) = col; 
               G(i) = (*itvdm).second;

                  // Set the geometry matrix
               hMatrix(row, col) = (*itvdm).second;

                  // Incrment of the float variable 
               i++;
            }

					// True measurement update
				singleMeasCorrect( tempPrefit, 
										 weight,
									    G,
									    index );


               // insert current 'prefit' into 'prefitResiduals'
            prefitResiduals(row) = tempPrefit;

               // Increment row number
            ++row;


         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'


			// Test code vvv
//			cout << "cpu time for measurement update in MeasUpdate function: "
//					<< Counter::end() << endl;
			// Test code ^^^

            // Reset
         xhatminus = xhat;
         Pminus = P;

            // Reset
         solution = xhat;
         covMatrix = P;

            // Compute the postfit residuals Vector
         postfitResiduals = prefitResiduals - (hMatrix* solution);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );
         GPSTK_THROW(e);
      }

      return gdsMap;
   }

      // Now, fix all the ambiguities to integers 
   gnssDataMap& SolverGenUpd::AmbiguityFixing( gnssDataMap& gData )
      throw(ProcessingException)
   {

      try
      {
         Vector<double> newState;
         Matrix<double> newCov;

            // Firsly, store the solution/covMatrix into 'newState/newCov'
         newState = solution;
         newCov = covMatrix; 

            /**
             * Firstly, fix the widelane ambiguities
             */
			// Test code vvv
//			Counter::begin();
			// Test code ^^^
         
			int numUnknowns(currentUnknowns.size());

            // Flags indicating whether this unknown variable has been fixed.
         Vector<double> stateFlag(numUnknowns, 0.0);

            // Cutting decision to fix the ambiguities
         double cutDec(1000.0);

            // Ambiguity Fixing method
         ARRound ambRes(1000,0.15,0.15);

            /*
             * Loop to fix a ambiguity
             */
         int numFixed(0);

            // Still have ambiguity to be fixed? 
         bool done(false); 

            // Now, fix the N1 ambiguities
         while( !done )
         {
               // Indicator variable
            int index(-1);
            double maxDec(-100.0);
            Variable tempVar;

            int i(0);
            for( VariableSet::const_iterator itVar1 = currentUnknowns.begin();
                 itVar1 != currentUnknowns.end();
                 ++itVar1 )
            {
               if( stateFlag(i) == 0.0 && (*itVar1).getType() == ambiguityType )
               {
                     // Ambiguity value
                  double bw = newState(i);

                     // Sigma 
                  double bwSig = std::sqrt( newCov(i,i) );

                     // Ambiguity fixing decision
                  double decision = ambRes.getDecision(bw, bwSig);

                     // Look for the largest fixing decision
                  if( decision > maxDec )
                  {
                     index = i;
                     maxDec = decision;
                     tempVar = (*itVar1);
                  }
               }
                  // Increment
               i++;
            }

               // If found 
            if(index != -1)
            {
                  // If can be fixed
               if( maxDec > cutDec )
               {
                     // Ambiguity value
                  double bw = newState(index);

                     // Fixed value of bw
                  double bwFixed = std::floor( bw + 0.5 );

                     // Update the solution and covarinace  
                  AmbiguityUpdate(newState, newCov, stateFlag, index, bwFixed );

                     // Not free at all
                  stateFlag(index) = 1.0;

                     // Store the fixed ambiguity value into 'state'
                  newState(index) = bwFixed;
                    
                     // Insert the ambiguity into 'ambFixedMap'
                  ambFixedMap[(tempVar)] = bwFixed;

                     // Update the fixed number
                  numFixed = numFixed + 1;

               }
               else
               {
                    // If ambiguity can't be fixed
                  done = true;
               } 

            }
            else
            {
                 // if no ambiguity is found
               done = true;
            }
                
         }  // End of 'while(done)'

			// Test code vvv
//			cout << "cpu time for WL amb fixing in AmbiguityFixing: " 
//					<< Counter::end() << endl;
//			Counter::begin();
			// Test code ^^^

            // Now, Let's compute the new postfitRediduals
            // using the fixed solution
         postfitResiduals = prefitResiduals - (hMatrix* newState);

            // Now, Let's check the posfitResiduals           
            
         std::list<Equation> equList;
         equList = equSystem.getCurrentEquationsList();
         int numEqu(equList.size());
            
            // Visit each Equation in "equList"
         int row(0);
         for( std::list<Equation>::const_iterator itEq = equList.begin();
              itEq != equList.end();
              ++itEq )
         {
               // Get the type value data from the header of the equation
            typeValueMap tData( (*itEq).header.typeValueData );

               // Weight
            double weight;
  
               // First, fill weights matrix
               // Check if current 'tData' has weight info. If you don't want those
               // weights to get into equations, please don't put them in GDS
            if( tData.find(TypeID::weight) != tData.end() )
            {
                  // Weights matrix = Equation weight * observation weight
               weight = (*itEq).header.constWeight * tData(TypeID::weight);
            }
            else
            {
               weight = (*itEq).header.constWeight; // Weights matrix = Equation weight
            }
  
            Variable tempAmbWL;
  
               // Let's get the ambiguity variable in this equation
            for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
                 vcmIter != (*itEq).body.end();
                 ++vcmIter )
            {
               if( (*vcmIter).first.getType() == ambiguityType )
               {
                  tempAmbWL = (*vcmIter).first;
               }
            }
  
            double threshold( 3*std::sqrt(1.0/weight) );

               // If exceeds the threshold, then remove the ambiguity 
               // from 'ambFixedMap'
            if( postfitResiduals(row) > threshold )
            {

                  // Warning:
                  // The map only remove the varialbes already 
                  // existed in the map
               VariableDataMap::iterator itvdm = ambFixedMap.find(tempAmbWL);
                  
                  // If found in the 'ambFixedMap', then remove it 
               if( itvdm != ambFixedMap.end() )
               {
                  ambFixedMap.erase( tempAmbWL );
               }

////              // Reset the ambiguity variance and covariance
////           VariableSet::iterator itVar1 = currentUnknowns.begin();
////           int k(0);
////           while( (*itVar1) != tempAmbWL )
////           {
////              k++;
////              itVar1++;
////           }

////              // Set the variance to a large variance
////           covMatrix(k,k) = 1.0e+9;

////              // Set all the covariance to zero
////           for( int i=0; i<numUnknowns; i++ )
////           {
////              if( i!=k )
////              {
////                 covMatrix(i,k) = covMatrix(k,i) = 0.0;
////              }
////           }
                  
            }
  
               // Increment row number
            ++row;
   
         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

			// Test code vvv
//			cout << "cpu time for validation of fixed solution in AmbiguityFixing: " 
//					<< Counter::end() << endl;
//			Counter::begin();
			// Test code ^^^

			
         Vector<double> newState2;
         Matrix<double> newCov2;

            // Firsly, store the solution/covMatrix into 'newState/newCov'
         newState2 = solution;
         newCov2 = covMatrix; 
  
            // Flags indicating whether this unknown variable has been fixed.
         stateFlag.resize(numUnknowns, 0.0);
  
            // Now, Let's update the solution/covariance again using
            // only the correct fixed ambiguities
         for( VariableDataMap::iterator itAmb = ambFixedMap.begin();
              itAmb != ambFixedMap.end();
              ++ itAmb)
         {
            VariableSet::iterator itVar1 = currentUnknowns.begin();
            int k(0);
            while( (*itVar1) != (*itAmb).first )
            {
               k++;
               itVar1++;
            }
  
            int index(k);
  
               // Fixed value of bw
            double bwFixed = (*itAmb).second;
  
               // Update the solution and covarinace  
            AmbiguityUpdate(newState2, newCov2, stateFlag, index, bwFixed );
  
               // Not free at all
            stateFlag(index) = 1.0;

         }

            // Clean up the map
         stateMapFixed.clear();
//         covMapFixed.clear();

			// CoMmEnT by Lei Zhao vvv
            // Store values of the fixed solution
         int i(0);      // Set an index
         for( VariableSet::const_iterator itVar = currentUnknowns.begin();
              itVar != currentUnknowns.end();
              ++itVar )
         {
            stateMapFixed[ (*itVar) ] = newState2(i);
            ++i;
         }
//
//            // Store values of covariance matrix
//            // We need a copy of 'currentUnknowns'
//         VariableSet tempSet( currentUnknowns );
//         i = 0;         // Reset 'i' index
//         for( VariableSet::const_iterator itVar1 = currentUnknowns.begin();
//              itVar1 != currentUnknowns.end();
//              ++itVar1 )
//         {
//               // Fill the diagonal element
//            covMapFixed[ (*itVar1) ][ (*itVar1) ] = newCov2(i, i);
//
//            int j(i+1);      // Set 'j' index
//               // Remove current Variable from 'tempSet'
//            tempSet.erase( (*itVar1) );
//            for( VariableSet::const_iterator itVar2 = tempSet.begin();
//                 itVar2 != tempSet.end();
//                 ++itVar2 )
//            {
//               covMapFixed[ (*itVar1) ][ (*itVar2) ] = newCov2(i, j);
//
//               ++j;
//            }
//            
//            ++i;
//
//         }  // End of for( VariableSet::const_iterator itVar1 = currentUnknowns...'

			// CoMmEnT by Lei Zhao ^^^ 

			// Test code vvv
//			cout << "cpu time for fixed WL amb storing in AmbiguityFixing: " 
//					<< Counter::end() << endl;
			// Test code ^^^
			
            // Transfer the 'newState2/newCov2'
         if(fixAndHold)
         {
            solution = newState2;
            covMatrix = newCov2;
         }

            // Return 
         return gData;
  
      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGenUpd::AmbiguityFixing()'



      /** Update the free unknonws' solution and covariance after
       *  fixing the ambiguities
       */
   void SolverGenUpd::AmbiguityUpdate( Vector<double>& state,
                                      Matrix<double>& covariance,
                                      Vector<double>& fixedFlag,
                                      int& index,
                                      double& fixedValue )
      throw(ProcessingException)
   {

      try
      {
            // Variance epsilon to avoid numerical exception
         const double eps(1.0e-9);

            // Get unknown number
         int numUnknowns(state.size());

            // Covariance related to the fixed ambiguity
         Vector<double> Qba(numUnknowns, 0.0);

            // Get the covariance
         for( int i=0; i<numUnknowns; i++)
         {
            Qba(i) = covariance(i,index);
         }

            // Qii
         double Qaa;
         Qaa = Qba(index) + eps;
         Qaa = 1.0/Qaa;

            // Free value
         double freeValue = state(index);

            // Solution udpate
         for(int i=0; i<numUnknowns; i++)
         {
            if( fixedFlag(i) == 0.0 )
            {
               state(i) = state(i) + Qba(i)*Qaa*(fixedValue - freeValue); 
            }
         }

            // Update the fixed solution
         state(index) = fixedValue ;

            // Covariance update
         for(int i=0; i<numUnknowns; i++)
         {
            if( (fixedFlag(i) == 0.0) && (i != index) )
            {
               for(int j=0; j<numUnknowns; j++)
               {
                  if( (fixedFlag(j) == 0.0) && (j != index) )
                  {
                     covariance(i,j) = covariance(i,j) - Qba(i)*Qaa*Qba(j);
                  }
               }
            }
         }

            // Q(i, index), Q(index, j)
         for(int i=0; i<numUnknowns; i++)
         {
             if(i==index)
             {
                covariance(i,index) = eps;
             }
             else
             {
                covariance(i,index) = 
                    covariance(index,i) = 0.0;
             }
         }

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGenUpd::AmbiguityUpdate()'
  

      // Recover the state and covariance map after get the 
      // solution and covMatrix
      //
      // @param gData    Data object holding the data.
      //
   gnssDataMap& SolverGenUpd::postCompute( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {
            // Clean up values in 'stateMap' and 'covarianceMap'
         stateMap.clear();
         covarianceMap.clear();

      clock_t start,finish;
      double totaltime;
      start=clock();
         
            // Store values of current state
         int i(0);      // Set an index
         for( VariableSet::const_iterator itVar = currentUnknowns.begin();
              itVar != currentUnknowns.end();
              ++itVar )
         {
            stateMap[ (*itVar) ] = solution(i);
            ++i;
         }

			// Test code vvv
			mCoVarMatrix = Matrix<double>( covMatrix );
			// Test code ^^^

//            // Store values of covariance matrix
//            // We need a copy of 'currentUnknowns'
//         VariableSet tempSet( currentUnknowns );
//         i = 0;         // Reset 'i' index
//         for( VariableSet::const_iterator itVar1 = currentUnknowns.begin();
//              itVar1 != currentUnknowns.end();
//              ++itVar1 )
//         {
//               // Fill the diagonal element
//            covarianceMap[ (*itVar1) ][ (*itVar1) ] = covMatrix(i, i);
//            int j(i+1);      // Set 'j' index
//               // Remove current Variable from 'tempSet'
//            tempSet.erase( (*itVar1) );
//            for( VariableSet::const_iterator itVar2 = tempSet.begin();
//                 itVar2 != tempSet.end();
//                 ++itVar2 )
//            {
//               covarianceMap[ (*itVar1) ][ (*itVar2) ] = covMatrix(i, j);
//               ++j;
//            }
//            ++i;
//         }  // End of for( VariableSet::const_iterator itVar1 = currentUnknowns...'

            /**
             *  Now, get the ambigity number and fixing rate for 
             *  each satellite.
             */
           
            // Firstly, clear the data to store the 
            // information for current epoch
         fixingDataMap.clear();

            // Number for float/fixed ambiguities
//         int numBWL(0);
//         int numNWL(0);
//			int numFloatAmb(0);
//			int numFixedAmb(0);
        
            // Get the float ambiguity number
         for( VariableSet::const_iterator itVar = currentUnknowns.begin();
              itVar != currentUnknowns.end();
              ++itVar )
         {
             if( (*itVar).getType() == (ambiguityType) )
             {
                SatID satellite = (*itVar).getSatellite();
                fixingDataMap[ satellite ].floatAmbNumb++;
             }
         }

  
            // Get the fixed ambiguity number
         for( VariableDataMap::iterator itAmb = ambFixedMap.begin();
              itAmb != ambFixedMap.end();
              ++ itAmb)
         {
                // Satellite of current ambiguity
             SatID satellite = (*itAmb).first.getSatellite();

                // Increment of the fixed widelane ambiguities
             fixingDataMap[ satellite ].fixedAmbNumb++;            
             
         }

            // Get current satellites
         SatIDSet currentSatSet = gdsMap.getSatIDSet();

         for( SatIDSet::iterator itSat = currentSatSet.begin();
              itSat != currentSatSet.end();
              ++itSat)
         {

				double numFloatAmb(0.0);
				double numFixedAmb(0.0);
				
               // Get number of float/fixed widelane ambiguities
            numFloatAmb = fixingDataMap[(*itSat)].floatAmbNumb;
            numFixedAmb = fixingDataMap[(*itSat)].fixedAmbNumb;


               // Compute the fixing rate for each satellite
            fixingDataMap[(*itSat)].fixingRate = numFixedAmb/numFloatAmb;

         }

            /**
             * Let's correct the BLC value with the fixed widelane ambiguity
             * according to the formula.
             */

			if( ambiguityType == TypeID::BWL )
			{

	            // Define a temporary gnssDataMap
	         gnssDataMap dataMap;
	
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
	
	                  // Get gnssRinex for this source
	               gnssRinex gRin( gdsMap.getGnssRinex( (*sdmIter).first ) );
	               satTypeValueMap& gData(gRin.body);
	
	                  // Satellite set with widelane ambiguity unfixed
	               SatIDSet deleteSats;
	                
	                  // Finally, iterate through corresponding 'satTypeValueMap'
	               for( satTypeValueMap::iterator stvmIter = gData.begin();
	                    stvmIter != gData.end();
	                    stvmIter++ )
	               {
	
	                  Variable tempVar(ambType);
	                  tempVar.setSource( (*sdmIter).first );
	                  tempVar.setSatellite( (*stvmIter).first );
	
	                     // Find this variable in 'ambFixedMap'
	                  VariableDataMap::iterator itvdm = ambFixedMap.find(tempVar);
	
	                     // If not found the "BWL" of this satellite in the 'ambFixedMap'
	                  if( itvdm == ambFixedMap.end() )
	                  {
	                     deleteSats.insert( (*stvmIter).first );
	                  }
	                  else
	                  {
	                        // The fixed widelane ambiguity
	                     double NWL( (*itvdm).second );
	
	                        // Get the float BLC ambiguity
	                     typeValueMap tData( (*stvmIter).second );
	
	                     typeValueMap::iterator tvmIter = tData.find(TypeID::BLC);
	                     if(tvmIter!= tData.end())
	                     {
	                        double BLC;
	                        BLC = tData( TypeID::BLC ) + 0.377482511 * NWL;
	                        (*stvmIter).second[TypeID::BLC] = BLC;
	                     }
	
	                     typeValueMap::iterator tvmIter2= tData.find(TypeID::prefitL);
	                     if(tvmIter2!= tData.end())
	                     {
	                        double prefitL;
	                        prefitL = tData( TypeID::prefitL) + 0.377482511 * NWL;
	                        (*stvmIter).second[TypeID::prefitL] = prefitL;
	                     }
	                  }
	
	               }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'
	
	                  // Remove satellites, the widelane ambiguities of which 
	                  // are not fixed
	               gRin.removeSatID( deleteSats);
	        
	                  // Add the gRin into 'dataMap', in which the widelane 
	                  // ambiguities are all fixed
	               dataMap.addGnssRinex(gRin);
	
	            }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'
	
	         }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'
				gdsMap = dataMap;

			}  // End of ' if( ambiguityType == TypeID::BWL )  ' 

            /**
             * Let's correct the BLC value with the fixed widelane ambiguity
             * according to the formula.
             */
            // Now copy dataMap to gdsMap
//         gdsMap = dataMap;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

         // Return 
      return gdsMap;

   }  // End of method 'SolverGenUpd::postMeasUpdate()'


      /* Returns the solution associated to a given TypeID and SourceID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGenUpd::getSolution( const TypeID& type,
                                      const SourceID& source ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type and source
      while( !( (*it).first.getType()   == type &&
                (*it).first.getSource() == source ) &&
             it != stateMap.end() )
      {
         ++it;

         // If it is not found, throw an exception
         if( it == stateMap.end() )
         {
             InvalidRequest e("Type and source not found in solution vector.");
             GPSTK_THROW(e);
         }
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGenUpd::getSolution()'


      /* Returns the solution associated to a given TypeID, SourceID and
       *  SatID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGenUpd::getSolution( const TypeID& type,
                                      const SatID& sat ) const
      throw(InvalidRequest)
   {

      // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

      // Look for a variable with the same type and source
      while( !( (*it).first.getType()   == type &&
                (*it).first.getSatellite() == sat ) &&
                it != stateMap.end() )
      {
         ++it;

            // If it is not found, throw an exception
         if( it == stateMap.end() )
         {
            InvalidRequest e("Type and source not found in solution vector.");
            GPSTK_THROW(e);
         }
      }

      // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGenUpd::getSolution()'


      /* Returns the solution associated to a given TypeID and SourceID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGenUpd::getFixedSolution( const TypeID& type,
                                         const SourceID& source ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMapFixed' and go to the first element
      VariableDataMap::const_iterator it = stateMapFixed.begin();

         // Look for a variable with the same type and source
      while( !( (*it).first.getType()   == type &&
                (*it).first.getSource() == source ) &&
             it != stateMapFixed.end() )
      {
         ++it;

         // If it is not found, throw an exception
         if( it == stateMapFixed.end() )
         {
             InvalidRequest e("Type and source not found in solution vector.");
             GPSTK_THROW(e);
         }
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGenUpd::getSolution()'


      /* Returns the solution associated to a given TypeID, SourceID and
       *  SatID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGenUpd::getFixedSolution( const TypeID& type,
                                         const SatID& sat ) const
      throw(InvalidRequest)
   {

      // Declare an iterator for 'stateMapFixed' and go to the first element
      VariableDataMap::const_iterator it = stateMapFixed.begin();

      // Look for a variable with the same type and source
      while( !( (*it).first.getType()   == type &&
                (*it).first.getSatellite() == sat ) &&
                it != stateMapFixed.end() )
      {
         ++it;

            // If it is not found, throw an exception
         if( it == stateMapFixed.end() )
         {
            InvalidRequest e("Type and source not found in solution vector.");
            GPSTK_THROW(e);
         }
      }

      // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGenUpd::getSolution()'


}  // End of namespace gpstk

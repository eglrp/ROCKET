#pragma ident "$Id: SolverGeneral2.cpp 2877 2011-08-20 08:40:11Z yanweignss $"

/**
 * @file SolverGeneral2.hpp
 * General Solver.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009
//
//============================================================================
//
//  Revision
//
//  2014/03/02      Change the 'currentErrorCov' setting: When the 'Variable'
//                  is 'NEW', which can't be found in 'covarianceMap', the
//                  Variance of the variable will be set as the default values,
//                  and the covariance of the variable between other variables
//                  are set 'ZERO'. In the previous version, the variance is
//                  set as '0', and the covariance are set as the default 
//                  values, which are not reasonable.
//
//============================================================================


#include "SolverGeneral2.hpp"
#include "SystemTime.hpp"
#include "Counter.hpp"


#ifdef USE_OPENMP
#include <omp.h>
#endif

namespace gpstk
{

      // Index initially assigned to this class
   int SolverGeneral2::classIndex = 9600000;


      // Returns an index identifying this object.
   int SolverGeneral2::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverGeneral2::getClassName() const
   { return "SolverGeneral2"; }


      /* Explicit constructor.
       *
       * @param equationList  List of objects describing the equations
       *                      to be solved.
       */
   SolverGeneral2::SolverGeneral2( const std::list<Equation>& equationList )
      : firstTime(true)
   {

         // Visit each "Equation" in 'equationList' and add them to 'equSystem'
      for( std::list<Equation>::const_iterator itEq = equationList.begin();
           itEq != equationList.end();
           ++itEq )
      {
         equSystem.addEquation( (*itEq) );
      }

   }  // End of constructor 'SolverGeneral2::SolverGeneral2()'


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverGeneral2::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverGeneral2::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverGeneral2::Process(gnssRinex& gData)
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

   }  // End of method 'SolverGeneral2::Process()'



      /* Returns a reference to a gnssDataMap object after solving
       *  the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGeneral2::Process( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {

         TimeUpdate( gdsMap );
         MeasUpdate( gdsMap );

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

   }  // End of method 'SolverGeneral2::Prepare()'



      /* TimeUpdate
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGeneral2::TimeUpdate( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {  
         /// time counter begin
         Counter::begin();

         oldUnknowns = currentUnknowns;

            // Prepare the equation system with current data
         equSystem.Prepare(gdsMap);

            // Get current unknowns
         currentUnknowns = equSystem.getCurrentUnknowns();
         
         cout << "cputime equSystem:" << Counter::end() << endl;

            // Get the number of unknowns being processed
         int numUnknowns( currentUnknowns.size() );

         cout << "numUnknowns=" << numUnknowns << endl;
         
         /// preCompute time begin
         Counter::begin();

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );

               // Fill the initialErrorCovariance matrix

            int i(0);      // Set an index

            for( VariableSet::const_iterator itVar = currentUnknowns.begin();
                 itVar != currentUnknowns.end();
                 ++itVar )
            {
               initialErrorCovariance(i,i) = (*itVar).getInitialVariance();
               ++i;
            }

               // Reset Kalman filter state and covariance matrix
            xhat = initialState; 
            P    = initialErrorCovariance;

               // No longer first time
            firstTime = false;

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
                  // Fill the diagonal element
                 
                  // Check if '(*itVar1)' belongs to 'covarianceMap'
              /* if( covarianceMap.find( (*itVar1) ) != covarianceMap.end() )
               {
                     // If it belongs, get element from 'covarianceMap'
                  currentErrorCov(i, i) = covarianceMap[ (*itVar1) ][ (*itVar1) ];
               }
               else  
               {     
                     // If it doesn't belong, ask for default covariance
                  currentErrorCov(i, i) = (*itVar1).getInitialVariance();
               }*/

               int now_index = itVar1->getNowIndex();
               int pre_index = itVar1->getPreIndex();

               if( -1 == pre_index ){
                  currentErrorCov( now_index, now_index ) = (*itVar1).getInitialVariance();
               }else{
                  currentErrorCov( now_index, now_index ) = mCoVarMatrix( pre_index, pre_index );
               }


               //int j(i+1);      // Set 'j' index

                  // Remove current Variable from 'tempSet'
               tempSet.erase( (*itVar1) );

               /*for( VariableSet::const_iterator itVar2 = tempSet.begin();
                    itVar2 != tempSet.end();
                    ++itVar2 )
               {
                  currentErrorCov(i, j) =
                     currentErrorCov(j, i) =
                        covarianceMap[ (*itVar1) ][ (*itVar2) ];

                  ++j;
               }*/

               //++i;
               for( VariableSet::const_iterator itVar2 = tempSet.begin();
                     itVar2 != tempSet.end() ; itVar2++ ){
                  
                  int now_index_2 = itVar2->getNowIndex();
                  int pre_index_2 = itVar2->getPreIndex();

                  if( -1 == pre_index || -1 == pre_index_2 ){
                     currentErrorCov( now_index, now_index_2 ) =
                        currentErrorCov( now_index_2, now_index ) = 0;
                  }else{
                     currentErrorCov( now_index, now_index_2 ) =
                        currentErrorCov( now_index_2, now_index ) =
                           mCoVarMatrix( pre_index, pre_index_2 );
                  }

               }

            }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'

               // Reset Kalman filter to current state and covariance matrix
            xhat = currentState;
            P    = currentErrorCov;

         }  // End of 'if(firstTime)'

      //finish=clock();
      //totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
      cout << "cputtime preCompute" << Counter::end() << endl;

            ////////////////////////////////////////////
            //
            // Time update based on block multiplication
            //
            ////////////////////////////////////////////
      
      Counter::begin();

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
                // Now, let's visit all variables and the corresponding 
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

                      // Now, Let's get the position of this variable in 
                      // 'currentUnknowns'
                  /* int index(0);
                   VariableSet::const_iterator itVar1=currentUnknowns.begin();
                   while( (*itVar1) != var )
                   {
                      index++;
                      itVar1++;
                   }*/
                  
                   int index = var.getNowIndex(); 

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

                         // Now, Let's update the state and covariance
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

      //finish=clock();
      //totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
      cout << "cputtime TimeUpdate" << Counter::end() << endl;

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

   }  // End of method 'SolverGeneral2::TimeUpdate()'


      /* Code to be executed after 'TimeUpdate()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGeneral2::MeasUpdate( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {
            ////////////////////////////////////////////
            //                    
            // Measurement update using single observables each time.
            //
            ////////////////////////////////////////////
            
      //clock_t start,finish;
      //double totaltime;
      //start=clock();
         Counter::begin();
            // Now, Let's copy the predicted state/covariance to 
            // xhat/P
         xhat = xhatminus;
         P = Pminus;

         int numUnknowns( equSystem.getCurrentNumVariables() );

            // Get the set with unknowns being processed
         VariableSet currentUnknowns( equSystem.getCurrentUnknowns() );

         std::list<Equation> equList;
         equList = equSystem.getCurrentEquationsList();
         int numEqu(equList.size());

         Vector<double> prefitResiduals(numEqu);
         Matrix<double> hMatrix( numEqu, numUnknowns, 0.0 );
         
         // declare here for memory reuse
         Vector<double> M( numUnknowns, 0.0 );
         Vector<double> K( numUnknowns, 0.0 );

            // Visit each Equation in "equList"
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

               // number of Variables in current Equation
            int numVar = itEq->body.size();
            
               // holding current Equation Variable index in Unknowns
            Vector<int> index(numVar);

               // holding current Equation Variable coeffience
            Vector<double> G(numVar);
            
            /// resize the Matrix and Vector, it just reset all element as zero
            M.resize( M.size(), 0.0 );
            K.resize( K.size(), 0.0 );

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
            // VariableDataMap tempCoeffMap;
            
            /// allocator double array to holding coeffience for Variable
            /// double *tempCoefArray = new double[ numVar ];
            
               // Now, let's visit all Variables and the corresponding 
               // coefficient in this equation description
            int i = 0;
            for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
                 vcmIter != (*itEq).body.end();
                 ++vcmIter, i++ )
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

               //tempCoeffMap[var] = tempCoef ;
               //tempCoefArray[ var->getNowIndex() ] = tempCoef;
               hMatrix( row, var.getNowIndex() ) = tempCoef;
               index(i) = var.getNowIndex();
               G(i) = tempCoef;

              // cout << "index="<<var.getNowIndex()<<",coef="<<tempCoef<<endl;
            }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'
               // Now, Let's create the index for current float unks

               // Float unks number for current equation
            //int numVar(tempCoeffMap.size());
            //Vector<int> index(numVar);
            //Vector<double> G(numVar);

               // Loop the tempCoeffMap
            //int i(0);
            /*i=0;
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
            }*/

               // Temp measurement
            double z(tempPrefit);

               // Inverse weight
            double inv_W(1.0/weight);  

               // M to store the value of  P*G
            //Vector<double> M(numUnknowns,0.0);
            
               // Now, let's compute M=P*G.
            for(int i=0; i<numUnknowns; i++)
            {
               for(int j=0; j<numVar; j++)
               {
                  M(i) = M(i) + P(i,index(j)) * G(j);
               }
            }

            double dotGM(0.0);
            for(int i=0; i<numVar; i++)
            {
               dotGM = dotGM + G(i)*M(index(i)); 
            }

               // Compute the Kalman gain
            //Vector<double> K(numUnknowns,0.0); 

            double beta(inv_W + dotGM);

            K = M/beta;

            double dotGX(0.0);
            for(int i=0; i<numVar; i++)
            {
               dotGX = dotGX + G(i)*xhat(index(i)); 
            }

               // State update
            xhat = xhat + K*( z - dotGX );
            
               // Covariance update
               // old version:
               // P = P - outer(K,M);
               // Considering that the P and KM matrix are symetric, 
               // thus the computation can be accelerated by operating 
               // the upper triangular matrix.
#ifdef USE_OPENMP            
   #pragma omp parallel for
#endif
            for(int i=0;i<numUnknowns;i++)
            {
                  // The diagonal element
               P(i,i) = P(i,i) -  K(i)*M(i);

                  // The upper/lower triangular element
               for(int j=(i+1);j<numUnknowns;j++)
               {
                  P(j,i) = P(i,j) = P(i,j) - K(i)*M(j); 
               }

            }  // End of 'for(int i = 0; ...)'

               // insert current 'prefit' into 'prefitResiduals'
            prefitResiduals(row) = tempPrefit;

               // Increment row number
            ++row;

         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

      //finish=clock();
      //totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
      cout << "cputtime measupdate" << Counter::end() << endl;
      //start=finish;
      Counter::begin();

            // Reset
         xhatminus = xhat;
         Pminus = P;

            // Reset
         solution = xhat;
         covMatrix = P;

            // Compute the postfit residuals Vector
         postfitResiduals = prefitResiduals - (hMatrix* solution);

      //finish=clock();
      //totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
      cout << "cputtime postfitResiduals" << Counter::end() << endl;
      //start=finish;

         //////////// //////////// //////////// ////////////
         //
         //  postCompute, store the xhat and P into stateMap
         //  and covarianceMap for the next epoch.
         //
         //////////// //////////// //////////// ////////////

            // Clean up values in 'stateMap' and 'covarianceMap'
         stateMap.clear();
         covarianceMap.clear();


            // Get the set with unknowns being processed
         VariableSet unkSet( equSystem.getCurrentUnknowns() );

            // Store values of current state
         
         mCoVarMatrix = Matrix<double>( covMatrix );

         int i(0);      // Set an index
         for( VariableSet::const_iterator itVar = unkSet.begin();
              itVar != unkSet.end();
              ++itVar )
         {
            stateMap[ (*itVar) ] = solution(i);
            ++i;
         }

            // Store values of covariance matrix
            // We need a copy of 'unkSet'
         /*VariableSet tempSet( unkSet );
         i = 0;         // Reset 'i' index
         for( VariableSet::const_iterator itVar1 = unkSet.begin();
              itVar1 != unkSet.end();
              ++itVar1 )
         {
               // Fill the diagonal element
            covarianceMap[ (*itVar1) ][ (*itVar1) ] = covMatrix(i, i);
            int j(i+1);      // Set 'j' index
               // Remove current Variable from 'tempSet'
            tempSet.erase( (*itVar1) );
            for( VariableSet::const_iterator itVar2 = tempSet.begin();
                 itVar2 != tempSet.end();
                 ++itVar2 )
            {
               covarianceMap[ (*itVar1) ][ (*itVar2) ] = covMatrix(i, j);
               ++j;
            }
            ++i;
         }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'
         */

            // Store the postfit residuals in the GNSS Data Structure
         i = 0;         // Reset 'i' index
            // Visit each equation in "equList"
         for( std::list<Equation>::const_iterator itEq = equList.begin();
              itEq != equList.end();
              ++itEq )
         {
               // Get the TypeID of the residuals. The original type by default.
            TypeID residualType( (*itEq).header.indTerm.getType() );

               // Prefits are treated different
            if( residualType == TypeID::prefitC )
            {
               residualType = TypeID::postfitC;
            }
            else if( residualType == TypeID::prefitL )
            {
               residualType = TypeID::postfitL;
            }
            else if( residualType == TypeID::prefitP2 )
            {
               residualType = TypeID::postfitP2;
            }
            else if( residualType == TypeID::prefitPdelta )
            {
               residualType = TypeID::postfitPdelta;
            }
            else if( residualType == TypeID::prefitL1 )
            {
               residualType = TypeID::postfitL1;
                  // Extract the ambiguity of L1
            }
            else if( residualType == TypeID::prefitL2 )
            {
               residualType = TypeID::postfitL2;
                  // Extract the ambiguity of L2
            }
            else if( residualType == TypeID::prefitLdelta )
            {
               residualType = TypeID::postfitLdelta;
                  // Extract the ambiguity of Lw
            }
            gdsMap.insertValue( (*itEq).header.equationSource,
                                (*itEq).header.equationSat,
                                residualType,
                                postfitResiduals(i) );

            ++i;  // Increment counter

         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'


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

   }  // End of method 'SolverGeneral2::postCompute()'



      /* Returns the solution associated to a given Variable.
       *
       * @param variable    Variable object solution we are looking for.
       */
   double SolverGeneral2::getSolution( const Variable& variable ) const
      throw(InvalidRequest)
   {

         // Look the variable inside the state map
      VariableDataMap::const_iterator it( stateMap.find( variable ) );

         // Check if the provided Variable exists in the solution. If not,
         // an InvalidSolver exception will be issued.
      if( it == stateMap.end() )
      {
         InvalidRequest e("Variable not found in solution vector.");
         GPSTK_THROW(e);
      }

         // Return value
      return (*it).second;

   }  // End of method 'SolverGeneral2::getSolution()'



      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getSolution( const TypeID& type ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type
      while( (*it).first.getType() != type &&
             it != stateMap.end() )
      {
         ++it;

         // If the same type is not found, throw an exception
         if( it == stateMap.end() )
         {
             InvalidRequest e("Type not found in solution vector.");
             GPSTK_THROW(e);
         }
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGeneral2::getSolution()'



      /* Returns the solution associated to a given TypeID and SourceID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getSolution( const TypeID& type,
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

   }  // End of method 'SolverGeneral2::getSolution()'


      /* Returns the solution associated to a given TypeID, SourceID and
       *  SatID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getSolution( const TypeID& type,
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

   }  // End of method 'SolverGeneral2::getSolution()'


      /* Returns the solution associated to a given TypeID, SourceID and
       * SatID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getSolution( const TypeID& type,
                                      const SourceID& source,
                                      const SatID& sat ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type, source and satellite
      while( !( (*it).first.getType()      == type    &&
                (*it).first.getSource()    == source  &&
                (*it).first.getSatellite() == sat        ) &&
             it != stateMap.end() )
      {
         ++it;

         // If it is not found, throw an exception
         if( it == stateMap.end() )
         {
             InvalidRequest e("Type, source and SV not found in solution vector.");
             GPSTK_THROW(e);
         }
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGeneral2::getSolution()'

      /* Returns the covariance associated to a given Variable.
       *
       * @param var1    first variable object
       * @param var2    second variable object
       */
   double SolverGeneral2::getCovariance( const Variable& var1, 
                                        const Variable& var2 ) const
      throw(InvalidRequest)
   {
      std::map<Variable, VariableDataMap >::const_iterator it1 = covarianceMap.find(var1);
      if(it1!=covarianceMap.end())
      {
         VariableDataMap::const_iterator it2 = it1->second.find(var2);
         if(it2!=it1->second.end())
         {
            return it2->second;
         }
         else
         {
            it1 = covarianceMap.find(var2);
            if(it1!=covarianceMap.end())
            {
               it2 = it1->second.find(var1);
               if(it2!=it1->second.end()) return it2->second;
            }
         }
      }
      
         // Once code go here, we failed to find the value, and throw exception.
      InvalidRequest e("Failed to get the covariance value.");
      GPSTK_THROW(e);

      return 0.0;
   }


      /* Returns the variance associated to a given Variable.
       *
       * @param variable    Variable object variance we are looking for.
       */
   double SolverGeneral2::getVariance(const Variable& variable) const 
      throw(InvalidRequest)
   {

         // Check if the provided Variable exists in the solution. If not,
         // an InvalidSolver exception will be issued.
      if( stateMap.find( variable ) == stateMap.end() )
      {
         InvalidRequest e("Variable not found in covariance matrix.");
         GPSTK_THROW(e);
      }

         // Return value
      return getCovariance(variable,variable);

   }  // End of method 'SolverGeneral2::getVariance()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getVariance(const TypeID& type) const 
      throw(InvalidRequest)
   {

         // Declare an iterator for 'covarianceMap' and go to the first element
      std::map<Variable, VariableDataMap >::const_iterator it
                                                      = covarianceMap.begin();

         // Look for a variable with the same type
      while( (*it).first.getType() != type &&
             it != covarianceMap.end() )
      {
         ++it;

         // If the same type is not found, throw an exception
         if( it == covarianceMap.end() )
         {
             InvalidRequest e("Type not found in covariance matrix.");
             GPSTK_THROW(e);
         }
      }

         // Else, return the corresponding value
      return getCovariance(it->first,it->first); 

   }  // End of method 'SolverGeneral2::getVariance()'


      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       * @param source  SourceID of the solution we are looking for.
       * 
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getVariance( const TypeID& type,
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
      return getCovariance(it->first,it->first);

   }  // End of method 'SolverGeneral2::getVariance()'


      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       * @param source  SourceID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getVariance( const TypeID& type,
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
      return getCovariance(it->first,it->first);


   }  // End of method 'SolverGeneral2::getVariance()'


      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       * @param source  SourceID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral2::getVariance( const TypeID& type,
                                      const SourceID& source,
                                      const SatID& sat ) const 
      throw(InvalidRequest)
   {
         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type, source and satellite
      while( !( (*it).first.getType()      == type    &&
         (*it).first.getSource()    == source  &&
         (*it).first.getSatellite() == sat        ) &&
         it != stateMap.end() )
      {
         ++it;

         // If it is not found, throw an exception
         if( it == stateMap.end() )
         {
            InvalidRequest e("Type, source and SV not found in solution vector.");
            GPSTK_THROW(e);
         }
      }

         // Else, return the corresponding value
      return getCovariance(it->first,it->first);

   }  // End of method 'SolverGeneral2::getVariance()'


      /* Set the solution associated to a given Variable.
       *
       * @param variable    Variable object solution we are looking for.
       * @param val         solution value for the Variable object
       */
   SolverGeneral2& SolverGeneral2::setSolution( const Variable& variable,
                                              const double& val )
      throw(InvalidRequest)
   {
      VariableDataMap::iterator it = stateMap.find(variable);
      if(it!=stateMap.end())
      {
         stateMap[variable] = val;
      }
      else
      {
         InvalidRequest e("The variable not exist in the solver.");
         GPSTK_THROW(e);
      }

      return (*this);
   }


      /** Set the covariance associated to a given Variable.
       *
       * @param var1    first variable object
       * @param var2    second variable object
       * @param cov     covariance value for the variable objects
       */
   SolverGeneral2& SolverGeneral2::setCovariance( const Variable& var1, 
                                              const Variable& var2,
                                              const double& cov)
      throw(InvalidRequest)
   {  
      std::map<Variable, VariableDataMap >::iterator it1 = covarianceMap.find(var1);
      if(it1!=covarianceMap.end())
      {
         VariableDataMap::iterator it2 = it1->second.find(var2);
         if(it2!=it1->second.end())
         {
            covarianceMap[var1][var2] = cov;
            return (*this);
         }
         else
         {
            it1 = covarianceMap.find(var2);
            if(it1!=covarianceMap.end())
            {
               it2 = it1->second.find(var1);
               if(it2!=it1->second.end())
               {
                  covarianceMap[var2][var1] = cov;
                  return (*this);
               }
            }
         }
      }

      // One code go here, we failed to find the value, and throw exception.
      InvalidRequest e("The input variables are not exist in the solver.");
      GPSTK_THROW(e);
      
      return (*this);
   }

}  // End of namespace gpstk

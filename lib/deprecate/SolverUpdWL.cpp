#pragma ident "$Id: SolverUpdWL.cpp 2346 2012-05-23 15:55:58Z shjzhang $"

/**
 * @file SolverUpdWL.cpp
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
//  Shoujian Zhang, Wuhan University (2011, 2012)
//
//============================================================================


#include "ARRound.hpp"
#include "SolverUpdWL.hpp"
#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"
#include <time.h>

using namespace gpstk::StringUtils;


namespace gpstk
{

      // Index initially assigned to this class
   int SolverUpdWL::classIndex = 9510000;


      // Returns an index identifying this object.
   int SolverUpdWL::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverUpdWL::getClassName() const
   { return "SolverUpdWL"; }


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverUpdWL::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverUpdWL::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverUpdWL::Process(gnssRinex& gData)
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

   }  // End of method 'SolverUpdWL::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gdsMap     Map object holding the gnss data.
       */
   gnssDataMap& SolverUpdWL::Process(gnssDataMap& gdsMap)
      throw(ProcessingException)
   {

      try
      {

            // Prepare everything before computing
         preCompute( gdsMap );

            // Call the Compute() method with the defined equation model.
         Compute( gdsMap );

            // Call the postCompute() method to store the state and covMatrix.
         postCompute( gdsMap );


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

   }  // End of method 'SolverUpdWL::Process()'


      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverUpdWL::preCompute( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {

            // Prepare the equation system with current data
         equSystem.Prepare(gdsMap);

            // Get matrices and vectors out of equation system
            // Measurements vector (Prefit-residuals)
         measVector = equSystem.getPrefitsVector();

            // Geometry matrix
         hMatrix = equSystem.getGeometryMatrix();

            // Weights matrix
         rMatrix = equSystem.getWeightsMatrix();

            // State Transition Matrix (PhiMatrix)
         phiMatrix = equSystem.getPhiMatrix();

            // Noise covariance matrix (QMatrix)
         qMatrix = equSystem.getQMatrix();

            // Get the number of unknowns being processed
         int numUnknowns( equSystem.getTotalNumVariables() );

            // Get the set with unknowns being processed
         VariableSet unkSet( equSystem.getVarUnknowns() );

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );


               // Fill the initialErrorCovariance matrix

            int i(0);      // Reset an index
            for( VariableSet::const_iterator itVar = unkSet.begin();
                 itVar != unkSet.end();
                 ++itVar )
            {
               initialErrorCovariance(i,i) = (*itVar).getInitialVariance();
               ++i;
            }

               // Reset Kalman filter
            xhat = initialState; 
            P = initialErrorCovariance;

            firstTime = false;

         }
         else
         {
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);

               // Fill the state vector

            int i(0);      // Set an index
            for( VariableSet::const_iterator itVar = unkSet.begin();
                 itVar != unkSet.end();
                 ++itVar )
            {
               currentState(i) = stateMap[ (*itVar) ];
               ++i;
            }

               // Fill the covariance matrix

               // We need a copy of 'unkSet'
            VariableSet tempSet( unkSet );

            i = 0;         // Reset 'i' index
            for( VariableSet::const_iterator itVar1 = unkSet.begin();
                 itVar1 != unkSet.end();
                 ++itVar1 )
            {
                  // Fill the diagonal element

                  // Check if '(*itVar1)' belongs to 'covarianceMap'
               if( covarianceMap.find( (*itVar1) ) != covarianceMap.end() )
               {
                     // If it belongs, get element from 'covarianceMap'
                  currentErrorCov(i, i) = covarianceMap[ (*itVar1) ][ (*itVar1) ];
               }
               else  
               {     
                     // If it doesn't belong, ask for default covariance
                  currentErrorCov(i, i) = (*itVar1).getInitialVariance();
               }


               int j(i+1);      // Set 'j' index

                  // Remove current Variable from 'tempSet'
               tempSet.erase( (*itVar1) );

               for( VariableSet::const_iterator itVar2 = tempSet.begin();
                    itVar2 != tempSet.end();
                    ++itVar2 )
               {
                     // If it belongs, get element from 'covarianceMap'
                  currentErrorCov(i, j) =
                     currentErrorCov(j, i) =
                        covarianceMap[ (*itVar1) ][ (*itVar2) ];

                  ++j;
               }

               ++i;

            }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'

               // Reset Kalman filter to current state and covariance matrix
            xhat = currentState;
            P = currentErrorCov;

         }  // End of 'if(firstTime)'

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

   }  // End of method 'SolverUpdWL::preCompute()'



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
   gnssDataMap& SolverUpdWL::Compute( gnssDataMap& gdsMap )
      throw(InvalidSolver)
   {
      clock_t start,finish;
      double totaltime;
      start=clock();

         // Call the TimeUpdate() of the kalman filter, which will predict the 
         // state vector and their covariance matrix
      TimeUpdate( phiMatrix, qMatrix );

         // Ambiguity Constraints
      AmbiguityConstr( gdsMap );

         // Call the MeasUpdate() of the kalman filter, which will update the 
         // state vector and their covariance using new measurements.
      MeasUpdate( measVector, hMatrix, rMatrix );

//        // Ambiguity Constraints
//    AmbiguityFixing( gdsMap );
//


         // Return  
      return gdsMap;

   }  // End of method 'SolverUpdWL::Compute()'


      // Predict the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverUpdWL::TimeUpdate( const Matrix<double>& phiMatrix,
                                const Matrix<double>& processNoiseCovariance )
      throw(InvalidSolver)
   {

         // Check whether the phiMatrix is square
      if (!(phiMatrix.isSquare()))
      {
         InvalidSolver e("phiMatrix is not square");
         GPSTK_THROW(e);
      }

         // Get the number of unknowns being processed
      int numUnknowns( equSystem.getTotalNumVariables() );


      int stateSize(xhat.size());
      if(!(stateSize==numUnknowns))
      {
         InvalidSolver e("TimeUpdate(): Size of a posteriori state estimation vector \
do not match the number of unknowns");
         GPSTK_THROW(e);
      }


      int phiRow = static_cast<int>(phiMatrix.rows());
      if (!(phiRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of phiMatrix");
         GPSTK_THROW(e);
      }

      if (!(qMatrix.isSquare()))
      {
         InvalidSolver e("qMatrix is not square");
         GPSTK_THROW(e);
      }

      int qRow = static_cast<int>(qMatrix.rows());
      if (!(qRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of qMatrix");
         GPSTK_THROW(e);
      }

      try
      {
            // Compute the a priori state vector
         xhatminus = phiMatrix*xhat;

         Matrix<double> phiT(transpose(phiMatrix));

            // Compute the a priori estimate error covariance matrix
         Pminus = phiMatrix*P*phiT + processNoiseCovariance;

      }
      catch(...)
      {
         InvalidSolver e("TimeUpdate(): Unable to predict next state.");
         GPSTK_THROW(e);
         return -1;
      }
        
      return 0;

   }  // End of method 'SolverUpdWL::Compute()'


      /** Code to be executed before 'TimeUpdate()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverUpdWL::AmbiguityConstr( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {
      try
      {
            /**
             * Firstly, Let's get the predicted state and covariance 
             */

            // Get the set with unknowns being processed
         VariableSet unkSet( equSystem.getVarUnknowns() );

            // Get the number of unknowns being processed
         int numUnknowns( equSystem.getTotalNumVariables() );

            // Fill the initialState vector  
//       if( firstTime )
//       {
//          int i(0);      // Set an index
//          for( VariableSet::const_iterator itVar = unkSet.begin();
//               itVar != unkSet.end();
//               ++itVar )
//          {
//                // If the type of (*itVar) is 'TypeID::BWL'
//             if( (*itVar).getType() == TypeID::BWL )
//             {
//                   // Get the MWubbena value
//                double MWubbena = gdsMap.getValue( (*itVar).getSource(), 
//                                                   (*itVar).getSatellite(), 
//                                                   TypeID::MWubbena );

//                   // Compute the initial widelan ambiguity
//                xhatminus(i) = std::floor( MWubbena/(-0.861918400) + 0.5 );

//             } // End of 'if( (*itVar).getType() == ... )'

//                // Index
//             ++i;
//          }
//             // No longer first time
//          firstTime = false;
//       }

            // Clear predited state and covariance map
         stateMapminus.clear();
         covMapminus.clear();

            // Store values of current state
         int i(0);      // Set an index
         for( VariableSet::const_iterator itVar = unkSet.begin();
              itVar != unkSet.end();
              ++itVar )
         {
            stateMapminus[ (*itVar) ] = xhatminus(i);
            ++i;
         }

            // Store values of covariance matrix
            // We need a copy of 'unkSet'
         VariableSet tempSet( unkSet );
         i = 0;         // Reset 'i' index
         for( VariableSet::const_iterator itVar1 = unkSet.begin();
              itVar1 != unkSet.end();
              ++itVar1 )
         {
               // Remove current Variable from 'tempSet'
            tempSet.erase( (*itVar1) );

               // Set 'j' index
            int j(i+1); 

               // Fill the diagonal element
            covMapminus[ (*itVar1) ][ (*itVar1) ] = Pminus(i, i);

               // Fill the non-diagonal element
            for( VariableSet::const_iterator itVar2 = tempSet.begin();
                 itVar2 != tempSet.end();
                 ++itVar2 )
            {
               covMapminus[ (*itVar1) ][ (*itVar2) ] = Pminus(i, j);
               ++j;
            }

            ++i;

         }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'


            /**
             * Now, Let's get the ambiguity datum 
             */

         if(isIndepAmb)
         {
               // Set apriori state solution to the ambiguityDatum 
            indepAmbDatum.Reset(stateMapminus, covMapminus);

               // Fixing all the potential independent ambiguities
            indepAmbDatum.Prepare(gdsMap);

               // Now, get the fixed ambiguity sats and their values
            ambFixedMap = indepAmbDatum.getIndepAmbMap();

         }
         else
         {
               // Set apriori state solution to the ambiguityDatum 
            ambiguityDatum.Reset(stateMapminus, covMapminus);

               // Fixing all the potential independent ambiguities
            ambiguityDatum.Prepare(gdsMap);

               // Now, get the fixed ambiguity sats and their values
            ambFixedMap = ambiguityDatum.getAmbFixedMap();
         }


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


         Vector<double> measVectorFix(numFix,0.0);
         Matrix<double> hMatrixFix(numFix, numUnknowns,0.0);
         Matrix<double> rMatrixFix(numFix, numFix,0.0);


            // Now, get fixed ambiguity equation
         int nrow(0);
         for( VariableDataMap::iterator it = ambFixedMap.begin();
              it!= ambFixedMap.end();
              ++it )
         {
               // Find the ambiguity variable in 'unkSet'
            int jcol(0);
            VariableSet::const_iterator itUnk( unkSet.begin() );
            while( (*itUnk) != (*it).first )
            {
               ++jcol;
               ++itUnk;
            }

               // Prefit equals the fixed ambiguity value 
            measVectorFix( nrow ) = (*it).second;

               // Coefficient of ambiguity is set to 1.0
            hMatrixFix( nrow, jcol ) = 1.0;

               // The weight is set to a large value
            rMatrixFix( nrow, nrow ) = 1.0E+14;

               // Row number increment
            nrow++;
            
         }

         int numMeas( measVector.size() );

            /**
             * Now, Let's combine the measurement equation and constraint 
             * equation together 
             */

            // The observation equation number is enlarged
         int numEqu( numMeas + numFix );

            // New observation prefit/geometry/weight matrix
         Vector<double> tempPrefit(numEqu, 0.0);
         Matrix<double> tempGeometry(numEqu, numUnknowns, 0.0);
         Matrix<double> tempWeight(numEqu, numEqu, 0.0);

            /**
             * Firstly, copy the measVector/hMatrix/rMatrix to temporary store
             */

            // Now, Let's store the measVector/hMatrix/rMatrix
         for(int i=0; i<numMeas; i++)
         {
               // Prefit
            tempPrefit(i) = measVector(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++) 
            { 
               tempGeometry(i,j) = hMatrix(i,j); 
            }

               // Weight
            tempWeight(i,i) = rMatrix(i,i); 

         }

            // Now, Let's store the measVectorFix/hMatrixFix/rMatrixFix
         int rowStart(numMeas) ;
         for(int i=0; i<numFix; i++)
         {
               // Prefit
            tempPrefit(rowStart+i) = measVectorFix(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++)
            {
               tempGeometry(rowStart+i,j) = hMatrixFix(i,j); 
            }  

               // Weight
            tempWeight(rowStart+i,rowStart+i)  = rMatrixFix(i,i); 

         }

            // Resize the measVector/hMatrix/rMatrix
         measVector.resize(numEqu,  0.0);
         hMatrix.resize(numEqu, numUnknowns, 0.0);
         rMatrix.resize(numEqu, numEqu, 0.0);


            // New Measurement vector update
         measVector = tempPrefit ;
         hMatrix = tempGeometry ;
         rMatrix = tempWeight ;

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



      // Correct the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverUpdWL::MeasUpdate( const Vector<double>& prefitResiduals,
                                const Matrix<double>& designMatrix,
                                const Matrix<double>& weightMatrix )
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

      if (!(weightMatrix.isSquare()))
      {
         InvalidSolver e("MeasUpdate(): Weight matrix is not square");
         GPSTK_THROW(e);
      }


      int wRow = static_cast<int>(weightMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(wRow==pRow))
      {
         InvalidSolver e("MeasUpdate(): prefitResiduals size does not match dimension of \
weightMatrix");
         GPSTK_THROW(e);
      }


      int gRow = static_cast<int>(designMatrix.rows());
      if (!(gRow==pRow))
      {
         InvalidSolver e("MeasUpdate(): prefitResiduals size does not match \
dimension of designMatrix");
         GPSTK_THROW(e);
      }


      int gCol = static_cast<int>(designMatrix.cols());
      int aprioriStateRow = static_cast<int>(xhatminus.size());
      if (!(gCol==aprioriStateRow))
      {
         InvalidSolver e("MeasUpdate(): designMatrix size and a priori state \
estimation vector do not match.");
         GPSTK_THROW(e);
      }


      int pMCol = static_cast<int>(Pminus.cols());
      if ( pMCol != aprioriStateRow )
      {
         InvalidSolver e("MeasUpdate(): Sizes of a priori error covariance \
matrix and a priori state estimation vector do not match.");
         GPSTK_THROW(e);
      }


         // After checking sizes, let's do the real correction work
      Matrix<double> invPMinus;
      Matrix<double> designMatrixT( transpose(designMatrix) );

      try
      {

         invPMinus = inverseChol(Pminus);

      }
      catch(...)
      {
         InvalidSolver e("MeasUpdate(): Unable to compute invPMinus matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {

         Matrix<double> invTemp( designMatrixT * weightMatrix * designMatrix +
                                 invPMinus );

            // Compute the a posteriori error covariance matrix
         P = inverseChol( invTemp );

      }
      catch(...)
      {
         InvalidSolver e("MeasUpdate(): Unable to compute P matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {

            // Compute the a posteriori state estimation
         xhat = P * ( (designMatrixT * weightMatrix * prefitResiduals) + 
                      (invPMinus * xhatminus) );

      }
      catch(Exception e)
      {
         InvalidSolver eis("MeasUpdate(): Unable to compute xhat.");
         GPSTK_THROW(eis);
         return -1;
      }

      xhatminus = xhat;
      Pminus = P;

      solution = xhat;
      covMatrix = P;

         // Compute the postfit residuals Vector
      postfitResiduals = prefitResiduals - (designMatrix * solution);

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverUpdWL::MeasUpdate()'



      // Recover the state and covariance map after get the 
      // solution and covMatrix
      //
      // @param gData    Data object holding the data.
      //
   gnssDataMap& SolverUpdWL::postCompute( gnssDataMap& gdsMap )
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

            // Get the set with unknowns being processed
         VariableSet unkSet( equSystem.getVarUnknowns() );

            // Store values of current state

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
         VariableSet tempSet( unkSet );

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


            // Store the postfit residuals in the GNSS Data Structure

            // We need the list of equations being processed
         std::list<Equation> equList( equSystem.getCurrentEquationsList() );

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
            else
            {
               if( residualType == TypeID::prefitL )
               {
                  residualType = TypeID::postfitL;
               }
            }

            gdsMap.insertValue( (*itEq).header.equationSource,
                                (*itEq).header.equationSat,
                                residualType,
                                postfitResiduals(i) );

            ++i;  // Increment counter

         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

            /**
             *  Now, get the ambigity number and fixing rate for 
             *  each satellite.
             */
           
            // Firstly, clear the data to store the 
            // information for current epoch
         fixingDataMap.clear();

            // Number for float/fixed ambiguities
         int numBWL(0);
         int numNWL(0);
        
            // Get the float ambiguity number
         for( VariableSet::const_iterator itVar = unkSet.begin();
              itVar != unkSet.end();
              ++itVar )
         {
             if( (*itVar).getType() == (TypeID::BWL) )
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
               // Get number of float/fixed widelane ambiguities
            double numBWL = fixingDataMap[(*itSat)].floatAmbNumb;
            double numNWL = fixingDataMap[(*itSat)].fixedAmbNumb;


               // Compute the fixing rate for each satellite
            fixingDataMap[(*itSat)].fixingRate = numNWL/numBWL;
         }

            /**
             * Let's correct the BLC value with the fixed widelane ambiguity
             * according to the formula.
             */

            // Iterate the fixed ambiguity
         for( VariableDataMap::iterator itAmb = ambFixedMap.begin();
              itAmb != ambFixedMap.end();
              ++ itAmb)
         {
               // Source for this variable in 'ambFixedMap'
            SourceID source( (*itAmb).first.getSource() );

               // Satellite for this variable in 'ambFixedMap'
            SatID satellite( (*itAmb).first.getSatellite() );

               // The fixed widelane ambiguity
            double NWL( (*itAmb).second );

               // Get the float BLC ambiguity
            double BLC = gdsMap.getValue( source, satellite, TypeID::BLC );

               // Compute float ambiguities on LC
            BLC = BLC  + 0.377482511 * NWL;

               // Only Insert widelane ambigutity which are fixed. 
            gdsMap.insertValue( source, satellite, TypeID::BWL, NWL);

               // Only insert narrowlane ambiguity with widelane ambiguity fixed.
            gdsMap.insertValue( source, satellite, TypeID::BLC, BLC);


         }  // End of 'for(VariableDataMap::iterator itAmb = ...'


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

                  // Satellite set with widelane ambiguity unfixed
               SatIDSet deleteSats;
                
                  // Finally, iterate through corresponding 'satTypeValueMap'
               for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                    stvmIter != (*sdmIter).second.end();
                    stvmIter++ )
               {
                      // Look for TypeID::BLC
                  typeValueMap::const_iterator itObs = 
                                     (*stvmIter).second.find(TypeID::BWL);

                     // If not found 
                  if( itObs == (*stvmIter).second.end() )
                  {
                     deleteSats.insert( (*stvmIter).first );
                  }

                  Variable tempVar;

                     // Set SourceID
                  tempVar.setSource( (*sdmIter).first );
                  tempVar.setSatellite( (*stvmIter).first );


               }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

                  // Remove satellites, the widelane ambiguities of which 
                  // are not fixed
               gRin.removeSatID( deleteSats);
        
                  // Add the gRin into 'dataMap', in which the widelane 
                  // ambiguities are all fixed
               dataMap.addGnssRinex(gRin);


            }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

         }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

            // Now copy dataMap to gdsMap
         gdsMap = dataMap;

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

   }  // End of method 'SolverUpdWL::postMeasUpdate()'


      /* Returns the solution associated to a given TypeID and SourceID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverUpdWL::getSolution( const TypeID& type,
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

   }  // End of method 'SolverUpdWL::getSolution()'


      /* Returns the solution associated to a given TypeID, SourceID and
       *  SatID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverUpdWL::getSolution( const TypeID& type,
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

//    if( fixingDataMap[(sat)].fixingSatNumb < 4 || 
//        fixingDataMap[(sat)].fixingRate < 0.75 )
//    {
//        InvalidRequest e("fixing rate is too low");
//        GPSTK_THROW(e);
//    }
      

      // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverUpdWL::getSolution()'



}  // End of namespace gpstk

#pragma ident "$Id: MeasUpdate.cpp 2877 2011-08-20 08:40:11Z yanweignss $"

/**
 * @file MeasUpdate.hpp
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


#include "MeasUpdate.hpp"
#include "SystemTime.hpp"
#include "Counter.hpp"


#ifdef USE_OPENMP
#include <omp.h>
#endif

namespace gpstk
{

      // Index initially assigned to this class
   int MeasUpdate::classIndex = 9600000;


      // Returns an index identifying this object.
   int MeasUpdate::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string MeasUpdate::getClassName() const
   { return "MeasUpdate"; }


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& MeasUpdate::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'MeasUpdate::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& MeasUpdate::Process(gnssRinex& gData)
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

   }  // End of method 'MeasUpdate::Process()'



      /* Returns a reference to a gnssDataMap object after solving
       *  the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& MeasUpdate::Process( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      int times = 0;

      try
      {

      do{
	
            ////////////////////////////////////////////
            //                    
            // Measurement update using single observables each time.
            //
            ////////////////////////////////////////////
         //Counter::begin();	
	 

         // Prepare the equation system with current data
         equSystem.Prepare( gdsMap );

         int numUnknowns( equSystem.getCurrentNumVariables() );
	 
	 std::cout<<"num="<<numUnknowns<<std::endl;

            // Get the set with unknowns being processed
         VariableSet currentUnknowns( equSystem.getCurrentUnknowns() );
        
         std::list<Equation> equList;
         equList = equSystem.getCurrentEquationsList();
	 
	 int numEqu = equList.size();

         Vector<double> prefitResiduals(numEqu);
         Matrix<double> hMatrix( numEqu, numUnknowns, 0.0 );
	
         // declare here for memory reuse
         Vector<double> M( numUnknowns, 0.0 );
         Vector<double> K( numUnknowns, 0.0 );

	 times++;

	 xhat = m_pStateStore->getStateVector( currentUnknowns );
	 P = m_pStateStore->getCovarMatrix( currentUnknowns );

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

               hMatrix( row, var.getNowIndex() ) = tempCoef;
               index(i) = var.getNowIndex();
               G(i) = tempCoef;

            }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'
               // Now, Let's create the index for current float unks


               // Temp measurement
            double z(tempPrefit);

               // Inverse weight
            double inv_W(1.0/weight);  

            
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
#ifdef _OPENMP            
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
	

         //cout << "cputtime measupdate:" << Counter::end() << endl;
         //Counter::begin();

            // Compute the postfit residuals Vector
         postfitResiduals = prefitResiduals - (hMatrix* xhat);

         //cout << "cputtime postfitResiduals:" << Counter::end() << endl;
         

	} while( !postfitFilter( gdsMap ) && times <= 2); 

         //////////// //////////// //////////// ////////////
         //
         //  postCompute, store the xhat and P into stateMap
         //  and covarianceMap for the next epoch.
         //
         //////////// //////////// //////////// ////////////

	  m_pStateStore->setStateVector( xhat );
	  m_pStateStore->setCovarMatrix( P );
	  m_pStateStore->setVariableSet( equSystem.getCurrentUnknowns() );	

	 /// holding the solution and covariance matrix
         //  in StateStore
         //m_pStateStore->setStateVector( xhat );
         //m_pStateStore->setCovarMatrix( P );
	

            // Store the postfit residuals in the GNSS Data Structure
         //int i = 0;         // Reset 'i' index
            
	 /*std::list<Equation> equList = equSystem.getCurrentEquationsList();
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

	*/	
      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );
	 std::cout<<"exception:"<<e.what()<<std::endl;
         GPSTK_THROW(e);

      }

      return gdsMap;

   }  // End of method 'MeasUpdate::postCompute()'

  
  bool MeasUpdate::postfitFilter(gnssDataMap& gData) 
 {
    // get equation list
    std::list<Equation> equList = equSystem.getCurrentEquationsList();

    double sigma = 0.0;
    bool isValid = true;
	
    int i = 0;
    for( std::list<Equation>::iterator itEq = equList.begin();
	 itEq != equList.end(); itEq++ )
    {
	TypeID residualType( (*itEq).header.indTerm.getType() );
	double weight = (*itEq).header.constWeight;
	typeValueMap tvm( (*itEq).header.typeValueData );
	
	if( tvm.end() != tvm.find(TypeID::weight) )
	{
	   weight = weight * tvm(TypeID::weight);
	}

	sigma += std::pow( postfitResiduals(i), 2 ) * weight;

        std::cout<<"sat:"<<(*itEq).header.equationSat
    	     	 <<",source:"<<(*itEq).header.equationSource
    	         <<",type:"<<(*itEq).header.indTerm.getType()
                 <<",v:"<< std::fabs( postfitResiduals(i) ) * std::sqrt(weight)
    	         <<",postfit="<<std::fabs( postfitResiduals(i) )<<std::endl;
	i++;
    }
    
    int n =  equList.size();
    int t =  equSystem.getCurrentNumVariables();
    sigma = std::sqrt( sigma/(n-t) );
    std::cout<<"sigma:"<<sigma<<std::endl;

    mSigma = sigma;

    if( sigma <= mMaxSigma ) return true;

	
    i = 0;
    for( std::list<Equation>::iterator itEq = equList.begin();
	   itEq != equList.end(); itEq++ )
    {
	TypeID residualType( (*itEq).header.indTerm.getType() );
	            
	double weight = (*itEq).header.constWeight;
	typeValueMap tvm( (*itEq).header.typeValueData );
	
	if( tvm.end() != tvm.find(TypeID::weight) )
	{
	   weight = weight * tvm(TypeID::weight);
	}
	
	double v = std::sqrt(weight) * std::fabs( postfitResiduals(i) );

	if( v  >= 3*sigma )
	{
	   isValid = false;
	   gData.removeSatID( (*itEq).header.equationSource,
		              (*itEq).header.equationSat );

           std::cout<<"sat:"<<(*itEq).header.equationSat
    	     	    <<",source:"<<(*itEq).header.equationSource
    	            <<",type:"<<(*itEq).header.indTerm.getType()
	            <<",v:"<<v
    	            <<",postfit="<<std::fabs( postfitResiduals(i) )<<std::endl;
	 }

	i++;
     }
	 

    return isValid;	
 }


}  // End of namespace gpstk

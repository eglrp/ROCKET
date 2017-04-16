#pragma ident "$Id: TimeUpdate.cpp 2877 2011-08-20 08:40:11Z yanweignss $"

/**
 * @file TimeUpdate.hpp
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


#include "TimeUpdate.hpp"
#include "SystemTime.hpp"
#include "Counter.hpp"


#ifdef USE_OPENMP
#include <omp.h>
#endif

namespace gpstk
{

      // Index initially assigned to this class
   int TimeUpdate::classIndex = 9600000;


      // Returns an index identifying this object.
   int TimeUpdate::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string TimeUpdate::getClassName() const
   { return "TimeUpdate"; }


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& TimeUpdate::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'TimeUpdate::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& TimeUpdate::Process(gnssRinex& gData)
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

   }  // End of method 'TimeUpdate::Process()'



      /* Returns a reference to a gnssDataMap object after solving
       *  the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& TimeUpdate::Process( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {  
         // time counter begin
         Counter::begin();
         
         // covariance matrix from stateStore.
         Matrix<double> covarMatrix( m_pStateStore->getCovarMatrix() );
         
         // state vector from stateStore.
         Vector<double> stateVec( m_pStateStore->getStateVector() );

         // Prepare the equation system with current data
         equSystem.Prepare(gdsMap);

         // current unknowns
         VariableSet currentUnknowns( equSystem.getCurrentUnknowns() );
         
         cout << "cputime equSystem:" << Counter::end() << endl;

         // the number of unknowns being processed
         int numUnknowns( currentUnknowns.size() );

         cout << "numUnknowns=" << numUnknowns << endl;
         
         Counter::begin();
         
         // holding relative Variable in order
         std::vector<Variable> relVarVec;

         // holding the ready index for P * phi
         std::vector<int> readyIndexVec;

         // copy current unknowns
         VariableSet tempUnknowns( currentUnknowns );

         // P1 Matrix for holding phi * P
         Matrix<double> P1( numUnknowns, numUnknowns, 0.0 );  

         // resize the xhatminus vector
         xhatminus.resize( numUnknowns, 0.0 );
         
         // resize the Pminus Matrix.
         Pminus.resize( numUnknowns, numUnknowns, 0.0 );

         // get variable iterator.
         VariableSet::iterator varIter = tempUnknowns.begin();
         
         while( tempUnknowns.end() != varIter )
         {

            // clear vector container for relative variable
            relVarVec.clear();
            
            // copy current variable
            Variable var( *varIter );

            // get relative TypeIDS in order
            std::vector<TypeID> relTypeIDVec( var.getModel()->getRelTypeIDVec() );
            
            // the size of relative TypeID Vector.
            int relSize = relTypeIDVec.size();

            // get all relative variable and insert to container 'relVarVec'
            for( VariableSet::iterator it = tempUnknowns.begin();
                  it != tempUnknowns.end(); it++ )
            {
                  
               // if current variable is satellite-indexed,
               // we need to guarantee the satellite consistent.
               if( var.getSatIndexed() )
               {
                  if( var.getSatellite() != 
                        (*it).getSatellite() 
                     )
                  {
                     continue;
                  }
               }

               // if current variable is source-indexed,
               // we need to guarantee the source consistent.
               if( var.getSourceIndexed() )
               {
                  if( var.getSource() !=
                        (*it).getSource()
                    )
                  {
                     continue;
                  }
               }
               
               // judge whether TypeID is included in relative TypeID vector 
               // or not, if it exists, add variable to 'relVarVec'
               if( relTypeIDVec.end() != std::find( relTypeIDVec.begin(), relTypeIDVec.end(),  
                        (*it).getType()  ) )
               {
                  Variable relVar( *it );
                  relVarVec.push_back( relVar );
               }
               
               // judge the size of relative variable vector and relative
               // TypeID vector, if it equals, it shows all of variables are
               // found and breaking routine for saving time.
               if( relVarVec.size() == relTypeIDVec.size() ) break;

            } // End of ' for( VariableSet::iterator it = ... ) '
            
            // prepare stochastic model for current variable and relative
            // variables
            var.getModel()->Prepare( relVarVec, gdsMap );
            
            // get phi matrix
            Matrix<double> phiMatrix = var.getModel()->getPhi();
           
            // get q Matrix
            Matrix<double> qMatrix = var.getModel()->getQ();

            //// update xhatminus routine 
            // holding the value for relative variable 
            // in 'relTypeIDVec' order.
            Vector<double> valVec( relSize, 0.0 );

            // holding the now index for relative variable
            // in 'relTypeIDVec' order.
            Vector<int> indexNowVec( relSize, -1 );

            // holding the pre index for relative variable
            // in 'relTypeIDVec' order
            Vector<int> indexPreVec( relSize, -1 );

            for( std::vector<Variable>::iterator it = relVarVec.begin();
                 it != relVarVec.end(); it++ )
            {
               TypeID type( (*it).getType() );
               int preIndex = (*it).getPreIndex();
               int nowIndex = (*it).getNowIndex();

               // add to ready index vector for P * phi
               readyIndexVec.push_back( (*it).getNowIndex() );

               for( int i=0; i< relSize; i++ )
               {
                  if( relTypeIDVec[i] == type )
                  {

                     indexNowVec(i) = nowIndex;
                     indexPreVec(i) = preIndex;

                     // if variable is new, the preIndex will be set -1
                     // and the value will be set 0.0 or get from the
                     // 'stateVec'
                     if( -1 != preIndex )
                     {
                        valVec(i) = stateVec(preIndex);
                     }
                     else
                     {
                        valVec(i) = 0.0;
                     }
                     
                     break;
                  }
               }

            } // End of 'for( std::vector<Variable>::iterator varIter = ...'
            
            for( int i=0; i<relSize; i++ )
            {
               double xVal = 0.0;

               for( int j=0; j<relSize; j++ )
               { xVal += phiMatrix(i,j) * valVec(j); }

               xhatminus( indexNowVec(i) ) = xVal ;
            }
            
            //// update Pminus routine 

            // update Phi * P
            for( VariableSet::iterator it = currentUnknowns.begin();
                 it != currentUnknowns.end(); it++ )
            {
               for( int i=0; i<relSize; i++ )
               {
                  double pVal = 0.0;

                  for( int j=0; j<relSize; j++)
                  { 
                     double covar = 0.0;
                     
                     // if two variables are not new, we get covariance 
                     // from 'covarMatrix'
                     if( -1 != (*it).getPreIndex() &&
                         -1 != indexPreVec(j) )
                     {
                        covar = covarMatrix( indexPreVec(j), (*it).getPreIndex() );
                     }
                     else
                     {
                        // if now index equals, it shows two variable
                        // the same and covariance set to initialVariance,
                        // otherwise, it will be 0.0.
                        if( (*it).getNowIndex() == indexNowVec(j) )
                        {
                           covar = (*it).getInitialVariance();
                        }
                     }
                     
                     pVal += phiMatrix(i,j) * covar;
                  }
                  
                  P1( indexNowVec(i), (*it).getNowIndex() ) = pVal; 
               }
            }
            
            /// for P * transport(Phi)
            int readyIndexSize = readyIndexVec.size();
            for( int i=0; i<readyIndexSize; i++ )
            {
               int readyIndex = readyIndexVec[i];

               for( int j=0; j<relSize; j++ )
               {
                  double pVal = 0.0;
                  
                  for( int k=0; k<relSize; k++)
                  {
                     int relIndex = indexNowVec(k);
                     pVal += P1( readyIndex, relIndex) * phiMatrix(j,k); 
                  }

                  Pminus( readyIndex, indexNowVec(j) ) =
                     Pminus( indexNowVec(j), readyIndex ) = pVal;
               }

            }

            // add q matrix to Pminus
            for( int i=0; i<relSize; i++ )
            {
               for( int j=i; j<relSize; j++ )
               {
                  Pminus( indexNowVec(i), indexNowVec(j) ) 
                     += qMatrix( i, j );
                  Pminus( indexNowVec(j), indexNowVec(i) )
                     += qMatrix( i, j);
               }
            }

            // remove relative variables in tempUnknowns
            for( std::vector<Variable>::iterator it = relVarVec.begin();
                 it != relVarVec.end(); it++ )
            {
               tempUnknowns.erase( *it );
            }

            // reset varIter
            varIter = tempUnknowns.begin();
         }
         
         cout << "cputtime TimeUpdate" << Counter::end() << endl;
         
         m_pStateStore->setStateVector( xhatminus );
         m_pStateStore->setCovarMatrix( Pminus );
	 m_pStateStore->setVariableSet(currentUnknowns);

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

   }  // End of method 'TimeUpdate::Process()'


}  // End of namespace gpstk

#pragma ident "$Id$"

/**
 * @file IndepAmbiguityDatum.hpp
 * Class to generate independent ambiguity map given 'gData'
 */

#ifndef GPSTK_INDEPAMBIGUITYDATUM_HPP
#define GPSTK_INDEPAMBIGUITYDATUM_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009, 2011
//
//============================================================================
//
//  History 
//
//  2012.06.19   Modify of the Method "getPhiQ()". The gnssRinex is needed to 
//               'Prepare' the stochastic model. When the variable is 
//               source-indexed, the gnssRinex can be given by finding the source 
//               in gdsMap, but when the variable is only satellite-indexed, 
//               the gnssRinex is the first gnssRinex which "SEE" this 
//               satellite. ( by shjzhang )
//
//  - ....
//
//============================================================================


#include <algorithm>

#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Equation.hpp"

#include "Arc.hpp"
#include "Edge.hpp"
#include "Kruskal.hpp"
#include "ARRound.hpp"

namespace gpstk
{

      /// Thrown when attempting to use an invalid IndepAmbiguityDatum
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidIndepAmbiguityDatum, gpstk::Exception);


      /** @addtogroup DataStructures */
      //@{


      /** This class defines and handles complex equation systems for solvers,
       *  generating the Vectors and Matrices they will need, including:
       *
       *    \li Measurements Vector (prefit residuals).
       *    \li Geometry matrix.
       *    \li Equation weights matrix.
       *    \li State Transition Matrix (PhiMatrix).
       *    \li Process noise covariance matrix (QMatrix).
       *
       * In addition, this class will provide important/interesting information
       * such as number of variables, satellites and data sources being
       * processed.
       *
       * In order to achieve this, the 'IndepAmbiguityDatum' class will start from a
       * list of 'Equation descriptions' and the currently available GNSS data.
       * From there, it will deduce the unknowns corresponding to each data
       * source as well as the specific equations.
       *
       * A typical way to use this class follows, showing how to set up an
       * object to perform "Precise Point Positioning" (PPP):
       *
       * @code
       *
       *      // DEFINE VARIABLES
       *
       *      // Declare stochastic models to be used
       *   StochasticModel coordinatesModel;
       *   TropoRandomWalkModel tropoModel;
       *   PhaseAmbiguityModel ambiModel;
       *
       *      // These variables are, by default, SourceID-indexed
       *   Variable dx( TypeID::dx, &coordinatesModel, true, false, 100.0 );
       *
       *   Variable dy( TypeID::dy, &coordinatesModel );
       *   dy.setInitialVariance( 100.0 );     // Equivalent to 'dx' setup
       *
       *   Variable dz( TypeID::dz, &coordinatesModel );
       *   dz.setInitialVariance( 100.0 );
       *
       *   Variable cdt( TypeID::cdt );
       *   cdt.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *   Variable tropo( TypeID::wetMap, &tropoModel );
       *   tropo.setInitialVariance( 25.0 );
       *
       *
       *      // The following variable is, SourceID and SatID-indexed
       *   Variable ambi( TypeID::BLC, &ambiModel, true, true );
       *   ambi.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *      // This will be the independent term for code equation
       *   Variable prefitC( TypeID::prefitC );
       *
       *      // This will be the independent term for phase equation
       *   Variable prefitL( TypeID::prefitL );
       *
       *
       *      // DESCRIBE EQUATIONS
       *
       *      // Define Equation object for code equations, and add variables
       *   Equation equPC( prefitC );
       *
       *   equPC.addVariable(dx);
       *   equPC.addVariable(dy);
       *   equPC.addVariable(dz);
       *   equPC.addVariable(cdt);
       *   equPC.addVariable(tropo);
       *
       *      // Define Equation object for phase equations, and add variables
       *   Equation equLC( prefitL );
       *
       *   equLC.addVariable(dx);
       *   equLC.addVariable(dy);
       *   equLC.addVariable(dz);
       *   equLC.addVariable(cdt);
       *   equLC.addVariable(tropo);
       *   equLC.addVariable(ambi);       // This variable is for phase only
       *
       *      // Phase equations should have higher relative weight
       *   equLC.setWeight(10000.0);     // 100.0 * 100.0
       *
       *
       *      // SETUP EQUATION SYSTEM
       *
       *      // Create 'IndepAmbiguityDatum' object
       *   IndepAmbiguityDatum eqSystem;
       *
       *      // Add equation descriptions
       *   eqSystem.addEquation(equPC);
       *   eqSystem.addEquation(equLC);
       *
       * @endcode
       *
       * In this way, rather complex processing strategies may be set up in a
       * handy and flexible way.
       *
       * \warning Please be aware that this class requires a significant amount
       * of overhead. Therefore, if your priority is execution speed you should
       * either use the already provided 'purpose-specific' solvers (like
       * 'SolverPPP' or hand-code your own class. For new processing strategies
       * you should balance the importance of machine time (extra overhead)
       * versus researcher time (writing a new solver).
       *
       * @sa Variable.hpp, Equation.hpp.
       *
       */
   class IndepAmbiguityDatum
   {
   public:

         /// Default constructor
      IndepAmbiguityDatum()
         : isPrepared(false)
      {};


         /** Add a new ambiguity variable to be managed.
          *
          * @param equation   Equation object to be added.
          */
      virtual IndepAmbiguityDatum& setAmbType( const Variable& type);


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual IndepAmbiguityDatum& Prepare( gnssSatTypeValue& gData )
      { gnssRinex gds(gData); return ( Prepare(gds) ); };


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual IndepAmbiguityDatum& Prepare( gnssRinex& gData );


         /** Prepare this object to carry out its work.
          *
          * @param gdsMap     Map of GNSS data structures (GDS), indexed
          *                   by SourceID.
          *
          */
      virtual IndepAmbiguityDatum& Prepare( gnssDataMap& gdsMap );



         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriStateMap  Apriori state map.
          */
      virtual void setStateMap( VariableDataMap& apriStateMap )
         throw(InvalidIndepAmbiguityDatum)
      {
           // Set the apriori state
         ambMap = apriStateMap;

           // You must clear the 'IndepAmbMap' firstly, or, it will 
           // keep all the fixed ambiguities from the last epoch, which may 
           // not be fixed at current epoch.
         IndepAmbMap.clear();
      };


         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriCovMap  Apriori covariance map.
          */
      virtual void setCovarianceMap( 
                   std::map<Variable,VariableDataMap> apriCovMap )
         throw(InvalidIndepAmbiguityDatum)
      {
           // Set the apriori covariance
         ambVarMap = apriCovMap;
      };


         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriStateMap  Apriori state map.
          */
      virtual void Reset( VariableDataMap& apriStateMap,
                          std::map<Variable,VariableDataMap>& apriCovMap )
         throw(InvalidIndepAmbiguityDatum)
      {
         ambMap = apriStateMap;
         ambVarMap = apriCovMap;
      };

         /** Get the ambiguity datum which are fixed directly.
          */
      virtual VariableDataMap getIndepAmbMap( void )
         throw(InvalidIndepAmbiguityDatum);


         /// Destructor
      virtual ~IndepAmbiguityDatum() {};


   private:


         /// List containing the ambiguity variables.
      Variable ambType;


         /// Map holding the a priori unknowns values 
      VariableDataMap ambMap;
         

         /// Map holding covariance information
      std::map<Variable, VariableDataMap > ambVarMap;


         /// Map holding the a priori unknowns values 
      VariableDataMap IndepAmbMap;


         /// Global set of edges 
      EdgeSet oldIndepEdges;


         /// Current set of edges 
      EdgeSet currentIndepEdges;


         /// Global set of unknowns
      EdgeSet currentEdges;


         /// Whether or not this IndepAmbiguityDatumNet is ready to be used
      bool isPrepared;


         /// Current epoch
      CommonTime currentEpoch;


         /// Set containing all sources being currently processed
      SourceIDSet currentSourceSet;


         /// Set containing satellites being currently processed
      SatIDSet currentSatSet;


         /// Get current sources (SourceID's) and satellites (SatID's)
      void prepareCurrentSourceSat( gnssDataMap& gdsMap );


         /// Prepare set of current edges for all sources and satellites
      EdgeSet prepareCurrentEdgesAndAmbs( gnssDataMap& gdsMap );

          
         /// Prepare set of current edges for all sources and satellites
      EdgeSet prepareCurrentEdges( gnssDataMap& gdsMap );


         /// Prepare set of current independent edges for all sources and satellites
      EdgeSet prepareCurrentIndepEdgesAndAmbs();


         /// Prepare set of current inependent edges 
      EdgeSet prepareCurrentIndepEdges();


         /// Prepare set of current unknowns and list of current equations
      VariableSet prepareCurrentAmbs( gnssDataMap& gdsMap );


         /// Prepare set of current unknowns and list of current equations
      VariableSet prepareCurrentIndepAmbs( EdgeSet& currentIndepEdgeSet );


   }; // End of class 'IndepAmbiguityDatum'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_INDEPAMBIGUITYDATUM_HPP

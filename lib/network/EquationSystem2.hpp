#pragma ident "$Id$"

/**
 * @file EquationSystem2.hpp
 * Class to define and handle complex equation systems for solvers.
 */

#ifndef GPSTK_EQUATIONSYSTEM2_HPP
#define GPSTK_EQUATIONSYSTEM2_HPP

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
//  shjzhang - Wuhan University. 2011, 2015
//
//============================================================================
//
//  Revisions
//
//  2012/06/19   
//
//  Modify of the Method "getPhiQ()". The gnssRinex is needed to 
//  'Prepare' the stochastic model. When the variable is 
//  source-indexed, the gnssRinex can be given by finding the source 
//  in gdsMap, but when the variable is only satellite-indexed, 
//  the gnssRinex is the first gnssRinex which "SEE" this 
//  satellite. ( by shjzhang )
//
//  2015/06/19   
//  Remove all the code related to gdsMap.getValue(), which is 
//  very time-consuming.
//
//  2015/07/15   
//  Design a new class to prepare unknowns and equations for
//  solvers.
//
//============================================================================


#include <algorithm>

#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Equation.hpp"


namespace gpstk
{

      /// Thrown when attempting to use an invalid EquationSystem2
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidEquationSystem2, gpstk::Exception);


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
       * In order to achieve this, the 'EquationSystem2' class will start from a
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
       *      // Create 'EquationSystem2' object
       *   EquationSystem2 eqSystem;
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
   class EquationSystem2
   {
   public:

         /// Default constructor
      EquationSystem2()
         : isPrepared(false)
      {};


         /** Add a new equation to be managed.
          *
          * @param equation   Equation object to be added.
          */
      virtual EquationSystem2& addEquation( const Equation& equation );


         /** Remove an Equation being managed. In this case the equation is
          *  identified by its independent term.
          *
          * @param indterm  Variable object of the equation independent term
          *                 (measurement type).
          *
          * \warning All Equations with the same independent term will be
          *          erased.
          */
      virtual EquationSystem2& removeEquation( const Variable& indterm );


         /// Remove all Equation objects from this EquationSystem2.
      virtual EquationSystem2& clearEquations();


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual EquationSystem2& Prepare( gnssSatTypeValue& gData )
      { gnssRinex gds(gData); return ( Prepare(gds) ); };


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual EquationSystem2& Prepare( gnssRinex& gData );


         /** Prepare this object to carry out its work.
          *
          * @param gdsMap     Map of GNSS data structures (GDS), indexed
          *                   by SourceID.
          *
          */
      virtual EquationSystem2& Prepare( gnssDataMap& gdsMap );


         /** Return the CURRENT number of variables, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual int getCurrentNumVariables() const
         throw(InvalidEquationSystem2);


         /** Return the set containing variables being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual VariableSet getCurrentUnknowns() const
         throw(InvalidEquationSystem2);


         /** Return the CURRENT number of sources, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual int getCurrentNumSources() const
         throw(InvalidEquationSystem2);


         /** Return the set containing sources being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual SourceIDSet getCurrentSources() const
         throw(InvalidEquationSystem2);


         /** Return the CURRENT number of satellites, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual int getCurrentNumSats() const
         throw(InvalidEquationSystem2);


         /** Return the set containing satellites being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual SatIDSet getCurrentSats() const
         throw(InvalidEquationSystem2);


         /// Get the number of equation descriptions being currently processed.
      virtual int getEquationDefinitionNumber() const
      { return equDescripList.size(); };


         /// Get the list of current equations.
      virtual std::list<Equation> getCurrentEquationsList() const
      { return currentEquationsList; };


         /// Destructor
      virtual ~EquationSystem2() {};


   private:

         /// List containing the DESCRIPTIONS of Equation objects.
      std::list<Equation> equDescripList;

         /// List of current equations
      std::list<Equation> currentEquationsList;

         /// Current set of unknowns
      VariableSet currentUnknowns;

         /// List of all unknowns(std::list is used to keep order)
      std::list<Variable> allUnknowns;

         /// Set of reject unknowns
      VariableSet rejectUnknowns;

         /// Whether or not this EquationSystem2 is ready to be used
      bool isPrepared;

         /// Set containing all sources being currently processed
      SourceIDSet currentSourceSet;

         /// Set containing satellites being currently processed
      SatIDSet currentSatSet;

         /// General white noise stochastic model
      static WhiteNoiseModel whiteNoiseModel;

         /// Prepare set of current unknowns and list of current equations
      VariableSet prepareUnknownsAndEquations( gnssDataMap& gdsMap );

         /// Get current sources (SourceID's) and satellites (SatID's)
      void prepareCurrentSourceSat( gnssDataMap& gdsMap );


   }; // End of class 'EquationSystem2'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_EQUATIONSYSTEM2_HPP

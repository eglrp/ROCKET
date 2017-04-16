#pragma ident "$Id$"

/**
 * @file EquationSystemEx.hpp
 * Class to define and handle complex equation systems for solvers.
 */

#ifndef GPSTK_EQUATIONSYSTEMEX_HPP
#define GPSTK_EQUATIONSYSTEMEX_HPP

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
#include "StateStore.hpp"


namespace gpstk
{

      /// Thrown when attempting to use an invalid EquationSystemEx
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidEquationSystemEx, gpstk::Exception);


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
       * In order to achieve this, the 'EquationSystemEx' class will start from a
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
       *      // Create 'EquationSystemEx' object
       *   EquationSystemEx eqSystem;
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
   class EquationSystemEx
   {
   public:

         /// Default constructor
      EquationSystemEx()
         : m_IsPrepared(false)
      {};
      

         /** Add a new equation to SourceID relatived
          *
          * @param equation  the Equation object to be added.
          * @param source    the SourceID to be relatived.
          */
      virtual EquationSystemEx& addEquation2Source( const Equation& equation, 
                                                    const SourceID& source );


         /// Remove all Equation objects from this EquationSystemEx.
      virtual EquationSystemEx& clearEquations();


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual EquationSystemEx& Prepare( gnssSatTypeValue& gData )
      { gnssRinex gds(gData); return ( Prepare(gds) ); };


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual EquationSystemEx& Prepare( gnssRinex& gData );


         /** Prepare this object to carry out its work.
          *
          * @param gdsMap     Map of GNSS data structures (GDS), indexed
          *                   by SourceID.
          *
          */
      virtual EquationSystemEx& Prepare( gnssDataMap& gdsMap );


         /** Return the CURRENT number of variables, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystemEx exception.
          */
      virtual int getCurrentNumVariables() const
         throw(InvalidEquationSystemEx);


         /** Return the set containing variables being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystemEx exception.
          */
      virtual VariableSet getCurrentUnknowns() const
         throw(InvalidEquationSystemEx);


         /** Return the CURRENT number of sources, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystemEx exception.
          */
      virtual int getCurrentNumSources() const
         throw(InvalidEquationSystemEx);


         /** Return the set containing sources being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystemEx exception.
          */
      virtual SourceIDSet getCurrentSources() const
         throw(InvalidEquationSystemEx);


         /** Return the CURRENT number of satellites, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystemEx exception.
          */
      virtual int getCurrentNumSats() const
         throw(InvalidEquationSystemEx);


         /** Return the set containing satellites being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystemEx exception.
          */
      virtual SatIDSet getCurrentSats() const
         throw(InvalidEquationSystemEx);

         /// Get the list of current equations.
      virtual std::list<Equation> getCurrentEquationsList() const
      { return m_CurrentEquationsList; };

         /// Setup Equation Index
      void setUpEquationIndex(VariableSet& oldVariableSet);
         
         // set state store reference object
      EquationSystemEx& setStateStore( StateStore& stateStore )
      { m_pStateStore = &stateStore; return (*this); }
         
         // get state store reference object.
      StateStore& getStateStore()
      { return *m_pStateStore; }

         /// Destructor
      virtual ~EquationSystemEx() {};


   private:

      typedef std::list<Equation>  EquationList;

      // current equation list
      EquationList  m_CurrentEquationsList;

         /// Current set of unknowns
      VariableSet m_CurrentUnknowns;
      
      // store the equations relative to Source.
      std::map<SourceID, EquationList> m_SourceEquMap;   

         /// Whether or not this EquationSystemEx is ready to be used
      bool m_IsPrepared;

         /// Set containing all sources being currently processed
      SourceIDSet m_CurrentSourceSet;

         /// Set containing satellites being currently processed
      SatIDSet m_CurrentSatSet;

      // reference object of StateStore
      StateStore* m_pStateStore;

         /// General white noise stochastic model
      static WhiteNoiseModel whiteNoiseModel;

         /// Prepare set of current unknowns and list of current equations
      VariableSet prepareUnknownsAndEquations( gnssDataMap& gdsMap );

         /// Get current sources (SourceID's) and satellites (SatID's)
      void prepareCurrentSourceSat( gnssDataMap& gdsMap );


   }; // End of class 'EquationSystemEx'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_EQUATIONSYSTEMEX_HPP

#pragma ident "$Id: MeasUpdate.hpp $"

/**
 * @file MeasUpdate.hpp
 * Extended Kalman filter process of MeasUpdate.
 */

#ifndef GPSTK_MEASUPDATE_HPP
#define GPSTK_MEASUPDATE_HPP

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
//  shjzhang, wuhan university (2015)
//
//============================================================================
//
//  Revision
//
//  2014/02/20      Copy to the new version of 'gpstk', by shjzhang.
//  2014/03/16      add two member function:
//                  'getCurrentSources()' and 'getCurrentSats()'.
//                  shjzhang.
//  2015/07/16      A new solver for fast time and measurement update
//============================================================================


#include "SolverBase.hpp"
#include "ProcessingClass.hpp"
#include "EquationSystemEx.hpp"


namespace gpstk
{

    /** @addtogroup GPSsolutions */
    /// @ingroup math

    //@{

    /** This class is an Extended Kalman Filter (EKF) implementation that
     *  is run-time programmable, making it extremely flexible.
     *
     * MeasUpdate is programmed using class "EquationSystem", that defines
     * a set of rules to "tune" the solver to solve a specific problem. In
     * turn, "EquationSystem" relies in other classes like "Variable" and
     * "Equation", responsible of setting rules such as TypeID's, SourceID's,
     * stochastic models, etc.
     *
     * In this way, complex multi-station and/or hybrid GNSS-INS problems can
     * be tackled with relatively few code lines, encouraging code
     * reusability.
     *
     * A typical way to use this class follows, showing how to set up a
     * MeasUpdate object to perform "Precise Point Positioning" (PPP):
     *
     * @code
     *      // SETTING THE RULES: DEFINE VARIABLES
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
     *      // SETTING THE RULES: DESCRIBE EQUATIONS
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
     *      // SETTING THE RULES: SETUP EQUATION SYSTEM
     *
     *      // Create 'EquationSystem' object
     *   EquationSystem eqSystem;
     *
     *      // Add equation descriptions
     *   eqSystem.addEquation(equPC);
     *   eqSystem.addEquation(equLC);
     *
     *
     *      // SETUP "MeasUpdate" OBJECT
     *
     *      // Create 'MeasUpdate' object and add equation system
     *   MeasUpdate solver( eqSystem );
     *
     * @endcode
     *
     * The "MeasUpdate" object is then ready to be fed with data
     * encapsulated in an appropriate GDS. Take notice that for problems
     * involving multiple epochs and/or multiple stations the recommended
     * GDS is "gnssDataMap", which "MeasUpdate" fully supports.
     *
     * \warning Please be aware that this class requires a significant amount
     * of overhead. Therefore, if your priority is execution speed you should
     * either use the already provided 'purpose-specific' solvers (like
     * 'SolverPPP' or hand-code your own class. For new processing strategies
     * you should balance the importance of machine time (extra overhead)
     * versus researcher time (writing a new solver).
     *
     * \warning "MeasUpdate" is based on an Extended Kalman filter, and
     * Kalman filters are objets that store their internal state, so you MUST
     * NOT use the SAME object to process DIFFERENT data streams.
     *
     * @sa Variable.hpp, Equation.hpp, EquationSystem.hpp.
     *
     */
    class MeasUpdate : public SolverBase, public ProcessingClass
    {
    public:

        /// Default constructor.
        MeasUpdate() {};

        /** Explicit constructor.
         *
         * @param equationSys         Object describing an equation system to
         *                            be solved.
         */
        MeasUpdate( const EquationSystemEx& equationSys )
        { equSystem = equationSys; };


         /// Get a copy of the equation system being solved.
        virtual EquationSystemEx getEquationSystem() const
        { return equSystem; };


        /** Set the equation system to be solved.
         *
         * @param equationSys         Object describing an equation system to
         *                            be solved.
         */
        virtual MeasUpdate& setEquationSystem( const EquationSystemEx& equationSys )
        { equSystem = equationSys; return (*this); };


        /** Add a new equation to EquationSystemEx.
         *
         * @param equation      the Equation object to be added.
         * @param source        the SourceID relative to.
         */
        virtual MeasUpdate& addEquation2Source( const Equation& equation,
                                                const SourceID& source )
        { equSystem.addEquation2Source( equation, source); return (*this); };


        /** Add clock constraint to EquationSystemEx.
         *
         * @param equation      the Equation object to be added.
         *
         */
//        virtual MeasUpdate& addClockConstraint( const Equation& equation );


        /** Remove all Equation objects currently defined.
         *
         * \warning This method will left this MeasUpdate method in an
         *          unstable state. You MUST add at least one equation
         *          definition for this object to work.
         */
        virtual MeasUpdate& clearEquations()
        { equSystem.clearEquations(); return (*this); };


        /** Return a reference to a gnssSatTypeValue object after
         *  solving the previously defined equation system.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process( gnssSatTypeValue& gData )
            throw(ProcessingException);


        /** Return a reference to a gnssRinex object after solving
         *  the previously defined equation system.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process( gnssRinex& gData )
            throw(ProcessingException);


        /** Return a reference to a gnssDataMap object after solving
         *  the previously defined equation system.
         *
         * @param gdsMap    Data object holding the data.
         */
        virtual gnssDataMap& Process( gnssDataMap& gdsMap )
            throw(ProcessingException);


        /// Set State Store.
        MeasUpdate& setStateStore(StateStore& stateStore)
        {
            m_pStateStore = &stateStore;
            equSystem.setStateStore( stateStore );
            return (*this);
        }


        /// Postfit filter.
        virtual bool postfitFilter(gnssDataMap& gdsMap);


        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor.
        virtual ~MeasUpdate() {};


    protected:

        /// Equation system
        EquationSystemEx equSystem;

        /// A posteriori state
        Vector<double> xhat;

        /// A posteriori covariance
        Matrix<double> P;

        /// State Store
        StateStore*  m_pStateStore;

    }; // End of class 'MeasUpdate'

    //@}

}  // End of namespace gpstk

#endif   // GPSTK_MEASUPDATE_HPP

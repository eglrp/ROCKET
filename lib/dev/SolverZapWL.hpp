#pragma ident "$Id: SolverZapWL.hpp 2648 2011-06-09 08:01:41Z shjzhang $"

/**
 * @file SolverZapWL.hpp
 * General Solver with ambiguity fixing.
 */

#ifndef GPSTK_SOLVERZAPWL_HPP
#define GPSTK_SOLVERZAPWL_HPP

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
//  Shoujian Zhang, Wuhan University, 2010
//
//============================================================================


#include <list>
#include <set>
#include "CommonTime.hpp"
#include "AmbiguityDatum2.hpp"
#include "IndepAmbiguityDatum.hpp"
#include "ProcessingClass.hpp"
#include "EquationSystem2.hpp"
#include "StochasticModel.hpp"
#include "SimpleKalmanFilter.hpp"
#include "SolverBase.hpp"
#include "TypeID.hpp"


namespace gpstk
{

      //@{

      /** This class is an Extended Kalman Filter (EKF) implementation that
       *  is run-time programmable, making it extremely flexible.
       *
       * SolverZapWL is a program that inherited from the solverGeneral class,
       * which mainly deals with the "ambiguity" paramters, i.e. generate 
       * independent ambiguity datums for the solver, and will fix the potential
       * ambiguities after the compute all the parameters.
       *
       *
       * @code
       *      // SETTING THE RULES: DEFINE VARIABLES
       *
       * ...Please add the codes here to show how to use this class
       *
       * @endcode
       *
       * \warning "SolverZapWL" is based on an Extended Kalman filter, and
       * Kalman filters are objets that store their internal state, so you MUST
       * NOT use the SAME object to process DIFFERENT data streams.
       *
       * @sa SolverZapWL.hpp.
       *
       */
   class SolverZapWL : public SolverBase, public ProcessingClass
   {
   public:

         /** Common constructor.
          *
          * @param equation      Object describing the equations to be solved.
          *
          */
      SolverZapWL(const Equation& equation)
          : firstTime(true)
      { equSystem.addEquation(equation); };


         /** Explicit constructor.
          * 
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          **/
      SolverZapWL( const std::list<Equation>& equationList )
          : firstTime(true)
      {
            // Visit each "Equation" in 'equationList' and add them to 'equSystem'
         for( std::list<Equation>::const_iterator itEq = equationList.begin();
              itEq != equationList.end();
              ++itEq )
         {
            equSystem.addEquation( (*itEq) );
         }
      };

      
         /** Explicit constructor.
          *
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          **/
      SolverZapWL( const EquationSystem2& equationSys )
         :firstTime(true)
      {
          equSystem = equationSys;
      };



         /** Returns a reference to a gnnsSatTypeValue object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);



         /** Returns a reference to a gnnsRinex object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /** Returns a reference to a gnssDataMap object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Code to be executed before 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Predict( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Returns a reference to a gnssDataMap object after computing .
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Correct( gnssDataMap& gdsMap )
         throw(ProcessingException);

         /** Code to be executed after 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& postCompute( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Ambiguity constraints for the kalman filter.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& MeasUpdate( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Fix the ambiguities.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& AmbiguityFixing( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Update the free unknonws' solution and covariance after
          *  fixing the ambiguities
          */
      virtual void AmbiguityUpdate( Vector<double>& state,
                                    Matrix<double>& covariance,
                                    Vector<double>& fixedFlag,
                                    int& index,
                                    double& fixedValue )
         throw(ProcessingException);


         /** Set constraint system for the equationSystem.
          *
          * @param constrSystem  Object holding the constraint system
          */

      virtual SolverZapWL& setIndepAmbiguityDatum( 
                                    const IndepAmbiguityDatum& ambDatum)
      { indepAmbDatum = ambDatum; return (*this); };


         /** Set constraint system for the equationSystem.
          *
          * @param constrSystem  Object holding the constraint system
          */
      virtual SolverZapWL& setAmbType( const Variable& var)
      { ambType = var; return (*this); };


         /** Returns the solution associated to a given TypeID and SourceID.
          *
          * @param type    TypeID of the solution we are looking for.
          * @param source  SourceID of the solution we are looking for.
          *
          * \warning In the case the solution contains more than one variable
          * of this type, only the first one will be returned.
          */
      virtual double getSolution( const TypeID& type,
                                  const SourceID& source ) const
         throw(InvalidRequest);


         /** Returns the solution associated to a given TypeID, SourceID and
          *  SatID.
          *
          * @param type    TypeID of the solution we are looking for.
          * @param sat     SatID of the solution we are looking for.
          *
          * \warning In the case the solution contains more than one variable
          * of this type, only the first one will be returned.
          */
      virtual double getSolution( const TypeID& type,
                                  const SatID& sat ) const
         throw(InvalidRequest);



         /// Return the set containing sources being currently processed.
      virtual SourceIDSet getCurrentSources() const
      { return equSystem.getCurrentSources(); };


         /// Return the set containing satellites being currently processed.
      virtual SatIDSet getCurrentSats() const
      { return equSystem.getCurrentSats(); };


         /** Return the CURRENT number of satellites, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual int getCurrentUnknownsNumber() const
         throw(InvalidRequest)
      { return currentUnknowns.size(); }


         /** Return the CURRENT number of satellites, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem2 exception.
          */
      virtual int getFixedAmbNumber() const
         throw(InvalidRequest)
      { return ambFixedMap.size(); }


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverZapWL() {};


   private:

      
         /// Ambiguity Datum system
      AmbiguityDatum2 ambiguityDatum;
      IndepAmbiguityDatum indepAmbDatum;

         /// Current set of unknowns
      VariableSet currentUnknowns;
      
         /// Old set of unknowns
      VariableSet oldUnknowns;

         /// Equation system
      EquationSystem2 equSystem;

         /// State Transition Matrix (PhiMatrix)
      Matrix<double> phiMatrix;

         /// Noise covariance matrix (QMatrix)
      Matrix<double> qMatrix;

         /// Geometry matrix
      Matrix<double> hMatrix;

         /// Weights matrix
      Matrix<double> rMatrix;

         /// Measurements vector (Prefit-residuals)
      Vector<double> measVector;

         // prefitResiduals
      Vector<double> prefitResiduals;

         /// Map holding state information
      VariableDataMap stateMap;

         /// Map holding covariance information
      std::map<Variable, VariableDataMap > covarianceMap;

         // Map holding state information
      VariableDataMap stateMapFixed;

         // Map holding covariance information
      std::map<Variable, VariableDataMap > covMapFixed;

         /// Boolean indicating if this filter was run at least once
      bool firstTime;

         /// Variable to store the ambiguity type
      Variable ambType;

         // Predicted state
      Vector<double> xhatminus;

         // Predicted covariance.
      Matrix<double> Pminus;

         // A posteriori state
      Vector<double> xhat;

         // A posteriori covariance.
      Matrix<double> P;

         // Map holding state information
      VariableDataMap stateMapminus;

         // Map holding covariance information
      std::map<Variable, VariableDataMap > covMapminus;

         // Map holding fixed ambiguities
      VariableDataMap contAmbFixedMap;
      VariableDataMap ambFixedMap;
      VariableDataMap indepAmbMap;
      VariableDataMap newFixedMap;

      VariableSet fixedUnkSet;
      VariableSet floatUnkSet;

         /// A structure used to store fixing data for a SV.
      struct fixingData 
      {
            // Default constructor initializing the data in the structure
         fixingData() 
             : floatAmbNumb(0), fixedAmbNumb(0), fixingRate(0.0)
         {};

         int floatAmbNumb;  ///< float ambiguity number.
         int fixedAmbNumb;  ///< fixed ambiguity number
         double fixingRate; ///< fixing rate
      };


         /// Map holding the ambiguity fixing information 
         /// regarding every satellite
      std::map<SatID, fixingData> fixingDataMap;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };

   }; // End of class 'SolverZapWL'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_SOLVERZAPWL_HPP

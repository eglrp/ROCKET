#pragma ident "$Id: SolverUpdWL.hpp 2648 2011-06-09 08:01:41Z shjzhang $"

/**
 * @file SolverUpdWL.hpp
 * General Solver with ambiguity fixing.
 */

#ifndef GPSTK_SOLVERUPDWL_HPP
#define GPSTK_SOLVERUPDWL_HPP

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
#include "SolverGeneral.hpp"
#include "CommonTime.hpp"

#include "AmbiguityDatum.hpp"
#include "IndepAmbiguityDatum.hpp"


namespace gpstk
{

      //@{

      /** This class is an Extended Kalman Filter (EKF) implementation that
       *  is run-time programmable, making it extremely flexible.
       *
       * SolverUpdWL is a program that inherited from the solverGeneral class,
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
       * \warning "SolverUpdWL" is based on an Extended Kalman filter, and
       * Kalman filters are objets that store their internal state, so you MUST
       * NOT use the SAME object to process DIFFERENT data streams.
       *
       * @sa SolverUpdWL.hpp.
       *
       */
   class SolverUpdWL : public SolverGeneral
   {
   public:

         /** Common constructor.
          *
          * @param equation      Object describing the equations to be solved.
          *
          */
      SolverUpdWL(const Equation& equation)
          : SolverGeneral(equation), isIndepAmb(false)
      {};


         /** Explicit constructor.
          * 
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          **/
      SolverUpdWL( const std::list<Equation>& equationList )
          : SolverGeneral(equationList)
      {};

      
         /** Explicit constructor.
          *
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          **/
      SolverUpdWL( const EquationSystem& equationSys )
         : SolverGeneral(equationSys)
      {};


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
      virtual gnssDataMap& preCompute( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Returns a reference to a gnssDataMap object after computing .
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Compute( gnssDataMap& gdsMap )
         throw(InvalidSolver);


         /** Code to be executed after 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& postCompute( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Time Update of the kalman filter.
          *
          * @param gData    Data object holding the data.
          */
      virtual int TimeUpdate( const Matrix<double>& phiMatrix,
                              const Matrix<double>& processNoiseCovariance  )
         throw(InvalidSolver);


         /** Ambiguity constraints for the kalman filter.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& AmbiguityConstr( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Measurement Update of the kalman filter.
          *
          * @param gData    Data object holding the data.
          */
      virtual int MeasUpdate( const Vector<double>& prefitResiduals,
                              const Matrix<double>& designMatrix,
                              const Matrix<double>& weightMatrix  )
         throw(InvalidSolver);


         /** Set constraint system for the equationSystem.
          *
          * @param constrSystem  Object holding the constraint system
          */
      virtual SolverUpdWL& setAmbiguityDatum( 
                                    const AmbiguityDatum& ambDatum)
      { isIndepAmb = false; ambiguityDatum = ambDatum; return (*this); };

      virtual SolverUpdWL& setIndepAmbiguityDatum( 
                                    const IndepAmbiguityDatum& ambDatum)
      { isIndepAmb = true; indepAmbDatum = ambDatum; return (*this); };


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


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverUpdWL() {};


   private:

      bool isIndepAmb;
      
         /// Ambiguity Datum system
      AmbiguityDatum ambiguityDatum;
      IndepAmbiguityDatum indepAmbDatum;


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
      VariableDataMap ambFixedMap;


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

   }; // End of class 'SolverUpdWL'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_SOLVERUPDWL_HPP

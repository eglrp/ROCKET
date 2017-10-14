#pragma ident "$Id$"

/**
 * @file StochasticModel2.hpp
 * Base class to define stochastic models, plus implementations
 * of common ones.
 */

#ifndef GPSTK_STOCHASTICMODEL2_HPP
#define GPSTK_STOCHASTICMODEL2_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================
//
//  Revision
//
//  2012/09/23  Add method 'getCS()' for 'PhaseAmbiguityModel' class, which
//              will return the cycle slip flags. (shjzhang)
//
//  2014/02/18  Add the 'IonoRandomWalkModel' for the slant ionospheric
//              delays estimation. (shjzhang)
//
//  2014/10/23  Add the processing of the interrupts for the
//              'IonoRandomWalkModel'
//
//  2015/07/15  Add a new interface for 'Prepare', which will be more
//              efficient for preparing.
//
//  2015/10/05  Add 'getPhiMatrix()/getQMatrix()' for stochastic model
//              with dependent variables. e.g. SatClockModel, SatOrbitModel
//
//============================================================================


#include "CommonTime.hpp"
#include "DataStructures.hpp"



namespace gpstk
{

    class Variable;

      /** @addtogroup DataStructures */
      //@{


      /** This is a base class to define stochastic models. It computes the
       *  elements of Phi and Q matrices corresponding to a constant
       *  stochastic model.
       *
       * @sa RandomWalkModel, WhiteNoiseModel, PhaseAmbiguityModel
       *
       */
    class StochasticModel2
    {
    public:

        /// Default constructor
        StochasticModel2() {};


        // Set element of the state transition matrix Phi
        virtual StochasticModel2& setPhi( const Matrix<double>& phi )
        {  m_phiMatrix = Matrix<double>(phi); return (*this); };


        /// Get element of the state transition matrix Phi
        virtual Matrix<double> getPhi() const
        { return m_phiMatrix; };


        // Set element of the process noise matrix Q
        virtual StochasticModel2& setQ( const Matrix<double>& q )
        { m_qMatrix = Matrix<double>(q); return (*this); };


        /// Get element of the process noise matrix Q
        virtual Matrix<double> getQ() const
        { return m_qMatrix; };


        // Add TypeID to relative TypeID vector
        virtual StochasticModel2& addTypeID( const TypeID& typeID )
        {
            m_relTypeIDVec.push_back(typeID);

            int size( m_relTypeIDVec.size() );
            m_phiMatrix.resize(size,size,0.0);
            m_qMatrix.resize(size,size,0.0);

            return (*this);
        };


        // Set relative TypeID vector
        virtual StochasticModel2& setRelTypeIDVec(const std::vector<TypeID>& types)
        {
            m_relTypeIDVec = types;

            int size( m_relTypeIDVec.size() );
            m_phiMatrix.resize(size,size,0.0);
            m_qMatrix.resize(size,size,0.0);

            return (*this);
        };


        /** This method provides TypeID vector, which contains all
         *  relative TypeIDs in order.
         *
         * @return  TypeID vector.
         *
         */
        virtual std::vector<TypeID> getRelTypeIDVec() const
        { return m_relTypeIDVec; };


        /** This method provides the stochastic model with all the available
         *  information and takes appropriate actions. By default, it does
         *  nothing.
         *
         * @param relVarVec  relative variable set
         * @param gData      Data object holding the data.
         *
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssSatTypeValue& gData )
        { return; };


        /** This method provides the stochastic model with all the available
         *  information and takes appropriate actions. By default, it does
         *  nothing.
         *
         * @param relVarVec  relative variable vector.
         * @param gData      Data object holding the data.
         *
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssRinex& gData )
        { return; };


        /** This method provides the stochastic model with all the available
         *  information and takes appropriate actions. By default, it does
         *  nothing.
         *
         * @param relVarVec  relative variable vector.
         * @param gData      Data object holding the data.
         *
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssDataMap& gData )
        { return; };


        /// Destructor
        virtual ~StochasticModel2() {};


    protected:

        // holding relative TypeIDs in order
        std::vector<TypeID> m_relTypeIDVec;

        // holding phi value for relative TypeIDs in order
        Matrix<double> m_phiMatrix;

        // holding q value for relative TypeIDs in order
        Matrix<double> m_qMatrix;

    }; // End of class 'StochasticModel2'



    /** This class compute the elements of Phi and Q matrices corresponding
     *  to a white noise stochastic model.
     *
     * @sa StochasticModel, ConstantModel, RandomWalkModel
     *
     */
    class WhiteNoiseModel2 : public StochasticModel2
    {
    public:

        /** Common constructor
         *
         * @param sigma   Standard deviation (sigma) of white noise process
         *
         */
        WhiteNoiseModel2( double sigma = 3e5 )
            : variance(sigma*sigma)
        {
            m_phiMatrix = Matrix<double>( 1, 1, 0.0 );
            m_qMatrix = Matrix<double>( 1, 1, sigma*sigma );
        };


        /// Set the value of white noise sigma
        virtual WhiteNoiseModel2& setSigma(double sigma)
        {
            variance = sigma*sigma;
            m_qMatrix(0,0) = variance;
            return (*this);
        };


        /// Destructor
        virtual ~WhiteNoiseModel2() {};

    private:

        /// White noise variance
        double variance;

    }; // End of class 'WhiteNoiseModel2'



    /** This class compute the elements of Phi and Q matrices corresponding
     *  to a random walk stochastic model.
     *
     * @sa StochasticModel, ConstantModel, WhiteNoiseModel
     *
     * \warning RandomWalkModel objets store their internal state, so you
     * MUST NOT use the SAME object to process DIFFERENT data streams.
     *
     */
    class RandomWalkModel2 : public StochasticModel2
    {
    public:

        /// Default constructor. By default sets a very high Qprime and both
        /// previousTime and currentTime are CommonTime::BEGINNING_OF_TIME.
        RandomWalkModel2()
            : m_previousTime(CommonTime::BEGINNING_OF_TIME),
              m_currentTime(CommonTime::BEGINNING_OF_TIME)
        {};


        /** Set the value of previous epoch
         *
         * @param prevTime   Value of previous epoch
         *
         */
        virtual RandomWalkModel2& setPreviousTime(const CommonTime& prevTime)
        { m_previousTime = prevTime; return (*this); };


        /** Set the value of current epoch
         *
         * @param currTime   Value of current epoch
         *
         */
        virtual RandomWalkModel2& setCurrentTime(const CommonTime& currTime)
        { m_currentTime = currTime; return (*this); };


        // Add TypeID to relative TypeID vector
        virtual RandomWalkModel2& addTypeID( const TypeID& typeID )
        {
            m_relTypeIDVec.push_back(typeID);

            int size( m_relTypeIDVec.size() );
            m_phiMatrix.resize(size,size,0.0);
            m_qprimeVec.resize(size,0.0);
            m_qMatrix.resize(size,size,0.0);

            return (*this);
        };


        // Set relative TypeID vector
        virtual RandomWalkModel2& setRelTypeIDVec(const std::vector<TypeID>& types)
        {
            m_relTypeIDVec = types;

            int size( m_relTypeIDVec.size() );
            m_phiMatrix.resize(size,size,0.0);
            m_qprimeVec.resize(size,0.0);
            m_qMatrix.resize(size,size,0.0);

            return (*this);
        };


        /** Set the value of process spectral density.
         *
         * @param qp        Process spectral density: d(variance)/d(time) or
         *                  d(sigma*sigma)/d(time).
         * @param type      TypeID
         *
         * \warning Beware of units: Process spectral density units are
         * sigma*sigma/time, while other models take plain sigma as input.
         * Sigma units are usually given in meters, but time units MUST BE
         * in SECONDS.
         *
         */
        virtual RandomWalkModel2& setQprime(double qp, const TypeID& type)
        {
            if(m_qprimeVec.size() != m_relTypeIDVec.size())
            {
                m_qprimeVec.resize(m_relTypeIDVec.size(),0.0);
            }

            for( int i=0; i<m_relTypeIDVec.size(); i++ )
            {
                if( type == m_relTypeIDVec[i] ) {m_qprimeVec[i] = qp; break;}
            }

            return (*this);
        };


        /** Set the value of process spectral density.
         *
         * @param qp        Process spectral density: d(variance)/d(time) or
         *                  d(sigma*sigma)/d(time).
         *
         * \warning Beware of units: Process spectral density units are
         * sigma*sigma/time, while other models take plain sigma as input.
         * Sigma units are usually given in meters, but time units MUST BE
         * in SECONDS.
         *
         */
        virtual RandomWalkModel2& setQPrime(std::vector<double> qp)
        { m_qprimeVec = qp; return (*this); };


        /// Get element of the state transition matrix Phi
        virtual Matrix<double> getPhi() const
        { return m_phiMatrix; };


        /// Get element of the process noise matrix Q
        virtual Matrix<double> getQ() const
        { return m_qMatrix; };


        /** Prepare q Matrix for relative variables.
         *
         * @param relVarVec  relative variable vector.
         * @param gData      Data object holding the data.
         *
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssDataMap& gData );


        /// Destructor
        virtual ~RandomWalkModel2() {};


    protected:

        /// Process spectral density
        std::vector<double> m_qprimeVec;

        /// Epoch of previous measurement
        CommonTime m_previousTime;

        /// Epoch of current measurement
        CommonTime m_currentTime;

    }; // End of class 'RandomWalkModel2'



    /** This class compute the elements of Phi and Q matrices corresponding
     *  to a satellite clock variable, modeled as random walk stochastic model.
     *
     * @sa StochasticModel, ConstantModel, WhiteNoiseModel
     *
     * \warning RandomWalkModel objets store their internal state, so you
     * MUST NOT use the SAME object to process DIFFERENT data streams.
     *
     */
    class SatOrbRandomWalkModel : public RandomWalkModel2
    {
    public:

        // Default constructor
        SatOrbRandomWalkModel()
        {
            // add relative types for satellite orbit model
            m_relTypeIDVec.push_back( TypeID::dSatX );
            m_relTypeIDVec.push_back( TypeID::dSatY );
            m_relTypeIDVec.push_back( TypeID::dSatZ );
            m_relTypeIDVec.push_back( TypeID::dSatVX );
            m_relTypeIDVec.push_back( TypeID::dSatVY );
            m_relTypeIDVec.push_back( TypeID::dSatVZ );
            m_relTypeIDVec.push_back( TypeID::dSRPC1 );
            m_relTypeIDVec.push_back( TypeID::dSRPC2 );
            m_relTypeIDVec.push_back( TypeID::dSRPC3 );
            m_relTypeIDVec.push_back( TypeID::dSRPC4 );
            m_relTypeIDVec.push_back( TypeID::dSRPC5 );
//            m_relTypeIDVec.push_back( TypeID::dSRPC6 );
//            m_relTypeIDVec.push_back( TypeID::dSRPC7 );
//            m_relTypeIDVec.push_back( TypeID::dSRPC8 );
//            m_relTypeIDVec.push_back( TypeID::dSRPC9 );

            // initialize qprime vector for satellite orbit model
            for(int i=0; i<6; ++i)
            {
                m_qprimeVec.push_back( 0.0 );
            }
            for(int i=6; i<11; ++i)
            {
                m_qprimeVec.push_back( 1e-15 );
            }

            // initialize phi matrix for satellite orbit model
            m_phiMatrix = Matrix<double>( 11, 11, 0.0 );
            for(int i=0; i<11; ++i)
            {
                m_phiMatrix(i,i) = 1.0;
            }

            // initialize q matrix for satellite orbit model
            m_qMatrix = Matrix<double>( 11, 11, 0.0 );

            for(int i=0; i<6; ++i)
            {
                m_qMatrix(i,i) = 0.0;
            }
            for(int i=6; i<11; ++i)
            {
                m_qMatrix(i,i) = 1e-15;
            }
        };


        // Set satVectorMap
        virtual SatOrbRandomWalkModel& setSatOrbit(const satVectorMap& orbit)
        { satOrbit = orbit; return (*this); };


        // Get satVectorMap
        virtual satVectorMap getSatOrbit() const
        { return satOrbit; };


        /** Prepare q Matrix for relative variables.
         *
         * @param relVarVec  relative variable vector.
         * @param gData      Data object holding the data.
         *
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssDataMap& gData );


        // Destructor
        ~SatOrbRandomWalkModel() {};


    private:

        // Object holding satellite orbit data
        satVectorMap satOrbit;

    }; // End of class 'SatOrbRandomWalkModel'



    /** This class compute the elements of Phi and Q matrices corresponding
     *  to a satellite clock variable, modeled as random walk stochastic model.
     *
     * @sa StochasticModel, ConstantModel, WhiteNoiseModel
     *
     * \warning RandomWalkModel objets store their internal state, so you
     * MUST NOT use the SAME object to process DIFFERENT data streams.
     *
     */
    class SatClkRandomWalkModel : public RandomWalkModel2
    {
    public:

        // Default constructor
        SatClkRandomWalkModel()
        {
            // add relative types for satellite clock model
            m_relTypeIDVec.push_back( TypeID::dcdtSat );
            m_relTypeIDVec.push_back( TypeID::dcdtSatDot );

            // initialize qprime vector for satellite clock model
            m_qprimeVec.push_back( 1.5e-6 );
            m_qprimeVec.push_back( 3e-10  );

            // initialize phi matrix for satellite clock model
            m_phiMatrix = Matrix<double>( 2, 2, 0.0 );
            m_phiMatrix( 0, 0 ) = 1.0;
            m_phiMatrix( 0, 1 ) = 0.0;
            m_phiMatrix( 1, 0 ) = 0.0;
            m_phiMatrix( 1, 1 ) = 1.0;

            // initialize q matrix for satellite clock model
            m_qMatrix = Matrix<double>( 2, 2, 0.0 );
        };


        /// Get element of the state transition matrix Phi
        virtual Matrix<double> getPhi()
        {
            double dt(0.0);

            if( (m_previousTime != CommonTime::BEGINNING_OF_TIME) &&
                (m_previousTime < m_currentTime) )
            {
                dt = m_currentTime - m_previousTime;
            }

            m_phiMatrix(0,1) = dt;

            m_qMatrix(0,0) = 1.5e-6*dt;
            m_qMatrix(1,1) = 3e-10*dt;

            return m_phiMatrix;
        };

    }; // End of class 'SatClkRandomWalkModel'


    /** This class compute the elements of Phi and Q matrices corresponding
     *  to a phase ambiguity variable: Constant stochastic model within
     *  cycle slips and white noise stochastic model when a cycle slip
     *  happens.
     *
     * @sa StochasticModel, ConstantModel, WhiteNoiseModel
     *
     * \warning By default, this class expects each satellite to have
     * 'TypeID::satArc' data inserted in the GNSS Data Structure. Such data
     * are generated by 'SatArcMarker' objects. Use 'setWatchSatArc()'
     * method to change this behaviour and use cycle slip flags directly.
     * By default, the 'TypeID' of the cycle slip flag is 'TypeID::CSL1'.
     */
    class PhaseAmbiguityModel2 : public StochasticModel2
    {
    public:

        /** Common constructor
         *
         * @param sigma   Standard deviation (sigma) of white noise process
         *
         */
        PhaseAmbiguityModel2( double sigma = 2e7 )
            : variance(sigma*sigma), cycleSlip(false),
              watchSatArc(true), csFlagType(TypeID::CSL1)
        {
            m_phiMatrix = Matrix<double>( 1, 1, 0.0 );
            m_qMatrix = Matrix<double>( 1, 1, sigma*sigma );
        };


        /// Set the value of white noise sigma
        virtual PhaseAmbiguityModel2& setSigma(double sigma)
        {
            variance = sigma*sigma;
            m_qMatrix(0,0) = sigma*sigma;
            return (*this);
        };


        /** Feed the object with information about occurrence of cycle slips.
         *
         * @param cs   Boolean indicating if there is a cycle slip at current
         *             epoch.
         *
         */
        virtual PhaseAmbiguityModel2& setCS(bool cs)
        { cycleSlip = cs; return (*this); };


        /** Get the cycle slip flag.
         *
         * @return  Boolean indicating if there is a cycle slip at current
         *          epoch.
         */
        virtual bool getCS() const
        { return cycleSlip; };


        /// Set whether satellite arc will be used instead of cycle slip flag
        virtual PhaseAmbiguityModel2& setWatchSatArc(bool watchArc)
        { watchSatArc = watchArc; return (*this); };


        /** This method sets the 'TypeID' of the cycle slip flag to be used.
         *
         * @param type       Type of cycle slip flag to be used.
         *
         * \warning Method 'setWatchSatArc()' must be set 'false' for this
         * method to have any effect.
         */
        virtual PhaseAmbiguityModel2& setCycleSlipFlag( const TypeID& type )
        { csFlagType = type; return (*this); };


        /// Get the 'TypeID' of the cycle slip flag being used.
        virtual TypeID getCycleSlipFlag() const
        { return csFlagType; };


        /** Prepare q Matrix for relative variablesa.
         *
         * @param relVarVec   relative variable vector container
         * @param gData       gnssDataMap
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssDataMap& gData );


        /// Destructor
        virtual ~PhaseAmbiguityModel2() {};


    private:

        /// White noise variance
        double variance;

        /// Boolean stating if there is a cycle slip at current epoch.
        bool cycleSlip;

        /// Whether satellite arcs will be watched. False by default.
        bool watchSatArc;

        /// 'TypeID' of the cycle slip flag being used.
        TypeID csFlagType;

        /// Map holding information regarding every satellite.
        std::map<SourceID, std::map<SatID, double> > satArcMap;


        /** This method checks if a cycle slip happened.
         *
         * @param sat        Satellite.
         * @param data       Object holding the data.
         * @param source     Object holding the source of data.
         *
         */
        virtual void checkCS( const SatID& sat,
                              satTypeValueMap& data,
                              const SourceID& source );

    }; // End of class 'PhaseAmbiguityModel2'



      /** This class compute the elements of Phi and Q matrices corresponding
       *  to zenital tropospheric wet delays, modeled as a random walk
       *  stochastic model.
       *
       * This class is designed to support multiple stations simultaneously
       *
       * @sa RandomWalkModel, StochasticModel, ConstantModel, WhiteNoiseModel
       *
       */
    class TropoRandomWalkModel2 : public StochasticModel2
    {
    public:

         /// Default constructor.
        TropoRandomWalkModel2()
        {
            m_relTypeIDVec.push_back( TypeID::wetMap );

            m_phiMatrix = Matrix<double>( 1, 1, 1.0 );
            m_qMatrix = Matrix<double>( 1, 1, 0.0 );
        };


         /** Set the value of previous epoch for a given source
          *
          * @param source     SourceID whose previous epoch will be set
          * @param prevTime   Value of previous epoch
          *
          */
        virtual TropoRandomWalkModel2& setPreviousTime( const SourceID& source,
                                                        const CommonTime& prevTime )
        { tmData[source].previousTime = prevTime; return (*this); };


         /** Set the value of current epoch for a given source
          *
          * @param source     SourceID whose current epoch will be set
          * @param currTime   Value of current epoch
          *
          */
        virtual TropoRandomWalkModel2& setCurrentTime( const SourceID& source,
                                                       const CommonTime& currTime )
        { tmData[source].currentTime = currTime; return (*this); };


         /** Set the value of process spectral density for ALL current sources.
          *
          * @param qp         Process spectral density: d(variance)/d(time) or
          *                   d(sigma*sigma)/d(time).
          *
          * \warning Beware of units: Process spectral density units are
          * sigma*sigma/time, while other models take plain sigma as input.
          * Sigma units are usually given in meters, but time units MUST BE
          * in SECONDS.
          *
          * \warning New sources being added for processing AFTER calling
          * method 'setQprime()' will still be processed at the default process
          * spectral density for zenital wet tropospheric delay, which is set
          * to 3e-8 m*m/s (equivalent to about 1.0 cm*cm/h).
          *
          */
        virtual TropoRandomWalkModel2& setQprime(double qp);


         /** Set the value of process spectral density for a given source.
          *
          * @param source     SourceID whose process spectral density will
          *                   be set.
          * @param qp         Process spectral density: d(variance)/d(time) or
          *                   d(sigma*sigma)/d(time).
          *
          * \warning Beware of units: Process spectral density units are
          * sigma*sigma/time, while other models take plain sigma as input.
          * Sigma units are usually given in meters, but time units MUST BE
          * in SECONDS.
          *
          * \warning New sources being added for processing AFTER calling
          * method 'setQprime()' will still be processed at the default process
          * spectral density for zenital wet tropospheric delay, which is set
          * to 3e-8 m*m/s (equivalent to about 1.0 cm*cm/h).
          *
          */
        virtual TropoRandomWalkModel2& setQprime( const SourceID& source,
                                                  double qp )
        { tmData[source].qprime = qp; return (*this); }


        /** Prepare q Matrix for relative variablesa.
         *
         * @param relVarVec   relative variable vector container
         * @param gData       gnssDataMap
         */
        virtual void Prepare( std::vector<Variable>& relVarVec,
                              gnssDataMap& gData );


         /// Destructor
        virtual ~TropoRandomWalkModel2() {};


    private:


         /// Structure holding object data
        struct tropModelData
        {
            // Default constructor initializing the data in the structure
            tropModelData() : qprime(3e-8),
                              previousTime(CommonTime::BEGINNING_OF_TIME),
                              currentTime(CommonTime::BEGINNING_OF_TIME)
            {};

            double qprime;          ///< Process spectral density
            CommonTime previousTime;   ///< Epoch of previous measurement
            CommonTime currentTime;    ///< Epoch of current measurement

        }; // End of struct 'tropModelData'


         /// Map holding the information regarding each source
        std::map<SourceID, tropModelData> tmData;


         /// Field holding value of current variance
        double variance;

    }; // End of class 'TropoRandomWalkModel2'


}  // End of namespace gpstk

#endif // GPSTK_STOCHASTICMODEL2_HPP

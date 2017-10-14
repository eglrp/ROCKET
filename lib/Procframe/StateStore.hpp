#pragma ident "$Id: StateStore.hpp$"

/**
 * @file StateStore.hpp
 * Store State of Kalman filter
 */

#ifndef GPSTK_STATE_STORE_HPP
#define GPSTK_STATE_STORE_HPP

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
//  Modifications
//  -------------
//
//  - Create this subroutine, 2016/11/24.
//
//  Copyright
//  ---------
//
//  Gao Kang, Wuhan University, 2016
//
//============================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include "DataStructures.hpp"
#include "Variable.hpp"
#include "MSCStore.hpp"
#include "Rinex3EphemerisStore2.hpp"

namespace gpstk
{

    /** @addtogroup DataStructures */

    class StateStore
    {
    public:

        /// Constructor
        StateStore()
        {};

        /// Convenience output method
        std::ostream& dump(std::ostream& s) const;


        /** Returns the solution associated to a given Variable.
         *
         * @param variable      Variable object solution we are looking for.
         */
        virtual double getSolution( const Variable& variable ) const
            throw(InvalidRequest);


        /** set solution value associated to a given Variable.
         *
         *  @param variable     Variable object solution we are looking for.
         *  @param value        Variable value.
         *
         *  @return this object.
         */
        virtual StateStore& setSolution( const Variable& variable,
                                         double value )
            throw(InvalidRequest);


        /** Returns the solution associated to a given TypeID.
         *
         * @param type   TypeID of the solution we are looking for.
         *
         * \warning In the case the solution contains more than one variable
         * of this type, only the first one will be returned.
         */
        virtual double getSolution( const TypeID& type ) const
            throw(InvalidRequest);


        /** set the solution value associated to a given TypeID.
         *
         * @param type   TypeID of the solution we are looking for.
         * @param value  TypeID value.
         *
         * \warning In the case the solutin contains more than one variable
         * of this type, we will set the same value.
         */
        virtual StateStore& setSolution( const TypeID& type,
                                         double value )
            throw(InvalidRequest);


        /** Returns the solution associated to a given TypeID and SourceID
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


        /** set the solution value associated to a given TypeID and SourceID
         *
         * @param type    TypeID of the solution we are looking for.
         * @param source  SourceID of the solution we are looking for.
         * @param value   value of solution
         *
         * \warning In the case the solutin contains more than one variable
         * of this type, we will set the same value.
         */
        virtual StateStore& setSolution( const TypeID& type,
                                         const SourceID& source,
                                         double value )
            throw(InvalidRequest);



        /** Returns the solution associated to a given TypeID and SatID
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



        /** set the solution value associated to a given TypeID and SatID
         *
         * @param type    TypeID of the solution we are looking for.
         * @param sat     SatID of the solution we are looking for.
         * @param value   value of solution
         *
         * \warning In the case the solutin contains more than one variable
         * of this type, we will set the same value.
         */
        virtual StateStore& setSolution( const TypeID& type,
                                         const SatID& sat,
                                         double value )
            throw(InvalidRequest);


        /** Returns the solution associated to a given TypeID, SourceID and SatID
         *
         * @param type    TypeID of the solution we are looking for.
         * @param source  SourceID of the solution we are looking for.
         * @param sat     SatID of the solution we are looking for.
         *
         * \warning In the case the solution contains more than one variable
         * of this type, only the first one will be returned.
         */
        virtual double getSolution( const TypeID& type,
                                    const SourceID& source,
                                    const SatID& sat ) const
            throw(InvalidRequest);


        /** set the solution value associated to a given TypeID, SourceID and SatID
         *
         * @param type    TypeID of the solution we are looking for.
         * @param source  SourceID of the solution we are looking for.
         * @param sat     SatID of the solution we are looking for.
         * @param value   value of solution
         *
         * \warning In the case the solutin contains more than one variable
         * of this type, we will set the same value.
         */
        virtual StateStore& setSolution( const TypeID& type,
                                         const SourceID& source,
                                         const SatID& sat,
                                         double value )
            throw(InvalidRequest);



        /// Get the Variable Set
        virtual VariableSet& getVariableSet()
        { return m_VariableSet;}


        /// Set the Variable Set
        virtual StateStore& setVariableSet(VariableSet variableSet)
        { m_VariableSet = variableSet; return (*this);}


        /// Get the Covariance Matrix
        virtual Matrix<double> getCovarMatrix()
        { return m_CovarMatrix;}


        /// Set the Covariance Matrix
        virtual StateStore& setCovarMatrix(const Matrix<double>& covarMatrix)
        { m_CovarMatrix = covarMatrix; return (*this);}



        /** get Covariance Matrix from subVariableSet
         *
         * @param subVariableSet  the sub variable set
         *
         * @return sub covariance matrix.
         *
         * \warning If the variable in subVariableSet is not included in
         * current variable set, the value will be set zero.
         */
        virtual Matrix<double> getCovarMatrix(const VariableSet& subVariableSet);



        /** get Covariance Matrix from subVariableSet
         *
         * @param subVariableSet  the sub variable set
         * @param gData  gnssDataMap for some flags.
         *
         * @return sub covariance matrix.
         *
         * \warning If the variable in subVariableSet is not included in
         * current variable set, the value will be set zero.
         */
        virtual Matrix<double> getCovarMatrix( const VariableSet& subVariableSet,
                                               gnssDataMap& gData,
                                               const TypeID& type );


        /** set State Vector
         *
         * @param stateVector  state Vector needs to be stored.
         *
         * @return this object
         */
        virtual StateStore& setStateVector(const Vector<double>& stateVector)
        { m_StateVec = stateVector; return (*this);}


        /** get State Vector
         *
         * @return the state vector.
         */
        virtual Vector<double> getStateVector()
        { return m_StateVec;}


        /** get State Vector from sub VariableSet
         *
         * @param subVariableSet  the sub variable set
         *
         * @return sub state vector.
         *
         * \warning If the variable in subset is not included in current
         *  variable set, the value will be set zero.
         */
        virtual Vector<double> getStateVector(const VariableSet& subVariableSet);



        /** get State Vector from sub VariableSet
         *
         * @param subVariableSet  the sub variable set
         * @param gData  the gnssDataMap contains some flags.
         * @return sub state vector.
         *
         * \warning If the variable in subset is not included in current
         *  variable set, the value will be set zero.
         */
        virtual Vector<double> getStateVector( const VariableSet& subVariableSet,
                                               gnssDataMap& gData,
                                               const TypeID& type );


        /** get all sourceID contained in variable set.
         *
         * @param sourceSet  reference set of SourceIDSet as input and output.
         */
        virtual void getSourceIDSet(SourceIDSet& sourceSet);


        /** get all satID contained in variable set.
         *
         * @param satSet   reference set of SatIDSet as input and output.
         */
        virtual void getSatIDSet(SatIDSet& satSet);


        /** set ephemeris data store
         *
         * @param ephStore  ephemeris data store.
         *
         * @return this object.
         */
        virtual StateStore& setEphStore( XvtStore<SatID>& ephStore )
        { m_pEphStore=&ephStore; return (*this); }


        /** set MSC data store
         *
         * @param mscStore  msc data store.
         *
         * @return this object.
         */
        virtual StateStore& setMSCStore( MSCStore& mscStore )
        { m_pMSCStore=&mscStore; return (*this); }


        /** set master name
         *
         * @param masterName  master station name
         *
         * @return this object
         */
        virtual StateStore& setMasterName( std::string masterName )
        { m_MasterName = masterName; return (*this); }


        /** get master station name
         *
         * @return the name of master station name
         */
        virtual std::string getMasterName()
        { return m_MasterName;}


        /** set master SourceID
         *
         * @param set master station of SourceID
         *
         * @return this object
         */
        virtual StateStore& setMasterSource( SourceID& source )
        { m_MasterSource = source; return (*this); }


        /** get master SourceID
         *
         * @return the SourceID of master station.
         */
        virtual SourceID& getMasterSource()
        { return m_MasterSource;}


        // get state epoch
        virtual CommonTime getStateEpoch() const
        { return m_StateEpoch; }


        // set state epoch
        virtual StateStore& setStateEpoch( const CommonTime& epoch )
        {
            if( m_StateEpoch != epoch )
            {
                m_prevEpoch = m_StateEpoch;
                m_StateEpoch = epoch;
            }

            return *this;
        }

        // get previous epoch
        virtual CommonTime getPrevEpoch() const
        { return m_prevEpoch;}


        /** update station position
         *
         * @param gData  the data to process.
         *
         * @return this object.
         */
        StateStore& updateNominalPos( gnssDataMap& gData );


        /// Destructor
        virtual ~StateStore() {};


    private:

        /// previous state epoch
        CommonTime m_prevEpoch;

        /// current state epoch
        CommonTime m_StateEpoch;

        /// variable set
        VariableSet m_VariableSet;

        /// state vector
        Vector<double> m_StateVec;

        /// state covariance matrix
        Matrix<double> m_CovarMatrix;

        // master station name
        std::string m_MasterName;

        // master station source
        SourceID m_MasterSource;

        // MSC data
        MSCStore* m_pMSCStore;

        // BCE data
        XvtStore<SatID>* m_pEphStore;

    }; // End of class 'StateStore'


    namespace StringUtils
    {

        // convert this object to a string representation
        inline std::string asString(const StateStore& p)
        {
            std::ostringstream oss;
            p.dump(oss);

            return oss.str();

        }  // End of function 'asString()'

    }  // End of namespace StringUtils



    // Stream output for StateStore
    inline std::ostream& operator<<( std::ostream& s,
                                     const StateStore& p )
    {
        p.dump(s);

        return s;

    }  // End of 'operator<<'

}  // End of namespace gpstk

#endif   // GPSTK_STATE_STORE_HPP

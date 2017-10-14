#pragma ident "$Id: EquationSystemEx.cpp$"

/**
 * @file EquationSystemEx.cpp
 * Class to define and handle complex equation systems for solvers.
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
//  Copyright
//  ---------
//
//  shjzhang - Wuhan University. 2015
//
//============================================================================
//
//  History
//
//  - Modify of the Method "getPhiQ()". The gnssRinex is needed to 'Prepare'
//    the stochastic model. When the variable is source-indexed, the gnssRinex
//    can be given by finding the source in gdsMap, but when the variable is
//    only satellite-indexed, the gnssRinex is the first gnssRinex which "SEE"
//    this satellite. (2012.06.19)
//
//  - Change
//    "varUnknowns.insert( currentUnknowns.begin(), currentUnknowns.end() );"
//    to
//    "varUnknowns = currentUnknowns;"
//
//  - Version 2.0 (majar modification)
//
//    The EquationSystemEx is totoally revised to compute the related matrix/vector
//    very fast. The new version can improve the speed by a fator of about 100.
//    In the new equation version, the major revision includes:
//    (1) The typeValueMap data will be stored in each equation header, then
//        in the following Phi/Q, Prefit/Geometry/Weight computation, the data
//        will be directly feeded instead of extracting from 'gdsDataMap'.
//    (2) The 'getPrefit' and 'getGeometryWeights' are merged into
//        'getPrefitGeometryWeights', which will compute prefit/geometry/weights
//        in one loop. This will save time in further.
//    shjzhang, 2016.6.26
//
//============================================================================


#include "EquationSystemEx.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"
#include <iterator>

using namespace std;

namespace gpstk
{

    // General white noise stochastic model
    WhiteNoiseModel2 EquationSystemEx::whiteNoiseModel;


    /** Add a new equation to SourceID relatived
     *
     * @param equation  the Equation object to be added.
     * @param source    the SourceID to be relatived.
     */
    EquationSystemEx& EquationSystemEx::addEquation2Source( const Equation& equation,
                                                            const SourceID& source )
    {
        SourceEquationMap::iterator sourceEqnIter = m_SourceEquationMap.find( source );

        if( m_SourceEquationMap.end() == sourceEqnIter )
        {
            EquationList newEqnList;
            newEqnList.push_back(equation);
            m_SourceEquationMap[source] = newEqnList;
        }
        else
        {
            sourceEqnIter->second.push_back(equation);
        }

        return (*this);

    }  // End of method 'EquationSystemEx::addEquation2Source(...)'


    // Remove all Equation objects from this EquationSystemEx.
    EquationSystemEx& EquationSystemEx::clearEquations()
    {
        m_SourceEquationMap.clear();

        m_IsPrepared = false;

        return (*this);

    }  // End of method 'EquationSystemEx::clearEquations(...)'


    /** Prepare this object to carry out its work.
     *
     * @param gData   GNSS data structure (GDS).
     */
    EquationSystemEx& EquationSystemEx::Prepare( gnssRinex& gData )
    {
        // First, create a temporary gnssDataMap
        gnssDataMap myGDSMap;

        // Get gData into myGDSMap
        myGDSMap.addGnssRinex( gData );

        // Call the map-enabled method, and return the result
        return (Prepare(myGDSMap));

    }  // End of method 'EquationSystemEx::Prepare()'



    /** Prepare this object to carry out its work.
     *
     * @param gdsMap     Map of GNSS data structures (GDS), indexed
     *                   by SourceID.
     */
    EquationSystemEx& EquationSystemEx::Prepare( gnssDataMap& gdsMap )
    {

        // Let's prepare current sources and satellites
        prepareCurrentSourceSat(gdsMap);

        // Store old unknowns parameters
        VariableSet tempOldUnknowns( m_pStateStore->getVariableSet() );

        // Prepare set of current unknowns and list of current equations
        prepareUnknownsAndEquations(gdsMap);

        // Set up index for Variables now
        setUpEquationIndex( tempOldUnknowns );

        // Set this object as "prepared"
        m_IsPrepared = true;

        return (*this);

    }  // End of method 'EquationSystemEx::Prepare()'



    /** Return the CURRENT number of variables, given the current equation
     *  system definition and the GDS's involved.
     *
     *  \warning You must call method Prepare() first, otherwise this
     *  method will throw an InvalidEquationSystemEx exception.
     */
    int EquationSystemEx::getCurrentNumVariables() const
        throw(InvalidEquationSystemEx)
    {
        // If the object as not ready, throw an exception
        if (!m_IsPrepared)
        {
            GPSTK_THROW(InvalidEquationSystemEx("EquationSystemEx is not prepared"));
        }

        return m_CurrentUnknowns.size();

    }  // End of method 'EquationSystemEx::getCurrentNumVariables()'



    /** Return the set containing variables being currently processed.
     *
     *  \warning You must call method Prepare() first, otherwise this
     *  method will throw an InvalidEquationSystemEx exception.
     */
    VariableSet EquationSystemEx::getCurrentUnknowns() const
        throw(InvalidEquationSystemEx)
    {
        // If the object as not ready, throw an exception
        if (!m_IsPrepared)
        {
            GPSTK_THROW(InvalidEquationSystemEx("EquationSystemEx is not prepared"));
        }

        return m_CurrentUnknowns;

    }  // End of method 'EquationSystemEx::getCurrentUnknowns()'



    /** Return the CURRENT number of sources, given the current equation
     *  system definition and the GDS's involved.
     *
     *  \warning You must call method Prepare() first, otherwise this
     *  method will throw an InvalidEquationSystemEx exception.
     */
    int EquationSystemEx::getCurrentNumSources() const
        throw(InvalidEquationSystemEx)
    {
        // If the object as not ready, throw an exception
        if (!m_IsPrepared)
        {
            GPSTK_THROW(InvalidEquationSystemEx("EquationSystemEx is not prepared"));
        }

        return m_CurrentSourceSet.size();

    }  // End of method 'EquationSystemEx::getCurrentNumSources()'



    /** Return the set containing sources being currently processed.
     *
     *  \warning You must call method Prepare() first, otherwise this
     *  method will throw an InvalidEquationSystemEx exception.
     */
    SourceIDSet EquationSystemEx::getCurrentSources() const
        throw(InvalidEquationSystemEx)
    {
        // If the object as not ready, throw an exception
        if (!m_IsPrepared)
        {
            GPSTK_THROW(InvalidEquationSystemEx("EquationSystemEx is not prepared"));
        }

        return m_CurrentSourceSet;

    }  // End of method 'EquationSystemEx::getCurrentSources()'



    /** Return the CURRENT number of satellites, given the current equation
     *  system definition and the GDS's involved.
     *
     *  \warning You must call method Prepare() first, otherwise this
     *  method will throw an InvalidEquationSystemEx exception.
     */
    int EquationSystemEx::getCurrentNumSats() const
        throw(InvalidEquationSystemEx)
    {
        // If the object as not ready, throw an exception
        if (!m_IsPrepared)
        {
            GPSTK_THROW(InvalidEquationSystemEx("EquationSystemEx is not prepared"));
        }

        return m_CurrentSatSet.size();

    }  // End of method 'EquationSystemEx::getCurrentNumSats()'



    /** Return the set containing satellites being currently processed.
     *
     *  \warning You must call method Prepare() first, otherwise this
     *  method will throw an InvalidEquationSystemEx exception.
     */
    SatIDSet EquationSystemEx::getCurrentSats() const
        throw(InvalidEquationSystemEx)
    {

        // If the object as not ready, throw an exception
        if (!m_IsPrepared)
        {
            GPSTK_THROW(InvalidEquationSystemEx("EquationSystemEx is not prepared"));
        }

        return m_CurrentSatSet;

    }  // End of method 'EquationSystemEx::getCurrentSats()'



    /// Setup Equation Index.
    void EquationSystemEx::setUpEquationIndex( VariableSet& oldVariableSet )
    {
        /// Setup Variable index in m_CurrentUnknowns
        int now_index  = 0;

        for( VariableSet::iterator nowIter = m_CurrentUnknowns.begin();
             nowIter != m_CurrentUnknowns.end();
             ++nowIter )
        {

            Variable& nowVar = (Variable&)(*nowIter);

            // set current index
            nowVar.setNowIndex( now_index++ );

            // set previous index
            VariableSet::iterator preIter = oldVariableSet.find( *nowIter );
            if( oldVariableSet.end() != preIter)
            {
                nowVar.setPreIndex( (*preIter).getNowIndex() );
            }
        }

        // Setup Variable index in m_CurrentEquationsList
        for( EquationList::iterator equIter = m_CurrentEquationsList.begin();
             equIter != m_CurrentEquationsList.end();
             ++equIter )
        {

            for( VarCoeffMap::iterator varIter = equIter->body.begin();
                 varIter != equIter->body.end();
                 ++varIter )
            {

                Variable& var = (Variable&)(*varIter);

                VariableSet::iterator it = m_CurrentUnknowns.find( varIter->first );
                if( it != m_CurrentUnknowns.end() )
                {
                    var.setPreIndex( (*it).getPreIndex() );
                    var.setNowIndex( (*it).getNowIndex() );
                }
            }
        }

    }  // End of method 'EquationSystemEx::setUpEquationIndex(...)'



    // Get current sources (SourceID's) and satellites (SatID's)
    void EquationSystemEx::prepareCurrentSourceSat( gnssDataMap& gdsMap )
    {
        // Clear "currentSatSet" and "currentSourceSet"
        m_CurrentSatSet.clear();
        m_CurrentSourceSet.clear();

        // Insert the corresponding SatID's in "currentSatSet"
        m_CurrentSatSet = gdsMap.getSatIDSet();

        // Insert the corresponding SourceID's in "currentSourceSet"
        m_CurrentSourceSet = gdsMap.getSourceIDSet();

    }  // End of method 'EquationSystemEx::prepareCurrentSourceSat()'



    // Prepare set of current unknowns and list of current equations
    void EquationSystemEx::prepareUnknownsAndEquations( gnssDataMap& gdsMap )
    {
        // Let's clear the current unknowns set
        m_CurrentUnknowns.clear();

        // Let's clear the current equations list
        m_CurrentEquationsList.clear();

//        cout << "EquationSystemEx::prepareUnknownsAndEquations()" << endl;

        TypeID satClock(TypeID::cdtSat);
        map<SatID, double> satClockMap;

        VariableSet satClkVarSet;

        // Let's retrieve the unknowns according to the
        // equation descriptions and 'gdsMap'
        // Iterate through all items in the gnssDataMap
        for( gnssDataMap::const_iterator it = gdsMap.begin();
             it != gdsMap.end();
             ++it )
        {
            // Then, iterate through corresponding 'sourceDataMap'
            for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
                 sdmIter != (*it).second.end();
                 ++sdmIter )
            {
                // finding this SourceID
                SourceEquationMap::iterator sourceEqnIter
                                            = m_SourceEquationMap.find( sdmIter->first );

                // if not found, go to next SourceID
                if( m_SourceEquationMap.end() == sourceEqnIter ) continue;

                // if found, extract data from gnssDataMap
                for( satTypeValueMap::const_iterator stvmIter = sdmIter->second.begin();
                     stvmIter != sdmIter->second.end();
                     ++stvmIter )
                {
                    if(stvmIter->second.find(satClock) != stvmIter->second.end())
                    {
                        satClockMap[stvmIter->first] = stvmIter->second.getValue(satClock);
                    }

                    for( EquationList::iterator equIter = sourceEqnIter->second.begin();
                         equIter != sourceEqnIter->second.end();
                         ++equIter )
                    {
                        // independent TypeID corresponding to this Equation
                        TypeID indType( (*equIter).header.indTerm.getType() );

                        // finding this TypeID in satTypeValueMap
                        typeValueMap::const_iterator tvmIter
                                            = stvmIter->second.find( indType );

                        // if not found, go to next Equation
                        if( stvmIter->second.end() == tvmIter ) continue;

                        // if found, process it

                        // copy current equation object
                        Equation equation( *equIter );

                        // set equation source
                        equation.header.equationSource = sdmIter->first;

                        // set equation satellite
                        equation.header.equationSat = stvmIter->first;

                        // clear all variables and coeffiences from equation
                        equation.clear();

                        // set the type value data
                        equation.header.typeValueData = stvmIter->second;

                        // visit all coefficient in equation
                        for( VarCoeffMap::const_iterator vcmIter = (*equIter).body.begin();
                             vcmIter != (*equIter).body.end();
                             ++vcmIter )
                        {
                            // copy the variable
                            Variable var( vcmIter->first );

                            // copy the coefficient
                            Coefficient coef( vcmIter->second );

                            // if variable is source-indexed, set SourceID
                            if( var.getSourceIndexed() )
                            {
                                var.setSource( sdmIter->first );
                            }

                            // if variable is satellite-indexed, set SatID
                            if( var.getSatIndexed() )
                            {
                                var.setSatellite( stvmIter->first );
                            }

                            if( var.getType() == TypeID::dcdtSat )
                            {
                                satClkVarSet.insert( var );
                            }
//                            cout << StringUtils::asString(var) << endl;

                            // insert to unknowns set
                            m_CurrentUnknowns.insert( var );

                            // add variable to equation
                            equation.addVariable( var, coef );

                        } // End of 'VarCoeffMap::const_iterator vcmIter = ...'

                        // add to currentEquationList
                        m_CurrentEquationsList.push_back( equation );

                    } // End of 'EquationList::const_iterator equIter = ...'

                } // End of 'satTypeValueMap::const_iterator stvmIter = ...'

            } // End of 'SourceDataMap::const_iterator sdmIter = ...'

        } // End of 'gnssDataMap::const_iterator gdmIter = ...'


        Variable satCS(TypeID::cdtSatSum);
        typeValueMap typeValueData;

        Equation equation;
        equation.header.constWeight = 2e0;
        equation.header.indTerm = satCS;

        double sum(0.0);

        for(VariableSet::iterator varIt = satClkVarSet.begin();
            varIt != satClkVarSet.end();
            ++varIt)
        {
            SatID sat( varIt->getSatellite() );
            map<SatID,double>::iterator scmIt( satClockMap.find(sat) );

            if( scmIt != satClockMap.end() )
            {
                equation.addVariable(*varIt, true, 1.0);
                sum += scmIt->second;
            }
        }

        typeValueData[TypeID::cdtSatSum] = sum;

        equation.header.typeValueData = typeValueData;

        m_CurrentEquationsList.push_back( equation );

}  // End of method 'EquationSystemEx::prepareUnknownsAndEquations()'


}  // End of namespace gpstk

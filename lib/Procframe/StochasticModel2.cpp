#pragma ident "$Id$"

/**
 * @file StochasticModel.cpp
 * Base class to define stochastic models, plus implementations
 * of common ones.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include "StochasticModel2.hpp"
#include "Variable.hpp"


using namespace std;


namespace gpstk
{

    /** Prepare q Matrix for relative variables.
     *
     * @param relVarVec   relative variable vector container
     * @param gData       gnssDataMap
     */
    void RandomWalkModel2::Prepare( std::vector<Variable>& relVarVec,
                                    gnssDataMap& gData )
    {

        if( gData.begin()->first != m_currentTime )
        {
            // setPreviousTime
            setPreviousTime( m_currentTime );

            // setCurrentTime
            setCurrentTime( gData.begin()->first );
        }

        for( vector<Variable>::iterator it = relVarVec.begin();
             it != relVarVec.end();
             ++it )
        {
            TypeID type( (*it).getType() );

            for( int i=0; i<m_relTypeIDVec.size(); i++)
            {
                if( type == m_relTypeIDVec[i] )
                {
                    m_phiMatrix(i,i) = 1.0;

                    // if variable is new, we set qValue to initial variance.
                    if( -1 == (*it).getPreIndex() )
                    {
                        m_qMatrix( i, i ) = (*it).getInitialVariance();
                    }
                    else
                    {
                        double dt( std::abs(m_currentTime - m_previousTime) );
                        m_qMatrix( i, i ) = m_qprimeVec[i] * dt;
                    }

                    break;
                }
            }
        }

    } // End of 'RandomWalkModel2::Prepare(...)'



    /** Prepare q Matrix for relative variables.
     *
     * @param relVarVec  relative variable vector.
     * @param gData      Data object holding the data.
     *
     */
    void SatOrbRandomWalkModel::Prepare( std::vector<Variable>& relVarVec,
                                         gnssDataMap& gData )
    {
        // Variable
        Variable var( *(relVarVec.begin()) );

        // SatID
        SatID sat( var.getSatellite() );

        if( satOrbit.empty() ) return;

        Vector<double> orbit( satOrbit[sat] );

        int size( orbit.size() );
        int numSRP( (size-42)/6 );

        for(int i=0; i<3; i++)
        {
            for(int j=0; j<3; j++)
            {
                m_phiMatrix(i+0,j+0) = orbit( 6+3*i+j);         // dr/dr0
                m_phiMatrix(i+0,j+3) = orbit(15+3*i+j);         // dr/dv0
                m_phiMatrix(i+3,j+0) = orbit(24+3*i+j);         // dv/dr0
                m_phiMatrix(i+3,j+3) = orbit(33+3*i+j);         // dv/dv0
            }

            for(int j=0; j<numSRP; j++)
            {
                m_phiMatrix(i+0,j+6) = orbit(42+numSRP*(i+0)+j);// dr/dp0
                m_phiMatrix(i+3,j+6) = orbit(42+numSRP*(i+3)+j);// dv/dp0
            }
        }

    } // End of 'SatOrbRandomWalkModel::Prepare(...)'


    /** Prepare q Matrix for relative variables.
     *
     * @param relVarVec   relative variable vector container
     * @param gData       gnssDataMap
     */
    void PhaseAmbiguityModel2::Prepare( std::vector<Variable>& relVarVec,
                                        gnssDataMap& gData )
    {
        // Variable
        Variable var( *(relVarVec.begin()) );

        // SatID
        SatID sat( var.getSatellite() );

        // SourceID
        SourceID source( var.getSource() );

        satTypeValueMap stvm;

        for( gnssDataMap::iterator iter = gData.begin();
             iter != gData.end();
             iter++ )
        {
            sourceDataMap::iterator sdmIter = (*iter).second.find( source );

            if( sdmIter != (*iter).second.end() )
            {
                stvm = (*sdmIter).second;
            }
        }

        // check cycle slip
        checkCS( sat, stvm, source );

        if( cycleSlip )
        {
            m_phiMatrix( 0, 0 ) = 0.0;
            m_qMatrix( 0, 0 ) = variance;
        }
        else
        {
            m_phiMatrix( 0, 0 ) = 1.0;
            m_qMatrix( 0, 0 ) = 0.0;
        }

    } // End of 'PhaseAmbiguityModel2::Prepare(...)'


    /* This method checks if a cycle slip happened.
     *
     * @param sat        Satellite.
     * @param data       Object holding the data.
     * @param source     Object holding the source of data.
     *
     */
    void PhaseAmbiguityModel2::checkCS( const SatID& sat,
                                        satTypeValueMap& data,
                                        const SourceID& source )
    {

        try
        {
            // By default, assume there is no cycle slip
            setCS(false);

            // Check if satellite is present at this epoch
            if( data.find(sat) == data.end() )
            {
                // If satellite is not present, declare CS and exit
                setCS(true);

                return;
            }

            if (!watchSatArc)
            {
                // In this case, we only use cycle slip flags
                // Check if there was a cycle slip
                if (data(sat)(csFlagType) > 0.0)
                {
                    setCS(true);
                }
            }
            else
            {
                // Check if this satellite has previous entries
                if( satArcMap[ source ].find(sat) == satArcMap[ source ].end() )
                {
                    // If it doesn't have an entry, insert one
                    satArcMap[ source ][ sat ] = 0.0;
                }

                // Check if arc number is different than arc number in storage
                if ( data(sat)(TypeID::satArc) != satArcMap[ source ][ sat ] )
                {
                    setCS(true);
                    satArcMap[ source ][ sat ] = data(sat)(TypeID::satArc);
                }
            }
        }
        catch(Exception& e)
        {
            setCS(true);
        }

        return;

    } // End of method 'PhaseAmbiguityModel::checkCS()'


      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for zenital wet
       * tropospheric delay is set to 3e-8 m*m/s (equivalent to about
       * 1.0 cm*cm/h).
       *
       */
    TropoRandomWalkModel2& TropoRandomWalkModel2::setQprime(double qp)
    {

         // Look at each source being currently managed
        for( std::map<SourceID, tropModelData>::iterator it = tmData.begin();
             it != tmData.end();
             ++it )
        {
            // Assign new process spectral density value
            (*it).second.qprime = qp;
        }

        return (*this);

    }  // End of method 'TropoRandomWalkModel::setQprime()'



      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
    void TropoRandomWalkModel2::Prepare( std::vector<Variable>& relVarVec,
                                         gnssDataMap& gData )
    {

        Variable var( *(relVarVec.begin()) );

        // First, get current source
        SourceID source( var.getSource() );

        // Second, let's update current epoch for this source
        if( tmData[source].currentTime != gData.begin()->first )
        { setCurrentTime(source, gData.begin()->first ); }

        // Third, compute Q value
        double dt;
        if(tmData[source].previousTime == CommonTime::BEGINNING_OF_TIME)
        {
            m_qMatrix( 0, 0 ) = 1.0;
        }
        else
        {
            dt = tmData[source].currentTime - tmData[source].previousTime;
            m_qMatrix( 0, 0 ) = tmData[source].qprime * std::abs(dt);
        }

        // Fourth, prepare for next iteration updating previous epoch
        if( tmData[source].currentTime != gData.begin()->first )
        { setPreviousTime(source, tmData[source].currentTime); }

        return;

    }  // End of method 'TropoRandomWalkModel2::Prepare()'


}  // End of namespace gpstk

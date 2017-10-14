#pragma ident "$Id$"

/**
 * @file ComputeSatPCenter.cpp
 * This class computes the satellite antenna phase correction, in meters.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================


#include "ComputeSatPCenter.hpp"

using namespace std;

namespace gpstk
{

    // Returns a string identifying this object.
    std::string ComputeSatPCenter::getClassName() const
    { return "ComputeSatPCenter"; }


      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ComputeSatPCenter::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
        throw(ProcessingException)
    {

        try
        {

            // Compute Sun position at this epoch
            SunPosition sunPosition;
            Triple sunPos(sunPosition.getPosition(time));

            // Define a Triple that will hold satellite position, in ECEF
            Triple satPos(0.0, 0.0, 0.0);

            SatIDSet satRejectedSet;

            SatID sat;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                sat = it->first;

                // Use ephemeris if satellite position is not already computed
                if( ( (*it).second.find(TypeID::satX) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satY) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satZ) == (*it).second.end() ) )
                {

                    if(pEphStore==NULL)
                    {
                        // If ephemeris is missing, then remove all satellites
                        satRejectedSet.insert( sat );
                        continue;
                    }
                    else
                    {
                        // Try to get satellite position
                        // if it is not already computed
                        try
                        {
                            // For our purposes, position at receive time
                            // is fine enough
                            Xvt satPosVel(pEphStore->getXvt( sat, time ));

                            // If everything is OK, then continue processing.
                            satPos[0] = satPosVel.x.theArray[0];
                            satPos[1] = satPosVel.x.theArray[1];
                            satPos[2] = satPosVel.x.theArray[2];
                        }
                        catch(...)
                        {
                            // If satellite is missing, then schedule it
                            // for removal
                            satRejectedSet.insert( sat );
                            continue;
                        }
                    }
                }
                else
                {
                    // Get satellite position out of GDS
                    satPos[0] = (*it).second[TypeID::satX];
                    satPos[1] = (*it).second[TypeID::satY];
                    satPos[2] = (*it).second[TypeID::satZ];

                }  // End of 'if( ( (*it).second.find(TypeID::satX) == ...'


                // Let's get the satellite antenna phase correction value in
                // meters, and insert it in the GNSS data structure.

                Vector<double> satPCenter
                            = getSatPCenter((*it).first, time, satPos, sunPos);

                (*it).second[TypeID::satPCenterX] = satPCenter[0];
                (*it).second[TypeID::satPCenterY] = satPCenter[1];
                (*it).second[TypeID::satPCenterZ] = satPCenter[2];
                (*it).second[TypeID::satPCenter]  = satPCenter[3];

            }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );

            GPSTK_THROW(e);
        }

    }  // End of method 'ComputeSatPCenter::Process()'


     /** Returns a gnssDataMap object, adding the new data generated
      *  when calling this object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& ComputeSatPCenter::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        SourceIDSet sourceRejectedSet;

        SourceID source;
        string station;

        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            CommonTime epoch( gdmIt->first );
            epoch.setTimeSystem( TimeSystem::Unknown );

            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                source = sdmIt->first;
                station = source.sourceName;

                if(pMSCStore == NULL)
                {
                    sourceRejectedSet.insert( source );
                    continue;
                }

                MSCData mscData;
                try
                {
                    mscData = pMSCStore->findMSC(station,epoch);
                }
                catch(...)
                {
                    sourceRejectedSet.insert( source );
                    continue;
                }

                nominalPos = mscData.coordinates;

                Process( gdmIt->first, sdmIt->second );
            }
        }

        gData.removeSourceID( sourceRejectedSet );

        return gData;

    }  // End of method 'ComputeSatPCenter::Process()'


      /* Compute the value of satellite antenna phase correction, in meters.
       * @param satid     Satellite ID
       * @param time      Epoch of interest
       * @param satpos    Satellite position, as a Triple
       * @param sunpos    Sun position, as a Triple
       *
       * @return Satellite antenna phase correction, in meters.
       */
    Vector<double> ComputeSatPCenter::getSatPCenter( const SatID& satid,
                                                     const CommonTime& time,
                                                     const Triple& satpos,
                                                     const Triple& sunpos )
    {

        // Unitary vector from satellite to geocenter (ECEF)
        Triple rk( ( (-1.0)*(satpos.unitVector()) ) );

        // Vector from Sun to satellite (ECEF)
        Triple sat2sun( sunpos - satpos );

        // Unitary vector from Sun to satellite (ECEF)
        Triple ri( sat2sun.unitVector() );

        // rj = rk x ri: Rotation axis of solar panels (ECEF)
        Triple rj(rk.cross(ri));

        // Redefine ri: ri = rj x rk (ECEF)
        ri = rj.cross(rk);

        // Let's convert ri to an unitary vector (ECEF)
        ri = ri.unitVector();

        // Get vector from geocenter to station
        Triple stapos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

        // Compute unitary vector from satellite to station
        Triple rrho( (satpos-stapos).unitVector() );


        // This variable that will hold the correction, 0.0 by default
        Vector<double> satPCcorr(4, 0.0);


        // Check is Antex antenna information is available or not, and if
        // available, whether satellite phase center information is absolute
        // or relative
        bool absoluteModel( false );
        if( pAntexReader != NULL )
        {
            absoluteModel = pAntexReader->isAbsolute();
        }

        if( absoluteModel )
        {

            // We will need the elevation, in degrees. It is found using
            // dot product and the corresponding unitary angles

            double temp( rrho.dot(rk) );
            if(temp > +1.0) temp = +1.0;

            double nadir( std::acos( temp ) * RAD_TO_DEG );

            double elev( 0.0 );

            // Get satellite information in Antex format. Currently this
            // only works for GPS and GLONASS.
            if( satid.system == SatID::systemGPS )
            {
                std::stringstream sat;
                sat << "G";
                if( satid.id < 10 ) sat << "0";
                sat << satid.id;

                // Get satellite antenna information out of AntexReader object
                Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

                double zen2( antenna.getZen2() );

                nadir = (nadir>zen2) ? zen2 : nadir;

                elev = 90.0 - nadir;

                try
                {
                    // Get antenna eccentricity for frequency "G01" (L1), in
                    // satellite reference system.
                    // NOTE: It is NOT in ECEF, it is in UEN!!!
                    Triple satAnt( antenna.getAntennaEccentricity( Antenna::G01) );

                    // Now, get the phase center variation.
                    Triple var( antenna.getAntennaPCVariation( Antenna::G01, elev) );

                    // We must substract them
                    satAnt = satAnt;

                    // Change to ECEF
                    Triple satAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

                    satPCcorr[0] = satAntenna.theArray[0];
                    satPCcorr[1] = satAntenna.theArray[1];
                    satPCcorr[2] = satAntenna.theArray[2];

                    // Projection of "satAntenna" vector to line of sight vector rrho
                    // This correction is interpreted as an "advance" in the signal,
                    // instead of a delay. Therefore, it has negative sign
                    satPCcorr[3] =  rrho.dot(satAntenna) + var[0];

                    return satPCcorr;

                }
                catch(...)
                {
                    return satPCcorr;
                }
            }
            // Check if this satellite belongs to GLONASS system
            else if(satid.system == SatID::systemGLONASS)
            {
                std::stringstream sat;
                sat << "R";
                if( satid.id < 10 ) sat << "0";
                sat << satid.id;

                // Get satellite antenna information out of AntexReader object
                Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

                double zen2( antenna.getZen2() );

                nadir = (nadir>zen2) ? zen2 : nadir;

                elev = 90.0 - nadir;

                try
                {
                    // Get antenna offset for frequency "R01" (GLONASS), in
                    // satellite reference system.
                    // NOTE: It is NOT in ECEF, it is in UEN!!!
                    Triple satAnt( antenna.getAntennaEccentricity( Antenna::R01) );

                    // Now, get the phase center variation.
                    Triple var( antenna.getAntennaPCVariation( Antenna::R01, elev) );

                    // We must substract them
                    satAnt = satAnt;

                    // Change to ECEF
                    Triple satAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

                    satPCcorr[0] = satAntenna.theArray[0];
                    satPCcorr[1] = satAntenna.theArray[1];
                    satPCcorr[2] = satAntenna.theArray[2];

                    // Project "satAntenna" vector to line of sight vector rrho
                    // This correction is interpreted as an "advance" in the signal,
                    // instead of a delay. Therefore, it has negative sign
                    satPCcorr[3] = rrho.dot(satAntenna) - var[0];

                    return satPCcorr;
                }
                catch(...)
                {
                    return satPCcorr;
                }
            }
            // Check if this satellite belongs to Galileo system
            else if( satid.system == SatID::systemGalileo )
            {
                std::stringstream sat;
                sat << "E";
                if( satid.id < 10 ) sat << "0";
                sat << satid.id;

                // Get satellite antenna information out of AntexReader object
                Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

                double zen2( antenna.getZen2() );

                nadir = (nadir>zen2) ? zen2 : nadir;

                elev = 90.0 - nadir;

                try
                {
                    // Get antenna offset for frequency "E01" (Galileo), in
                    // satellite reference system.
                    // NOTE: It is NOT in ECEF, it is in UEN!!!
                    Triple satAnt( antenna.getAntennaEccentricity( Antenna::E01) );

                    // Now, get the phase center variation.
                    Triple var( antenna.getAntennaPCVariation( Antenna::E01, elev) );

                    // We must substract them
                    satAnt = satAnt - var;

                    // Change to ECEF
                    Triple satAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

                    satPCcorr[0] = satAntenna.theArray[0];
                    satPCcorr[1] = satAntenna.theArray[1];
                    satPCcorr[2] = satAntenna.theArray[2];

                    // Project "satAntenna" vector to line of sight vector rrho
                    // This correction is interpreted as an "advance" in the signal,
                    // instead of a delay. Therefore, it has negative sign
                    satPCcorr[3] = rrho.dot(satAntenna) - var[0];

                    return satPCcorr;
                }
                catch(...)
                {
                    return satPCcorr;
                }
            }
            // Check if this satellite belongs to BeiDou system
            else if( satid.system == SatID::systemBDS )
            {
                std::stringstream sat;
                sat << "C";
                if( satid.id < 10 ) sat << "0";
                sat << satid.id;

                if(satid.id > 30) return satPCcorr;

                // Get satellite antenna information out of AntexReader object
                Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

                double zen2( antenna.getZen2() );

                nadir = (nadir>zen2) ? zen2 : nadir;

                elev = 90.0 - nadir;

                try
                {
                    // Get antenna offset for frequency "C01" (BeiDou), in
                    // satellite reference system.
                    // NOTE: It is NOT in ECEF, it is in UEN!!!
                    Triple satAnt( antenna.getAntennaEccentricity( Antenna::C01) );

                    // Now, get the phase center variation.
                    Triple var( antenna.getAntennaPCVariation( Antenna::C01, elev) );

                    // Change to ECEF
                    Triple satAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

                    satPCcorr[0] = satAntenna.theArray[0];
                    satPCcorr[1] = satAntenna.theArray[1];
                    satPCcorr[2] = satAntenna.theArray[2];

                    // Project "satAntenna" vector to line of sight vector rrho
                    // This correction is interpreted as an "advance" in the signal,
                    // instead of a delay. Therefore, it has negative sign
                    satPCcorr[3] = rrho.dot(satAntenna) - var[0];

                    return satPCcorr;
                }
                catch(...)
                {
                    return satPCcorr;
                }
            }
            // Check if this satellite belongs to QZSS system
            else if( satid.system == SatID::systemQZSS )
            {
                std::stringstream sat;
                sat << "J";
                if( satid.id < 10 ) sat << "0";
                sat << satid.id;

                // Get satellite antenna information out of AntexReader object
                Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

                double zen2( antenna.getZen2() );

                nadir = (nadir>zen2) ? zen2 : nadir;

                elev = 90.0 - nadir;

                try
                {
                    // Get antenna offset for frequency "J01" (QZSS), in
                    // satellite reference system.
                    // NOTE: It is NOT in ECEF, it is in UEN!!!
                    Triple satAnt( antenna.getAntennaEccentricity( Antenna::J01) );

                    // Now, get the phase center variation.
                    Triple var( antenna.getAntennaPCVariation( Antenna::J01, elev) );

                    // Change to ECEF
                    Triple satAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

                    satPCcorr[0] = satAntenna.theArray[0];
                    satPCcorr[1] = satAntenna.theArray[1];
                    satPCcorr[2] = satAntenna.theArray[2];

                    // Project "satAntenna" vector to line of sight vector rrho
                    // This correction is interpreted as an "advance" in the signal,
                    // instead of a delay. Therefore, it has negative sign
                    satPCcorr[3] = rrho.dot(satAntenna) - var[0];

                    return satPCcorr;
                }
                catch(...)
                {
                    return satPCcorr;
                }
            }
            else
            {
                // In this case no correction will be computed
                return satPCcorr;
            }
        }
        else
        {
            return satPCcorr;
        }

    }  // End of method 'ComputeSatPCenter::getSatPCenter()'


}  // End of namespace gpstk

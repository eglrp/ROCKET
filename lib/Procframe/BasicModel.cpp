#pragma ident "$Id$"

/**
 * @file BasicModel.cpp
 * This is a class to compute the basic parts of a GNSS model, i.e.:
 * Geometric distance, relativity correction, satellite position and
 * velocity at transmission time, satellite elevation and azimuth, etc.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
//
//============================================================================


#include "BasicModel.hpp"
#include "Rinex3EphemerisStore2.hpp"
#include "YDSTime.hpp"
#include "constants.hpp"

using namespace std;

namespace gpstk
{


      // Returns a string identifying this object.
   std::string BasicModel::getClassName() const
   { return "BasicModel"; }



      /* Explicit constructor taking as input reference
       * station coordinates.
       *
       * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
       * (Latitude, Longitude, Altitude), but defaults to Cartesian.
       *
       * Also, a pointer to GeoidModel may be specified, but default is
       * NULL (in which case WGS84 values will be used).
       *
       * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
       * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
       * @param cRx   third coordinate [ Z, height above ellipsoid or
       *              radius, in meters ]
       * @param s     coordinate system (default is Cartesian, may be set
       *              to Geodetic).
       * @param ell   pointer to EllipsoidModel
       * @param frame Reference frame associated with this position
       */
    BasicModel::BasicModel( const double& aSta,
                            const double& bSta,
                            const double& cSta,
                            Position::CoordinateSystem s,
                            EllipsoidModel *ell,
                            ReferenceFrame frame )
    {

        minElev = 10.0;
        nominalPos = Position(aSta, bSta, cSta, s, ell, frame);
        pEphStore = NULL;
        pMSCStore = NULL;
        defaultObsOfGPS = TypeID::C1;
        defaultObsOfGAL = TypeID::C1;
        defaultObsOfBDS = TypeID::C2;
        useTGDOfGPS = false;
        useTGDOfGAL = false;
        useTGDOfBDS = false;

    }  // End of 'BasicModel::BasicModel()'


      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
    BasicModel::BasicModel(const Position& staPos)
    {

        minElev = 10.0;
        nominalPos = staPos;
        pEphStore = NULL;
        pMSCStore = NULL;
        defaultObsOfGPS = TypeID::C1;
        defaultObsOfGAL = TypeID::C1;
        defaultObsOfBDS = TypeID::C2;
        useTGDOfGPS = false;
        useTGDOfGAL = false;
        useTGDOfBDS = false;

    }  // End of 'BasicModel::BasicModel()'



      /* Explicit constructor, taking as input reference station
       * coordinates, ephemeris to be used, default observable
       * and whether TGD will be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param applyTGD      Whether or not C1 observable will be
       *                      corrected from TGD effect or not.
       *
       */
    BasicModel::BasicModel( const Position& StaPos,
                            XvtStore<SatID>& ephStore,
                            MSCStore& mscStore,
                            const TypeID& dObsOfGPS,
                            const TypeID& dObsOfGAL,
                            const TypeID& dObsOfBDS,
                            const bool& applyTGDOfGPS,
                            const bool& applyTGDOfGAL,
                            const bool& applyTGDOfBDS )
    {

        minElev = 10.0;
        nominalPos = StaPos;
        pEphStore = &ephStore;
        pMSCStore = &mscStore;
        defaultObsOfGPS = dObsOfGPS;
        defaultObsOfGAL = dObsOfGAL;
        defaultObsOfBDS = dObsOfBDS;
        useTGDOfGPS = applyTGDOfGPS;
        useTGDOfGAL = applyTGDOfGAL;
        useTGDOfBDS = applyTGDOfBDS;

    }  // End of 'BasicModel::BasicModel()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& BasicModel::Process( const CommonTime& time,
                                          satTypeValueMap& gData )
        throw(ProcessingException)
    {

        try
        {
            SatIDSet satRejectedSet;

            SatID sat;

            TypeID defaultObs;

            // Loop through all the satellites
            satTypeValueMap::iterator stv;
            for( stv = gData.begin(); stv != gData.end(); ++stv )
            {
                sat = stv->first;

                // Scalar to hold temporal value
                double obs(0.0);

                if(sat.system == SatID::systemGPS)
                {
                    defaultObs = defaultObsOfGPS;
                }
                else if(sat.system == SatID::systemGalileo)
                {
                    defaultObs = defaultObsOfGAL;
                }
                else if(sat.system == SatID::systemBDS)
                {
                    defaultObs = defaultObsOfBDS;
                }

                obs = (*stv).second(defaultObs);


                // A lot of the work is done by a CorrectedEphemerisRange object
                CorrectedEphemerisRange cerange;

                try
                {
                    // Compute most of the parameters
                    cerange.ComputeAtTransmitTime( time,
                                                   obs,
                                                   nominalPos,
                                                   sat,
                                                   *(getEphStore()) );
                }
                catch(InvalidRequest& e)
                {
                    // If some problem appears, then schedule this satellite
                    // for removal
                    satRejectedSet.insert( sat );
                    continue;    // Skip this SV if problems arise
                }

                // Let's test if satellite has enough elevation over horizon
                if ( nominalPos.elevationGeodetic(cerange.svPosVel) < minElev )
                {
                    // Mark this satellite if it doesn't have enough elevation
                    satRejectedSet.insert( sat );
                    continue;
                }

                if(satClock.find(sat) == satClock.end())
                    satClock[sat] = cerange.svclkbias;

                // rho, relativity, elevation, azimuth
                (*stv).second[TypeID::rho] = cerange.rawrange;
                (*stv).second[TypeID::relativity] = -cerange.relativity;
                (*stv).second[TypeID::elevation] = cerange.elevationGeodetic;
                (*stv).second[TypeID::azimuth] = cerange.azimuthGeodetic;

//                cout << sat;

//                cout << setprecision(3)
//                     << setw(15) << cerange.svPosVel.x[0]
//                     << setw(15) << cerange.svPosVel.x[1]
//                     << setw(15) << cerange.svPosVel.x[2]
//                     << setprecision(6)
//                     << setw(15) << cerange.svPosVel.v[0]
//                     << setw(15) << cerange.svPosVel.v[1]
//                     << setw(15) << cerange.svPosVel.v[2]
//                     << endl;

//                cout << setprecision(3)
//                     << setw(15) << cerange.rawrange
//                     << setw(15) << cerange.relativity
//                     << endl;

                // Let's insert satellite position at transmit time
                (*stv).second[TypeID::satX] = cerange.svPosVel.x[0];
                (*stv).second[TypeID::satY] = cerange.svPosVel.x[1];
                (*stv).second[TypeID::satZ] = cerange.svPosVel.x[2];

                // Let's insert satellite velocity at transmit time
                (*stv).second[TypeID::satVX] = cerange.svPosVel.v[0];
                (*stv).second[TypeID::satVY] = cerange.svPosVel.v[1];
                (*stv).second[TypeID::satVZ] = cerange.svPosVel.v[2];

                // Let's insert satellite clock bias at transmit time
                (*stv).second[TypeID::cdtSat] = cerange.svclkbias;

                // Let's insert station position at receive time
                (*stv).second[TypeID::staX] = nominalPos.X();
                (*stv).second[TypeID::staY] = nominalPos.Y();
                (*stv).second[TypeID::staZ] = nominalPos.Z();

                // Let's insert partials for satellite position at transmit time
                (*stv).second[TypeID::dSatX] = -cerange.cosines[0];
                (*stv).second[TypeID::dSatY] = -cerange.cosines[1];
                (*stv).second[TypeID::dSatZ] = -cerange.cosines[2];

                // Let's insert partials for station position at receive time
                (*stv).second[TypeID::dStaX] = cerange.cosines[0];
                (*stv).second[TypeID::dStaY] = cerange.cosines[1];
                (*stv).second[TypeID::dStaZ] = cerange.cosines[2];


            } // End of loop for(stv = gData.begin()...

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }   // End of try...
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );

            GPSTK_THROW(e);
        }

    }  // End of method 'BasicModel::Process()'


     /** Returns a gnssDataMap object, adding the new data generated when
      *  calling a modeling object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& BasicModel::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        SourceIDSet sourceRejectedSet;

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
                SourceID source( sdmIt->first );
                string station( source.sourceName );

//                cout << source << endl;

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

    }  // End of method 'BasicModel::Process()'

}  // End of namespace gpstk

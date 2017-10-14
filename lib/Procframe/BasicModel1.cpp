#pragma ident "$Id$"

/**
 * @file BasicModel1.cpp
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


#include "BasicModel1.hpp"
#include "Rinex3EphemerisStore2.hpp"
#include "YDSTime.hpp"
#include "constants.hpp"

using namespace std;

namespace gpstk
{


      // Returns a string identifying this object.
   std::string BasicModel1::getClassName() const
   { return "BasicModel1"; }



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
    BasicModel1::BasicModel1( const double& aSta,
                              const double& bSta,
                              const double& cSta,
                              Position::CoordinateSystem s,
                              EllipsoidModel *ell,
                              ReferenceFrame frame )
    {

        minElev = 10.0;
        nominalPos = Position(aSta, bSta, cSta, s, ell, frame);
        pSP3Store = NULL;
        pBCEStore = NULL;
        pMSCStore = NULL;
        defaultObsOfGPS = TypeID::C1;
        defaultObsOfGAL = TypeID::C1;
        defaultObsOfBDS = TypeID::C2;
        useTGDOfGPS = false;
        useTGDOfGAL = false;
        useTGDOfBDS = false;

    }  // End of 'BasicModel1::BasicModel1()'


      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
    BasicModel1::BasicModel1(const Position& staPos)
    {

        minElev = 10.0;
        nominalPos = staPos;
        pSP3Store = NULL;
        pBCEStore = NULL;
        pMSCStore = NULL;
        defaultObsOfGPS = TypeID::C1;
        defaultObsOfGAL = TypeID::C1;
        defaultObsOfBDS = TypeID::C2;
        useTGDOfGPS = false;
        useTGDOfGAL = false;
        useTGDOfBDS = false;

    }  // End of 'BasicModel1::BasicModel1()'



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
    BasicModel1::BasicModel1( const Position& StaPos,
                              XvtStore<SatID>& sp3Store,
                              XvtStore<SatID>& bceStore,
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
        pSP3Store = &sp3Store;
        pBCEStore = &bceStore;
        pMSCStore = &mscStore;
        defaultObsOfGPS = dObsOfGPS;
        defaultObsOfGAL = dObsOfGAL;
        defaultObsOfBDS = dObsOfBDS;
        useTGDOfGPS = applyTGDOfGPS;
        useTGDOfGAL = applyTGDOfGAL;
        useTGDOfBDS = applyTGDOfBDS;

    }  // End of 'BasicModel1::BasicModel1()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& BasicModel1::Process( const CommonTime& time,
                                           satTypeValueMap& gData )
        throw(ProcessingException)
    {

        try
        {
            // Satellites to be rejected
            SatIDSet satRejectedSet;

            // Satellite
            SatID sat;

            // Type
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
                CorrectedEphemerisRange cerange1;
                CorrectedEphemerisRange cerange2;

                try
                {
                    // Compute most of the parameters
                    cerange1.ComputeAtTransmitTime( time,
                                                    obs,
                                                    nominalPos,
                                                    sat,
                                                    *pSP3Store );
                    cerange2.ComputeAtTransmitTime( time,
                                                    obs,
                                                    nominalPos,
                                                    sat,
                                                    *pBCEStore );
                }
                catch(InvalidRequest& e)
                {
                    // If some problem appears, then schedule this satellite
                    // for removal
                    satRejectedSet.insert( sat );
                    continue;    // Skip this SV if problems arise
                }

                // Let's test if satellite has enough elevation over horizon
                if ( nominalPos.elevationGeodetic(cerange1.svPosVel) < minElev )
                {
                    // Mark this satellite if it doesn't have enough elevation
                    satRejectedSet.insert( sat );
                    continue;
                }


                if(satClock.find(sat) == satClock.end())
                    satClock[sat] = cerange2.svclkbias;


                // Computing Total Group Delay (TGD - meters), if possible
                double tempTGD(0.0);
//                double tempTGD(getTGDCorrections( time,
//                                                  (*pEphStore),
//                                                  sat,
//                                                  defaultObs) );

                // coefficients of satellite position in ITRS
                (*stv).second[TypeID::dSatX] = -cerange1.cosines[0];
                (*stv).second[TypeID::dSatY] = -cerange1.cosines[1];
                (*stv).second[TypeID::dSatZ] = -cerange1.cosines[2];

                // coefficients of station position in ITRS
                (*stv).second[TypeID::dStaX] = cerange1.cosines[0];
                (*stv).second[TypeID::dStaY] = cerange1.cosines[1];
                (*stv).second[TypeID::dStaZ] = cerange1.cosines[2];

                // Now we have to add the new values to the data structure
                (*stv).second[TypeID::rho] = cerange1.rawrange;
                (*stv).second[TypeID::relativity] = -cerange1.relativity;
                (*stv).second[TypeID::elevation] = cerange1.elevationGeodetic;
                (*stv).second[TypeID::azimuth] = cerange1.azimuthGeodetic;

                // Let's insert satellite position at transmission time
                (*stv).second[TypeID::satX] = cerange1.svPosVel.x[0];
                (*stv).second[TypeID::satY] = cerange1.svPosVel.x[1];
                (*stv).second[TypeID::satZ] = cerange1.svPosVel.x[2];

                // Let's insert satellite velocity at transmission time
                (*stv).second[TypeID::satVX] = cerange1.svPosVel.v[0];
                (*stv).second[TypeID::satVY] = cerange1.svPosVel.v[1];
                (*stv).second[TypeID::satVZ] = cerange1.svPosVel.v[2];

                // Let's insert satellite clock bias at transmission time
                (*stv).second[TypeID::cdtSat] = cerange2.svclkbias;

                // Let's insert station position
                (*stv).second[TypeID::staX] = nominalPos.X();
                (*stv).second[TypeID::staY] = nominalPos.Y();
                (*stv).second[TypeID::staZ] = nominalPos.Z();

                // Do not apply TGD corrections to observable here,
                // since observation equations contain TGD terms.
                if(sat.system == SatID::systemGPS)
                {
                    if( defaultObs == TypeID::C1 )
                    {
                        (*stv).second[TypeID::instC1] = tempTGD;
                    }
                    else if( defaultObs == TypeID::C2 )
                    {
                        (*stv).second[TypeID::instC2] = tempTGD;
                    }
                    else if( defaultObs == TypeID::PC )
                    {
                        (*stv).second[TypeID::instPC] = tempTGD;
                    }
                }
                else if(sat.system == SatID::systemGalileo)
                {
                    if( defaultObs == TypeID::C1 )
                    {
                        (*stv).second[TypeID::instC1] = tempTGD;
                    }
                    else if( defaultObs == TypeID::C5 )
                    {
                        (*stv).second[TypeID::instC5] = tempTGD;
                    }
                    else if( defaultObs == TypeID::PC )
                    {
                        (*stv).second[TypeID::instPC] = tempTGD;
                    }
                }
                else if(sat.system == SatID::systemBDS)
                {
                    if( defaultObs == TypeID::C2 )
                    {
                        (*stv).second[TypeID::instC2] = tempTGD;
                    }
                    else if( defaultObs == TypeID::C7 )
                    {
                        (*stv).second[TypeID::instC7] = tempTGD;
                    }
                    else if( defaultObs == TypeID::PC )
                    {
                        (*stv).second[TypeID::instPC] = tempTGD;
                    }
                }

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

    }  // End of method 'BasicModel1::Process()'


     /** Returns a gnssDataMap object, adding the new data generated when
      *  calling a modeling object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& BasicModel1::Process(gnssDataMap& gData)
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

    }  // End of method 'BasicModel1::Process()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int BasicModel1::setInitialStaPosition( const double& aSta,
                                           const double& bSta,
                                           const double& cSta,
                                           Position::CoordinateSystem s,
                                           EllipsoidModel *ell,
                                           ReferenceFrame frame )
   {

      try
      {
         Position stapos( aSta, bSta, cSta, s, ell, frame );
         setInitialStaPosition(stapos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'BasicModel1::setInitialStaPosition()'



      // Method to set the initial (a priori) position of receiver.
   int BasicModel1::setInitialStaPosition(const Position& StaCoordinates)
   {

      try
      {
         nominalPos = StaCoordinates;
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'BasicModel1::setInitialStaPosition()'



      // Method to set the initial (a priori) position of receiver.
   int BasicModel1::setInitialStaPosition()
   {
      try
      {
         Position stapos(0.0, 0.0, 0.0, Position::Cartesian, NULL);
         setInitialStaPosition(stapos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'BasicModel1::setInitialStaPosition()'



      // Method to get TGD corrections.
   double BasicModel1::getTGDCorrections( CommonTime Tr,
                                          const XvtStore<SatID>& Eph,
                                          SatID sat,
                                          TypeID type )
      throw()
   {

      try
      {
          const Rinex3EphemerisStore2& bce =
                            dynamic_cast<const Rinex3EphemerisStore2&>(Eph);

          if(sat.system == SatID::systemGPS)
          {
              const GPSEphemerisStore& gps = bce.getGPSEphemerisStore();

              Tr.setTimeSystem(TimeSystem::GPS);
              double tgd = gps.findEphemeris(sat,Tr).Tgd * C_MPS;

              if(type == TypeID::C1)        return (    1.0    * tgd );
              else if(type == TypeID::C2)   return ( GAMMA_GPS * tgd );
              else if(type == TypeID::PC)   return (       0.0       );
          }
          else if(sat.system == SatID::systemGalileo)
          {
              const GalEphemerisStore& gal = bce.getGalEphemerisStore();

              Tr.setTimeSystem(TimeSystem::GAL);
              double tgda = gal.findEphemeris(sat,Tr).Tgda * C_MPS;

              if(type == TypeID::C1)        return (    1.0    * tgda );
              else if(type == TypeID::C5)   return ( GAMMA_GAL * tgda );
              else if(type == TypeID::PC)   return (       0.0        );
          }
          else if(sat.system == SatID::systemBDS)
          {
              const BDSEphemerisStore& bds = bce.getBDSEphemerisStore();

              Tr.setTimeSystem(TimeSystem::BDT);
              double tgd13 = bds.findEphemeris(sat,Tr).Tgd13 * C_MPS;
              double tgd23 = bds.findEphemeris(sat,Tr).Tgd23 * C_MPS;

              if(type == TypeID::C2)        return ( 1.0 * tgd13 );
              else if(type == TypeID::C7)   return ( 1.0 * tgd23 );
              else if(type == TypeID::PC)   return (     0.0     );
          }

      }
      catch(...)
      {
          cout << "Exception " << sat << ' ' << type << endl;
          return 0.0;
      }

   }  // End of method 'BasicModel1::getTGDCorrections()'


}  // End of namespace gpstk

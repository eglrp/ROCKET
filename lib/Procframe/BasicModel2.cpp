#pragma ident "$Id$"

/**
 * @file BasicModel2.cpp
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


#include "BasicModel2.hpp"
#include "constants.hpp"


using namespace std;


namespace gpstk
{

    // Returns a string identifying this object.
    std::string BasicModel2::getClassName() const
    { return "BasicModel2"; }


      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& BasicModel2::Process( const CommonTime& time,
                                           satTypeValueMap& gData )
        throw(ProcessingException)
    {

        try
        {
            CommonTime utc( pRefSys->GPS2UTC(time) );

            SatIDSet satRejectedSet;

            SatID sat;

            double rho(0.0), rel(0.0);

            // satellite state
            Vector<double> state;

            Vector<double> rsat(3,0.0), vsat(3,0.0);

            // satellite pos/vel in ICRS
            posSatECI.resize(3,0.0);
            velSatECI.resize(3,0.0);

            // satellite pos/vel in ITRS
            posSatECEF.resize(3,0.0);
            velSatECEF.resize(3,0.0);

            double dclock = cdtSource/C_MPS;

            Matrix<double> c2tRaw, c2tDot;

            // Loop through all the satellites
            for( satTypeValueMap::iterator it = gData.begin();
                 it != gData.end();
                 ++it )
            {
                sat = it->first;

                if(satState.find(sat) == satState.end())
                {
                    satRejectedSet.insert( sat );
                    continue;
                }

                // satellite state
                state = satState[sat];

                // rsat_c, vsat_c
                for(int i=0; i<3; ++i) rsat(i) = state(i+0);
                for(int i=0; i<3; ++i) vsat(i) = state(i+3);

                // satellite pos at (t-dT) in ICRS
                posSatECI = rsat;

                // satellite vel at (t-dT) in ICRS
                velSatECI = vsat;

                // norm( rsat(t) )
                double r( norm(rsat) );
                double r2( r * r );
                double r3( r2 * r );
                double k( GM_EARTH/r3 );

                // initial value of rho
                double rho( norm(posSatECI - posSourceECI) );
                double dlight( rho/C_MPS );

                double drel( 0.0 );

                // rho/c + dtr + drel
                double dT(dlight + dclock - drel);
                double dT2 = dT * dT;

                // iterate to get rsat(t-dT)/vsat(t-dT)
                for(int i=0; i<3; ++i)
                {
//                    cout << fixed << setprecision(10);
//                    cout << setw(15) << dT << endl;

                    velSatECI = vsat + k*rsat*dT;
                    posSatECI = rsat - vsat*dT - 0.5*k*rsat*dT2;

                    rho = norm(posSatECI - posSourceECI);
                    dlight = rho/C_MPS;

                    drel = -2.0*( posSatECI(0)*velSatECI(0) +
                                  posSatECI(1)*velSatECI(1) +
                                  posSatECI(2)*velSatECI(2) )/C_MPS/C_MPS;

                    dT = dlight + dclock - drel;
                }

                // R(t-dT), dR(t-dT)
                c2tRaw = pRefSys->C2TMatrix(utc-dT);
                c2tDot = pRefSys->dC2TMatrix(utc-dT);

                // satellite pos at (t-dT) in ITRS
                posSatECEF = c2tRaw * posSatECI;

                // satellite vel at (t-dT) in ITRS
                velSatECEF = c2tRaw * velSatECI + c2tDot * posSatECI;

                Position posSat( posSatECEF(0),
                                 posSatECEF(1),
                                 posSatECEF(2) );

                double elevationGeodetic( posSource.elevationGeodetic(posSat) );
                double azimuthGeodetic( posSource.azimuthGeodetic(posSat) );

                if(elevationGeodetic < minElev)
                {
                    satRejectedSet.insert( sat );
                    continue;
                }

                if(satPosECI.find(sat) == satPosECI.end())
                    satPosECI[sat] = posSatECI;

                if(satPosECEF.find(sat) == satPosECEF.end())
                    satPosECEF[sat] = posSatECEF;

                Matrix<double> I = ident<double>(3);

                // rsat_c(t) * rsat_c(t)'
                Matrix<double> rrt( outer(rsat,rsat) );

                // drsat_c(t-dT) / drsat_c(t)
                Matrix<double> phi( I - 0.5*k*(I-3.0*rrt/r2)*dT2 );

                // rho
                rho = norm(posSatECI - posSourceECI);

                // drho / drsat_c(t-dT)
                Vector<double> par( (posSatECI-posSourceECI)/rho );

                // drho / drsat_c(t)
                Vector<double> rho_rsat( phi * par );

                // drho / dvsat_c(t)
                Vector<double> rho_vsat( -par * dT );

//                cout << sat;

//                cout << setprecision(3)
//                     << setw(15) << posSatECEF[0]
//                     << setw(15) << posSatECEF[1]
//                     << setw(15) << posSatECEF[2]
//                     << setprecision(6)
//                     << setw(15) << velSatECEF[0]
//                     << setw(15) << velSatECEF[1]
//                     << setw(15) << velSatECEF[2]
//                     << endl;

//                cout << setprecision(3)
//                     << setw(15) << rho
//                     << setw(15) << drel * C_MPS
//                     << endl;

                // rho, elevation, azimuth, cdtSta
                it->second[TypeID::rho] = rho;
                it->second[TypeID::relativity] = -drel * C_MPS;
                it->second[TypeID::elevation] = elevationGeodetic;
                it->second[TypeID::azimuth] = azimuthGeodetic;
                it->second[TypeID::cdtSta] = cdtSource;

                // satellite position in ITRS
                it->second[TypeID::satX] = posSatECEF[0];
                it->second[TypeID::satY] = posSatECEF[1];
                it->second[TypeID::satZ] = posSatECEF[2];

                // satellite position in ICRS
                it->second[TypeID::satXJ2K] = posSatECI[0];
                it->second[TypeID::satYJ2K] = posSatECI[1];
                it->second[TypeID::satZJ2K] = posSatECI[2];

                // satellite velocity in ITRS
                it->second[TypeID::satVX] = velSatECEF[0];
                it->second[TypeID::satVY] = velSatECEF[1];
                it->second[TypeID::satVZ] = velSatECEF[2];

                // satellite velocity in ICRS
                it->second[TypeID::satVXJ2K] = velSatECI[0];
                it->second[TypeID::satVYJ2K] = velSatECI[1];
                it->second[TypeID::satVZJ2K] = velSatECI[2];

                // coefficients of satellite position in ICRS
                it->second[TypeID::dSatX] = rho_rsat[0];
                it->second[TypeID::dSatY] = rho_rsat[1];
                it->second[TypeID::dSatZ] = rho_rsat[2];

                // coefficients of satellite velocity in ICRS
                it->second[TypeID::dSatVX] = rho_vsat[0];
                it->second[TypeID::dSatVY] = rho_vsat[1];
                it->second[TypeID::dSatVZ] = rho_vsat[2];


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

    }  // End of method 'BasicModel2::Process()'


     /** Returns a gnssDataMap object, adding the new data generated when
      *  calling a modeling object.
      *
      * @param gData    Data object holding the data.
      */
    gnssDataMap& BasicModel2::Process(gnssDataMap& gData)
        throw(ProcessingException)
    {
        SourceIDSet sourceRejectedSet;

        SourceID source;

        sourcePosECI.clear();
        sourcePosECEF.clear();

        if(pRefSys == NULL) return gData;
        if(pMSCStore == NULL) return gData;
        if(sourceClock.empty()) return gData;
        if(satState.empty()) return gData;

        Matrix<double> t2cRaw(3,3,0.0);

        for( gnssDataMap::iterator gdmIt = gData.begin();
             gdmIt != gData.end();
             ++gdmIt )
        {
            CommonTime gps( gdmIt->first );
            CommonTime utc( pRefSys->GPS2UTC(gps) );

            for( sourceDataMap::iterator sdmIt = gdmIt->second.begin();
                 sdmIt != gdmIt->second.end();
                 ++sdmIt )
            {
                posSourceECI.resize(3,0.0);
                posSourceECEF.resize(3,0.0);

                source = sdmIt->first;
                string station( source.sourceName );

//                cout << source << endl;

                MSCData mscData;

                gps.setTimeSystem( TimeSystem::Unknown );
                try
                {
                    mscData = pMSCStore->findMSC(station,gps);
                }
                catch(...)
                {
                    sourceRejectedSet.insert(source);
                    continue;
                }
                gps.setTimeSystem( TimeSystem::GPS );

                posSource = mscData.coordinates;

                if(sourceClock.find(source) == sourceClock.end())
                {
                    sourceRejectedSet.insert(source);
                    continue;
                }

                posSourceECEF[0] = posSource.X();
                posSourceECEF[1] = posSource.Y();
                posSourceECEF[2] = posSource.Z();

                cdtSource = sourceClock[source];

                t2cRaw = pRefSys->T2CMatrix(utc-cdtSource/C_MPS);

                posSourceECI = t2cRaw * posSourceECEF;

                Process( gdmIt->first, sdmIt->second );

                sourcePosECI[source] = posSourceECI;
                sourcePosECEF[source] = posSourceECEF;
            }
        }

        gData.removeSourceID( sourceRejectedSet );

        return gData;

    }  // End of method 'BasicModel2::Process()'


}  // End of namespace gpstk

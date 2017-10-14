#pragma ident "$Id$"

/**
 * @file BasicModel.hpp
 * This is a class to compute the basic parts of a GNSS model, i.e.:
 * Geometric distance, relativity correction, satellite position and
 * velocity at transmission time, satellite elevation and azimuth, etc.
 */

#ifndef GPSTK_BASICMODEL_HPP
#define GPSTK_BASICMODEL_HPP

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



#include "ProcessingClass.hpp"
#include "EphemerisRange.hpp"
#include "XvtStore.hpp"
#include "MSCStore.hpp"


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /** This is a class to compute the basic parts of a GNSS model, like
       *  geometric distance, relativity correction, satellite position and
       *  velocity at transmission time, satellite elevation and azimuth, etc.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Load the precise ephemeris file
       *   SP3EphemerisStore sp3Eph;
       *   sp3Eph.loadFile("igs11513.sp3");
       *
       *      // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *      // Some more code and definitions here...
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *      // Set defaults of models. A typical C1-based modeling is used
       *   BasicModel model( nominalPos, sp3Eph );
       *
       *   while(rin >> gRin)
       *   {
       *
       *         // Apply the model on the GDS
       *      gRin >> model;
       *   }
       *
       * @endcode
       *
       * The "BasicModel" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute
       * its model: Geometric distance, relativity delay, satellite position
       * at transmission time, satellite elevation and azimuth, etc.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the extra data inserted along their
       * corresponding satellites. Be warned that if a given satellite does
       * not have ephemeris information, it will be summarily deleted
       * from the data structure.
       *
       */
    class BasicModel : public ProcessingClass
    {
    public:

         /// Default constructor. Observable C1 will be used for computations
         /// and satellites with elevation less than 10 degrees will be
         /// deleted.
        BasicModel()
            : minElev(10.0), pEphStore(NULL), pMSCStore(NULL),
              defaultObsOfGPS(TypeID::C1), useTGDOfGPS(false),
              defaultObsOfGAL(TypeID::C1), useTGDOfGAL(false),
              defaultObsOfBDS(TypeID::C2), useTGDOfBDS(false)
        {
            nominalPos = Position(0.0,0.0,0.0,Position::Cartesian,NULL);
        };


         /** Explicit constructor taking as input reference
          *  station coordinates.
          *
          * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
          * (Latitude, Longitude, Altitude), but defaults to Cartesian.
          *
          * Also, a pointer to EllipsoidModel may be specified, but default is
          * NULL (in which case WGS84 values will be used).
          *
          * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
          * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
          * @param cRx   third coordinate [ Z, height above ellipsoid or
          *              radius, in meters ]
          * @param s     coordinate system (default is Cartesian, may be set
          *              to Geodetic).
          * @param ell   pointer to EllipsoidModel.
          * @param frame Reference frame associated with this position.
          */
        BasicModel( const double& aRx,
                    const double& bRx,
                    const double& cRx,
                    Position::CoordinateSystem s = Position::Cartesian,
                    EllipsoidModel *ell = NULL,
                    ReferenceFrame frame = ReferenceFrame::Unknown );


         /// Explicit constructor, taking as input a Position object
         /// containing reference station coordinates.
        BasicModel(const Position& staCoordinates);


         /** Explicit constructor, taking as input reference station
          *  coordinates, ephemeris to be used and whether TGD will
          *  be computed or not.
          *
          * @param RxCoordinates Reference station coordinates.
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param applyTGD      Whether or not C1 observable will be
          *                      corrected from TGD effect.
          *
          */
        BasicModel( const Position& StaCoordinates,
                    XvtStore<SatID>& ephStore,
                    MSCStore& mscStore,
                    const TypeID& dObsOfGPS = TypeID::C1,
                    const TypeID& dObsOfGAL = TypeID::C1,
                    const TypeID& dObsOfBDS = TypeID::C2,
                    const bool& applyTGDOfGPS = false,
                    const bool& applyTGDOfGAL = false,
                    const bool& applyTGDOfBDS = false );


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            throw(ProcessingException);


         /** Returns a gnssSatTypeValue object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssRinex object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException)
        { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnssDataMap object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
        virtual gnssDataMap& Process(gnssDataMap& gData)
            throw(ProcessingException);


         /// Method to get satellite elevation cut-off angle. By default, it
         /// is set to 10 degrees.
        virtual double getMinElev() const
        { return minElev; };


         /// Method to set satellite elevation cut-off angle. By default, it
         /// is set to 10 degrees.
        virtual BasicModel& setMinElev(double newElevation)
        { minElev = newElevation; return (*this); };


         /// Method to get the default observable for computations.
        virtual TypeID getDefaultObs( const SatID::SatelliteSystem& sys
                                                    = SatID::systemGPS ) const
        {
            if(sys == SatID::systemGPS)
                return defaultObsOfGPS;
            else if(sys == SatID::systemGalileo)
                return defaultObsOfGAL;
            else if(sys == SatID::systemBDS)
                return defaultObsOfBDS;
        };


         /** Method to set the default observable for computations.
          *
          * @param type      TypeID object to be used by default
          */
        virtual BasicModel& setDefaultObs( const SatID::SatelliteSystem& sys
                                                        = SatID::systemGPS,
                                           const TypeID& type = TypeID::PC )
        {
            if(sys == SatID::systemGPS)
                defaultObsOfGPS = type;
            else if(sys == SatID::systemGalileo)
                defaultObsOfGAL = type;
            else if(sys == SatID::systemBDS)
                defaultObsOfBDS = type;

            return (*this);
        };


         /// Method to get use of TGD
        virtual bool getUseTGD( const SatID::SatelliteSystem sys
                                                = SatID::systemGPS )
        {
            if(sys == SatID::systemGPS)
                return useTGDOfGPS;
            else if(sys == SatID::systemGalileo)
                return useTGDOfGAL;
            else if(sys == SatID::systemBDS)
                return useTGDOfBDS;
        };

         /// Method to set use of TGD
        virtual BasicModel& setUseTGD( bool tgd,
                                       const SatID::SatelliteSystem& sys
                                            = SatID::systemGPS )
        {
            if(sys == SatID::systemGPS)
                useTGDOfGPS = tgd;
            else if(sys == SatID::systemGalileo)
                useTGDOfGAL = tgd;
            else if(sys == SatID::systemBDS)
                useTGDOfBDS = tgd;

            return (*this);
        };


         /// Method to get Position to be used with GNSS data structures.
        virtual Position getNominalPosition() const
        { return nominalPos; };


         /** Method to set the station position to be used with GNSS
          *  data structures.
          *
          * @param pos     Position object to be used
          */
        virtual BasicModel& setNominalPosition(const Position& pos)
        { nominalPos = pos; return (*this); };


         /// Method to get a pointer to the default XvtStore<SatID> to be used
         /// with GNSS data structures.
        virtual XvtStore<SatID>* getEphStore() const
        { return pEphStore; };


         /** Method to set the default XvtStore<SatID> to be used with GNSS
          *  data structures.
          *
          * @param ephem     XvtStore<SatID> object to be used by default
          */
        virtual BasicModel& setEphStore(XvtStore<SatID>& ephStore)
        { pEphStore = &ephStore; return (*this); };


         /// Returns a pointer to the MSCStore object currently in use.
        virtual MSCStore *getMSCStore(void) const
        { return pMSCStore; };


         /** Sets MSCStore object to be used.
          *
          * @param msc  MSCStore object.
          */
        virtual BasicModel& setMSCStore(MSCStore& msc)
        { pMSCStore = &msc; return (*this); };


        /// Get satellite clock map.
        virtual satValueMap getSatClock() const
        { return satClock; };


         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor.
        virtual ~BasicModel() {};


    protected:

         /// The elevation cut-off angle for accepted satellites.
         /// By default it is set to 10 degrees.
        double minElev;

         /// Pointer to XvtStore<SatID> object
        XvtStore<SatID>* pEphStore;

         /// Station position
        Position nominalPos;

         /// Pointer to MSCStore object
        MSCStore* pMSCStore;

         /// Satellite clock map
        satValueMap satClock;


         /// Default observable to be used
        TypeID defaultObsOfGPS;
        TypeID defaultObsOfGAL;
        TypeID defaultObsOfBDS;


         /// Whether the TGD effect will be applied to C1 observable or not.
        bool useTGDOfGPS;
        bool useTGDOfGAL;
        bool useTGDOfBDS;

    }; // End of class 'BasicModel'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_BASICMODEL_HPP

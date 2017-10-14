#pragma ident "$Id$"

/**
 * @file TypeID.hpp
 * gpstk::TypeID - This class was written taking as inspiration ObsID. The
 * objective of this class is to create an index able to represent any type
 * of observation, correction, model parameter or other data value of interest
 * for GNSS data processing. This class is extensible in run-time, so the
 * programmer may add indexes on-demand.
 */

#ifndef GPSTK_TYPEID_HPP
#define GPSTK_TYPEID_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include "Rinex3ObsHeader.hpp"
#include "RinexObsID.hpp"

namespace gpstk
{

    /** This class creates an index able to represent any type of observation,
     *  correction, model parameter or other data value of interest for GNSS
     *  data processing.
     *
     * This class is extensible in run-time, so the programmer may add
     * indexes on-demand. For instance, in order to create a new TypeID
     * object referring INS-related data, and with "Inertial" as description
     * string, you may write the following:
     *
     * @code
     *    TypeID INS = TypeID::newValueType("Inertial");
     * @endcode
     *
     * Or using the constructor:
     *
     * @code
     *    TypeID INS(TypeID::newValueType("Inertial"));
     * @endcode
     *
     * From now on, you'll be able to use INS as TypeID when you need to
     * refer to inertial system data.
     *
     */
    class TypeID
    {
    public:

        /// The type of the data value.
        enum ValueType
        {
            Unknown,

            // C1*
            C1C,
            C1S,
            C1L,
            C1X,
            C1P,
            C1W,
            C1Y,
            C1M,
            C1A,
            C1B,
            C1Z,

            // L1*
            L1C,
            L1S,
            L1L,
            L1X,
            L1P,
            L1W,
            L1Y,
            L1M,
            L1N,
            L1A,
            L1B,
            L1Z,

            // D1*
            D1C,
            D1S,
            D1L,
            D1X,
            D1P,
            D1W,
            D1Y,
            D1M,
            D1N,
            D1A,
            D1B,
            D1Z,

            // S1*
            S1C,
            S1S,
            S1L,
            S1X,
            S1P,
            S1W,
            S1Y,
            S1M,
            S1N,
            S1A,
            S1B,
            S1Z,

            // C2*
            C2C,
            C2D,
            C2S,
            C2L,
            C2X,
            C2P,
            C2W,
            C2Y,
            C2M,
            C2I,
            C2Q,

            // L2*
            L2C,
            L2D,
            L2S,
            L2L,
            L2X,
            L2P,
            L2W,
            L2Y,
            L2M,
            L2N,
            L2I,
            L2Q,

            // D2*
            D2C,
            D2D,
            D2S,
            D2L,
            D2X,
            D2P,
            D2W,
            D2Y,
            D2M,
            D2N,
            D2I,
            D2Q,

            // S2*
            S2C,
            S2D,
            S2S,
            S2L,
            S2X,
            S2P,
            S2W,
            S2Y,
            S2M,
            S2N,
            S2I,
            S2Q,

            // C3*
            C3I,
            C3Q,
            C3X,

            // L3*
            L3I,
            L3Q,
            L3X,

            // D3*
            D3I,
            D3Q,
            D3X,

            // S3*
            S3I,
            S3Q,
            S3X,

            // C5*
            C5I,
            C5Q,
            C5X,
            C5A,
            C5B,
            C5C,

            // L5*
            L5I,
            L5Q,
            L5X,
            L5A,
            L5B,
            L5C,

            // D5*
            D5I,
            D5Q,
            D5X,
            D5A,
            D5B,
            D5C,

            // S5*
            S5I,
            S5Q,
            S5X,
            S5A,
            S5B,
            S5C,

            // C6*
            C6A,
            C6B,
            C6C,
            C6X,
            C6Z,
            C6S,
            C6L,
            C6I,
            C6Q,

            // L6*
            L6A,
            L6B,
            L6C,
            L6X,
            L6Z,
            L6S,
            L6L,
            L6I,
            L6Q,

            // D6*
            D6A,
            D6B,
            D6C,
            D6X,
            D6Z,
            D6S,
            D6L,
            D6I,
            D6Q,

            // S6*
            S6A,
            S6B,
            S6C,
            S6X,
            S6Z,
            S6S,
            S6L,
            S6I,
            S6Q,

            // C7*
            C7I,
            C7Q,
            C7X,

            // L7*
            L7I,
            L7Q,
            L7X,

            // D7*
            D7I,
            D7Q,
            D7X,

            // S7*
            S7I,
            S7Q,
            S7X,

            // C8*
            C8I,
            C8Q,
            C8X,

            // L8*
            L8I,
            L8Q,
            L8X,

            // D8*
            D8I,
            D8Q,
            D8X,

            // S8*
            S8I,
            S8Q,
            S8X,

            // C9*
            C9A,
            C9B,
            C9C,
            C9X,

            // L9*
            L9A,
            L9B,
            L9C,
            L9X,

            // D9*
            D9A,
            D9B,
            D9C,
            D9X,

            // S9*
            S9A,
            S9B,
            S9C,
            S9X,


            // LLI
            LLI1,
            LLI2,
            LLI3,
            LLI5,
            LLI6,
            LLI7,
            LLI8,
            LLI9,

            // SSI
            SSI1,
            SSI2,
            SSI3,
            SSI5,
            SSI6,
            SSI7,
            SSI8,
            SSI9,


            // C*
            C1,
            C2,
            C3,
            C5,
            C6,
            C7,
            C8,
            C9,
            P1,
            P2,
            P3,
            P5,
            P6,
            P7,
            P8,
            P9,

            // L*
            L1,
            L2,
            L3,
            L4,
            L5,
            L6,
            L7,
            L8,
            L9,

            // D*
            D1,
            D2,
            D3,
            D5,
            D6,
            D7,
            D8,
            D9,

            // S*
            S1,
            S2,
            S3,
            S5,
            S6,
            S7,
            S8,
            S9,

            // Cycle Slip
            CSL1,
            CSL2,
            CSL3,
            CSL5,
            CSL6,
            CSL7,
            CSL8,
            CSL9,

            // Linear Combination
            PC,            ///< Ionosphere-free code combination
            LC,            ///< Ionosphere-free phase combination
            PI,            ///< Ionospheric code combination
            LI,            ///< Ionospheric phase combination
            PW,            ///< Wide-lane code combination
            LW,            ///< Wide-lane phase combination
            PN,            ///< Narrow-lane code combination
            LN,            ///< Narrow-lane phase combination
            MW,            ///< Melbourne-Wubbena combination
            Pdelta,        ///< Narrow-lane code combination
            Ldelta,        ///< Wide-lane phase combination
            MWubbena,      ///< Melbourne-Wubbena combination
            WL,            ///< Wide-lane combination(+1*L1-1*L2)
            WL1,           ///< Wide-lane combination(-1*L1+2*L2)
            WL2,           ///< Wide-lane combination(-2*L1+3*L2)
            WL3,           ///< Wide-lane combination(-3*L1+4*L2)
            WL4,           ///< Wide-lane combination(+4*L1-5*L2)
            EWL,           ///< Wide-lane combination(-7*L1+9*L2)

            Q1,            ///< precise code with minus ionospheric delays in L1
            Q2,            ///< precise code with minus ionospheric delays in L2
            Q3,            ///< precise code with minus ionospheric delays in L3
            Q5,            ///< precise code with minus ionospheric delays in L5
            Q6,            ///< precise code with minus ionospheric delays in L6
            Q7,            ///< precise code with minus ionospheric delays in L7
            Q8,            ///< precise code with minus ionospheric delays in L8
            Q9,            ///< precise code with minus ionospheric delays in L9


            // Prefit Residual
            prefitC1,
            prefitC2,
            prefitC3,
            prefitC5,
            prefitC6,
            prefitC7,
            prefitC8,
            prefitC9,
            prefitC,
            prefitCWithoutClock,
            prefitCWithSatClock,
            prefitCWithStaClock,
            prefitCForPOD,

            prefitP1,
            prefitP2,

            prefitL1,
            prefitL2,
            prefitL3,
            prefitL5,
            prefitL6,
            prefitL7,
            prefitL8,
            prefitL9,
            prefitL,
            prefitLWithoutClock,
            prefitLWithSatClock,
            prefitLWithStaClock,
            prefitLForPOD,

            prefitPC,
            prefitLC,
            prefitPI,
            prefitLI,
            prefitPW,
            prefitLW,
            prefitPN,
            prefitLN,
            prefitMW,
            prefitWL,
            prefitWL1,
            prefitWL2,
            prefitWL3,
            prefitWL4,
            prefitEWL,


            // Postfit Residual
            postfitC1,
            postfitC2,
            postfitC3,
            postfitC5,
            postfitC6,
            postfitC7,
            postfitC8,
            postfitC9,
            postfitC,

            postfitP1,
            postfitP2,

            postfitL1,
            postfitL2,
            postfitL3,
            postfitL5,
            postfitL6,
            postfitL7,
            postfitL8,
            postfitL9,
            postfitL,

            postfitPC,
            postfitLC,
            postfitPI,
            postfitLI,
            postfitPW,
            postfitLW,
            postfitPN,
            postfitLN,
            postfitMW,
            postfitGRAPHIC1,
            postfitGRAPHIC2,
            postfitGRAPHIC3,
            postfitGRAPHIC5,
            postfitGRAPHIC6,
            postfitGRAPHIC7,
            postfitGRAPHIC8,
            postfitGRAPHIC9,
            postfitWL,
            postfitWL1,
            postfitWL2,
            postfitWL3,
            postfitWL4,
            postfitEWL,

            // Phase Ambiguity
            BL1,
            BL2,
            BL3,
            BL5,
            BL6,
            BL7,
            BL8,
            BL9,
            BLC,
            BWL,
            BWL1,
            BWL2,
            BWL3,
            BWL4,


            // Uncalibrated Phase Delay
            updSatL1,
            updSatL2,
            updSatL3,
            updSatL5,
            updSatL6,
            updSatL7,
            updSatL8,
            updSatL9,
            updSatLC,
            updSatWL,
            updSatMW,

            // Instrumental Delay
            instC1,
            instC2,
            instC3,
            instC5,
            instC6,
            instC7,
            instC8,
            instC9,
            instPC,
            instPN,

            instL1,
            instL2,
            instL3,
            instL5,
            instL6,
            instL7,
            instL8,
            instL9,
            instLC,
            instLW,

            instMW,


            // ISB
            ISB_GLO,
            ISB_GAL,
            ISB_BDS,


            // Geometric Distance
            rho,
            rhoDot,
            rhoDot2,

            // Relativity
            relativity,


            // Gravitational Delay
            gravDelay,


            // Tropospheric Delay
            tropo,          ///< Vertical tropospheric delay, total
            tropoWeight,    ///< Vertical tropospheric delay weight, total
            dryTropo,       ///< Vertical tropospheric delay, dry component
            dryMap,         ///< Tropospheric mapping function, dry component
            wetTropo,       ///< Vertical tropospheric delay, wet component
            wetMap,         ///< Tropospheric mapping function, wet component
            tropoSlant,     ///< Slant tropospheric delay, total


            // Ionospheric Delay
            iono,           ///< Vertical ionospheric delay
            ionoTEC,        ///< Total Electron Content (in TECU), 1TECU = 1e+16 electrons per m**2
            ionoMap,        ///< Ionospheric mapping function
            ionoMap2,       ///< Ionospheric mapping function for second order ionospheric delay
            ionoL1,         ///< Slant ionospheric delay, frequency L1
            ionoL2,         ///< Slant ionospheric delay, frequency L2
            ionoL3,         ///< Slant ionospheric delay, frequency L3
            ionoL5,         ///< Slant ionospheric delay, frequency L5
            ionoL6,         ///< Slant ionospheric delay, frequency L6
            ionoL7,         ///< Slant ionospheric delay, frequency L7
            ionoL8,         ///< Slant ionospheric delay, frequency L8
            ionoL9,         ///< Slant ionospheric delay, frequency L9
            ionoPN,         ///< Slant ionospheric delay, frequency PN
            ionoLW,         ///< Slant ionospheric delay, frequency LW
            ionoL1Weight,   ///< Weight for slant ionospheric delay on frequency L1


            // Epoch difference
            diffPrefitC1,
            diffPrefitC2,
            diffPrefitC,

            diffPrefitL1,
            diffPrefitL2,
            diffPrefitL,

            diffWetTropo,


            // Wind-Up Effect (in radians)
            windUp,


            // Eclipse Indicator
            eclipse,


            // Satellite Antenna Phase Center Correction
            satPCenter,     ///< Satellite antenna phase center correction, ray direction
            satPCenterX,    ///< Satellite antenna phase center correction, X component
            satPCenterY,    ///< Satellite antenna phase center correction, Y component
            satPCenterZ,    ///< Satellite antenna phase center correction, Z component


            // Station Antenna Offset
            AntOffU,        ///< Station antenna offset, U component
            AntOffN,        ///< Station antenna offset, N component
            AntOffE,        ///< Station antenna offset, E component


            // Satellite Elevation and Azimuth
            elevation,
            azimuth,


            // Satellite Arc Number
            satArc,


            // Weight
            weight,         ///< Weight assigned to a given observation
            weightC,        ///< Weight assigned to a given code observation
            weightL,        ///< Weight assigned to a given phase observation


            // Station State
            staX,           ///< Station position, X component
            staY,           ///< Station position, Y component
            staZ,           ///< Station position, Z component
            staVX,          ///< Station velocity, X component
            staVY,          ///< Station velocity, Y component
            staVZ,          ///< Station velocity, Z component
            staAX,          ///< Station acceleration, X component
            staAY,          ///< Station acceleration, Y component
            staAZ,          ///< Station acceleration, Z component
            staLat,         ///< Station position, Latitude component
            staLon,         ///< Station position, Longitude component
            staH,           ///< Station position, Height component
            staVLat,        ///< Station velocity, Latitude component
            staVLon,        ///< Station velocity, Longitude component
            staVH,          ///< Station velocity, Height component
            staALat,        ///< Station acceleration, Latitude component
            staALon,        ///< Station acceleration, Longitude component
            staAH,          ///< Station acceleration, Height component

            // Station Clock
            cdtSta,         ///< Station clock offset
            cdtStaDot,      ///< Station clock drift
            cdtStaDot2,     ///< Station clock drift rate


            // Satellite State
            satX,           ///< Satellite position in ECEF, X component
            satY,           ///< Satellite position in ECEF, Y component
            satZ,           ///< Satellite position in ECEF, Z component
            satVX,          ///< Satellite velocity in ECEF, X component
            satVY,          ///< Satellite velocity in ECEF, Y component
            satVZ,          ///< Satellite velocity in ECEF, Z component
            satAX,          ///< Satellite acceleration in ECEF, X component
            satAY,          ///< Satellite acceleration in ECEF, Y component
            satAZ,          ///< Satellite acceleration in ECEF, Z component
            satXJ2K,        ///< Satellite position in ECI, X component
            satYJ2K,        ///< Satellite position in ECI, Y component
            satZJ2K,        ///< Satellite position in ECI, Z component
            satVXJ2K,       ///< Satellite velocity in ECI, X component
            satVYJ2K,       ///< Satellite velocity in ECI, Y component
            satVZJ2K,       ///< Satellite velocity in ECI, Z component
            satAXJ2K,       ///< Satellite acceleration in ECI, X component
            satAYJ2K,       ///< Satellite acceleration in ECI, Y component
            satAZJ2K,       ///< Satellite acceleration in ECI, Z component

            // Satellite Clock
            cdtSat,         ///< Satellite clock offset
            cdtSatDot,      ///< Satellite clock drift
            cdtSatDot2,     ///< Satellite clock drift rate
            cdtSatSum,      ///< Satellite clock offset sum


            // Correction or Coefficient of Station State
            dStaX,
            dStaY,
            dStaZ,
            dStaLat,
            dStaLon,
            dStaH,

            // Correction or Coefficient of Station Clock
            dcdtSta,
            dcdtStaDot,
            dcdtStaDot2,


            // Correction or Coefficient of Satellite State
            dSatX,
            dSatY,
            dSatZ,
            dSatVX,
            dSatVY,
            dSatVZ,

            // Correction or Coefficient of Satellite Clock
            dcdtSat,
            dcdtSatDot,
            dcdtSatDot2,


            // Solar Radiation Pressure (SRP) Coefficients
            SRPC1,
            SRPC2,
            SRPC3,
            SRPC4,
            SRPC5,
            SRPC6,
            SRPC7,
            SRPC8,
            SRPC9,

            // Correction or Coefficient of SRP Coefficients
            dSRPC1,
            dSRPC2,
            dSRPC3,
            dSRPC4,
            dSRPC5,
            dSRPC6,
            dSRPC7,
            dSRPC8,
            dSRPC9,


            // Earth Ratation Parameters (ERPs)
            xpole,          ///< X Pole
            ypole,          ///< Y Pole
            xpoleRate,      ///< X Pole Rate
            ypoleRate,      ///< Y Pole Rate
            UT1mUTC,        ///< UT1mUTC
            LOD,            ///< LOD

            // Correction or Coefficient of ERPs
            dxpole,
            dypole,
            dxpoleRate,
            dypoleRate,
            dUT1mUTC,
            dLOD,


            // Handy dummy types for non-standard processing
            dummy0,         ///< Generic, undefined type #0
            dummy1,         ///< Generic, undefined type #1
            dummy2,         ///< Generic, undefined type #2
            dummy3,         ///< Generic, undefined type #3
            dummy4,         ///< Generic, undefined type #4
            dummy5,         ///< Generic, undefined type #5
            dummy6,         ///< Generic, undefined type #6
            dummy7,         ///< Generic, undefined type #7
            dummy8,         ///< Generic, undefined type #8
            dummy9,         ///< Generic, undefined type #9

            Last,      ///< used to extend this...
            Placeholder = Last+1000
        };


        /// empty constructor, creates an invalid object
        TypeID()
            : type(Unknown)
        {};


        /** Explicit constructor
         *
         * @param vt   ValueType for the new TypeID. If you want to use the
         *             next available ValueType, generate it using the
         *             'newValueType()' method, as indicated in the example in
         *             the documentation.
         */
        TypeID(ValueType vt)
            : type(vt)
        {};


        /** Explicit constructor
         *
         * @param name string name for ValueType, first search tString and
         *             then search mapUserTypeID, if it is not found, create
         *             it.
         */
        TypeID(std::string name);


        /// Equality requires all fields to be the same
        virtual bool operator==(const TypeID& right) const
        { return type==right.type; };


        /// This ordering is somewhat arbitrary but is required to be able
        /// to use an TypeID as an index to a std::map. If an application
        /// needs some other ordering, inherit and override this function.
        virtual bool operator<(const TypeID& right) const
        { return type < right.type; };


        /// Inequality operator
        bool operator!=(const TypeID& right) const
        { return !(operator==(right)); };


        /// Greater than operator
        bool operator>(const TypeID& right) const
        {  return (!operator<(right) && !operator==(right)); };


        /// Less than or equal operator
        bool operator<=(const TypeID& right) const
        { return (operator<(right) || operator==(right)); };


        /// Greater than or equal operator
        bool operator>=(const TypeID& right) const
        { return !(operator<(right)); };


        /// Assignment operator
        virtual TypeID operator=(const TypeID& right);


        /// Convenience output method
        virtual std::ostream& dump(std::ostream& s) const;


        /// Returns true if this is a valid TypeID. Basically just
        /// checks that the enum is defined
        virtual bool isValid() const;


        /// Destructor
        virtual ~TypeID() {};


        /** Static method to add new TypeID's
         * @param s      Identifying string for the new TypeID
         */
        static ValueType newValueType(const std::string& s);


        /// Type of the value
        ValueType type;


        /// Map holding type descriptions
        static std::map< ValueType, std::string > tStrings;


    public:

        class Initializer
        {
        public:
            Initializer();
        };

        static Initializer TypeIDsingleton;

    public:

        /** Static method to get the user registered TypeID by name string
         * @param name      Identifying string for the new TypeID
         * @return          The desired TypeID
         */
        static TypeID byName(std::string name)
            throw(InvalidRequest);

        /** Static method to add new TypeID's by name string
         * @param name      Identifying string for the new TypeID
         * @param desc      Descriptions of the new TypeID
         * @return          The new TypeID
         */
        static TypeID regByName(std::string name,std::string desc);

        /// unregister a TypeID by it's name string
        static void unregByName(std::string name);

        /// unregister all TypeIDs registered by name string
        static void unregAll();

    private:

        /// Have user deined TypeIDs been registered ?
        static bool bUserTypeIDRegistered;

        /// Map holding user defined TypeIDs by a string
        static std::map<std::string,TypeID> mapUserTypeID;


    }; // End of class 'TypeID'



    namespace StringUtils
    {
        /// convert this object to a string representation
        std::string asString(const TypeID& p);
    }



    /// stream output for TypeID
    std::ostream& operator<<(std::ostream& s, const TypeID& p);

    int GetCarrierBand(const RinexObsID& roi);

    TypeID::ValueType ConvertToTypeID(const RinexObsID& roi,
                                      const RinexSatID& sat);

    TypeID::ValueType ConvertToTypeID(std::string &str);

}  // End of namespace gpstk

#endif   // GPSTK_TYPEID_HPP

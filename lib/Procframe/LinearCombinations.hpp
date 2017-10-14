#pragma ident "$Id$"

/**
 * @file LinearCombinations.hpp
 * This class defines handy linear combinations of GDS data.
 */

#ifndef GPSTK_LINEARCOMBINATIONS_HPP
#define GPSTK_LINEARCOMBINATIONS_HPP

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
//
//  Revision
//
//  2014/02/24  Add p2Prefit and l2Prefit
//  2014/03/20  Add c1MinusIono, p1MinusIono, p2MinusIono
//
//  2014/06/17  Add pcCorrection/lcCorrection/p1Correction/l1Correction/
//                  pdeltaCorrection/ldeltaCorrection.
//              Modified from the previous version.
//
//============================================================================



#include "DataStructures.hpp"
#include "constants.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class defines handy linear combinations of GDS data.
       *
       * This class is meant to be used with the GNSS data structures (GDS)
       * objects found in "DataStructures" class, and it is intended to be
       * coupled with class ComputeLinear.hpp.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Define a LinearCombinations object
       *   LinearCombinations comb;
       *
       *      // Object to compute linear combinations of data
       *      // Linear combinations will be computed in a FIFO basis
       *   ComputeLinear linear;
       *
       *      // Add a linear combination to compute PC combination using C1
       *   linear.addLinear(comb.pcCombWithC1);
       *
       *      // Add a linear combination to compute prefit residual using PC
       *   linear.addLinear(comb.pcPrefit);
       *
       *
       *      // Load observation data
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Loads precise ephemeris object with file data
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11513.sp3");
       *
       *      // Sets nominal position of receiver
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *      // Declare a MOPSTropModel object, setting the defaults
       *   MOPSTropModel mopsTM( nominalPos.getAltitude(),
       *                         nominalPos.getGeodeticLatitude(), 30);
       *
       *      // Object to compute the tropospheric data
       *   ComputeTropModel computeTropo(mopsTM);
       *
       *      // Declare a basic modeler
       *   BasicModel basic(nominalPos, SP3EphList);
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin)
       *   {
       *
       *      gRin >> basic >> computeTropo >> linear;
       *
       *         // Dump results
       *      gRin.body.dump(cout,1);
       *
       *   }
       *
       * @endcode
       *
       * @sa ComputeLinear.hpp
       */
   class LinearCombinations
   {
   public:

         /// Default constructor
      LinearCombinations();



         /// Definition to compute MIGC1 (GPS L1 with minus ionospehric delays)
      gnssLinearCombination q1CombOfGPS;

         /// Definition to compute MIGC2 (GPS L2 with minus ionospehric delays)
      gnssLinearCombination q2CombOfGPS;


         /// Definition to compute MIEC1 (Galileo L1 with minus ionospehric delays)
      gnssLinearCombination q1CombOfGAL;

         /// Definition to compute MIEC5 (Galileo L5 with minus ionospehric delays)
      gnssLinearCombination q5CombOfGAL;


         /// Definition to compute MICC2 (BDS L2 with minus ionospehric delays)
      gnssLinearCombination q2CombOfBDS;

         /// Definition to compute MICC7 (BDS L7 with minus ionospehric delays)
      gnssLinearCombination q7CombOfBDS;




         /// Definition to compute PC combination of GPS, C1 and C2
      gnssLinearCombination pcCombOfGPS;

         /// Definition to compute LC combination of GPS, L1 and L2
      gnssLinearCombination lcCombOfGPS;


         /// Definition to compute PC combination of Galileo, C1 and C5
      gnssLinearCombination pcCombOfGAL;

         /// Definition to compute LC combination of Galileo, L1 and L5
      gnssLinearCombination lcCombOfGAL;


         /// Definition to compute PC combination of BDS, C2 and C7
      gnssLinearCombination pcCombOfBDS;

         /// Definition to compute LC combination of BDS, L2 and L7
      gnssLinearCombination lcCombOfBDS;




         /// Definition to compute PI combination of GPS, C1 and C2
      gnssLinearCombination piCombOfGPS;

         /// Definition to compute LI combination of GPS, L1 and L2
      gnssLinearCombination liCombOfGPS;


         /// Definition to compute PI combination of Galileo, C1 and C5
      gnssLinearCombination piCombOfGAL;

         /// Definition to compute LI combination of Galileo, L1 and L5
      gnssLinearCombination liCombOfGAL;


         /// Definition to compute PI combination of BDS, C2 and C7
      gnssLinearCombination piCombOfBDS;

         /// Definition to compute LI combination of BDS, L2 and L7
      gnssLinearCombination liCombOfBDS;




         /// Definition to compute PW combination of GPS, C1 and C2
      gnssLinearCombination pwCombOfGPS;

         /// Definition to compute LW combination of GPS, L1 and L2
      gnssLinearCombination lwCombOfGPS;


         /// Definition to compute PW combination of Galileo, C1 and C5
      gnssLinearCombination pwCombOfGAL;

         /// Definition to compute LW combination of Galileo, L1 and L5
      gnssLinearCombination lwCombOfGAL;


         /// Definition to compute PW combination of BDS, C2 and C7
      gnssLinearCombination pwCombOfBDS;

         /// Definition to compute LW combination of BDS, L2 and L7
      gnssLinearCombination lwCombOfBDS;




         /// Definition to compute PN combination of GPS, C1 and C2
      gnssLinearCombination pnCombOfGPS;

         /// Definition to compute LN combination of GPS, L1 and L2
      gnssLinearCombination lnCombOfGPS;


         /// Definition to compute PN combination of Galileo, C1 and C5
      gnssLinearCombination pnCombOfGAL;

         /// Definition to compute LN combination of Galileo, L1 and L5
      gnssLinearCombination lnCombOfGAL;


         /// Definition to compute PN combination of BDS, C2 and C7
      gnssLinearCombination pnCombOfBDS;

         /// Definition to compute LN combination of BDS, L2 and L7
      gnssLinearCombination lnCombOfBDS;




         /// Definition to compute the MW combination of GPS, C1+C2 and L1+L2
      gnssLinearCombination mwCombOfGPS;

         /// Definition to compute the MW combination of Galileo, C1+C5 and L1+L5
      gnssLinearCombination mwCombOfGAL;

         /// Definition to compute the MW combination of BDS, C2+C7 and L2+L7
      gnssLinearCombination mwCombOfBDS;




         /// Definition to compute prefit residual of C1
      gnssLinearCombination c1Prefit;

         /// Definition to compute prefit residual of C2
      gnssLinearCombination c2Prefit;

         /// Definition to compute prefit residual of C3
      gnssLinearCombination c3Prefit;

         /// Definition to compute prefit residual of C5
      gnssLinearCombination c5Prefit;

         /// Definition to compute prefit residual of C6
      gnssLinearCombination c6Prefit;

         /// Definition to compute prefit residual of C7
      gnssLinearCombination c7Prefit;

         /// Definition to compute prefit residual of C8
      gnssLinearCombination c8Prefit;

         /// Definition to compute prefit residual of C9
      gnssLinearCombination c9Prefit;




         /// Definition to compute prefit residual of GPS L1
      gnssLinearCombination l1PrefitOfGPS;

         /// Definition to compute prefit residual of GPS L2
      gnssLinearCombination l2PrefitOfGPS;

         /// Definition to compute prefit residual of GPS L5
      gnssLinearCombination l5PrefitOfGPS;


         /// Definition to compute prefit residual of Galileo L1
      gnssLinearCombination l1PrefitOfGAL;

         /// Definition to compute prefit residual of Galileo L5
      gnssLinearCombination l5PrefitOfGAL;

         /// Definition to compute prefit residual of Galileo L6
      gnssLinearCombination l6PrefitOfGAL;

         /// Definition to compute prefit residual of Galileo L7
      gnssLinearCombination l7PrefitOfGAL;

         /// Definition to compute prefit residual of Galileo L8
      gnssLinearCombination l8PrefitOfGAL;


         /// Definition to compute prefit residual of BDS L2
      gnssLinearCombination l2PrefitOfBDS;

         /// Definition to compute prefit residual of BDS L6
      gnssLinearCombination l6PrefitOfBDS;

         /// Definition to compute prefit residual of BDS L7
      gnssLinearCombination l7PrefitOfBDS;


         /// Definition to compute prefit residual of GPS/Galileo/BDS PC
      gnssLinearCombination pcPrefit;

      gnssLinearCombination pcPrefitWithoutClock;

      gnssLinearCombination pcPrefitWithSatClock;

      gnssLinearCombination pcPrefitWithStaClock;

      gnssLinearCombination pcPrefitForPOD;


         /// Definition to compute prefit residual of GPS LC, L1 and L2
      gnssLinearCombination lcPrefitOfGPS;

      gnssLinearCombination lcPrefitWithoutClockOfGPS;

      gnssLinearCombination lcPrefitWithSatClockOfGPS;

      gnssLinearCombination lcPrefitWithStaClockOfGPS;

      gnssLinearCombination lcPrefitForPODOfGPS;

         /// Definition to compute prefit residual of Galileo LC, L1 and L5
      gnssLinearCombination lcPrefitOfGAL;

      gnssLinearCombination lcPrefitWithoutClockOfGAL;

      gnssLinearCombination lcPrefitWithSatClockOfGAL;

      gnssLinearCombination lcPrefitWithStaClockOfGAL;

      gnssLinearCombination lcPrefitForPODOfGAL;

         /// Definition to compute prefit residual of BDS LC, L2 and L7
      gnssLinearCombination lcPrefitOfBDS;

      gnssLinearCombination lcPrefitWithoutClockOfBDS;

      gnssLinearCombination lcPrefitWithSatClockOfBDS;

      gnssLinearCombination lcPrefitWithStaClockOfBDS;

      gnssLinearCombination lcPrefitForPODOfBDS;




         /// Definition to compute prefit residual of GPS/Galileo/BDS PN
      gnssLinearCombination pnPrefit;

         /// Definition to compute prefit residual of GPS LW, L1-L2
      gnssLinearCombination lwPrefitOfGPS;

         /// Definition to compute prefit residual of Galileo LW, L1-L5
      gnssLinearCombination lwPrefitOfGAL;

         /// Definition to compute prefit residual of BDS LW, L2-L7
      gnssLinearCombination lwPrefitOfBDS;




         /// Definition to compute the prefit residual of MW combination
      gnssLinearCombination mwPrefit;




   public:

         /// Return the frequency of the combination in cycles: i * L1 + j * L2
      static double freqOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);

         /// Return the wavelength of the combination in cycles: i * L1 + j * L2
      static double wavelengthOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);

         /// Return the f1 factor of the combination in cycles: i * L1 + j * L2
      static double firstFactorOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);

         /// Return the f2 factor of the combination in cycles: i * L1 + j * L2
      static double secondFactorOfLC(int i, int j, double f1 = L1_FREQ_GPS, double f2 = L2_FREQ_GPS);

   }; // End of class 'LinearCombinations'

      //@}

}
#endif // GPSTK_LINEARCOMBINATIONS_HPP

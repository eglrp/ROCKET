#pragma ident "Id$"

/**
 * @file SuccessRate.hpp 
 * This is a class to compute success rate of different integer estimators
 * in the presence or absence of bias vector (ie. ionospheric bias ). 
 */

#ifndef GPSTK_SUCCESSRATE_HPP
#define GPSTK_SUCCESSRATE_HPP

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
//============================================================================
//  Date :			2017/05/16 - 2017/05/12
//  Version:		0.0
//  Author(s):		Lei Zhao, a Ph.D. candiate
//  School of Geodesy and Geomatics, Wuhan University 
//============================================================================

#include "Exception.hpp"
#include "Matrix.hpp"
#include "Vector.hpp" 
#include "GaussianDistribution.hpp"


namespace gpstk
{

	/** This class computes the success rate of different integer estimators
	 * 
	 * The algorithm is summarised in: 
	 * 
	 * Verhagen S, B Li, PJG Teunissen (2013): Ps-LAMBDA: Ambiguity Success 
	 * rate Evaluation Software for Interferometric Applications. Computers & 
	 * Geosciences, 54:361-376
	 *
	 */ 

	class SuccessRate 
	{
	public:
		/// Prepare to support these integer estimators
		enum IntegerEstimator
		{
			IR = 1,			///< integerRounding
			IB,				///< integerBootstrapping
			ILS				///< integerLeastSquares 
		};

		enum SuccessRateType
		{
			/// For Integer Least Square
			ADOPAP = 1,				///< ADOP based approximation
			LowerBoundIB,			///< LowerBound by Bootstrapped success rate
			LowerBoundRegion,		///< Lower bound by bounding pull-in region
			UpperBoundRegion,		///< Upper bound by bounding pull-in region
			LowerBoundVCMat,		///< Lower bound by bounding covariance matrix
			UpperBoundVCMat,		///< Upper bound by bounding covariance matrix

			/// For Integer Bootstrapping 
			Exact,					///< Exact success rate

			/// For Integer Rounding
			LowerBound,				///< Lower bound based on diagonal variance matrix
			UpperBound,				///< Upper bound based on bootstrapped estimation

			/// Common type for ILS and IB
			/// And for ILS, this is coupled with LowerBoundIB 
			UpperBoundADOP			///< Upper bound based on ADOP 
 
		};

		/** Explicit constructor
		 * 
		 * @param Q				variance-covariance matrix of float ambiguity estimates
		 * @param ie			type of integer estimator
		 * @param srty			type of success rate solution
		 * @param decorr		decorralate flag
		 * @param b				bias vector 
		 */
		SuccessRate( const Matrix<double>& Q, 
						 const Vector<double>& b = 0, 
						 const IntegerEstimator& ie = IB,
						 const SuccessRateType& srty = Exact, 
						 const bool decorr = true ) : 
						 Qahat(Q), bias(b), intEstimator(ie), srt(srty), decor(decorr) 
		{}


		/// Return success rate
		double getSuccessRate(); 


		/// Return bias vector 
		Vector<double> getBiasVector()
		{ return this->bias ;}


	protected:

		/** Compute Q = L'*diag(D)*L   
		 *
		 * @param Q			input
		 * @param L			output
		 * @param D			output
		 */
      int factorize( const Matrix<double>& Q,
                     Matrix<double>& L, 
                     Vector<double>& D );

       /// integer gauss transformation
      void gauss(Matrix<double>& L, Matrix<double>& Z, int i, int j );


         /// permutations
      void permute( Matrix<double>& L, 
                    Vector<double>& D, 
                    int j, 
                    double del, 
                    Matrix<double>& Z);


         /* lambda reduction (z=Z'*a, Qz=Z'*Q*Z=L'*diag(D)*L)
			 *
			 * @param L		input
			 * @param D		input
			 * @param Z		output
			 *
			 */
      void reduction( Matrix<double>& L, 
                      Vector<double>& D, 
                      Matrix<double>& Z );

			// swap `
      void swap(double& a,double& b)
      { double t(a); a = b; b = t; }
		

	private:

		/** Return ILS success rate
		 *
		 * @param Q
		 * @param srty
		 * @param b 
		 */ 
		double SRILS( Matrix<double>& Q,
						  SuccessRateType& srty,
						  Vector<double>& b )
		{ return 0; }


		/** Return IR success rate
		 *
		 * @param Q
		 * @param srty
		 * @param decorr 
		 */
		double SRRound( Matrix<double>& Q,
							 SuccessRateType& srty, 
							 bool decorr,
							 Vector<double>& b  )
		{ return 0; }

		/** Return IB success rate
		 * 
		 * @param Q
		 * @param srty
		 * @param decorr 
		 * @param b
		 */
		double SRBoot( Matrix<double>& Q,
							SuccessRateType& srty, 
							bool decorr,
							Vector<double>& b );

		/// Return biased exact success rate of IB estimator
		double SRBootExact( Matrix<double>& Q, Vector<double>& b );

		/// Return unbiased ADOP-based approximation of IB success rate 
		double SRILSADOPAP( Matrix<double>& Q, Vector<double>& b ) {  return 0;}

		
		
		/// For IB and IR, this parameter is needed. Their success rates differ
		/// with respect to original and decorrelated Qa
		/// false - without decorrelation
		/// true  - decorrelation 
		bool decor;

		IntegerEstimator intEstimator;   ///< integer estimator used 

		SuccessRateType srt;				///< type of success rate 

		Matrix<double> Qahat;   ///< VC matrix of float ambiguity estimates. 

		Vector<double> bias;   ///< bias vector 


	};   // End of class 'SuccessRate' 

}   // End of namespace gpstk

#endif   // GPSTK_SUCCESSRATE_HPP  

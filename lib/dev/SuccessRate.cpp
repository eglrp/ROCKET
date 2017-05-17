#pragma ident "Id$"

/**
 * @file SuccessRate.hpp 
 * This is a class to compute success rate of different integer estimators
 * in the presence or absence of bias vector (ie. ionospheric bias ). 
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
//============================================================================
//  Date :			2017/05/16 - 2017/05/17
//  Author(s):		Lei Zhao, a Ph.D. candiate
//  School of Geodesy and Geomatics, Wuhan University 
//============================================================================

#include "SuccessRate.hpp"


namespace gpstk
{

		// Return computed success Rate 
	double SuccessRate::getSuccessRate()
	{
		switch( intEstimator )
		{
			case IR:  return SRRound( Qahat, srt, decor, bias );		break;
			case IB:  return SRBoot( Qahat, srt, decor, bias );		break;
			case ILS: return SRILS( Qahat, srt, bias );					break;
		};
	}

		/** Return IB success rate
		 * 
		 * @param Q
		 * @param srty
		 * @param decorr
		 * @param b
		 */
	double SuccessRate::SRBoot( Matrix<double>& Q,
										 SuccessRateType& srty, 
										 bool decorr,
										 Vector<double>& b )
	{

			// Decorrelate the float ambiguity estimates 
		if( decorr )
		{
			// Qzhat = Z'T*Qahat*Z	
			Matrix<double> L( Q.rows(), Q.rows(), 0.0 );
			Vector<double> D( Q.rows(), 0.0 );

			Matrix<double> Z( ident<double>( Q.rows() ) );
			Matrix<double> Qzhat( Q.rows(), Q.rows(), 0.0 );

		   if( !factorize( Q, L, D) )
			{
				// Debug code vvv
				std::cout << "L: " << std::endl << L << std::endl;  
				std::cout << "D: " << std::endl << D << std::endl;  
				// Debug code ^^^ 
				reduction( L, D, Z );
				Qzhat = transpose(Z)*Q*Z;

				Q = Qzhat;

				// vvv debug code vvv
				std::cout << "Qzhat: " << std::endl << Qzhat << std::endl;  
				// ^^^ debug code ^^^ 

				if( !b.empty() )		// Biased
				{
					Vector<double> tempb(b);
					b = transpose(Z)*tempb;
				}
			}
			else
			{
				Exception e(" SuccessRate::factorize error ");
				GPSTK_THROW(e);;
			}
		}   // End of ' if( decorr ) '

		switch( srty )
		{
			case Exact:					return SRBootExact( Q, b );		break; 
			case UpperBoundADOP:		return SRILSADOPAP( Q, b );		break;
			default: 
				Exception e("illegal success rate type for IB");
				GPSTK_THROW(e);
			break;
		}   // End of ' switch( srty ) '

		return 0;
	}   // End of ' double SRBoot( Matrix<double>& Q, ... '  


		/** Compute Q = L'*diag(D)*L   
		 *
		 * @param Q			input
		 * @param L			output
		 * @param D			output
		 */
	int SuccessRate::factorize( const Matrix<double>& Q,
										 Matrix<double>& L, 
										 Vector<double>& D )

	{
	   // TODO: CHECK UNEXPECTED INPUT
      // L:nxn Z:nxn 0<=(i,j)<n
		
      const int n = static_cast<int>(Q.rows());

      Matrix<double> QC(Q);
      L.resize(n, n, 0.0);
      D.resize(n, 0.0);

      for(int i = n-1; i >= 0; i--) 
      {
         D(i) = QC(i,i);
         if( D(i) <= 0.0 ) return -1;
         double temp = std::sqrt(D(i));
         for(int j=0; j<=i; j++) L(i,j) = QC(i,j)/temp;
         for(int j=0; j<=i-1; j++) 
         {
            for(int k=0; k<=j; k++) QC(j,k) -= L(i,k) * L(i,j);
         }
         for(int j=0; j<=i; j++) L(i,j) /= L(i,i);
      }

      return 0;

	}   // End of ' int SuccessRate::factorize( const Matrix<double>& Q, ... '

		/// Integer Gauss transformation 
	void SuccessRate::gauss(Matrix<double>& L, Matrix<double>& Z, int i, int j)
   {
      //TODO: CHECK UNEXPECTED INPUT
      // L:nxn Z:nxn 0<=(i,j)<n

      const int n = L.rows();
      const int mu = (int)round(L(i,j));
      if(mu != 0) 
      {
         for(int k=i; k<n; k++) L(k,j) -= (double)mu*L(k,i);
         for(int k=0; k<n; k++) Z(k,j) -= (double)mu*Z(k,i);
      }
   }  // End of method 'ARLambda::gauss()'

   
   void SuccessRate::permute( Matrix<double>& L, 
                           Vector<double>& D, 
                           int j, 
                           double del, 
                           Matrix<double>& Z )
   {  
      //TODO: CHECK UNEXPECTED INPUT
      // L:nxn D:nx1 Z:nxn 0<=j<n

      const int n = L.rows();

      double eta=D(j)/del;
      double lam=D(j+1)*L(j+1,j)/del;

      D[j]=eta*D(j+1); 
      D(j+1)=del;
      for(int k=0;k<=j-1;k++) 
      {
         double a0=L(j,k); 
         double a1=L(j+1,k);
         L(j,k) =-L(j+1,j)*a0 + a1;
         L(j+1,k) = eta*a0 + lam*a1;
      }
      L(j+1,j)=lam;
      for(int k=j+2; k<n; k++) swap(L(k,j),L(k,j+1));
      for(int k=0; k<n; k++) swap(Z(k,j),Z(k,j+1));

   }  // End of method 'ARLambda::permute()'

   
   void SuccessRate::reduction( Matrix<double>& L, 
                             Vector<double>& D,
                             Matrix<double>& Z)
   {
      //TODO: CHECK UNEXPECTED INPUT
      // L:nxn D:nx1 Z:nxn 0<=j<n

      const int n = L.rows();

      int j(n-2), k(n-2);
      while(j>=0) 
      {
         if(j<=k)
         {
            for (int i=j+1; i<n; i++) 
            {
               gauss(L,Z,i,j);
            }
         } 

         double del=D(j)+L(j+1,j)*L(j+1,j)*D(j+1);

         if(del+1E-6<D(j+1)) 
         { 
            permute(L,D,j,del,Z);
            k=j; j=n-2;
         }
         else
         { 
            j--;
         }

      }  // end while

   }  // End of method 'ARLambda::reduction()'

	
	/// Return unbiased exact success rate of IB estimator
	double SuccessRate::SRBootExact( Matrix<double>& Q, Vector<double>& b )
	{

		bool unbiased( b.empty() );
			// Num of rows for Q
		const int n = Q.rows();

			// Decomposition Q = L'*diag(D)*L 
		Matrix<double> L( n, n, 0.0 );
		Vector<double> D( n, 0.0 );

		if( !factorize( Q, L, D) )
		{
			double Ps(1.0);
			GaussianDistribution normal;

			if( unbiased )		// Unbiased success rate
			{
				for( int i=0; i<n; i++ )
				{
					Ps = Ps * ( 2 * normal.cdf( 0.5/std::sqrt( D(i) ) ) - 1 );	
				}
			}
			else {				// Biased success rate  
				Vector<double> bias( inverse(L)*b );
				for( int i=0; i<n; i++ )
				{
					double sigma( std::sqrt( D(i) ) );
					Ps = Ps * ( normal.cdf( (1 - 2*bias(i))/(2*sigma) ) +
									normal.cdf( (1 + 2*bias(i))/(2*sigma) ) - 1 );
				}
		
			}   // End of ' if( unbiased ) '

			return Ps;
		}    // End of ' if( !factorize( Q, L, D) ) '

		return 0.0;

	}   // End of ' double SuccessRate::SRBootExact( Matrix<double>& Q ) '


}   // End of namespace gpstk 



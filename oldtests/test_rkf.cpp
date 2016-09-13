/*************************************************************************
	> File Name: test_rkf.cpp
	> Author:
	> Mail:
	> Created Time: 2016年07月24日 星期日 15时04分51秒
 ************************************************************************/

#include<iostream>

#include "RungeKuttaFehlberg.hpp"

#include "EquationOfMotion.hpp"


using namespace std;
using namespace gpstk;


class TestEOM : public EquationOfMotion
{
   public:

      virtual Vector<double> getDerivatives(const double &t,
                                            const Vector<double> &y);

};


// y = sin(t), dy = cos(t)
Vector<double> TestEOM::getDerivatives(const double &t,
                                       const Vector<double>& y)
{
   Vector<double> dy(y.size(),0.0);
   dy(0) = std::cos(t);

   return dy;
}


int main(void)
{
   TestEOM eom;

   double t0 = 0.0;
   double h  = 1.0;

   Vector<double> y0(1,0.0);

   RungeKuttaFehlberg rkf;
   rkf.setStepSize(0.25);

   for(int i=0; i<100000; ++i)
   {
      rkf.integrateTo(t0, y0, &eom, t0+h);

      double t = t0;
      double err = std::sin(t) - y0(0);

      cout << fixed;
      cout << setw(18) << setprecision(8) << t << " "
           << setw(18) << setprecision(8) << y0(0) << " "
           << setw(18) << setprecision(12) << err << endl;
   }

   return 1;
}


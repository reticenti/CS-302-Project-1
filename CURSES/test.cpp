#include <iostream>
#include "cubicSpline.h"

using namespace std;

int main()
{
	double *a = new double[3];
	double *b = new double[3];
	double *d = new double[3];
	double *B = new double[3];
	double *x = new double[3];

	a[0] = 4;
	a[1] = 3;
	a[2] = 0;

	b[0] = 0;
	b[1] = 4;
	b[2] = 3;

	d[0] = d[1] = d[2] = 14;

	B[0] = -6.5;
	B[1] = -5.5;
	B[2] = 2.5;

	solveTriDiag( b, d, a, B, x, 3 );

	cout << x[0] << endl << x[1] << endl << x[2] << endl;

	return 0;

}

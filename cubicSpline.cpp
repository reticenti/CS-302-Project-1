#include "cubicSpline.h"

cubicSpline::cubicSpline()
{
	coef_0 = NULL;
	coef_1 = NULL;
	a = NULL;
	y = NULL;
	len = 0;
	len2 = 0;
}

cubicSpline::cubicSpline( int points[], int num )
{
	coef_0 = NULL;
	coef_1 = NULL;
	a = NULL;
	y = NULL;
	len = 0;
	len2 = 0;
	
	create( points, num );
}

cubicSpline::~cubicSpline()
{
	if ( coef_0 != NULL )
		delete [] coef_0;
	if ( coef_1 != NULL )
		delete [] coef_1;
	if ( a != NULL )
		delete [] a;
	if ( y != NULL )
		delete [] y;
}

cubicSpline::cubicSpline( const cubicSpline& rhs )
{
	if ( len != rhs.len )
	{
		len = rhs.len;
		if ( coef_0 != NULL )
		{
			delete [] coef_0;
			coef_0 = NULL;
		}
		if ( coef_1 != NULL )
		{
			delete [] coef_1;
			coef_1 = NULL;
		}

		if ( len > 0 )
		{
			coef_1 = new double[len];
			coef_0 = new double[len];
		}
	}

	if ( len2 != rhs.len2 )
	{
		len2 = rhs.len2;
		if ( a != NULL )
		{
			delete [] a;
			a = NULL;
		}
		if ( y != NULL )
		{
			delete [] y;
			y = NULL;
		}

		if ( len2 > 0 )
		{
			a = new double[len2+2];
			y = new double[len2+2];
		}
	}

	for ( int i = 0; i < len; i++ )
	{
		coef_0[i] = rhs.coef_0[i];
		coef_1[i] = rhs.coef_1[i];
	}
	for ( int i = 0; i < len2; i++ )
	{
		a[i] = rhs.a[i];
		y[i] = rhs.y[i];
	}
}
/*
[ d_0 a_0 0   0   0   ... 0     0     0     0    ]   [ x_0 ]   [ B_0 ]
[ b_1 d_1 a_1 0   0   ... 0     0     0     0    ]   [ x_1 ]   [ B_1 ]
[ 0   b_2 d_2 a_2 0   ... 0     0     0     0    ]   [ x_2 ]   [ B_2 ]
[ 0   0   b_3 d_3 a_3 ... 0     0     0     0    ]   [ x_3 ]   [ B_3 ]
[ :   :   :   :   :   ... :     :     :     :    ] * [  :  ] = [  :  ]
[ :   :   :   :   :   ... :     :     :     :    ]   [  :  ]   [  :  ]
[ 0   0   0   0       ... b_n-2 d_n-2 a_n-2 0    ]   [x_n-2]   [B_n-2]
[ 0   0   0   0   0   ... 0     b_n-1 d_n-1 a_n-1]   [x_n-1]   [B_n-1]
[ 0   0   0   0   0   ... 0     0     b_n   d_n  ]   [ x_n ]   [ B_n ]

void solveTriDiag( double *b, double *d, double *a, double *B, double *x, int n ) */

void cubicSpline::createCubic( int points[], int num )
{
	double h = 100.0 / (num-1);

	if ( len2 != num-2 )
	{
		len2 = num-2;
		if ( a != NULL )
			delete [] a;
		if ( y != NULL )
			delete [] y;

		a = new double[num];
		y = new double[num];
	}
		
	double *diag_b = new double[len2];
	double *diag_d = new double[len2];
	double *diag_a = new double[len2];
	double *B = new double[len2];

	for ( int i = 0; i < len2; i++ )
	{
		diag_b[i] = h;
		diag_d[i] = 4*h;
		diag_a[i] = h;
		B[i] = 6/h*((points[i+2]-points[i+1])-(points[i+1]-points[i]));
		y[i] = points[i];
	}
	y[len2] = points[len2];
	y[len2+1] = points[len2+1];
	a[0] = a[len2+1] = 0;
	
	// passing a+1 to solve for a[1]->a[len] since a[0] is already defines as 0
	solveTriDiag( diag_b, diag_d, diag_a, B, a+1, len2 );

	delete [] diag_a;
	delete [] diag_b;
	delete [] diag_d;
	delete [] B;

}

void cubicSpline::create( int points[], int num )
{
	double stepsize = 100.0 / (num-1);
	double x1, y1, x2, y2;

	if ( len != num-1 )
	{
		if ( coef_0 != NULL )
			delete [] coef_0;
		if ( coef_1 != NULL )
			delete [] coef_1;

		len = num-1;
		coef_0 = new double[len];
		coef_1 = new double[len];
	}

	for ( int i = 0; i < len; i++ )
	{
		x1 = stepsize * i;
		y1 = points[i];
		x2 = stepsize * (i+1);
		y2 = points[i+1];

		coef_0[i] = -x2*y1/(x1-x2) - x1*y2/(x2-x1);
		coef_1[i] = y1/(x1-x2) + y2/(x2-x1);
	}
}

double cubicSpline::getVal( double x )
{
	double stepsize = 100.0 / len;

	int index = (int)(x / stepsize);

	if ( index >= len )
		index = len-1;
	if ( index < 0 )
		index = 0;

	return coef_1[index] * x + coef_0[index];
}

double cubicSpline::getCubicVal( double x )
{
	double h = 100.0 / (len2+1);

	int i = (int)(x/h);

	if ( i >= len2+1 )
		i = len2;
	if ( i < 0 )
		i = 0;
	
	// using the definition of Lagrand cubic interpolation
	return (a[i]/(6*h)*((i+1)*h-x)*((i+1)*h-x)*((i+1)*h-x) +
			a[i+1]/(6*h)*(x-(i*h))*(x-(i*h))*(x-(i*h)) +
			(y[i]/h - a[i]*h/6)*((i+1)*h-x) +
			(y[i+1]/h-a[i+1]*h/6)*(x-i*h));
}

/* Solves the following tri-diagonal matrix
   assumes the matrix is not singular
   a and B will be changed but we don't really care ^_^
	  [ d_0 a_0 0   0   0   ... 0     0     0     0    ]   [ x_0 ]   [ B_0 ]
	  [ b_1 d_1 a_1 0   0   ... 0     0     0     0    ]   [ x_1 ]   [ B_1 ]
	  [ 0   b_2 d_2 a_2 0   ... 0     0     0     0    ]   [ x_2 ]   [ B_2 ]
	  [ 0   0   b_3 d_3 a_3 ... 0     0     0     0    ]   [ x_3 ]   [ B_3 ]
	  [ :   :   :   :   :   ... :     :     :     :    ] * [  :  ] = [  :  ]
	  [ :   :   :   :   :   ... :     :     :     :    ]   [  :  ]   [  :  ]
	  [ 0   0   0   0       ... b_n-2 d_n-2 a_n-2 0    ]   [x_n-2]   [B_n-2]
	  [ 0   0   0   0   0   ... 0     b_n-1 d_n-1 a_n-1]   [x_n-1]   [B_n-1]
	  [ 0   0   0   0   0   ... 0     0     b_n   d_n  ]   [ x_n ]   [ B_n ]
*/
void solveTriDiag( double *b, double *d, double *a, double *B, double *x, int n )
{
	a[0] /= d[0];
	B[0] /= d[0];
	for ( int i = 1; i < n; i++ )
	{
		double temp = 1 / (d[i] - a[i-1] * b[i]);
		a[i] *= temp;
		B[i] = (B[i] - B[i-1] * b[i]) * temp;
	}

	x[n-1] = B[n-1];
	for ( int i = n - 2; i >= 0; i-- )
		x[i] = B[i] - a[i] * x[i+1];
}


#include <iostream>
#include "cubicSpline.h"

/******************************************************************************\
 default constructor, sets everything to zero or NULL
\******************************************************************************/
cubicSpline::cubicSpline()
{
	// initialize all values to zero or NULL
	for ( int i = 0; i < 3; i++ )
	{	
		a[i] = NULL;
		y[i] = NULL;
	}
	len = 0;
}

/******************************************************************************\
 paramaterized constructor, creates both a cubic spline and a linear spline
 based on the parameters
\******************************************************************************/
cubicSpline::cubicSpline( int points[], int num )
{
	// initialize all the values to zero or NULL
	for ( int i = 0; i < 3; i++ )
	{	
		a[i] = NULL;
		y[i] = NULL;
	}
	len = 0;
	
	// create a cubic or linear spline based on points
	create( points, num );
}

/******************************************************************************\
 paramaterized constructor, creates both a cubic spline and a linear spline
 based on the parameters
\******************************************************************************/
cubicSpline::cubicSpline( rgb points[], int num )
{
	// initialize all the values to zero or NULL
	for ( int i = 0; i < 3; i++ )
	{	
		a[i] = NULL;
		y[i] = NULL;
	}
	len = 0;
	
	// create a cubic
	create( points, num );
}


/******************************************************************************\
 destructor makes sure all the memory is de-allocated before object is lost
\******************************************************************************/
cubicSpline::~cubicSpline()
{
	for ( int i = 0; i < 3; i++ )
	{
		if ( a[i] != NULL )
			delete [] a[i];
		if ( y[i] != NULL )
			delete [] y[i];
	}
}

/*****************************************************************************\
 creates a CUBIC spline function for the given points, note that an equal
 distance between nodes is assumed, since this function is being used only
 for images this should be fine since pixels are evenly spaced

 NOTE: This creates a natural cubic spline
\*****************************************************************************/
void cubicSpline::create( int points[], int num )
{
	// defines the step size
	double h = 100.0 / (num-1);

	// allocate memory for the variables if required
	if ( len != num-2 )
	{
		len = num-2;
		if ( a != NULL )
			delete [] a[0];
		if ( y != NULL )
			delete [] y[0];

		a[0] = new double[num];
		y[0] = new double[num];
	}
		
	// the vectors that will be used to calculate the tri-diagonal matrix
	double *diag_b = new double[len];
	double *diag_d = new double[len];
	double *diag_a = new double[len];
	double *B = new double[len];

	// set up the vectors based on the Lagrange spline method
	for ( int i = 0; i < len; i++ )
	{
		diag_b[i] = h;
		diag_d[i] = 4*h;
		diag_a[i] = h;
		B[i] = 6/h*((points[i+2]-points[i+1])-(points[i+1]-points[i]));

		// y simply holds a copy of the points
		y[0][i] = points[i];
	}

	// get the last 2 y points since the previous loop went 2 to short
	y[0][len] = points[len];
	y[0][len+1] = points[len+1];

	// set the begining and end of a to zero like it should be
	a[0][0] = a[0][len+1] = 0;
	
	// passing a+1 to solve for a[1]->a[len] since a[0] is already defines as 0
	solveTriDiag( diag_b, diag_d, diag_a, B, a[0]+1, len );

	// now that a has been solved these are no longer required
	delete [] diag_a;
	delete [] diag_b;
	delete [] diag_d;
	delete [] B;

}

/*****************************************************************************\
 creates a CUBIC spline function for the given points, note that an equal
 distance between nodes is assumed, since this function is being used only
 for images this should be fine since pixels are evenly spaced

 NOTE: This creates a natural cubic spline
\*****************************************************************************/
void cubicSpline::create( rgb points[], int num )
{
	// defines the step size
	double h = 100.0 / (num-1);

	// allocate memory for the variables if required
	if ( len != num-2 )
	{
		len = num-2;
		for ( int i = 0; i < 3; i++ )
		{
			if ( a != NULL )
				delete [] a[i];
			if ( y != NULL )
				delete [] y[i];
			a[i] = new double[num];
			y[i] = new double[num];
		}
	}
		
	// the vectors that will be used to calculate the tri-diagonal matrix
	double* diag_b[3] = {new double[len], new double[len], new double[len]};
	double* diag_d[3] = {new double[len], new double[len], new double[len]};
	double* diag_a[3] = {new double[len], new double[len], new double[len]};
	double* B[3] = {new double[len], new double[len], new double[len]};

	// set up the vectors based on the Lagrange spline method
	for ( int j = 0; j < 3; j++ )
		for ( int i = 0; i < len; i++ )
		{
			diag_b[j][i] = h;
			diag_d[j][i] = 4*h;
			diag_a[j][i] = h;
			if ( j == 0 )
			{
				B[j][i] = 6/h*((points[i+2].r-points[i+1].r)
				          -(points[i+1].r-points[i].r));
				y[j][i] = points[i].r;
			}
			else if ( j == 1 )
			{
				B[j][i] = 6/h*((points[i+2].g-points[i+1].g)
				          -(points[i+1].g-points[i].g));
				y[j][i] = points[i].g;
			}
			else if ( j == 2 )
			{
				B[j][i] = 6/h*((points[i+2].b-points[i+1].b)
				          -(points[i+1].b-points[i].b));
				y[j][i] = points[i].b;
			}
		}

	// get the last 2 y points since the previous loop went 2 to short
	y[0][len] = points[len].r;
	y[1][len] = points[len].g;
	y[2][len] = points[len].b;

	y[0][len+1] = points[len+1].r;
	y[1][len+1] = points[len+1].g;
	y[2][len+1] = points[len+1].b;

	// set the begining and end of a to zero like it should be
	a[0][0] = a[0][len+1] = a[1][0] = a[1][len+1] = a[2][0] = a[2][len+1] = 0;
	
	// passing a+1 to solve for a[1]->a[len] since a[0] is already defines as 0
	for ( int i = 0; i < 3; i++ )
	{
		solveTriDiag( diag_b[i], diag_d[i], diag_a[i], B[i], a[i]+1, len );
	
		// now that a has been solved these are no longer required
		delete [] diag_a[i];
		delete [] diag_b[i];
		delete [] diag_d[i];
		delete [] B[i];
	}
}

/******************************************************************************\
 Return the value of the cubic spline function at the given x value, the spline
 is defined from 0 to 100, so x should be around there, if not x will be the
 nearest spline value
\******************************************************************************/
void cubicSpline::getVal( double x, int& val )
{
	if ( len == 0 )
	{
		val = 0;
		return;
	}

	// define the step size to be used in the calculation
	double h = 100.0 / (len+1);

	// define the index of a to be used
	int i = (int)(x/h);

	// if x is greater than 100 or less than 0 then just use the closest curve
	if ( i >= len+1 )
		i = len;
	if ( i < 0 )
		i = 0;
	
	// using the definition of Lagrange cubic interpolation
	if ( len != 0 )
		val = (a[0][i]/(6*h)*((i+1)*h-x)*((i+1)*h-x)*((i+1)*h-x)
			  + a[0][i+1]/(6*h)*(x-(i*h))*(x-(i*h))*(x-(i*h))
		      + (y[0][i]/h - a[0][i]*h/6)*((i+1)*h-x)
		      + (y[0][i+1]/h-a[0][i+1]*h/6)*(x-i*h));
	else
		val = 0;
}

/******************************************************************************\
 Return the value of the cubic spline function at the given x value, the spline
 is defined from 0 to 100, so x should be around there, if not x will be the
 nearest spline value, this is the rgb version
\******************************************************************************/
void cubicSpline::getVal( double x, rgb& val )
{
	if ( len == 0 )
	{
		val.r = 0;
		val.g = 0;
		val.b = 0;
		return;
	}

	// define the step size to be used in the calculation
	double h = 100.0 / (len+1);

	// define the index of a to be used
	int i = (int)(x/h);

	// if x is greater than 100 or less than 0 then just use the closest curve
	if ( i >= len+1 )
		i = len;
	if ( i < 0 )
		i = 0;
	
	// using the definition of Lagrange cubic interpolation
	if ( len != 0 )
	{
		val.r = (a[0][i]/(6*h)*((i+1)*h-x)*((i+1)*h-x)*((i+1)*h-x)
				+ a[0][i+1]/(6*h)*(x-(i*h))*(x-(i*h))*(x-(i*h))
				+ (y[0][i]/h - a[0][i]*h/6)*((i+1)*h-x)
				+ (y[0][i+1]/h-a[0][i+1]*h/6)*(x-i*h));

		val.g = (a[1][i]/(6*h)*((i+1)*h-x)*((i+1)*h-x)*((i+1)*h-x)
				+ a[1][i+1]/(6*h)*(x-(i*h))*(x-(i*h))*(x-(i*h))
				+ (y[1][i]/h - a[1][i]*h/6)*((i+1)*h-x)
				+ (y[1][i+1]/h-a[1][i+1]*h/6)*(x-i*h));

		val.b = (a[2][i]/(6*h)*((i+1)*h-x)*((i+1)*h-x)*((i+1)*h-x)
			    + a[2][i+1]/(6*h)*(x-(i*h))*(x-(i*h))*(x-(i*h))
		        + (y[2][i]/h - a[2][i]*h/6)*((i+1)*h-x)
		        + (y[2][i+1]/h-a[2][i+1]*h/6)*(x-i*h));
	}
	else
		val.r = val.g = val.b = 0;
}

/******************************************************************************\
Solves the following tri-diagonal matrix Ax+b shown below
   [ d_0 a_0 0   0   0   ... 0     0     0     0    ]   [ x_0 ]   [ B_0 ]
   [ b_1 d_1 a_1 0   0   ... 0     0     0     0    ]   [ x_1 ]   [ B_1 ]
   [ 0   b_2 d_2 a_2 0   ... 0     0     0     0    ]   [ x_2 ]   [ B_2 ]
   [ 0   0   b_3 d_3 a_3 ... 0     0     0     0    ]   [ x_3 ]   [ B_3 ]
   [ :   :   :   :   :   ... :     :     :     :    ] * [  :  ] = [  :  ]
   [ :   :   :   :   :   ... :     :     :     :    ]   [  :  ]   [  :  ]
   [ 0   0   0   0       ... b_n-2 d_n-2 a_n-2 0    ]   [x_n-2]   [B_n-2]
   [ 0   0   0   0   0   ... 0     b_n-1 d_n-1 a_n-1]   [x_n-1]   [B_n-1]

This function assumes the matrix is not singular and changes a and B 
but we don't really care in this case ^_^
\******************************************************************************/
void solveTriDiag( double *b, double *d, double *a, double *B, double *x, int n )
{
	// step 1 is to define the vectors which is already done
	// step 2 calculate new a[0] and B[0];
	a[0] = a[0] / d[0];
	B[0] = B[0] / d[0];

	// step 3 for 1 < i < n calculate new a[i] and B[i]
	for ( int i = 1; i < n; i++ )
	{
		a[i] = a[i] / (d[i]-b[i]*a[i-1]);
		B[i] = (B[i] - B[i-1] * b[i]) / (d[i]-b[i]*a[i-1]);
	}

	// step 4 was to calculate B[n-2] but it was done in the prev loop
	// step 5 calculate x starting at x[n-1] and going to x[0]
	x[n-1] = B[n-1];
	for ( int i = n - 2; i >= 0; i-- )
		x[i] = B[i] - a[i] * x[i+1];
}


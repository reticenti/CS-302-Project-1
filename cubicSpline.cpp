#include "cubicSpline.h"

/******************************************************************************\
 default constructor, sets everything to zero or NULL
\******************************************************************************/
cubicSpline::cubicSpline()
{
	// initialize all values to zero or NULL
	coef_0 = NULL;
	coef_1 = NULL;
	a = NULL;
	y = NULL;
	len = 0;
	len2 = 0;
}

/******************************************************************************\
 paramaterized constructor, creates both a cubic spline and a linear spline
 based on the parameters
\******************************************************************************/
cubicSpline::cubicSpline( int points[], int num )
{
	// initialize all the values to zero or NULL
	coef_0 = NULL;
	coef_1 = NULL;
	a = NULL;
	y = NULL;
	len = 0;
	len2 = 0;
	
	// creates both a cubic and linear spline based on points
	create( points, num );
	createCubic( points, num );
}

/******************************************************************************\
 destructor makes sure all the memory is de-allocated before object is lost
\******************************************************************************/
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

/******************************************************************************\
 copy constructor, copy's all data from the rhs, de-allocating and allocating
 memory as required
\******************************************************************************/
cubicSpline::cubicSpline( const cubicSpline& rhs )
{
	// if len isn't the same as rhs.len delete all the memory and re-allocate
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
		
		// only allocate if len is greater than zero
		if ( len > 0 )
		{
			coef_1 = new double[len];
			coef_0 = new double[len];
		}
	}
	
	// allocate memory only when len2 isnt equal to rhs.len2
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
		
		// make sure len2 isn't zero before allocating
		if ( len2 > 0 )
		{
			a = new double[len2+2];
			y = new double[len2+2];
		}
	}

	// copy the values of rhs to the arrays in this object
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

/*****************************************************************************\
 creates a CUBIC spline function for the given points, note that an equal
 distance between nodes is assumed, since this function is being used only
 for images this should be fine since pixels are evenly spaced
\*****************************************************************************/
void cubicSpline::createCubic( int points[], int num )
{
	// defines the step size
	double h = 100.0 / (num-1);

	// allocate memory for the variables if required
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
		
	// the vectors that will be used to calculate the tri-diagonal matrix
	double *diag_b = new double[len2];
	double *diag_d = new double[len2];
	double *diag_a = new double[len2];
	double *B = new double[len2];

	// set up the vectors based on the Lagrange spline method
	for ( int i = 0; i < len2; i++ )
	{
		diag_b[i] = h;
		diag_d[i] = 4*h;
		diag_a[i] = h;
		B[i] = 6/h*((points[i+2]-points[i+1])-(points[i+1]-points[i]));

		// y simply holds a copy of the points
		y[i] = points[i];
	}

	// get the last 2 y points since the previous loop went 2 to short
	y[len2] = points[len2];
	y[len2+1] = points[len2+1];

	// set the begining and end of a to zero like it should be
	a[0] = a[len2+1] = 0;
	
	// passing a+1 to solve for a[1]->a[len] since a[0] is already defines as 0
	solveTriDiag( diag_b, diag_d, diag_a, B, a+1, len2 );

	// now that a has been solved these are no longer required
	delete [] diag_a;
	delete [] diag_b;
	delete [] diag_d;
	delete [] B;

}

/******************************************************************************\
 this creates a LINEAR spline function for the given points from 0-100
\******************************************************************************/
void cubicSpline::create( int points[], int num )
{
	// define the step size
	double stepsize = 100.0 / (num-1);
	double x1, y1, x2, y2;

	// allocate memory if needed
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

	// set up the splines based on linear spline definition
	for ( int i = 0; i < len; i++ )
	{
		x1 = stepsize * i;
		y1 = points[i];
		x2 = stepsize * (i+1);
		y2 = points[i+1];

		// with x1, x2, y1 and y2 we can define the coefficients
		coef_0[i] = -x2*y1/(x1-x2) - x1*y2/(x2-x1);
		coef_1[i] = y1/(x1-x2) + y2/(x2-x1);
	}
}

/******************************************************************************\
 depending on the value of x return a value using the linear spline coef.
\******************************************************************************/
double cubicSpline::getVal( double x )
{
	// define the step size again so we can use it in our calculations
	double stepsize = 100.0 / len;

	// the index is simply x / stepsize rounded down
	int index = (int)(x / stepsize);

	// if x is greater than 100 or less than 0 just use the closest line
	if ( index >= len )
		index = len-1;
	if ( index < 0 )
		index = 0;

	return coef_1[index] * x + coef_0[index];
}

/******************************************************************************\
 Return the value of the cubic spline function at the given x value, the spline
 is defined from 0 to 100, so x should be around there, if not x will be the
 nearest spline value
\******************************************************************************/
double cubicSpline::getCubicVal( double x )
{
	// define the step size to be used in the calculation
	double h = 100.0 / (len2+1);

	// define the index of a to be used
	int i = (int)(x/h);

	// if x is greater than 100 or less than 0 then just use the closest curve
	if ( i >= len2+1 )
		i = len2;
	if ( i < 0 )
		i = 0;
	
	// using the definition of Lagrange cubic interpolation
	if ( len2 != 0 )
		return (a[i]/(6*h)*((i+1)*h-x)*((i+1)*h-x)*((i+1)*h-x) +
				a[i+1]/(6*h)*(x-(i*h))*(x-(i*h))*(x-(i*h)) +
				(y[i]/h - a[i]*h/6)*((i+1)*h-x) +
				(y[i+1]/h-a[i+1]*h/6)*(x-i*h));
	else
		return 0;
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


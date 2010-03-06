/******************************************************************************\
 Author: Joshua Gleason

 This object is used to create cubic spline functions for the purpose of
 intermediate pixel approximation, this object is also capable of creating
 linear spline functions.

 Create a spline with...
 createCubic( int[], int ) passing an array of points and the array length
 or...
 create( int[], int ) the create the linear spline

 Obtain a value of the spline with...
 getCubicVal( double ) the spline is defined for all values of x but is defined
                       such that f(0)=firstPoint and f(100)=lastPoint
 or...
 getVal( double ) which returns the value of the linear spline at x, defined
                  such that f(0)=firstPoint and f(100)=lastPoint

 WARNING: the cubic spline must be created before calling getCubicVal as well
          as the linear spline must be defined before calling getVal, for
          example if you use createCubic then calling getVal will not give you
          the value of the linear spline(although getCubicVal will work).
\******************************************************************************/

#ifndef CUBIC_SPL
#define CUBIC_SPL

#include "rgb.h"

class cubicSpline
{
public:	
	// default constructor, intialize everything to 0 or NULL
	cubicSpline();

	// initialize the cubic spline using createCubic with the parameters
	cubicSpline( int[], int );
	cubicSpline( rgb[], int );

	// destructor de-allocates memory for all dynamically allocated memory
	~cubicSpline();

	// build the cubic spline from a list of values
	void create(int[], int);
	void create(rgb[], int);
	
	// returns the value of the linear spline
	void getVal( double, int& );
	void getVal( double, rgb& );
private:
	// holds the values used to calculate the cubic splines
	double* a[3];
	double* y[3];

	int len;	// number of sub intervals for cubic function
};

/* solves the matrix equation Ax=b for a tri-diagonal matrix, implementation
   for a better description, used in cubic spline function */
void solveTriDiag( double*, double*, double*, double*, double*, int );

#endif


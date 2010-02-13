/******************************************************************************\
 cubicSpline by Joshua Gleason

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

class cubicSpline
{
public:	
	// default constructor, intialize everything to 0 or NULL
	cubicSpline();

	// copy constructor, set all values equal to the other cubicSpline
	cubicSpline( const cubicSpline& );

	// initialize the cubic spline using createCubic with the parameters
	cubicSpline( int[], int );

	// destructor de-allocates memory for all dynamically allocated memory
	~cubicSpline();

	// builds the linear spline from a list of values
	void create( int[], int );

	// build the cubic spline from a list of values
	void createCubic(int[], int);
	
	// returns the value of the linear spline
	double getVal( double );

	// returns the value of cubic spline
	double getCubicVal( double );
private:

	// holds the coeffiencts for the linear splines
	double *coef_0;
	double *coef_1;

	// holds the values used to calculate the cubic splines
	double *a;
	double *y;

	int len; 	// number of sub intervals for linear function
	int len2;	// number of sub intervals for cubic function
};

/* solves the matrix equation Ax=b for a tri-diagonal matrix, implementation
   for a better description, used in cubic spline function */
void solveTriDiag( double*, double*, double*, double*, double*, int );

#endif


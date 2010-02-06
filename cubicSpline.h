#include <iostream>

class cubicSpline
{
public:	
	cubicSpline();
	cubicSpline( const cubicSpline& );
	cubicSpline( int[], int );
	~cubicSpline();

	// builds the cubic spline from a list of values
	void create( int[], int );
	void createCubic(int[], int);
	
	// returns the value of the spline
	double getVal( double );
	double getCubicVal( double );
private:
	double *coef_0;
	double *coef_1;
	double *a;
	double *y;
	int len; // number of sub intervals (points - 1)
	int len2;	// number of sub intervals for cubic
};

// solves the matrix equation Ax=b for x given A and b
void solveTriDiag( double*, double*, double*, double*, double*, int );


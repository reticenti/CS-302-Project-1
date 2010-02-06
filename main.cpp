#include <iostream>
#include <ctime>
#include <cstdlib>
#include "cubicSpline.h"

using namespace std;

const int SIZE = 500000;
const bool PRINT = false;


int main()
{
	cubicSpline test;
	int asdf[SIZE];

	srand( time(0) );

	for ( int i = 0; i < SIZE; i++ )
		asdf[i] = rand() % 255;

	test.createCubic( asdf, SIZE );

	if ( PRINT )
		for ( double x = 0.0; x <= 100.0; x+=100.0/(SIZE-1) )
			cout << test.getCubicVal(x) << ' ';

	return 0;
}


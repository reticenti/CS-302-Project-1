#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "image.h"

void writeImage(const char fname[], ImageType& image)
{
	int i, j;
	int N, M, Q;
	unsigned char *charImage;
	string msg;
	ofstream ofp;

	image.getImageInfo( N, M, Q );

	charImage = (unsigned char *) new unsigned char [M*N];

	// convert the integer values to unsigned char

	int val;

	for( i=0; i<N; i++ )
		for( j=0; j<M; j++ )
		{
			val = image.getPixelVal(i, j);
			charImage[i*M+j] = (unsigned char)val;
		}

	ofp.open( fname, ios::out | ios::binary );

	if ( !ofp )
	{
		msg = "Can't open file: ";
		msg += fname;
		throw msg;
	}

	ofp << "P5" << endl;
	ofp << M << " " << N << endl;
	ofp << Q << endl;

	ofp.write( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char) );

	if ( ofp.fail() )
	{
		msg = "Can't write image ";
		msg += fname;
		throw msg;
	}

	ofp.close();

	delete [] charImage;
}


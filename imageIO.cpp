#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "image.h"

void readImage(const char fname[], ImageType& image)
{
	int i, j;
	int N, M, Q;
	unsigned char *charImage;
	char header [100], *ptr;
	string msg;
	ifstream ifp;

	ifp.open( fname, ios::in | ios::binary );

	if ( !ifp )
	{
		msg = "Can't read image: ";
		msg += fname;
		throw msg;
	}

	// read header

	ifp.getline( header, 100, '\n' );
	if ( ( header[0] != 80 ) ||    /* 'P' */
             ( header[1] != 53 ) ) {   /* '5' */
		msg = "Image ";
		msg += fname;
		msg += " is not PGM";
		throw msg;
	}

	ifp.getline( header, 100, '\n' );

	while( header[0] == '#' )
		ifp.getline( header, 100, '\n');

	M = strtol( header, &ptr, 0 );
	N = atoi( ptr );

	ifp.getline( header, 100, '\n' );
	Q = strtol( header, &ptr, 0 );

	charImage = (unsigned char *) new unsigned char [M*N];

	ifp.read( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char));

	if ( ifp.fail() )
	{
		msg = "Image ";
		msg += fname;
		msg += " has wrong size";
		throw msg;
	}

	ifp.close();

	//
	// Convert the unsigned characters to integers
	//

	int val;

	for(i=0; i<N; i++)
		for(j=0; j<M; j++)
		{
			val = (int)charImage[i*M+j];
			image.setPixelVal(i, j, val);     
   		}

	delete [] charImage;
}

void readImageHeader(const char fname[], int& N, int& M, int& Q, bool& type)
{
	int i, j;
	unsigned char *charImage;
	char header [100], *ptr;
	string msg;
	ifstream ifp;

	ifp.open( fname, ios::in | ios::binary );

	if ( !ifp )
	{
		msg = "Can't read image: ";
		msg += fname;
		throw msg;
	}

	// read header

	type = false; // PGM

	ifp.getline( header, 100, '\n' );
	if ( (header[0] == 80) &&  /* 'P' */
	     (header[1] == 53) )   /* '5' */
	{
		type = false;
	}
	else if ( (header[0] == 80) &&  /* 'P' */
	          (header[1] == 54) )   /* '6' */
	{
		type = true;
	} 
	else
	{
		msg = "Image ";
		msg += fname;
		msg += " is not PGM or PPM";
		throw msg;
	}

	ifp.getline( header, 100, '\n');
	while( header[0] == '#' )
		ifp.getline( header, 100, '\n' );

	M = strtol( header, &ptr, 0 );
	N = atoi( ptr );

	ifp.getline( header, 100, '\n' );

	Q = strtol( header, &ptr, 0 );

	ifp.close();

}

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


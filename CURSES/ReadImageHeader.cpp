#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "image.h"

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

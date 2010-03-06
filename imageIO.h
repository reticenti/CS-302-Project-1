#ifndef IMAGE_IO
#define IMAGE_IO

// These are the functions used to read and write images to .pgm files I just
// added them to this header file to make linking easier to understand
//    -Josh

	#include <iostream>
	#include <fstream>
	#include <stdlib.h>
	#include <stdio.h>
	#include "image.h"

	// name : readImageHeader
	// input : cstring of filename, int N, M, Q and bool value to hold image data
	// output : sets values of image header to last 4 paramters
	// dependencies : image.h
	void readImageHeader( const char[], int&, int&, int&, bool& );

	// name : readImage
	// input : cstring of filename, and ImageType object to hold image data
	// output : set image info to the ImageType object
	// dependencies : image.h
	void readImage( const char[], ImageType<int>& );
	void readImage( const char[], ImageType<rgb>& );

	// name : writeImage
	// input : cstring of filename, and ImageType object to be store in file
	// output : writes a pgm type file with ImageType stored as a RAW form
	// dependencies : image.h
	void writeImage( const char[], ImageType<int>& );
	void writeImage( const char[], ImageType<rgb>& );

#endif


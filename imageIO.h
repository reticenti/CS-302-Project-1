#ifndef IMAGE_IO
#define IMAGE_IO

#include "image.h"

// These are the functions used to read and write images to .pgm files I just
// added them to this header file to make linking easier to understand
//    -Josh

	// name : readImageHeader
	// input : cstring of filename, int N, M, Q and bool value to hold image data
	// output : sets values of image header to last 4 paramters
	// dependencies : image.h
	void readImageHeader( const char[], int&, int&, int&, bool& );

	// name : readImage
	// input : cstring of filename, and ImageType object to hold image data
	// output : set image info to the ImageType object
	// dependencies : image.h
	void readImage( const char[], ImageType& );

	// name : writeImage
	// input : cstring of filename, and ImageType object to be store in file
	// output : writes a pgm type file with ImageType stored as a RAW form
	// dependencies : image.h
	void writeImage( const char[], ImageType& );

#endif
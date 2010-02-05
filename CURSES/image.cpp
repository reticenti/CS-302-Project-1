#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#include "image.h"

ImageType::ImageType()
{
	N = 0;
	M = 0;
	Q = 0;

	pixelValue = NULL;
}

ImageType::~ImageType()
{
	int i;
	if ( pixelValue != NULL )
	{
		for ( i = 0; i < N; i++ )
			//first delete the rows
			delete [] pixelValue[i];
		//then delete the pointer array
		delete [] pixelValue;
	}
}

ImageType::ImageType(int tmpN, int tmpM, int tmpQ)
{
	int i, j;

	N = tmpN;
	M = tmpM;
	Q = tmpQ;

	pixelValue = new int* [N];
	for(i=0; i<N; i++)
	{
		pixelValue[i] = new int[M];

		for(j=0; j<M; j++)
			pixelValue[i][j] = 0;
 	}
}

ImageType::ImageType( const ImageType& rhs )
{
	int i, j;

	rhs.getImageInfo( M, N, Q );

	pixelValue = new int* [N];
	for ( i = 0; i < N; i++ )
	{
		pixelValue[i] = new int[M];

		for ( j = 0; j < M; j++ )
			pixelValue[i][j] = rhs.getPixelVal(i, j);
	}
}

ImageType& ImageType::operator= ( const ImageType& rhs )
{
	int i, j;

	if ( pixelValue != NULL )
	{
		for ( i = 0; i < N; i++ )
			delete [] pixelValue[i];
		delete [] pixelValue;
	}

	rhs.getImageInfo( N, M, Q );

	pixelValue = new int* [N];

	for ( i = 0; i < N; i++ )
	{
		pixelValue[i] = new int[M];

		for ( j = 0; j < M; j++ )
			pixelValue[i][j] = rhs.getPixelVal(i, j);
	}

	return *this;
}

void ImageType::getImageInfo(int& rows, int& cols, int& levels) const
{
	rows = N;
	cols = M;
	levels = Q;
} 

void ImageType::setImageInfo(int rows, int cols, int levels)
{
	int i, j;

	// re-allocate the integer array if the size changes

	if ( N != rows && M != cols )
	{
		if ( pixelValue != NULL )
		{
			for ( i = 0; i < N; i++ )
				delete [] pixelValue[i];
			delete [] pixelValue;
		}

		N = rows;
		M = cols;

		pixelValue = new int* [N];

		for ( i = 0; i < N; i++ )
		{
			pixelValue[i] = new int[M];

			for ( j = 0; j < M; j++ )
				pixelValue[i][j] = 0;
		}
	}
	
	Q = levels;
}

void ImageType::setPixelVal(int i, int j, int val)
{
	pixelValue[i][j] = val;
}

int ImageType::getPixelVal(int i, int j) const
{
	return pixelValue[i][j];
}

/*******************Josh's functions**********************/

int ImageType::meanGray() const
{
	return 0;
}

void ImageType::enlargeImage( int s, const ImageType& old )
{

}

void ImageType::reflectImage( bool flag, const ImageType& old )
{

}

ImageType& ImageType::operator- ( const ImageType& rhs )
{
	return *this;	// temp return value
}

void ImageType::negateImage()
{

}

/******************Josiah's functions*********************/

void ImageType::getSubImage( int ULr, int ULc, int LRr, int LRc, const ImageType& old )
{
	int width, height;
	//get x width
	height = abs(ULr - LRr);
	
	//get y height
	width = abs(ULc - LRc);

	//make a new array for the exact size of the new subimage
	setImageInfo(height, width, old.Q);
	

	//copy over the old stuff into the new subimage array
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			pixelValue[i][j] = old.pixelValue[ULr + i][ULc + j];
		}
	}
}

void ImageType::shrinkImage( int s, const ImageType& old )
{

}

void ImageType::translateImage( int t, const ImageType& old )
{

}

void ImageType::rotateImage( int theta, const ImageType& old )
{

}

ImageType& ImageType::operator+ ( const ImageType& rhs )
{
	//this is the value that determines the weight of each image
	//large a gives more weight to first image
	//small a gives more weight to second image
	//a must be 0 >= a <= 1
	float a = 0.5;

	//the general formula is aI1(r,c)+(1-a)I2(r,c)
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			//I don't know yet if I need to type cast it, I dont think so
			//(float)pixelValue[i][j]*a + (1.0 - a)*(float)rhs[i][j];
			//It turns out that c++ is smart enough to type cast for me
			pixelValue[i][j] = pixelValue[i][j]*a + (1.0 - a)*rhs.pixelValue[i][j];
		}
	}

	return *this;	// temp return value
}


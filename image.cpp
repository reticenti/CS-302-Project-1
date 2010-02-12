#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "cubicSpline.h"
#include <cmath>

using namespace std;

#include "image.h"

/*****************************************************************************\
 default constructor allocates no memory and sets the size to zero 
\*****************************************************************************/
ImageType::ImageType()
{
	// start everything at zero
	N = 0;
	M = 0;
	Q = 0;

	pixelValue = NULL;
}

/*****************************************************************************\
 destructor wipes any memory that was dynamically allocated
\*****************************************************************************/
ImageType::~ImageType()
{
	int i;
	if ( pixelValue != NULL )
	{
		// delete all the memory for the array
		for ( i = 0; i < N; i++ )
			delete [] pixelValue[i];
		delete [] pixelValue;
	}
}

/*****************************************************************************\
 change the dimensions of the image, delete and re-allocate memory if required
\*****************************************************************************/
ImageType::ImageType(int tmpN, int tmpM, int tmpQ)
{
	// set the new values of N, M and Q
	setImageInfo( tmpN, tmpM, tmpQ );
}

/*****************************************************************************\
 copy contructor, copys data from rhs to the current object
\*****************************************************************************/
ImageType::ImageType( const ImageType& rhs )
{
	int i, j;
	
	// set the info to the new image data
	setImageInfo( rhs.N, rhs.M, rhs.Q );

	for ( i = 0; i < N; i++ )
		for ( j = 0; j < M; j++ )
			pixelValue[i][j] = rhs.pixelValue[i][j];
}

/*****************************************************************************\
 equal operator overload, this is basically the same as the copy constructor
 except it will likely have to de-allocate memory before copying values, all
 this is decided in setImageInfo however
\*****************************************************************************/
ImageType& ImageType::operator= ( const ImageType& rhs )
{
	int i, j;

	setImageInfo( rhs.N, rhs.M, rhs.Q );

	for ( i = 0; i < N; i++ )
		for ( j = 0; j < M; j++ )
			pixelValue[i][j] = rhs.pixelValue[i][j];

	return *this;
}

/*****************************************************************************\
 returns the width height and color depth to reference variables
\*****************************************************************************/
void ImageType::getImageInfo(int& rows, int& cols, int& levels) const
{
	rows = N;
	cols = M;
	levels = Q;
} 

/*****************************************************************************\
 sets the image info, deleting and allocating memory as required
\*****************************************************************************/
void ImageType::setImageInfo(int rows, int cols, int levels)
{
	int i, j;

	// re-allocate the integer array if the size changes
	if ( N != rows && M != cols )
	{
		// delete memory if not NULL
		if ( pixelValue != NULL )
		{
			for ( i = 0; i < N; i++ )
				delete [] pixelValue[i];
			delete [] pixelValue;
		}

		// sets N and M
		N = rows;
		M = cols;

		// allocate the rows of pixel value
		pixelValue = new int* [N];

		// allocate the columns of pixel value
		for ( i = 0; i < N; i++ )
		{
			pixelValue[i] = new int[M];
			
			// set all the values to black
			for ( j = 0; j < M; j++ )
				pixelValue[i][j] = 0;
		}
	}
	
	// set Q equal to the levels
	Q = levels;
}

/*****************************************************************************\
 sets the value of a pixel
\*****************************************************************************/
void ImageType::setPixelVal(int i, int j, int val)
{
	pixelValue[i][j] = val;
}

/*****************************************************************************\
 returns the value of a pixel
\*****************************************************************************/
int ImageType::getPixelVal(int i, int j) const
{
	return pixelValue[i][j];
}

/****************************Josh's functions**********************************/


/*****************************************************************************\
 this calculates the average gray value in the picture, this is done by adding
 all of the pixels and dividing by the total number of pixels
\*****************************************************************************/
double ImageType::meanGray() const
{
	double gray = 0;

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			gray += pixelValue[i][j];

	return ( M*N != 0 ? gray/(M*N) : 0 );
}

/******************************************************************************\
 This simply calls the double version of enlargeImage
\******************************************************************************/
void ImageType::enlargeImage( int S, const ImageType& old, bool cubic )
{
	enlargeImage( (double)S, old, cubic );
}

/******************************************************************************\
 This function enlarges an image by a magnitude of s, so for example if the
 original function was 100x100 and s is 10, then the new image is 1000x1000

 The method I choose to use was bicubic interpolation which creates cubic
 splines for each row and column, the splines are then used to approximate the
 value of the pixels on the larger image.  This is possible because the spline
 function is a continuous function and is therefore defined at every point.

 if cubic = true then use cubic interpolation
 if cubic = false then use linear interpolation
\******************************************************************************/
void ImageType::enlargeImage( double S, const ImageType& old, bool cubic )
{
	// scale size rounded to integer value
	int s = S+0.5;
	int *horizVals = new int[old.M];
	int *vertVals = new int[old.N];

	ImageType temp;

	cubicSpline spline;

	setImageInfo( old.M * s, old.N * s, old.Q );

	temp.setImageInfo( old.M, N, old.Q );

	for ( int row = 0; row < old.N; row++ )
	{
		for ( int col = 0; col < old.M; col++ )
			horizVals[col] = old.pixelValue[row][col];

		spline.createCubic( horizVals, old.M );

		for ( int col = 0; col < M; col++ )
		{
			// value to pull from spline for current j
			double splineX = (col-s/2.0)/(M-S-1) * 100.0;
			int colorVal = spline.getCubicVal(splineX);
			
			if ( colorVal < 0 ) colorVal = 0;
			if ( colorVal > Q ) colorVal = Q;

			temp.pixelValue[row][col] = colorVal;
		}
	}

	for ( int col = 0; col < M; col++ )
	{
		for ( int row = 0; row < old.N; row++ )
			vertVals[row] = temp.pixelValue[row][col];

		spline.createCubic( vertVals, old.N );

		for ( int row = 0 ; row < N; row++ )
		{
			double splineX = (row-s/2.0)/(M-S-1) * 100.0;
			int colorVal = spline.getCubicVal(splineX);
			
			if ( colorVal < 0 ) colorVal = 0;
			if ( colorVal > Q ) colorVal = Q;

			pixelValue[row][col] = colorVal;
		}
	}

	delete [] horizVals;
	delete [] vertVals;
}

/******************************************************************************\
 reflects image by moving the pixel to N or M minus the current row or column
 depending on the value of the flag (true being a horizontal reflection and
 false being a vertical reflection)
\******************************************************************************/
void ImageType::reflectImage( bool flag, const ImageType& old )
{
	setImageInfo( old.N, old.M, old.Q );
	
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			pixelValue[i][j] = ( flag ? old.pixelValue[i][M-j-1] : old.pixelValue[N-i-1][j] );
}

/******************************************************************************\
 subtract two images from eachother to see the differences, if the magnitude of
 the difference is less then Q/10 then the pixel is replaced with black,
 otherwise white is used.
\******************************************************************************/
ImageType& ImageType::operator- ( const ImageType& rhs )
{
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
		{
			pixelValue[i][j] = abs(pixelValue[i][j] - rhs.pixelValue[i][j]);

			// if pixels are less than Q/10 different then make them black
			if ( pixelValue[i][j] < Q/10 )
				pixelValue[i][j] = 0;
			else
				pixelValue[i][j] = Q;
		}
	return *this;	// temp return value
}

/******************************************************************************\
 This function simply subtracts the current pixel value from the max value of
 the pixel, thus negating the image
\******************************************************************************/
void ImageType::negateImage()
{
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			pixelValue[i][j] = Q - pixelValue[i][j];
}

/*****************************Josiah's functions*******************************/

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
	int offset = 0;
        int row = 0, col = 0;

        //make new array with correct size
        setImageInfo(old.N / s, old.M / s, old.Q);

        //copy over every s pixel
        for(int i = 0; i < N; i++){
                for(int j = 0; j < M; j++){
                        pixelValue[i][j] = old.pixelValue[i*s][j*s];
                }
        }

}

void ImageType::translateImage( int t, const ImageType& old )
{

	//make this image's image array
	setImageInfo(old.N, old.M, old.Q);

	//find the bottom corner
	//go up s pixels
	//go left s pixels
	//cp current to i+t, j+t
	for(int i = N - 1; i >= 0 + t; i--)
		for(int j = M - 1; j >= 0 + t; j--){
			pixelValue[i][j] = old.pixelValue[i - t][j - t];
			//make old = 0
			pixelValue[i - t][j - t] = 0;
		}
}

void ImageType::rotateImage( int theta, const ImageType& old )
{
	setImageInfo(old.N, old.M, old.Q);
	float rad = theta * 3.1415/180;
	float r, c;

	for(int i = 0; i < N; i++){
		for(int j = 0; i < M; i++){
			r = i*cos(rad) - j*sin(rad);
			c = i*sin(rad) + j*cos(rad);
			if(((int)r < N) && ((int)c < M))
				pixelValue[i][j] = old.pixelValue[(int)r][(int)c];
		}
	}

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


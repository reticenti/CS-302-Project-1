#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "cubicSpline.h"

using namespace std;

#include "image.h"

const bool INTERPL = true;	// use interpolation to enlarge
const bool CUBIC   = true;	// only valid if INTERPL is true

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

/*enlargeImage*****************************************************************\
 This function enlarges an image by a magnitude of s, so for example if the
 original function was 100x100 and s is 10, then the new image is 1000x1000

 The method I choose to use was bicubic interpolation which creates cubic
 splines for each row and column, the splines are then used to approximate the
 value of the pixels on the larger image.  This is possible because the spline
 function is a continuous function and is therefore defined at every point.
\******************************************************************************/
void ImageType::enlargeImage( double S, const ImageType& old )
{
	int s = S;

	// if the INTERPL flag is set to false do nearest neighbour enlargement
	if ( !INTERPL )
	{
		// reset image info
		setImageInfo( old.N * s, old.M * s, old.Q );

		// go through each pixel copying the closest pixel on the old image
		for ( int i = 0; i < N; i++ )
			for ( int j = 0; j < M; j++ )
				pixelValue[i][j] = old.pixelValue[i/s][j/s];
	}
	else
	{
		cubicSpline row, col;
		int minOld, maxOld, diag, index;
		int *rows = new int[old.M];
		int *cols = new int[old.N];
		double offR, offC;

		setImageInfo( old.N * S, old.M * S, old.Q );

		std::cout << N << '\n' << M << '\n';
		
		// find the lesser of M and N
		if ( old.M > old.N )
		{
			minOld = old.N;
			maxOld = old.M;
		}
		else
		{
			minOld = old.M;
			maxOld = old.N;
		}

		// counts diagonally from one corner to the other
		for ( diag = 0; diag < minOld; diag++ )
		{	
			// fills the column array
			for ( index = 0; index < old.N; index++ )
				cols[index] = old.pixelValue[index][diag];

			// sets the rows array
			for ( index = 0; index < old.M; index++ )
				rows[index] = old.pixelValue[diag][index];

			// calculate the cubic spline functions for the current row and column
			( CUBIC ? row.createCubic( rows, old.M ) : row.create( rows, old.M ) );
			( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

			/* this counts through the rows of the new image setting their values
			   according to the spline functions */
			for ( int r = 0; r < N; r++ )
			{
				/* this offset makes the pixel on the new image centered rather then pressed up
				   against the upper left corner like they want to be */
				offR = r-S/2;
				
				// evaluate the value of the pixel here
				if ( CUBIC )
				{
					if ( (int)(diag*S+s/2) > M-1 || (int)(diag*S+s/2) < 0 )
					{
						std::cout << "N-1  : " << M-1  << std::endl;
						std::cout << "diag : " << diag << std::endl;
						std::cout << "S    : " << S    << std::endl;
						std::cout << "s    : " << s    << std::endl;
						std::cout << "s/2  : " << s/2  << std::endl;
						std::cout << "whole: " << (int)(diag*S+s/2) << std::endl;
					}
					pixelValue[r][(int)(diag*S+s/2)] = (int)col.getCubicVal( (double)offR/(N-S) * 100.0 );
				}
				else
					pixelValue[r][(int)(diag*S+s/2)] = (int)col.getVal( (double)offR/(N-S) * 100.0 );

				// clip the pixel value if it goes out of bounds
				if ( pixelValue[r][(int)(diag*S+s/2)] > Q )
					pixelValue[r][(int)(diag*S+s/2)] = Q;
				if ( pixelValue[r][(int)(diag*S+s/2)] < 0 )
					pixelValue[r][(int)(diag*S+s/2)] = 0;
			}
			
			// same as previous loop except counts through the columns
			for ( int c = 0; c < M; c++ )
			{
				offC = c-S/2;
				if ( CUBIC )
				{
					pixelValue[(int)(diag*S+s/2)][c] = (int)row.getCubicVal( (double)offC/(M-S) * 100.0 );
				}
				else
					pixelValue[(int)(diag*S+s/2)][c] = (int)row.getVal( (double)offC/(M-S) * 100.0 );

				if ( pixelValue[(int)(diag*S+s/2)][c] > Q )
					pixelValue[(int)(diag*S+s/2)][c] = Q;
				if ( pixelValue[(int)(diag*S+s/2)][c] < 0 )
					pixelValue[(int)(diag*S+s/2)][c] = 0;
			}
		}

		/* if the image isn't square finish filling the grid on the longer side
		   these are the same as the previous loops except they only do rows or
		   columns depending on the value of minOld and maxOld */
		if ( minOld != maxOld && maxOld == old.M )
			for ( diag = minOld; diag < maxOld; diag++ )
			{
				for ( index = 0; index < old.N; index++ )
					cols[index] = old.pixelValue[index][diag];

				( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

				for ( int r = 0; r < N; r++ )
				{
					offR = r-S/2;
					if ( CUBIC )
						pixelValue[r][(int)(diag*S+s/2)] = (int)col.getCubicVal( (double)offR/(N-S) * 100.0 );
					else
						pixelValue[r][(int)(diag*S+s/2)] = (int)col.getVal( (double)offR/(N-S) * 100.0 );

					if ( pixelValue[r][(int)(diag*S+s/2)] > Q )
						pixelValue[r][(int)(diag*S+s/2)] = Q;
					if ( pixelValue[r][(int)(diag*S+s/2)] < 0 )
						pixelValue[r][(int)(diag*S+s/2)] = 0;
				}	
			}
		else if ( minOld != maxOld && maxOld == old.N )
			for ( diag = minOld; diag < maxOld; diag++ )
			{
				for ( index = 0; index < old.M; index++ )
					rows[index] = old.pixelValue[diag][index];

				( CUBIC ? row.createCubic( rows, old.M ) : row.create( rows, old.M ) );

				for ( int c = 0; c < M; c++ )
				{
					offC = c-S/2;
					if ( CUBIC )
						pixelValue[(int)(diag*S+s/2)][c] = (int)row.getCubicVal( (double)offC/(M-S) * 100.0 );
					else
						pixelValue[(int)(diag*S+s/2)][c] = (int)row.getVal( (double)offC/(M-S) * 100.0 );

					if ( pixelValue[(int)(diag*S+s/2)][c] > Q )
						pixelValue[(int)(diag*S+s/2)][c] = Q;
					if ( pixelValue[(int)(diag*S+s/2)][c] < 0 )
						pixelValue[(int)(diag*S+s/2)][c] = 0;
				}
			}

		/* At this point in the function the image is just a grid of lines calculated
		   using the spline functions, everything else is black space, the old image
		   pixel values will no longer be needed as everything is now based on the
		   approximations between the known pixels */

		/* this counts vertically and fills in the rest of the picture based on
		   values of the gridlines */
		 
		for ( diag = 0; diag < N; diag++ )
		{
			// obtain the values of grid lines from the current image
			for ( index = 0; index < old.M; index++ )
				rows[index] = pixelValue[diag][(int)(index*S+s/2)];

			// create the spline function for the current row
			( CUBIC ? row.createCubic( rows, old.M ) : row.create( rows, old.M ) );

			// count through the columns calculating the values between every point
			for ( int c = 0; c < M; c++ )
			{
				offC = c-S/2;
				if ( CUBIC )
					pixelValue[diag][c] = (int)row.getCubicVal( (double)offC/(M-S) * 100.0 );
				else
					pixelValue[diag][c] = (int)row.getVal( (double)offC/(M-S) * 100.0 );

				if ( pixelValue[diag][c] > Q )
					pixelValue[diag][c] = Q;
				if ( pixelValue[diag][c] < 0 )
					pixelValue[diag][c] = 0;
			}
		}

		/* at this point the image is actually filled completely but this takes
		   the average of the values calculated from the horizonal approximation
		   in the above loop and this loop which is a vertical approximation */

		for ( diag = 0; diag < M; diag++ )
		{
			for ( index = 0; index < old.N; index++ )
				cols[index] = pixelValue[(int)(index*S+s/2)][diag];

			( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

			for ( int r = 0; r < N; r++ )
			{
				offR = r-S/2;
				if ( CUBIC )
					pixelValue[r][diag] += (int)col.getCubicVal( (double)offR/(N-S) * 100.0 );
				else
					pixelValue[r][diag] += (int)col.getVal( (double)offR/(N-S) * 100.0 );

				/* this is the line that takes the average of the pixel value
    			   that was just calculated */
				pixelValue[r][diag] /= 2;

				if ( pixelValue[r][diag] > Q )
					pixelValue[r][diag] = Q;
				if ( pixelValue[r][diag] < 0 )
					pixelValue[r][diag] = 0;
			}
		}
		
		// de-allocate the memory for the temporary arrays
		delete [] rows;
		delete [] cols;
	}
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
	//		if ( pixelValue[i][j] < Q/10 )
//				pixelValue[i][j] = 0;
//			else
//				pixelValue[i][j] = Q;
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


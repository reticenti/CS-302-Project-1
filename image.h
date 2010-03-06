/******************************************************************************\
 Authors: Josiah Humphrey and Joshua Gleason

 Date Due for Review: 02/16/2010

 This object is used for storing and manipulating images, it allows the images
 to be processed and manipulated in multiple ways including, re-scaling,
 rotating, translating, adding, subtracting, negating, etc...
\******************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "cubicSpline.h"

using namespace std;

// size of one grid on the background grid
const int BACKGRID = 25;

template <class pType>
class ImageType {
public:

// CONSTRUCTORS AND DESTRUCTOR /////////////////////////////////////////////////
	// default construtor, sets everything to NULL and 0
	ImageType();

	// parameterized constructor sets up image to N, M and Q values
	ImageType(int, int, int);

	// copy allocates memory and copies info from the right hand side
	ImageType( const ImageType<pType>& );

	// same as copy except it de-allocates memory first if necessary
	ImageType<pType>& operator= ( const ImageType<pType>& );

	// destructor removes all dynamically allocated memory
	~ImageType();
	
// IMAGE FUNCTIONS /////////////////////////////////////////////////////////////
	// returns the N, M and Q values to the calling function
	void getImageInfo(int&, int&, int&) const;

	// sets N, M and Q and also de-allocates and allocates memory if necessary
	void setImageInfo(int, int, int);

	// sets the value of a pixel at row, column to the 3rd parameters value
	void setPixelVal(int, int, pType);

	// return the pixel value at the desired location
	pType getPixelVal(int, int) const;

////// Josh's functions ////////////////////////////////////////////////////////

	// returns the mean value of all the pixels
	pType meanColor() const;

	// enlarge the image by a factor is s, uses bi-cubic interpolation if the
	// bool is true, otherwise uses bi-linear interpolation which is more light
	// weight.  The version with an int value for s simply casts s to a double
	// and calls the version that takes a double
	void enlargeImage( double, const ImageType<pType>& );
	void enlargeImage( int, const ImageType<pType>& );

	// reflect the image either horiz. or vert. depending on the bool value
	void reflectImage( bool, const ImageType<pType>& );

	// subtract 2 images, make any pixels that differ by more than Q/6 white
	// and make the rest black.  This helps reduce some noise
	ImageType& operator- ( const ImageType<pType>& );
	
	// calculate the negative of every pixel (Q - current value)
	void negateImage();

////// Josiah's functions //////////////////////////////////////////////////////

	// calculate a sub image given the row,col for the upper left and lower
	// right corners
	void getSubImage( int, int, int, int, const ImageType<pType>& );

	// shrink image by a factor of s, find the average value for each 'block' of
	// pixels that is reduced and use that value.  This makes a smoother reduce
	// function
	void shrinkImage( int, const ImageType<pType>& );

	// translate the image down to the right by t pixels, effectively cutting
	// off the bottom and right side by the same number of pixels
	void translateImage( int, const ImageType<pType>& );

	// rotate image by theta degrees counter-clockwise
	void rotateImage( int, const ImageType<pType>& );

	// sum two images giving no particular bias to one or the other
	ImageType& operator+ ( const ImageType<pType>& );


private:
	int N; // # of rows
	int M; // # of cols
	int Q; // # of gray-level values

	// array of pixel values
	pType **pixelValue;
};


/******************************************************************************\
 default constructor allocates no memory and sets the size to zero 
\******************************************************************************/
template <class pType>
ImageType<pType>::ImageType()
{
	// start everything at zero
	N = 0;
	M = 0;
	Q = 0;

	pixelValue = NULL;
}

/******************************************************************************\
 destructor wipes any memory that was dynamically allocated
\******************************************************************************/
template <class pType>
ImageType<pType>::~ImageType()
{
	if ( pixelValue != NULL )
	{
		// delete all the memory for the array
		for ( int i = 0; i < N; i++ )
			delete [] pixelValue[i];
		delete [] pixelValue;
	}
}

/******************************************************************************\
 change the dimensions of the image, delete and re-allocate memory if required
\******************************************************************************/
template <class pType>
ImageType<pType>::ImageType(int tmpN, int tmpM, int tmpQ)
{
	pixelValue = NULL;
	N = M = Q = 0;
	// set the new values of N, M and Q
	setImageInfo( tmpN, tmpM, tmpQ );
}

/******************************************************************************\
 copy contructor, copys data from rhs to the current object
\******************************************************************************/
template <class pType>
ImageType<pType>::ImageType( const ImageType<pType>& rhs )
{
	// set the info to the new image data
	setImageInfo( rhs.N, rhs.M, rhs.Q );

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			pixelValue[i][j] = rhs.pixelValue[i][j];
}

/******************************************************************************\
 equal operator overload, this is basically the same as the copy constructor
 except it will likely have to de-allocate memory before copying values, all
 this is decided in setImageInfo however
\******************************************************************************/
template <class pType>
ImageType<pType>& ImageType<pType>::operator= ( const ImageType<pType>& rhs )
{
	if(this != &rhs){
		setImageInfo( rhs.N, rhs.M, rhs.Q );

		// copy pixel values
		for ( int i = 0; i < N; i++ )
			for ( int j = 0; j < M; j++ )
				pixelValue[i][j] = rhs.pixelValue[i][j];
	}

	return *this;
}

/******************************************************************************\
 returns the width height and color depth to reference variables
\******************************************************************************/
template <class pType>
void ImageType<pType>::getImageInfo(int& rows, int& cols, int& levels) const
{
	rows = N;
	cols = M;
	levels = Q;
} 

/******************************************************************************\
 sets the image info, deleting and allocating memory as required, also create
 background grid
\******************************************************************************/
template <class pType>
void ImageType<pType>::setImageInfo(int rows, int cols, int levels)
{
	// re-allocate the integer array if the size changes
	if ( N != rows && M != cols )
	{
		// delete memory if not NULL
		if ( pixelValue != NULL )
		{
			for ( int i = 0; i < N; i++ )
				delete [] pixelValue[i];
			delete [] pixelValue;
		}

		// sets N and M
		N = rows;
		M = cols;

		// allocate the rows of pixel value
		pixelValue = new pType* [N];

		// allocate the columns of pixel value
		for ( int i = 0; i < N; i++ )
			pixelValue[i] = new pType[M];
	}

	// set Q equal to the levels
	Q = levels;

	// make a checkered background
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
		{
			if ( ( (i%(BACKGRID*2)+1.0) / (BACKGRID*2.0) > 0.5 &&
				   (j%(BACKGRID*2)+1.0) / (BACKGRID*2.0) <= 0.5 ) ||
			     ( (i%(BACKGRID*2)+1.0) / (BACKGRID*2.0) <= 0.5 &&
				   (j%(BACKGRID*2)+1.0) / (BACKGRID*2.0) > 0.5 ) )
				pixelValue[i][j] = Q/2;
			else
				pixelValue[i][j] = Q/3;
		}
}

/******************************************************************************\
 sets the value of a pixel
\******************************************************************************/
template <class pType>
void ImageType<pType>::setPixelVal(int i, int j, pType val)
{
	pixelValue[i][j] = val;
}

/******************************************************************************\
 returns the value of a pixel
\******************************************************************************/
template <class pType>
pType ImageType<pType>::getPixelVal(int i, int j) const
{
	return pixelValue[i][j];
}

/****************************Josh's functions**********************************/

/******************************************************************************\
 this calculates the average gray value in the picture, this is done by adding
 all of the pixels and dividing by the total number of pixels
\******************************************************************************/
template <class pType>
pType ImageType<pType>::meanColor() const
{
	pType total;

	total = 0;
	
	// sum the gray values
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			total += pixelValue[i][j];

	// return 0 if there are no pixels, otherwise divide gray by total pixels
	if ( M*N == 0 )
	{
		total = 0;
	} else
	{
		total = total/(M*N);
	}
	return total;
}

/******************************************************************************\
 This simply calls the double version of enlargeImage
\******************************************************************************/
template <class pType>
void ImageType<pType>::enlargeImage( int S, const ImageType<pType>& old )
{
		// call double version of enlarge
		enlargeImage( (double)S, old );
}

/******************************************************************************\
 This function enlarges an image by a magnitude of s, so for example if the
 original function was 100x100 and s is 10, then the new image is 1000x1000

 The method I choose to use was bicubic/linear interpolation which creates
 splines for each column(cubic or linear), then using those splines create an
 image which is a stretched version of the original image.  The way I achieved
 this was to stretch the entire image only vertically, and then stretch that
 image horizontally.  I did the same thing except reversed (stretched image
 horizontally first) and then summed the two images together.  This gives an
 average value between both methods.  Although it can handle S values less
 than 1, the shrinkImage function works better for this.

 if cubic = true then use cubic interpolation
 if cubic = false then use linear interpolation
\******************************************************************************/
template <class pType>
void ImageType<pType>::enlargeImage( double S, const ImageType<pType>& old )
{
	// check parameters
	if ( old.M < 4 || old.N < 4 )
		// force linear if less than 4 height or width
		throw (string)"Image too small to enlarge, must be at least 3x3";


	// slightly modify S to make it a better value for spline
	double Shoriz = (double)((int)(old.M*S))/old.M;
	double Svert = (double)((int)(old.N*S))/old.N;
	
	pType colorVal;	// used to hold the calculated color value
	double splineX; // the x value passed to the spline function

	pType *horizVals = new pType[old.M];	// holds points for spline
	pType *vertVals = new pType[old.N];		// holds points for spline

	// temporary images used to stretch before 2nd interpolation
	ImageType horiz, vert, half1, half2;

	// this is the spline object used to store the spline values
	cubicSpline spline;

	// set the new image to the
	setImageInfo( old.N * S, old.M * S, old.Q );

	// set temp to a stretched horizontally only
	horiz.setImageInfo( old.N, M, Q );
	vert.setImageInfo( N, old.M, Q );
	half1.setImageInfo( N, M, Q );
	half2.setImageInfo( N, M, Q );

	// stretch old image vertiacally and store in vert
	for ( int col = 0; col < old.M; col++ )
	{
		// get the values used to create the spline for the column
		for ( int row = 0; row < old.N; row++ )
			vertVals[row] = old.pixelValue[row][col];

		// actually create the spline (assumed the pixels are equally spaced)
		spline.create( vertVals, old.N );

		// using the spline set the values of vert
		for ( int row = 0; row < N; row++ )
		{
			// value to pull from spline for current row
			splineX = (row-Svert/2.0)/(N-Svert-1.0) * 100.0;
			
			spline.getVal(splineX, colorVal);
			
			// NOTE: don't clip values yet, it causes problems...

			vert.pixelValue[row][col] = colorVal;
		}
	}

	// now stretch vert horizontally and store in half1
	for ( int row = 0; row < N; row++ )
	{
		// get the values used to create the spline for the row
		for ( int col = 0; col < old.M; col++ )
			horizVals[col] = vert.pixelValue[row][col];

		// actually create the spline (cubic if parameter is true)
		spline.create( horizVals, old.M );

		// using the spline set the values
		for ( int col = 0 ; col < M; col++ )
		{
			splineX = (col-Shoriz/2.0)/(M-Shoriz-1.0) * 100.0;
			
			// obtain new color from spline value
			spline.getVal(splineX, colorVal);
			
			// set the pixel value for half1
			half1.pixelValue[row][col] = colorVal;
		}
	}

	// stretch old image horizontally and store in horiz
	for ( int row = 0; row < old.N; row++ )
	{
		// get the values used to create the spline for the row
		for ( int col = 0; col < old.M; col++ )
			horizVals[col] = old.pixelValue[row][col];

		// actually create the spline (assumed the pixels are equally spaced)
		spline.create( horizVals, old.M );

		// using the spline set the values of temp
		for ( int col = 0; col < M; col++ )
		{
			// value to pull from spline for current col
			splineX = (col-Shoriz/2.0)/(M-Shoriz-1.0) * 100.0;
			
			// obtain new color from spline value
			spline.getVal(splineX, colorVal);
			
			// NOTE: don't clip values yet, it causes problems...

			horiz.pixelValue[row][col] = colorVal;
		}
	}

	// now stretch horiz vertically and store in half2
	for ( int col = 0; col < M; col++ )
	{
		// get the values used to create the spline for the column
		for ( int row = 0; row < old.N; row++ )
			vertVals[row] = horiz.pixelValue[row][col];

		// actually create the spline (cubic if parameter is true)
		spline.create( vertVals, old.N );

		// using the spline set the values
		for ( int row = 0 ; row < N; row++ )
		{
			splineX = (row-Svert/2.0)/(N-Svert-1.0) * 100.0;
			
			// obtain new color from spline value
			spline.getVal(splineX, colorVal);
			
			// set the value of half2
			half2.pixelValue[row][col] = colorVal;
		}
	}

	// sum half1 and half2 to get the average value everywhere
	*this = half1 + half2;

	// de-allocate all that memory
	delete [] horizVals;
	delete [] vertVals;
}

/******************************************************************************\
 reflects image by moving the pixel to N or M minus the current row or column
 depending on the value of the flag (true being a horizontal reflection and
 false being a vertical reflection)
\******************************************************************************/
template <class pType>
void ImageType<pType>::reflectImage( bool flag, const ImageType<pType>& old )
{
	// set image info same as old's
	setImageInfo( old.N, old.M, old.Q );

	// if flag is set copy opposite row, if not copy opposite column
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			if ( flag )
				pixelValue[i][j] = old.pixelValue[i][M-j-1];
			else
				pixelValue[i][j] = old.pixelValue[N-i-1][j];
}

/******************************************************************************\
 subtract two images from eachother to see the differences, if the magnitude of
 the difference is less then Q/6 then the pixel is replaced with black,
 otherwise white is used.  This seems to help reduce the amount of noise in the
 pictures
\******************************************************************************/
template <class pType>
ImageType<pType>& ImageType<pType>::operator- ( const ImageType<pType>& rhs )
{
	// throw exception if images don't match
	if ( N != rhs.N || M != rhs.M || Q != rhs.Q )
		throw (string)"Images do not have the same dimensions!";

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
		{
			// calculate subtracted value
			pixelValue[i][j] = abs(pixelValue[i][j] - rhs.pixelValue[i][j]);

			// if pixels are less than Q/6 different then make them black
			// this helps prevent noise
			if ( pixelValue[i][j] < Q/6 )
				pixelValue[i][j] = 0;
			else
				pixelValue[i][j] = Q;
		}
	return *this;	// return current object
}

/******************************************************************************\
 This function simply subtracts the current pixel value from the max value of
 the pixel, thus negating the image
\******************************************************************************/
template <class pType>
void ImageType<pType>::negateImage()
{
	// calculate the negative of each pixel
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
		{
			pixelValue[i][j] = pixelValue[i][j] - Q;
			pixelValue[i][j] = pixelValue[i][j] * (-1);
		}
}

/*****************************Josiah's functions*******************************/

/******************************************************************************\
 Obtain a sub-image from old.  Uses the coordinates of the upper left corner
 and lower right corner to obtain image.
\******************************************************************************/
template <class pType>
void ImageType<pType>::getSubImage( int ULr, int ULc, int LRr, int LRc,
    const ImageType& old )
{
	int width, height;
	// get sub image height
	height = abs(ULr - LRr);
 
	// get sub image width
	width = abs(ULc - LRc);
 
	// make a new array for the exact size of the new subimage
	setImageInfo(height, width, old.Q);
  
	// copy over the old stuff into the new subimage array
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++)
			pixelValue[i][j] = old.pixelValue[ULr+i][ULc+j];
}

/******************************************************************************\
 Shrink image, average all the values in the block to make the new pixel, this
 makes the shrink much less jagged looking in the end
\******************************************************************************/
template <class pType>
void ImageType<pType>::shrinkImage( int s, const ImageType<pType>& old )
{
	// used to find average pixel value
	pType total;
	int num;

    // make new array with correct size
	setImageInfo(old.N / s, old.M / s, old.Q);

	// copy over every s pixel
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++) {
			// reset values for averaging
			total = 0;
			num = 0;

			// sum the total value of pixels in the row/col
			for ( int row = i*s; row < (i+1)*s; row++ )
				for ( int col = j*s; col < (j+1)*s; col++ ) {
					total += old.pixelValue[row][col];
					num++;
				}

			// set the new pixel value
			pixelValue[i][j] = total/num;
		}
}

/******************************************************************************\
 Translate the image down to the right, any part that goes out of the screen is
 not calculated.  Checkered background from setImageInfo is retained.
\******************************************************************************/
template <class pType>
void ImageType<pType>::translateImage( int t, const ImageType<pType>& old )
{
	//make this image's image array
	setImageInfo(old.N, old.M, old.Q);

	// count backwards through to the newly defined upper left corner copying
	// from everywhere before
	for(int i = N - 1; i >= 0 + t; i--)
		for(int j = M - 1; j >= 0 + t; j--)
			pixelValue[i][j] = old.pixelValue[i-t][j-t];
}

/******************************************************************************\
 Rotate the image counter-clockwise using bilinear interpolation, basically
 traversing the entire image going from the destination to the source by using
 the equation in reverse (which is why the angle is reversed).  Once a location
 is determined the surrounding pixels are used to calculate intermediate values
 between the pixels, this gives a pretty smooth rotate.

 - Originally written by Josiah, modified with Joshua's help
\******************************************************************************/
template <class pType>
void ImageType<pType>::rotateImage( int theta, const ImageType<pType>& old )
{
	// reverse theta to make a counter-clockwise rotation
	theta *= -1;

	// set image to correct size
	setImageInfo(old.N, old.M, old.Q);
	pType final;	// holds final color value for given location
	
	// holds various color values
	pType UL, UR, LL, LR, U, D, L, R, Hval, Vval;

	// holds the slopes between different points
	pType USlope, DSlope, LSlope, RSlope, HSlope, VSlope;

	// 4 * atan(1) = pi
	float rad = theta * 4 * atan(1.0)/180;

	// row, column, middle row, middle column
	double r, c, r_0, c_0;

	// set middle values
	r_0 = N/2.0;
	c_0 = M/2.0;

	// loops through entire picture, going from dest, to source to prev holes
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			// calculate where the original value should be
			r = r_0 + (i-r_0)*cos(rad) - (j-c_0)*sin(rad);
			c = c_0 + (i-r_0)*sin(rad) + (j-c_0)*cos(rad);

			// old method
			/*if ( r < N && r >= 0 && c < M && c >= 0 )
				final = old.pixelValue[(int)r][(int)c];
			else
				final = pixelValue[i][j];*/

			// only draw a pixel if source value is in range
			if ( r > 0 && ceil(r) < N && c > 0 && ceil(c) < M ) {

				// get four pixel value which surround the desired value
				UL = old.pixelValue[(int)r][(int)c];
				UR = old.pixelValue[(int)r][(int)ceil(c)];
				LL = old.pixelValue[(int)ceil(r)][(int)c];
				LR = old.pixelValue[(int)ceil(r)][(int)ceil(c)];

				// find the slope of the line between all four corners
				USlope = UR - UL;
				DSlope = LR - LL;
				LSlope = LL - UL; 
				RSlope = LR - UR;
	
				// get the intermediate value corresponding with desired r/c val
				U = UL + USlope*(c-(int)c);
				D = LL + DSlope*(c-(int)c);
				L = UL + LSlope*(r-(int)r);
				R = UR + RSlope*(r-(int)r);

				// get the slop between intermediate values
				HSlope = D - U;
				VSlope = R - L;

				// find 2 different color estimations of the desired pixel
				Hval = U + HSlope*(r-(int)r);
				Vval = L + VSlope*(c-(int)c);

				// average the estimations
				final = (Hval + Vval) / 2;
			}
			else if ( r > 0 && ceil(r) < N && c > 0 && c < M ) { // right edge
				// get upper and lower value
				UL = old.pixelValue[(int)r][(int)c];
				LL = old.pixelValue[(int)ceil(r)][(int)c];

				// find slope between two values
				LSlope = LL - UL;

				// get value of final point
				final = UL + LSlope*(r-(int)r);
			}
			else if ( r > 0 && r < N && c > 0 && ceil(c) < M ) { // bottom edge
				// get left and right values
				UL = old.pixelValue[(int)r][(int)c];
				UR = old.pixelValue[(int)r][(int)ceil(c)];

				// find slope between two values
				USlope = UR - UL;

				// get value of final point
				final = UL + USlope*(c-(int)c);
			}
			else if ( r > 0 && r < N && c > 0 && c < M ) { // lower right
				// no slopes, just set value
				final = old.pixelValue[(int)r][(int)c];
			}				
			else { // no value here
				final = pixelValue[i][j]; // retain background
			}

			// set final pixel value
			pixelValue[i][j] = final;
		}
	}
}

/******************************************************************************\
 Sum two images together, basically just finding the average pixel value of
 every pixel between two images.  Throws an exception if dimesions of both
 images don't match
\******************************************************************************/
template <class pType>
ImageType<pType>& ImageType<pType>::operator+ ( const ImageType<pType>& rhs )
{
	// throw error if images don't match
	if ( N != rhs.N || M != rhs.M || Q != rhs.Q )
		throw (string)"Images do not have the same dimensions!";

	// this is the value that determines the weight of each image
	// large a gives more weight to first image
	// small a gives more weight to second image
	// a must be 0 >= a <= 1
	float a = 0.5;

	//the general formula is aI1(r,c)+(1-a)I2(r,c)
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++)
			// set new pixel value
			pixelValue[i][j] = pixelValue[i][j]*a+rhs.pixelValue[i][j]*(1.0-a);

	return *this;	// return current object
}

#endif /* IMAGE */


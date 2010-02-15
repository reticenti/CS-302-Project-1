#include <iostream>
#include <cstdlib>
#include <cmath>
#include "cubicSpline.h"
#include "image.h"

using namespace std;

// size of one grid on the background grid
const int BACKGRID = 25;

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
	if ( pixelValue != NULL )
	{
		// delete all the memory for the array
		for ( int i = 0; i < N; i++ )
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
	// set the info to the new image data
	setImageInfo( rhs.N, rhs.M, rhs.Q );

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			pixelValue[i][j] = rhs.pixelValue[i][j];
}

/*****************************************************************************\
 equal operator overload, this is basically the same as the copy constructor
 except it will likely have to de-allocate memory before copying values, all
 this is decided in setImageInfo however
\*****************************************************************************/
ImageType& ImageType::operator= ( const ImageType& rhs )
{
	setImageInfo( rhs.N, rhs.M, rhs.Q );

	// copy pixel values
	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
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
 sets the image info, deleting and allocating memory as required, also create
 background grid
\*****************************************************************************/
void ImageType::setImageInfo(int rows, int cols, int levels)
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
		pixelValue = new int* [N];

		// allocate the columns of pixel value
		for ( int i = 0; i < N; i++ )
			pixelValue[i] = new int[M];
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

	// return 0 if there are no pixels
	return ( M*N != 0 ? gray/(M*N) : 0 );
}

/******************************************************************************\
 This simply calls the double version of enlargeImage
\******************************************************************************/
void ImageType::enlargeImage( int S, const ImageType& old, bool cubic )
{
	// call double version of enlarge
	enlargeImage( (double)S, old, cubic );
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
void ImageType::enlargeImage( double S, const ImageType& old, bool cubic )
{
	// slightly modify S to make it a better value for spline
	double Shoriz = (double)((int)(old.M*S))/old.M;
	double Svert = (double)((int)(old.N*S))/old.N;
	
	int colorVal;	// used to hold the calculated color value
	double splineX; // the x value passed to the spline function

	int *horizVals = new int[old.M];	// holds points for spline
	int *vertVals = new int[old.N];		// holds points for spline

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
		if ( cubic )
			spline.createCubic( vertVals, old.N );
		else
			spline.create( vertVals, old.N );

		// using the spline set the values of vert
		for ( int row = 0; row < N; row++ )
		{
			// value to pull from spline for current row
			splineX = (row-Svert/2.0)/(N-Svert-1.0) * 100.0;
			colorVal;
			
			if ( cubic )
				colorVal = spline.getCubicVal(splineX);
			else
				colorVal = spline.getVal(splineX);
			
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
		if ( cubic )
			spline.createCubic( horizVals, old.M );
		else
			spline.create( horizVals, old.M );

		// using the spline set the values
		for ( int col = 0 ; col < M; col++ )
		{
			splineX = (col-Shoriz/2.0)/(M-Shoriz-1.0) * 100.0;
			colorVal;
			
			// obtain new color from spline value
			if ( cubic )
				colorVal = spline.getCubicVal(splineX);
			else
				colorVal = spline.getVal(splineX);
			
			// clip the color if it goes out of bounds
			if ( colorVal < 0 ) colorVal = 0;
			if ( colorVal > Q ) colorVal = Q;

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
		if ( cubic )
			spline.createCubic( horizVals, old.M );
		else
			spline.create( horizVals, old.M );

		// using the spline set the values of temp
		for ( int col = 0; col < M; col++ )
		{
			// value to pull from spline for current col
			splineX = (col-Shoriz/2.0)/(M-Shoriz-1.0) * 100.0;
			colorVal;
			
			if ( cubic )
				colorVal = spline.getCubicVal(splineX);
			else
				colorVal = spline.getVal(splineX);
			
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
		if ( cubic )
			spline.createCubic( vertVals, old.N );
		else
			spline.create( vertVals, old.N );

		// using the spline set the values
		for ( int row = 0 ; row < N; row++ )
		{
			splineX = (row-Svert/2.0)/(N-Svert-1.0) * 100.0;
			colorVal;
			
			// obtain new color from spline value
			if ( cubic )
				colorVal = spline.getCubicVal(splineX);
			else
				colorVal = spline.getVal(splineX);
			
			// clip the color if it goes out of bounds
			if ( colorVal < 0 ) colorVal = 0;
			if ( colorVal > Q ) colorVal = Q;

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
void ImageType::reflectImage( bool flag, const ImageType& old )
{
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
 the difference is less then Q/9 then the pixel is replaced with black,
 otherwise white is used.
\******************************************************************************/
ImageType& ImageType::operator- ( const ImageType& rhs )
{
	// throw exception if images don't match
	if ( N != rhs.N || M != rhs.M || Q != rhs.Q )
		throw (string)"Images do not have the same dimensions!";

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
		{
			// calculate subtracted value
			pixelValue[i][j] = abs(pixelValue[i][j] - rhs.pixelValue[i][j]);

			// if pixels are less than Q/7 different then make them black
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
	// used to find average pixel value
	int total, num;

    //make new array with correct size
	setImageInfo(old.N / s, old.M / s, old.Q);

	//copy over every s pixel
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++) {
			// reset values for averaging
			total = num = 0;

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

void ImageType::translateImage( int t, const ImageType& old )
{

	//make this image's image array
	setImageInfo(old.N, old.M, old.Q);

	//find the bottom corner
	//go up s pixels
	//go left s pixels
	//cp current to i+t, j+t
	for(int i = N - 1; i >= 0 + t; i--)
		for(int j = M - 1; j >= 0 + t; j--)
			pixelValue[i][j] = old.pixelValue[i-t][j-t];
		
}

void ImageType::rotateImage( int theta, const ImageType& old )
{
	setImageInfo(old.N, old.M, old.Q);
	int final;	// holds final color value for given location
	
	// holds various color values
	int UL, UR, LL, LR, U, D, L, R, Hval, Vval;

	// holds the slopes between different points
	int USlope, DSlope, LSlope, RSlope, HSlope, VSlope;

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
				U = UL + USlope*(c - (int)c);
				D = LL + DSlope*(c - (int)c);
				L = UL + LSlope*(r - (int)r);
				R = UR + RSlope*(r - (int)r);

				// get the slop between intermediate values
				HSlope = D-U;
				VSlope = R-L;

				// find 2 different color estimations of the desired pixel
				Hval = U + HSlope*(r - (int)r);
				Vval = L + VSlope*(c - (int)c);

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
				final = UL + LSlope*(r - (int)r);
			}
			else if ( r > 0 && r < N && c > 0 && ceil(c) < M ) { // bottom edge
				// get left and right values
				UL = old.pixelValue[(int)r][(int)c];
				UR = old.pixelValue[(int)r][(int)ceil(c)];

				// find slope between two values
				USlope = UR - UL;

				// get value of final point
				final = UL + USlope*(c - (int)c);
			}
			else if ( r > 0 && r < N && c > 0 && c < M ) { // lower right
				// no slopes, just set value
				final = old.pixelValue[(int)r][(int)c];
			}				
			else { // no value here
				final = pixelValue[i][j]; // retain background
			}

			// make sure final value is not out of color bounds
			if ( final < 0 ) final = 0;
			if ( final > Q ) final = Q;

			// set final pixel value
			pixelValue[i][j] = final;
		}
	}

}

ImageType& ImageType::operator+ ( const ImageType& rhs )
{
	// throw error if images don't match
	if ( N != rhs.N || M != rhs.M || Q != rhs.Q )
		throw (string)"Images do not have the same dimensions!";

	//this is the value that determines the weight of each image
	//large a gives more weight to first image
	//small a gives more weight to second image
	//a must be 0 >= a <= 1
	float a = 0.5;

	//the general formula is aI1(r,c)+(1-a)I2(r,c)
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			// set new pixel value
			pixelValue[i][j] = pixelValue[i][j]*a + (1.0 - a)*rhs.pixelValue[i][j];
		}
	}

	return *this;	// return current object
}


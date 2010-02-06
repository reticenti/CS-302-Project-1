#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "cubicSpline.h"

using namespace std;

#include "image.h"

const bool INTERPL = true;	// use interpolation to enlarge
const bool CUBIC   = true;	// only valid if INTERPL is true

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
			delete [] pixelValue[i];
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
	if ( !INTERPL )
	{
		setImageInfo( old.N * s, old.M * s, old.Q );
		for ( int i = 0; i < N; i++ )
			for ( int j = 0; j < M; j++ )
				pixelValue[i][j] = old.pixelValue[i/s][j/s];
	}
	else
	{

		int minOld, maxOld, diag, index;
		int *rows = new int[old.M];
		int *cols = new int[old.N];
		cubicSpline row, col;
		int offR, offC;

		setImageInfo( old.N * s, old.M * s, old.Q );
		
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

		// counts diagonally
		for ( diag = 0; diag < minOld; diag++ )
		{	
			// fills the column array
			for ( index = 0; index < old.N; index++ )
				cols[index] = old.pixelValue[index][diag];

			// sets the rows array
			for ( index = 0; index < old.M; index++ )
				rows[index] = old.pixelValue[diag][index];

			( CUBIC ? row.createCubic( rows, old.M ) : row.create( rows, old.M ) );
			( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

			for ( int r = 0; r < N; r++ )
			{
				offR = r-s/2;
				if ( CUBIC )
					pixelValue[r][diag*s+s/2] = (int)col.getCubicVal( (double)offR/(N-s+1) * 100.0 );
				else
					pixelValue[r][diag*s+s/2] = (int)col.getVal( (double)offR/(N-s+1) * 100.0 );

				if ( pixelValue[r][diag*s+s/2] > 255 )
					pixelValue[r][diag*s+s/2] = 255;
				if ( pixelValue[r][diag*s+s/2] < 0 )
					pixelValue[r][diag*s+s/2] = 0;
			}

			for ( int c = 0; c < M; c++ )
			{
				offC = c-s/2;
				if ( CUBIC )
					pixelValue[diag*s+s/2][c] = (int)row.getCubicVal( (double)offC/(M-s+1) * 100.0 );
				else
					pixelValue[diag*s+s/2][c] = (int)row.getVal( (double)offC/(M-s+1) * 100.0 );

				if ( pixelValue[diag*s+s/2][c] > 255 )
					pixelValue[diag*s+s/2][c] = 255;
				if ( pixelValue[diag*s+s/2][c] < 0 )
					pixelValue[diag*s+s/2][c] = 0;
			}
		}

		// fills in the longer right/left sides
		if ( minOld != maxOld && maxOld == old.M )
			for ( diag = minOld; diag < maxOld; diag++ )
			{
				for ( index = 0; index < old.N; index++ )
					cols[index] = old.pixelValue[index][diag];

				( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

				for ( int r = 0; r < N; r++ )
				{
					offR = r-s/2;
					if ( CUBIC )
						pixelValue[r][diag*s+s/2] = (int)col.getCubicVal( (double)offR/(N-s+1) * 100.0 );
					else
						pixelValue[r][diag*s+s/2] = (int)col.getVal( (double)offR/(N-s+1) * 100.0 );

					if ( pixelValue[r][diag*s+s/2] > 255 )
						pixelValue[r][diag*s+s/2] = 255;
					if ( pixelValue[r][diag*s+s/2] < 0 )
						pixelValue[r][diag*s+s/2] = 0;
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
					offC = c-s/2;
					if ( CUBIC )
						pixelValue[diag*s+s/2][c] = (int)row.getCubicVal( (double)offC/(M-s+1) * 100.0 );
					else
						pixelValue[diag*s+s/2][c] = (int)row.getVal( (double)offC/(M-s+1) * 100.0 );

					if ( pixelValue[diag*s+s/2][c] > 255 )
						pixelValue[diag*s+s/2][c] = 255;
					if ( pixelValue[diag*s+s/2][c] < 0 )
						pixelValue[diag*s+s/2][c] = 0;
				}
			}

		for ( diag = 0; diag < N; diag++ )
		{
			for ( index = 0; index < old.M; index++ )
				rows[index] = pixelValue[diag][index*s+s/2];

			( CUBIC ? row.createCubic( rows, old.M ) : row.create( rows, old.M ) );

			for ( int c = 0; c < M; c++ )
			{
				offC = c-s/2;
				if ( CUBIC )
					pixelValue[diag][c] = (int)row.getCubicVal( (double)offC/(M-s+1) * 100.0 );
				else
					pixelValue[diag][c] = (int)row.getVal( (double)offC/(M-s+1) * 100.0 );

				if ( pixelValue[diag][c] > 255 )
					pixelValue[diag][c] = 255;
				if ( pixelValue[diag][c] < 0 )
					pixelValue[diag][c] = 0;
			}
		}

		for ( diag = 0; diag < M; diag++ )
		{
			for ( index = 0; index < old.N; index++ )
				cols[index] = pixelValue[index*s+s/2][diag];

			( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

			for ( int r = 0; r < N; r++ )
			{
				offR = r-s/2;
				if ( CUBIC )
					pixelValue[r][diag] += (int)col.getCubicVal( (double)offR/(N-s+1) * 100.0 );
				else
					pixelValue[r][diag] += (int)col.getVal( (double)offR/(N-s+1) * 100.0 );
				pixelValue[r][diag] /= 2;

				if ( pixelValue[r][diag] > 255 )
					pixelValue[r][diag] = 255;
				if ( pixelValue[r][diag] < 0 )
					pixelValue[r][diag] = 0;
			}
		} 
		
		delete [] rows;
		delete [] cols;
	}
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


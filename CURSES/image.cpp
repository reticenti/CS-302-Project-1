#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "cubicSpline.h"

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
	const bool CUBIC = true;

	int minOld, maxOld, diag, index;
	int *rows = new int[old.M];
	int *cols = new int[old.N];
	cubicSpline row, col;

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
			if ( CUBIC )
				pixelValue[r][diag*s] = (int)col.getCubicVal( (double)r/(N-s+1) * 100.0 );
			else
				pixelValue[r][diag*s] = (int)col.getVal( (double)r/(N-s+1) * 100.0 );

			if ( pixelValue[r][diag*s] > 255 )
				pixelValue[r][diag*s] = 255;
			if ( pixelValue[r][diag*s] < 0 )
				pixelValue[r][diag*s] = 0;
		}

		for ( int c = 0; c < M; c++ )
		{
			if ( CUBIC )
				pixelValue[diag*s][c] = (int)row.getCubicVal( (double)c/(M-s+1) * 100.0 );
			else
				pixelValue[diag*s][c] = (int)row.getVal( (double)c/(M-s+1) * 100.0 );

			if ( pixelValue[diag*s][c] > 255 )
				pixelValue[diag*s][c] = 255;
			if ( pixelValue[diag*s][c] < 0 )
				pixelValue[diag*s][c] = 0;
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
				if ( CUBIC )
					pixelValue[r][diag*s] = (int)col.getCubicVal( (double)r/(N-s+1) * 100.0 );
				else
					pixelValue[r][diag*s] = (int)col.getVal( (double)r/(N-s+1) * 100.0 );

				if ( pixelValue[r][diag*s] > 255 )
					pixelValue[r][diag*s] = 255;
				if ( pixelValue[r][diag*s] < 0 )
					pixelValue[r][diag*s] = 0;
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
				if ( CUBIC )
					pixelValue[diag*s][c] = (int)row.getCubicVal( (double)c/(M-s+1) * 100.0 );
				else
					pixelValue[diag*s][c] = (int)row.getVal( (double)c/(M-s+1) * 100.0 );

				if ( pixelValue[diag*s][c] > 255 )
					pixelValue[diag*s][c] = 255;
				if ( pixelValue[diag*s][c] < 0 )
					pixelValue[diag*s][c] = 0;
			}
		}

	for ( diag = 0; diag < N; diag++ )
	{
		for ( index = 0; index < old.M; index++ )
			rows[index] = pixelValue[diag][index*s];

		( CUBIC ? row.createCubic( rows, old.M ) : row.create( rows, old.M ) );

		for ( int c = 0; c < M; c++ )
		{
			if ( CUBIC )
				pixelValue[diag][c] = (int)row.getCubicVal( (double)c/(M-s+1) * 100.0 );
			else
				pixelValue[diag][c] = (int)row.getVal( (double)c/(M-s+1) * 100.0 );

			if ( pixelValue[diag][c] > 255 )
				pixelValue[diag][c] = 255;
			if ( pixelValue[diag][c] < 0 )
				pixelValue[diag][c] = 0;
		}
	}

	for ( diag = 0; diag < M; diag++ )
	{
		for ( index = 0; index < old.N; index++ )
			cols[index] = pixelValue[index*s][diag];

		( CUBIC ? col.createCubic( cols, old.N ) : col.create( cols, old.N ) );

		for ( int r = 0; r < N; r++ )
		{
			if ( CUBIC )
				pixelValue[r][diag] += (int)col.getCubicVal( (double)r/(N-s+1) * 100.0 );
			else
				pixelValue[r][diag] += (int)col.getVal( (double)r/(N-s+1) * 100.0 );
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
	return *this;	// temp return value
}


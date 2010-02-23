#include <iostream>
#include <ctime>
#include <cstdlib>
#include "imageIO.h"
#include "image.h"

// simply makes an image that has random pixel values in it NxM

using namespace std;

int main()
{
	int rows, cols;
	char filename[20];
	ImageType img;

	cout << "Enter number of rows    : ";
	cin >> rows;
	cout << "Enter number of columns : ";
	cin >> cols;
	cout << "Enter output file name  : ";
	cin >> filename;

	img.setImageInfo(rows, cols, 255);
	
	srand( time(0) );

	for ( int i = 0; i < rows; i++ )
		for ( int j = 0; j < cols; j++ )
			img.setPixelVal(i, j, rand() % 255);

	writeImage( filename, img );

	return 0;

}


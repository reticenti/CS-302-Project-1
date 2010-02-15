/******************************************************************************\
 Authors: Josiah Humphrey and Joshua Gleason

 Date Due for Review: 02/16/2010

 This object is used for storing and manipulating images, it allows the images
 to be processed and manipulated in multiple ways including, re-scaling,
 rotating, translating, adding, subtracting, negating, etc...
\******************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

class ImageType {
public:

// CONSTRUCTORS AND DESTRUCTOR /////////////////////////////////////////////////
	// default construtor, sets everything to NULL and 0
	ImageType();

	// parameterized constructor sets up image to N, M and Q values
	ImageType(int, int, int);

	// copy allocates memory and copies info from the right hand side
	ImageType( const ImageType& );

	// same as copy except it de-allocates memory first if necessary
	ImageType& operator= ( const ImageType& );

	// destructor removes all dynamically allocated memory
	~ImageType();
	
// IMAGE FUNCTIONS /////////////////////////////////////////////////////////////
	// returns the N, M and Q values to the calling function
	void getImageInfo(int&, int&, int&) const;

	// sets N, M and Q and also de-allocates and allocates memory if necessary
	void setImageInfo(int, int, int);

	// sets the value of a pixel at row, column to the 3rd parameters value
	void setPixelVal(int, int, int);

	// return the pixel value at the desired location
	int getPixelVal(int, int) const;

////// Josh's functions ////////////////////////////////////////////////////////

	// returns the mean value of all the pixels
	double meanGray() const;

	// enlarge the image by a factor is s, uses bi-cubic interpolation if the
	// bool is true, otherwise uses bi-linear interpolation which is more light
	// weight.  The version with an int value for s simply casts s to a double
	// and calls the version that takes a double
	void enlargeImage( double, const ImageType&, bool=true );
	void enlargeImage( int, const ImageType&, bool=true );

	// reflect the image either horiz. or vert. depending on the bool value
	void reflectImage( bool, const ImageType& );

	// subtract 2 images, make any pixels that differ by more than Q/6 white
	// and make the rest black.  This helps reduce some noise
	ImageType& operator- ( const ImageType& );
	
	// calculate the negative of every pixel (Q - current value)
	void negateImage();

////// Josiah's functions //////////////////////////////////////////////////////

	// calculate a sub image given the row,col for the upper left and lower
	// right corners
	void getSubImage( int, int, int, int, const ImageType& );

	// shrink image by a factor of s, find the average value for each 'block' of
	// pixels that is reduced and use that value.  This makes a smoother reduce
	// function
	void shrinkImage( int, const ImageType& );

	// translate the image down to the right by t pixels, effectively cutting
	// off the bottom and right side by the same number of pixels
	void translateImage( int, const ImageType& );

	// rotate image by theta degrees counter-clockwise
	void rotateImage( int, const ImageType& );

	// sum two images giving no particular bias to one or the other
	ImageType& operator+ ( const ImageType& );


private:
	int N; // # of rows
	int M; // # of cols
	int Q; // # of gray-level values

	// array of pixel values
	int **pixelValue;
};

#endif


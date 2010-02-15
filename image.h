#ifndef IMAGE_H
#define IMAGE_H

class ImageType {
 public:

 // CONSTRUCTORS AND DESTRUCTOR
	// default construtor, sets everything to NULL and 0
	ImageType();

	// parameterized constructor sets up image to N, M and Q values
	ImageType(int, int, int);

	// copy constructor, de-allocates memory if needed then allocates just
	// enough and copys values from the right hand side
	ImageType( const ImageType& );
	ImageType& operator= ( const ImageType& );

	~ImageType();
	
	
	void getImageInfo(int&, int&, int&) const;
	void setImageInfo(int, int, int);
	void setPixelVal(int, int, int);
	int getPixelVal(int, int) const;

	// Josiah's functions

	void getSubImage( int, int, int, int, const ImageType& );
	void shrinkImage( int, const ImageType& );
	void translateImage( int, const ImageType& );
	void rotateImage( int, const ImageType& );
	ImageType& operator+ ( const ImageType& );

	// Josh's functions

	double meanGray() const;
	void enlargeImage( double, const ImageType&, bool=true );
	void enlargeImage( int, const ImageType&, bool=true );
	void reflectImage( bool, const ImageType& );
	ImageType& operator- ( const ImageType& );
	void negateImage();

 private:
	int N; // # of rows
	int M; // # of cols
	int Q; // # of gray-level values

	// array of pixel values
	int **pixelValue;
};

#endif

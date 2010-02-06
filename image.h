#ifndef IMAGE_H
#define IMAGE_H

class ImageType {
 public:
	// already done (slightly modified)
	ImageType();
	ImageType(int, int, int);
	void getImageInfo(int&, int&, int&) const;
	void setImageInfo(int, int, int);
	void setPixelVal(int, int, int);
	int getPixelVal(int, int) const;

	// other functions

	~ImageType();
	ImageType( const ImageType& );
	ImageType& operator= ( const ImageType& );

	// Josiah's functions

	void getSubImage( int, int, int, int, const ImageType& );
	void shrinkImage( int, const ImageType& );
	void translateImage( int, const ImageType& );
	void rotateImage( int, const ImageType& );
	ImageType& operator+ ( const ImageType& );

	// Josh's functions

	double meanGray() const;
	void enlargeImage( double, const ImageType& );
	void enlargeImage( int, const ImageType& );
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

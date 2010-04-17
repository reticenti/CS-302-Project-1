
#include "list.h"
#include "image.h"
#include <cmath>

// This struct is used to store locations of pixels in a stack or queue
struct PixelType
{
	int r;							// holds row value
	int c;							// holds column value

	PixelType& operator=(const PixelType& rhs)
	{
		r = rhs.r;
		c = rhs.c;
		return *this;
	}
};

// holds all data on a region
class RegionType
{
public:
	RegionType();
	RegionType(const RegionType&);

	// operator overloads used for sorting
	bool operator>(const RegionType &rhs) const;
	bool operator<(const RegionType &rhs) const;
	bool operator>=(const RegionType &rhs) const;
	bool operator<=(const RegionType &rhs) const;
	bool operator==(const RegionType &rhs) const;
	RegionType& operator=(const RegionType &rhs);

	void setData(const ImageType<int>&);

	int getCentroidR()const;
	int getCentroidC()const;
	int getSize()const;
	double getOrientation()const;
	double getEccentricity()const;
	int getMeanVal()const;
	int getMinVal()const;
	int getMaxVal()const;

	list<PixelType> positions;		// positions of pixels in region

private:
	double moment(int, int);
	void xyBar();
	double mu(int, int);
	void lambda();
	void theta();
	void epsilon();

	int centroidR;					// centroid row
	int centroidC;					// centroid col
	int size;						// size of region (pixels)
	double orientation;				// orientation of region
	double eccentricity;			// eccentricity of region
	int meanVal;					// mean pixel value
	int minVal;						// minimum pixel value
	int maxVal;						// maximum pixel value
	double lambdaMax, lambdaMin; 		// lambda values

};


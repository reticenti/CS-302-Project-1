#include "list.h"
#include "image.h"
#include <cmath>

#ifndef PIXELTYPE

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

#endif

#ifndef REGIONTYPE
#define REGIONTYPE

// holds all data on a region
template <class pType>
class RegionType
{
public:
	RegionType();
	RegionType(const RegionType<pType>&);

	// operator overloads used for sorting
	bool operator>(const RegionType<pType> &rhs) const;
	bool operator<(const RegionType<pType> &rhs) const;
	bool operator>=(const RegionType<pType> &rhs) const;
	bool operator<=(const RegionType<pType> &rhs) const;
	bool operator==(const RegionType<pType> &rhs) const;
	RegionType<pType>& operator=(const RegionType<pType> &rhs);

	void setData( const ImageType<pType>& );

	double getCentroidR() const;
	double getCentroidC() const;
	int getSize() const;
	double getOrientation() const;
	double getEccentricity() const;
	pType getMeanVal() const;
	pType getMinVal() const;
	pType getMaxVal() const;

	list<PixelType> positions;		// positions of pixels in region

private:
	double moment(int, int);
	void xyBar();
	double mu(int, int);
	void lambda();
	void theta();
	void epsilon();

	double centroidR;				// centroid row
	double centroidC;				// centroid col
	int size;						// size of region (pixels)
	double orientation;				// orientation of region
	double eccentricity;			// eccentricity of region
	pType meanVal;					// mean pixel value
	pType minVal;						// minimum pixel value
	pType maxVal;						// maximum pixel value
	double lambdaMax, lambdaMin; 	// lambda values

};

template <class pType>
RegionType<pType>::RegionType()
{
	meanVal = minVal = maxVal = 0;
	size = 0;
	orientation = eccentricity = centroidR = centroidC = 0.0;
}

template <class pType>
RegionType<pType>::RegionType(const RegionType<pType>& rhs)
{
	// calls operator= overload
	*this = rhs;
}

template <class pType>
bool RegionType<pType>::operator>(const RegionType<pType> &rhs) const
{
	return (size > rhs.size);
}

template <class pType>
bool RegionType<pType>::operator<(const RegionType<pType> &rhs) const
{
	return (size < rhs.size);
}

template <class pType>
bool RegionType<pType>::operator>=(const RegionType<pType> &rhs) const
{
	return (size >= rhs.size);
}

template <class pType>
bool RegionType<pType>::operator<=(const RegionType<pType> &rhs) const
{
	return (size <= rhs.size);
}

template <class pType>
bool RegionType<pType>::operator==(const RegionType<pType> &rhs) const
{
	// if all these are the same the points must be the same too
	return (centroidR    == rhs.centroidR &&
			centroidC    == rhs.centroidC &&
			size         == rhs.size &&
			orientation  == rhs.orientation &&
			eccentricity == rhs.eccentricity );
}

template <class pType>
RegionType<pType>& RegionType<pType>::operator=(const RegionType<pType> &rhs)
{
	centroidR = rhs.centroidR;
	centroidC = rhs.centroidC;
	size = rhs.size;
	orientation = rhs.orientation;
	eccentricity = rhs.eccentricity;
	meanVal = rhs.meanVal;
	minVal = rhs.minVal;
	maxVal = rhs.maxVal;
	positions = rhs.positions;
}

template <class pType>
double RegionType<pType>::moment(int p, int q){

	double total = 0;

	positions.reset();
	PixelType tmp;

	while(!positions.atEnd()){
		
		tmp = positions.getNextItem();
		total += (pow((double)tmp.c,p))*(pow((double)tmp.r,q));
	}

	return total;
}

template <class pType>
void RegionType<pType>::xyBar(){

	double mom0;
	mom0 = moment(0,0);
	centroidC=(moment(1,0)/mom0);
	centroidR=(moment(0,1)/mom0);
}

template <class pType>
double RegionType<pType>::mu(int p, int q){

	double total = 0;

	positions.reset();
	PixelType tmp;

	while(!positions.atEnd()){

		tmp = positions.getNextItem();
		total += ( pow(((double)tmp.c - (double)centroidC),p) 
				* pow(((double)tmp.r - (double)centroidR),q));
	}

	return total;
}

template <class pType>
void RegionType<pType>::lambda(){

	lambdaMax = ( 0.5*(mu(2,0) + mu(0,2)) + 
			0.5*(sqrt( pow(mu(2,0),2) + pow(mu(0,2),2) - 2*(mu(0,2) * mu(2,0)) + 4*(pow(mu(1,1),2)) )));

	lambdaMin = ( 0.5*(mu(2,0) + mu(0,2)) - 
			0.5*(sqrt( pow(mu(2,0),2) + pow(mu(0,2),2) - 2*(mu(0,2) * mu(2,0)) + 4*(pow(mu(1,1),2)) )));
}

template <class pType>
void RegionType<pType>::theta(){
	double mu11 = mu(1,1);

	if ( mu11 != 0 )
		orientation = atan((lambdaMax - mu(2,0)) / (mu(1,1) ));
	else
		orientation = 2.0 * atan(1.0);	// PI / 2
}

template <class pType>
void RegionType<pType>::epsilon(){
		
	if ( lambdaMin = 0 )
		eccentricity = sqrt(lambdaMax/lambdaMin);
	else
		eccentricity = 0;
}

template <class pType>
void RegionType<pType>::setData( const ImageType<pType>& img ){

	if(positions.isEmpty())
		throw "There is not a region!";

	//set centroids{x,y}
	xyBar();

	//set size
	size = positions.getLength();
	
	//set lambda{max,min}
	lambda();

	//set orientation
	theta();

	//set eccentricity
	epsilon();

	//set mean, min, maxVal
	positions.reset();
	PixelType tmp;
	pType pixVal;

	bool first = true;

	while(!positions.atEnd()){
		tmp = positions.getNextItem();
		pixVal = img.getPixelVal(tmp.r, tmp.c);
		if ( first )
		{
			minVal = pixVal;
			maxVal = pixVal;
			meanVal = meanVal + pixVal;
			first = false;
		}
		else
		{
			if ( minVal > pixVal )
				pixVal = minVal;
			if ( maxVal < pixVal )
				maxVal = pixVal;
			meanVal = meanVal + pixVal;
		}
	}

	meanVal = meanVal / size;
}

template <class pType>
double RegionType<pType>::getCentroidR()const{
	return centroidR;
}

template <class pType>
double RegionType<pType>::getCentroidC()const{
	return centroidC;
}

template <class pType>
int RegionType<pType>::getSize()const{
	return size;
}

template <class pType>
double RegionType<pType>::getOrientation()const{
	return orientation;
}

template <class pType>
double RegionType<pType>::getEccentricity()const{
	return eccentricity;
}

template <class pType>
pType RegionType<pType>::getMeanVal() const{
	return meanVal;
}

template <class pType>
pType RegionType<pType>::getMinVal()const{
	return minVal;
}

template <class pType>
pType RegionType<pType>::getMaxVal()const{
	return maxVal;
}

#endif	// REGIONTYPE


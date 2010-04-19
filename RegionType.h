/******************************************************************************\
 Authors: Josiah Humphrey and Joshua Gleason

 Last Modified : 04-18-20

 RegionType is the class the contains the information on the regions of an
 image, all the calculations are done here.  RegionType also contains all the
 calculated values for the region.  This includes the list of locations if each
 pixel in the image

 PixelType is simply a struct containing the location of a pixel (row/column)
\******************************************************************************/
#include "list.h"
#include "image.h"
#include <cmath>

#ifndef PIXELTYPE

// This struct is used to store locations of pixels in a stack or queue
struct PixelType
{
	int r;							// holds row value
	int c;							// holds column value

	// simple = overload to make easy soft copies of the struct
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

template <class pType>
class RegionType
{
public:

// CONSTRUCTORS ////////////////////////////////////////////////////////////////
	// default constructor that zeros all the data members
	RegionType();

	// copy constructor, soft copies all members using overloaded = operator
	RegionType(const RegionType<pType>&);

// OTHER FUNCTIONS /////////////////////////////////////////////////////////////

	// operator overloads used for sorting (sorts based on size)
	bool operator>(const RegionType<pType> &rhs) const;
	bool operator<(const RegionType<pType> &rhs) const;
	bool operator>=(const RegionType<pType> &rhs) const;
	bool operator<=(const RegionType<pType> &rhs) const;
	bool operator==(const RegionType<pType> &rhs) const;

	// soft copies everything
	RegionType<pType>& operator=(const RegionType<pType> &rhs);

	// set all the private data values up using the private functions
	void setData( const ImageType<pType>& );

	// return copys of the data members for making compairisons
	double getCentroidR() const;
	double getCentroidC() const;
	int getSize() const;
	double getOrientation() const;
	double getEccentricity() const;
	pType getMeanVal() const;
	pType getMinVal() const;
	pType getMaxVal() const;

// PUBLIC DATA MEMBER //////////////////////////////////////////////////////////
	// this list was left public for easy access, should be just as safe
	list<PixelType> positions;		// positions of pixels in region

private:
// PRIVATE FUNCTIONS ///////////////////////////////////////////////////////////
	// calculate the moment
	double moment(int, int);
	
	// calculate the central moment
	double mu(int, int);

	// calculate the centroids
	void xyBar();

	// calculate lambda min and max
	void lambda();

	// calculate orrientation
	void theta();

	// calculate eccentricity
	void epsilon();

// DATA MEMBERS ////////////////////////////////////////////////////////////////
	double centroidR;				// centroid row
	double centroidC;				// centroid col
	int size;						// size of region (pixels)
	double orientation;				// orientation of region
	double eccentricity;			// eccentricity of region
	pType meanVal;					// mean pixel value
	pType minVal;					// minimum pixel value
	pType maxVal;					// maximum pixel value
	double lambdaMax, lambdaMin; 	// lambda values

};

/******************************************************************************\
 Default constructor that zeros all the data members
\******************************************************************************/
template <class pType>
RegionType<pType>::RegionType()
{
	meanVal = minVal = maxVal = 0;
	size = 0;
	orientation = eccentricity = centroidR = centroidC = 0.0;
	lambdaMax = lambdaMin = 0.0;
}

/******************************************************************************\
 Copy constructor, soft copies all members using overloaded = operator
\******************************************************************************/
template <class pType>
RegionType<pType>::RegionType(const RegionType<pType>& rhs)
{
	// calls operator= overload
	*this = rhs;
}

/******************************************************************************\
 Simple overload used to sort based on size
\******************************************************************************/
template <class pType>
bool RegionType<pType>::operator>(const RegionType<pType> &rhs) const
{
	return (size > rhs.size);
}

/******************************************************************************\
 Simple overload used to sort based on size
\******************************************************************************/
template <class pType>
bool RegionType<pType>::operator<(const RegionType<pType> &rhs) const
{
	return (size < rhs.size);
}

/******************************************************************************\
 Simple overload used to sort based on size
\******************************************************************************/
template <class pType>
bool RegionType<pType>::operator>=(const RegionType<pType> &rhs) const
{
	return (size >= rhs.size);
}

/******************************************************************************\
 Simple overload used to sort based on size
\******************************************************************************/
template <class pType>
bool RegionType<pType>::operator<=(const RegionType<pType> &rhs) const
{
	return (size <= rhs.size);
}

/******************************************************************************\
 This one checks all the data members for equality
\******************************************************************************/
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

/******************************************************************************\
 Makes and stores copies of all the data members in rhs (including positions)
\******************************************************************************/
template <class pType>
RegionType<pType>& RegionType<pType>::operator=(const RegionType<pType> &rhs)
{
	if ( this != &rhs )
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
}

/******************************************************************************\
 Calulate the moment of (p,q) using the positions list
\******************************************************************************/
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

/******************************************************************************\
 Calculate the central moment of (p,q) using the centroids and positions list

 precondition: centroids are already set
\******************************************************************************/
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

/******************************************************************************\
 Calculates the centroids using moments
\******************************************************************************/
template <class pType>
void RegionType<pType>::xyBar(){

	double mom0;
	mom0 = moment(0,0);
	centroidC=(moment(1,0)/mom0);
	centroidR=(moment(0,1)/mom0);
}

/******************************************************************************\
 Calulates lambda using central moments
 
 precondition: centroids are already set
\******************************************************************************/
template <class pType>
void RegionType<pType>::lambda(){

	lambdaMax = ( 0.5*(mu(2,0) + mu(0,2)) + 0.5*(sqrt( pow(mu(2,0),2) +
		pow(mu(0,2),2) - 2*(mu(0,2) * mu(2,0)) + 4*(pow(mu(1,1),2)) )));

	lambdaMin = ( 0.5*(mu(2,0) + mu(0,2)) - 0.5*(sqrt( pow(mu(2,0),2) +
		pow(mu(0,2),2) - 2*(mu(0,2) * mu(2,0)) + 4*(pow(mu(1,1),2)) )));
}

/******************************************************************************\
 Calculates orrientation using Lambda and central moments

 precondition: lambdaMin and lambdaMax are already calculated
\******************************************************************************/
template <class pType>
void RegionType<pType>::theta(){

	// pre calculate mu of (1,1)
	double mu_1_1 = mu(1,1);

	// calculate half pi
	double halfpi = 2.0 * atan(1.0);

	// if this is zero we will get a divide by zero
	if ( mu_1_1 != 0 )
		orientation = atan((lambdaMax - mu(2,0)) / (mu_1_1 ));
	else // instead set this to the limit of atan(x) as x goes to infinity
		orientation = halfpi;

	// this makes x axis axis of origin
	orientation += halfpi;
	
	// convert to degrees
	orientation *= 180.0 / (2.0 * halfpi);
}

/******************************************************************************\
 Calculate the eccentricity based on Lambda values

 precondition: Lambda values are already set
\******************************************************************************/
template <class pType>
void RegionType<pType>::epsilon(){
	if ( lambdaMin != 0.0 )
		eccentricity = sqrt(lambdaMax/lambdaMin);
	else // this only happens for regions that are extremely thin
		eccentricity = sqrt((lambdaMax+1.0)/(lambdaMin+1.0));
}

/******************************************************************************\
 Calculate all the data members values

 precondition: The list of positions has already been set and the image being
			   passed is the original image the region was calculated from
\******************************************************************************/
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

	// set the values for the first element
	bool first = true;

	while(!positions.atEnd())
	{
		// get the next item in the list
		tmp = positions.getNextItem();
		
		// get the pixel value at that location
		pixVal = img.getPixelVal(tmp.r, tmp.c);

		// if the first set min = max = mean = pixVal
		if ( first )
		{
			minVal = pixVal;
			maxVal = pixVal;
			meanVal = meanVal + pixVal;
			first = false;
		}
		else // otherwise check to see if the values are greater
		{
			if ( minVal > pixVal )
				pixVal = minVal;
			if ( maxVal < pixVal )
				maxVal = pixVal;
			meanVal = meanVal + pixVal;
		}
	}

	// divide by total to obtain mean
	meanVal = meanVal / size;
}

/******************************************************************************\
 Return the value of the row centroid (x bar)
\******************************************************************************/
template <class pType>
double RegionType<pType>::getCentroidR()const{
	return centroidR;
}

/******************************************************************************\
 Return the value of the column centroid (y bar)
\******************************************************************************/
template <class pType>
double RegionType<pType>::getCentroidC()const{
	return centroidC;
}

/******************************************************************************\
 Return the value of size
\******************************************************************************/
template <class pType>
int RegionType<pType>::getSize()const{
	return size;
}

/******************************************************************************\
 Return the value of orientation
\******************************************************************************/
template <class pType>
double RegionType<pType>::getOrientation()const{
	return orientation;
}

/******************************************************************************\
 Return the value of eccentricity
\******************************************************************************/
template <class pType>
double RegionType<pType>::getEccentricity()const{
	return eccentricity;
}

/******************************************************************************\
 Returns the value meanVal
\******************************************************************************/
template <class pType>
pType RegionType<pType>::getMeanVal() const{
	return meanVal;
}

/******************************************************************************\
 Returns the value of minVal
\******************************************************************************/
template <class pType>
pType RegionType<pType>::getMinVal()const{
	return minVal;
}

/******************************************************************************\
 Returns the value of maxVal
\******************************************************************************/
template <class pType>
pType RegionType<pType>::getMaxVal()const{
	return maxVal;
}

#endif	// REGIONTYPE


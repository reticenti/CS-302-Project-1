#include "RegionType.h"

RegionType::RegionType()
{
	centroidR = centroidC = size = meanVal = minVal = maxVal = 0;
	orientation = eccentricity = 0.;
}

RegionType::RegionType(const RegionType& rhs)
{
	// calls operator= overload
	*this = rhs;
}

bool RegionType::operator>(const RegionType &rhs) const
{
	return (size > rhs.size);
}

bool RegionType::operator<(const RegionType &rhs) const
{
	return (size < rhs.size);
}

bool RegionType::operator>=(const RegionType &rhs) const
{
	return (size >= rhs.size);
}

bool RegionType::operator<=(const RegionType &rhs) const
{
	return (size <= rhs.size);
}

bool RegionType::operator==(const RegionType &rhs) const
{
	// if all these are the same the points must be the same too
	return (centroidR    == rhs.centroidR &&
			centroidC    == rhs.centroidC &&
			size         == rhs.size &&
			orientation  == rhs.orientation &&
			eccentricity == rhs.eccentricity &&
			meanVal      == rhs.meanVal &&
			minVal       == rhs.minVal &&
			maxVal       == rhs.maxVal );
}

RegionType& RegionType::operator=(const RegionType &rhs)
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

double RegionType::moment(int p, int q){

	double total = 0;

	positions.reset();
	pixelType tmp = positions.getNextItem();

	while(!positions.isEmpty()){
		
		tmp = positions.getNextItem()
		total += (pow((double)tmp.c,p))*(pow((double)tmp.r,q))
	}

	return total;
}

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
	PixelType tmp = positions.getNextItem();

	while(!positions.atEnd()){
		
		total += (pow((double)tmp.c,p))*(pow((double)tmp.r,q));
		tmp = positions.getNextItem();
	}

	return total;
}

void RegionType::xyBar(){

	double mom0;
	mom0 = moment(0,0);
	centroidC=(moment(1,0)/mom0);
	centroidR=(moment(0,1)/mom0);
}

double RegionType::mu(int p, int q){

	double total;

	positions.reset();
	PixelType tmp = positions.getNextItem();

	while(!positions.atEnd()){

		total += ( pow(((double)tmp.c - (double)centroidC),p) 
				* pow(((double)tmp.r - (double)centroidR),q));

		tmp = positions.getNextItem();
	}
}

void RegionType::lambda(){

	lambdaMax = ( 0.5*(mu(2,0) + mu(0,2)) + 
			0.5*(sqrt( pow(mu(2,0),2) + pow(mu(0,2),2) - 2*(mu(0,2) * mu(2,0)) + 4*(pow(mu(1,1),2)) )));

	lambdaMin = ( 0.5*(mu(2,0) + mu(0,2)) - 
			0.5*(sqrt( pow(mu(2,0),2) + pow(mu(0,2),2) - 2*(mu(0,2) * mu(2,0)) + 4*(pow(mu(1,1),2)) )));
}

void RegionType::theta(){

	orientation = atan((lambdaMax - mu(2,0)) / (mu(1,1) ));

}

void RegionType::epsilon(){

	eccentricity = sqrt(lambdaMax/lambdaMin);
}

void RegionType::setData(const ImageType<int>& img){

	if(positions.isEmpty())
		throw "There is not a region!";

	//set centroids{x,y}
	xyBar();

	//set size
	size = (int)moment(0,0);
	
	//set orientation
	theta();

	//set lambda{max,min}
	lambda();
	//set eccentricity
	epsilon();

	//set mean Val
	

	//set minVal
	

	//set maxVal

}

const int RegionType::getCentroidR()const{
	return centroidR;
}

const int RegionType::getCentroidC()const{
	return centroidC;
}

const int RegionType::getSize()const{
	return size;
}

const double RegionType::getOrientation()const{
	return orientation;
}

const double RegionType::getEccentricity()const{
	return eccentricity;
}

const int RegionType::getMeanVal()const{
	return meanVal;
}

const int RegionType::getMinVal()const{
	return minVal;
}

const int RegionType::getMaxVal()const{
	return maxVal;
}


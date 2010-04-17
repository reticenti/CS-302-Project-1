#include "RegionType.h"

RegionType::RegionType()
{
	size = meanVal = minVal = maxVal = 0;
	orientation = eccentricity = centroidR = centroidC = 0.0;
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
			eccentricity == rhs.eccentricity );
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
	PixelType tmp;

	while(!positions.atEnd()){
		
		tmp = positions.getNextItem();
		total += (pow((double)tmp.c,p))*(pow((double)tmp.r,q));
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

	double total = 0;

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

void RegionType::setData(){

	if(positions.isEmpty())
		throw "There is not a region!";

	//set centroids{x,y}
	xyBar();

	//set size
	size = positions.getLength();
	
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

double RegionType::getCentroidR()const{
	return centroidR;
}

double RegionType::getCentroidC()const{
	return centroidC;
}

int RegionType::getSize()const{
	return size;
}

double RegionType::getOrientation()const{
	return orientation;
}

double RegionType::getEccentricity()const{
	return eccentricity;
}

int RegionType::getMeanVal()const{
	return meanVal;
}

int RegionType::getMinVal()const{
	return minVal;
}

int RegionType::getMaxVal()const{
	return maxVal;
}


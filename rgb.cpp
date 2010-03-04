#include "rgb.h"

// default construtor, sets to 0
rgb::rgb(){

	r = 0;
	g = 0;
	b = 0;

}

// parameterized constructor sets up pixels for r,g,b values
rgb::rgb(int rVal, int gVal, int bVal){

	r = rVal;
	g = gVal;
	b = bVal;

}

//copies info from the right hand side
rgb::rgb( const rgb& rhs){


	r = rhs.r;
	g = rhs.g;
	b = rhs.b;

}

// same as copy 
rgb& rgb::operator= ( const rgb& rhs){

	r = rhs.r;
	g = rhs.g;
	b = rhs.b;

	return *this;
}


// destructor sets all values to 0
rgb::~rgb(){

	r = 0;
	g = 0;
	b = 0;
}

// operator = 
rgb& rgb::operator= ( const int& rhs){

	r = rhs;
	g = rhs;
	b = rhs;

	return *this;
}

// Operator +
rgb& rgb::operator+(const int& rhs){

	r += rhs;
	g += rhs;
	b += rhs;

	return *this;
}

rgb& rgb::operator+(const rgb& rhs){

	r += rhs.r;
	g += rhs.g;
	b += rhs.b;

	return *this;
}

// Operator -
rgb& rgb::operator-(const int& rhs){

	r -= rhs;
	g -= rhs;
	b -= rhs;

	return *this;
}

rgb& rgb::operator-(const rgb& rhs){

	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;

	return *this;
}

// Operator /
rgb& rgb::operator/(const int& rhs){

	r /= rhs;
	g /= rhs;
	b /= rhs;

	return *this;
}

rgb& rgb::operator/(const rgb& rhs){

	r /= rhs.r;
	g /= rhs.g;
	b /= rhs.b;

	return *this;
}

// Operator *
rgb& rgb::operator*(const int& rhs){

	r *= rhs;
	g *= rhs;
	b *= rhs;

	return *this;
}

rgb& rgb::operator*(const rgb& rhs){

	r *= rhs.r;
	g *= rhs.g;
	b *= rhs.b;

	return *this;
}

//operator +=
void rgb::operator+=(const rgb& rhs){

	r += rhs.r;
	r += rhs.g;
	r += rhs.b;

}

//operator -=
void rgb::operator-=(const rgb& rhs){

	r -= rhs.r;
	r -= rhs.g;
	r -= rhs.b;
}

//operator /=
void rgb::operator/=(const rgb& rhs){

	r /= rhs.r;
	r /= rhs.g;
	r /= rhs.b;
}

//operator *=
void rgb::operator*=(const rgb& rhs){

	r *= rhs.r;
	r *= rhs.g;
	r *= rhs.b;

}

//operator <
bool rgb::operator<(const rgb& rhs)const{

	if(rhs.r > r)
		return false;

	else if(rhs.g > g)
		return false;

	else if(rhs.b > b)
		return false;

	else return true;
}

bool rgb::operator<(const int& rhs)const{

	int avg = (r+g+b)/3;
	return (avg < rhs);
}

//operator >
bool rgb::operator>(const rgb& rhs)const{

	if(rhs.r < r)
		return false;

	else if(rhs.g < g)
		return false;

	else if(rhs.b < b)
		return false;

	else return true;
}

bool rgb::operator>(const int& rhs)const{

	int avg = (r+g+b)/3;
	return (avg > rhs);
}

//operator ==
bool rgb::operator==(const rgb& rhs)const{

	if(rhs.r != r)
		return false;

	else if(rhs.g != g)
		return false;

	else if(rhs.b != b)
		return false;

	else return true;
}

bool rgb::operator==(const int& rhs)const{

	if(rhs != r)
		return false;

	else if(rhs != g)
		return false;

	else if(rhs != b)
		return false;

	else return true;
}

//operator !=
bool rgb::operator!=(const rgb& rhs)const{

	if(rhs.r == r)
		return false;

	else if(rhs.g == g)
		return false;

	else if(rhs.b == b)
		return false;

	else return true;
}

bool rgb::operator!=(const int& rhs)const{

	if(rhs == r)
		return false;

	else if(rhs == g)
		return false;

	else if(rhs == b)
		return false;

	else return true;
}

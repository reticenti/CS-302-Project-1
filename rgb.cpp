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
rgb::rgb& operator= ( const rgb& rhs){

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
rgb::rgb& operator= ( const int& rhs){

	r = rhs;
	g = rhs;
	b = rhs;

	return *this;
}

// Operator +
rgb::rgb& operator+(const int& rhs){

	r += rhs;
	g += rhs;
	b += rhs;

	return *this
}

rgb::rgb& operator+(const rgb& rhs){

	r += rhs.r;
	g += rhs.g;
	b += rhs.b;

	return *this
}

// Operator -
rgb::rgb& operator-(const int& rhs){

	r -= rhs;
	g -= rhs;
	b -= rhs;

	return *this
}

rgb::rgb& operator-(const rgb& rhs){

	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;

	return *this
}

// Operator /
rgb::rgb& operator/(const int& rhs){

	r /= rhs;
	g /= rhs;
	b /= rhs;

	return *this
}

rgb::rgb& operator/(const rgb& rhs){

	r /= rhs.r;
	g /= rhs.g;
	b /= rhs.b;

	return *this
}

// Operator *
rgb::rgb& operator*(const int& rhs){

	r *= rhs;
	g *= rhs;
	b *= rhs;

	return *this
}

rgb::rgb& operator*(const rgb& rhs){

	r *= rhs.r;
	g *= rhs.g;
	b *= rhs.b;

	return *this
}

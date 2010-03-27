#ifndef rgb_H
#define rgb_H

#include <cstdlib>

class rgb{

	public:

// CONSTRUCTORS AND DESTRUCTOR /////////////////////////////////////////////////
		// default construtor, sets to 0
		rgb();

		// parameterized constructor sets up pixels for r,g,b values
		rgb(int, int, int);

		// copy allocates memory and copies info from the right hand side
		rgb( const rgb& );	

		// allows to be initialized as integer
		rgb( const int& );

		// same as copy 
		rgb& operator= ( const rgb& );

		// destructor sets all values to 0
		~rgb();

		// OPERATORS ///////////////////////////////////////////////////////////////////

		rgb& operator= (const int& );
		
		rgb operator+(const int& );

		rgb operator+(const rgb& );

		rgb operator-(const int& );

		rgb operator-(const rgb& );

		rgb operator/(const int& );

		rgb operator/(const rgb& );

		rgb operator*(const int& );
		
		rgb operator*(const double& );
		
		rgb operator*(const rgb& );

		void operator+=(const rgb& );

		void operator-=(const rgb& );

		void operator/=(const rgb& );

		void operator/=(const int& );

		void operator*=(const rgb& );

		bool operator<(const rgb& )const;

		bool operator<(const int& )const;

		bool operator>(const rgb& )const;

		bool operator>(const int& )const;

		bool operator==(const rgb& )const;

		bool operator==(const int& )const;

		bool operator!=(const rgb& )const;

		bool operator!=(const int& )const;

		int r; // red value
		int g; // green value
		int b; // blue value
};

rgb abs( const rgb& );

#endif


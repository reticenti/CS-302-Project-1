#ifndef rgb_H
#define
class rgb{

	public:

// CONSTRUCTORS AND DESTRUCTOR /////////////////////////////////////////////////
		// default construtor, sets to 0
		rgb();

		// parameterized constructor sets up pixels for r,g,b values
		rgb(int, int, int);

		// copy allocates memory and copies info from the right hand side
		rgb( const rgb& );

		// same as copy 
		rgb& operator= ( const rgb& );

		// destructor sets all values to 0
		~rgb();

// OPERATORS ///////////////////////////////////////////////////////////////////


		rgb& operator=();

		rgb& operator+();

		rgb& operator-();

		rgb& operator/();

		rgb& operator*();

	private:
		int r; // red value
		int g; // green value
		int b; // blue value
};



}
#endif

#include "comp_curses.h"

/******************************************************************************\
 Start curses up with some useful settings, also set up all possible colors
\******************************************************************************/
void startCurses()
{
	initscr();	// initialize curses
	cbreak();	// don't wait for enter between input
	noecho();	// don't echo characters typed to the window
	nonl();		// makes return key readable

	// initialize colors if possible
	if ( has_colors() )
	{
		// start colors mode
		start_color();

		// set all possible color pairs
		for ( int i = 0; i < NUM_COLORS; i++ ) // background
			for ( int j = 0; j < NUM_COLORS; j++ ) // foreground
				init_pair( j+(i*NUM_COLORS), j, i );
	}
}

/******************************************************************************\
 Ends curses
\******************************************************************************/
void endCurses()
{
	endwin();		// exit curses
}

/******************************************************************************\
 Clears the screen
\******************************************************************************/
void clearScreen()
{
	// clear the screen by printing spaces across it
	for ( int i = 0; i < screenHeight(); i++ )
		for ( int j = 0; j < screenWidth(); j++ )
			mvaddch( i, j, ' ' );
	refresh();
}

/******************************************************************************\
 Returns the width of the current terminal
\******************************************************************************/
int screenWidth()
{
	return getmaxx( stdscr );
}

/******************************************************************************\
 Returns the height of the current terminal
\******************************************************************************/
int screenHeight()
{
	return getmaxy( stdscr );
}

/******************************************************************************\
 Set the color in a certain window
\******************************************************************************/
void setColor( WINDOW *somewin, int cf, int cb )
{
	// using the values created in startCurses set the color pair
	wattron( somewin, COLOR_PAIR( cf+cb*NUM_COLORS ) );
}

/******************************************************************************\
 Set the color in the stdscr window
\******************************************************************************/
void setColor( int cf, int cb )
{
	setColor( stdscr, cf, cb );
}

/******************************************************************************\
 Hides the cursor from site until showCursor() is called
\******************************************************************************/
void hideCursor()
{
	// set cursor to invisible
	curs_set( CURSOR_INVIS );
}

/******************************************************************************\
 Make cursor visible to the user
\******************************************************************************/
void showCursor()
{
	// set cursor to be visible
	curs_set( CURSOR_VIS );
}

/******************************************************************************\
 Prompts the user for an integer value at the given location.  Only allow user
 to enter valid integers and return the value when the return key is pressed.
\******************************************************************************/
int promptForInt( WINDOW *somewin, int y, int x, const char prompt[] )
{
	// holds the user input, current length of the string, and return value
	int input, length = 0, retVal = 0;

	// this is where all the users inputs are stored to be processed into an int
	// after the return key is pressed
	int intAry[MAX_INT_LEN];

	// set all the values in the array to 0
	for ( int i = 0; i < MAX_INT_LEN; i++ )
		intAry[i] = 0;

	// print the prompt
	mvwaddstr( somewin, y, x, prompt );

	// move the x value to the end
	x += strlen( prompt );

	// only echo integer values or negative sign
	do {
		input = wgetch( somewin );

		if ( input >= '0' && input <= '9' && length < MAX_INT_LEN )
		{
			// if valid number is pressed
			intAry[length] = input - (int)'0';
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
		else if ( (input == KEY_BACKSPACE || input == KEY_BS) && length > 0 )
		{
			// accounts for backspace
			length--;
			x--;
			mvwaddch( somewin, y, x, ' ' );
			intAry[length] = 0;

			// actually move cursor back
			move( y, x );
		}
		else if ( input == '-' && length == 0 )
		{
			// if negative is pressed, only on first location
			intAry[length] = 10;
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
	// loop if return key is not pressed or
	//      if return is pressed but length is zero or
	//      if return is pressed but only negative sign exists
	} while ( input != KEY_RETURN || 
	        ( input == KEY_RETURN && length == 0 ) ||
			( input == KEY_RETURN && length == 1 &&	intAry[0] == 10 ) );


	// process array into an integer
	for ( int i = length-1; i >= 0; i-- )
	{
		// process array into an integer
		if ( ( i == 0 && intAry[i] != 10 || i != 0 ) )
			retVal += intAry[i] * pow(10,length-1-i);
	}

	// make it negative if nessessary
	if ( intAry[0] == 10 )
		retVal *= -1;

	// return integer value
	return retVal;
}

/******************************************************************************\
 Prompts the user for a double value at the given location.  Only allow user
 to enter valid doubles and return the value when the return key is pressed.
\******************************************************************************/
double promptForDouble( WINDOW *somewin, int y, int x, const char prompt[] )
{
	// user input, length of double, and decmial point location
	int input, length = 0, decimal = -1;

	// holds the user inputs for the double into an int array, to be processed
	// after return is pressed
	int intAry[MAX_DBL_LEN];

	// the return value
	double retVal = 0.0;

	// intitialize int array to zeros
	for ( int i = 0; i < MAX_DBL_LEN; i++ )
		intAry[i] = 0;

	// print the prompt
	mvwaddstr( somewin, y, x, prompt );

	// move the x value to the end
	x += strlen( prompt );

	// only echo integer values or negative sign
	do {
		input = wgetch( somewin );

		// if integer values are pressed thats okay
		if ( input >= '0' && input <= '9' && length < MAX_DBL_LEN )
		{
			// if valid number is pressed
			intAry[length] = input - (int)'0';
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
		else if ( (input == KEY_BACKSPACE || input == KEY_BS) && length > 0 )
		{
			// accounts for backspace
			length--;
			x--;
			// reset decimal location if needed
			if ( intAry[length] == 11 )
				decimal = -1;

			// move cursor back
			mvwaddch( somewin, y, x, ' ' );
			intAry[length] = 0;

			// actually move cursor back
			move( y, x );
		}
		else if ( input == '-' && length == 0 )
		{
			// if negative is pressed, only on first location
			intAry[length] = 10;
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
		else if ( input == '.' && length < MAX_DBL_LEN-1 && decimal == -1 )
		{
			// only allow a decimal point if one hasn't been placed
			decimal = length;	// store location of decimal point
			intAry[length] = 11;
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
	// loop if return key is not pressed or
	//      if return is pressed but length is zero or
	//      if return is pressed but only negative sign or decimal exists
	//      if return is pressed but only a negative sign and decimal exists
	} while ( input != KEY_RETURN || 
	        ( input == KEY_RETURN && length == 0 ) ||
			( input == KEY_RETURN && length == 1 &&	intAry[0] >= 10 ) ||
			( input == KEY_RETURN && length == 2 && intAry[0] >= 10 &&
			  intAry[1] >= 10 ) );

	// process into actual double value

	// if there is a decimal point
	if ( decimal >= 0 )
	{
		// if negative start at 1 otherwise start at 0
		for ( int i = (intAry[0]==10?1:0); i < decimal; i++ )
			retVal += intAry[i] * pow(10,decimal-i-1);
		for ( int i = decimal+1; i < MAX_DBL_LEN; i++ )
			retVal += intAry[i] / (double)pow(10,i-decimal);
	}

	// no decimal point is same as integer
	if ( decimal < 0 )
		for ( int i = length-1; i >= 0; i-- )
		{
			if ( ( i == 0 && intAry[i] != 10 || i != 0 ) )
				retVal += intAry[i] * pow(10,length-1-i);
		}
	
	// if negative return value
	if ( intAry[0] == 10 )
		retVal *= -1;

	// return the value as a double
	return retVal;
}

/******************************************************************************\
 This is easiest of the prompts because it doesn't need any post input
 processing.  Simply prompt user for a string and return the string
\******************************************************************************/
void promptForString( WINDOW *somewin, int y, int x, const char prompt[],
    char str[], int len )
{
	// user input and current length of string
	int input, length = 0;

	// print the prompt
	mvwaddstr( somewin, y, x, prompt );

	// move the x value to the end
	x += strlen( prompt );

	// only echo integer values or negative sign
	do {
		input = wgetch( somewin );

		// accounts for all valid string values
		if ( input >= ' ' && input <= '~' && length < len-1 )
		{
			// if valid number is pressed
			str[length] = input;
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
		else if ( (input == KEY_BACKSPACE || input == KEY_BS) && length > 0 )
		{
			// accounts for backspace
			length--;
			x--;
			mvwaddch( somewin, y, x, ' ' );
			move( y, x );
		}
	// loop if return key is not pressed or
	//      if return is pressed but length is zero or
	} while ( input != KEY_RETURN || 
	        ( input == KEY_RETURN && length == 0 ) );

	// don't forget the null terminator
	str[length] = '\0';
}


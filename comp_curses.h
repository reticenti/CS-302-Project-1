#include <curses.h>
#include <cstring>
#include <cmath>

const int KEY_RETURN = 13;	// return key
const int KEY_BS = 127;		// alternate backspace key value (some linux terminals)
const int NUM_COLORS = 8;	// the number of colors in curses

const int CURSOR_INVIS = 0;
const int CURSOR_VIS = 1;

const int MAX_INT_LEN = 8;
const int MAX_DBL_LEN = 13;

// intialize curses
void startCurses();

// finish curses
void endCurses();

// set color of given window
void setColor( WINDOW*, int, int );

// set color of stdscr
void setColor( int, int );

int screenWidth();

int screenHeight();

// prompt user for an integer at the location
int promptForInt( WINDOW *somewin, int y, int x, const char promptString[] );

// prompt user for a double at that location
double promptForDouble( WINDOW *somewin, int y, int x, const char promptString[] );

// prompt user for a string at the location
void promptForString( WINDOW *somewin, int y, int x, const char promptString[], char str[], int len );

void startCurses()
{
	initscr();
	cbreak();		// don't wait for enter between input
	noecho();		// don't echo characters typed to the window
	nonl();

	// initialize colors
	if ( has_colors() )
	{
		start_color();

		// set all possible color pairs
		for ( int i = 0; i < NUM_COLORS; i++ ) // background
			for ( int j = 0; j < NUM_COLORS; j++ ) // foreground
				init_pair( j+(i*NUM_COLORS), j, i );
	}
}

void endCurses()
{
	endwin();		// exit curses
}

int screenWidth()
{
	return getmaxx( stdscr );
}

int screenHeight()
{
	return getmaxy( stdscr );
}

void setColor( WINDOW *somewin, int cf, int cb )
{
	wattron( somewin, COLOR_PAIR( cf+cb*NUM_COLORS ) );
}

void setColor( int cf, int cb )
{
	setColor( stdscr, cf, cb );
}

void hideCursor()
{
	curs_set( CURSOR_INVIS );
}

void showCursor()
{
	curs_set( CURSOR_VIS );
}

int promptForInt( WINDOW *somewin, int y, int x, const char promptString[] )
{
	int input, length = 0, retVal = 0;
	int intAry[MAX_INT_LEN];

	for ( int i = 0; i < MAX_INT_LEN; i++ )
		intAry[i] = 0;

	// print the prompt
	mvwaddstr( somewin, y, x, promptString );

	// move the x value to the end
	x += strlen( promptString );

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
	/* loop if return key is not pressed or
	        if return is pressed but length is zero or
	        if return is pressed but only negative sign exists */
	} while ( input != KEY_RETURN || 
	        ( input == KEY_RETURN && length == 0 ) ||
			( input == KEY_RETURN && length == 1 &&	intAry[0] == 10 ) );


	// turn values into integer
	for ( int i = length-1; i >= 0; i-- )
	{
		if ( ( i == 0 && intAry[i] != 10 || i != 0 ) )
			retVal += intAry[i] * pow(10,length-1-i);
	}

	// make it negative if nessessary
	if ( intAry[0] == 10 )
		retVal *= -1;

	return retVal;
}

double promptForDouble( WINDOW *somewin, int y, int x, const char promptString[] )
{
	int input, length = 0, count = -1, decimal = -1;
	int intAry[MAX_DBL_LEN];
	double retVal = 0.0;

	for ( int i = 0; i < MAX_DBL_LEN; i++ )
		intAry[i] = 0;

	// print the prompt
	mvwaddstr( somewin, y, x, promptString );

	// move the x value to the end
	x += strlen( promptString );

	// only echo integer values or negative sign
	do {
		input = wgetch( somewin );

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

			// reset decimal location

			length--;
			x--;
			if ( intAry[length] == 11 )
				decimal = -1;
			mvwaddch( somewin, y, x, ' ' );
			intAry[length] = 0;
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
			decimal = length;	// store location of decimal point
			intAry[length] = 11;
			mvwaddch( somewin, y, x, input );
			x++;
			length++;
		}
	/* loop if return key is not pressed or
	        if return is pressed but length is zero or
	        if return is pressed but only negative sign or decimal exists
			if return is pressed but only a negative sign and decimal exists */
	} while ( input != KEY_RETURN || 
	        ( input == KEY_RETURN && length == 0 ) ||
			( input == KEY_RETURN && length == 1 &&	intAry[0] >= 10 ) ||
			( input == KEY_RETURN && length == 2 && intAry[0] >= 10 && intAry[1] >= 10 ) );

	// parse into actual double value

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

	return retVal;
}

void promptForString( WINDOW *somewin, int y, int x, const char promptString[], char str[], int len )
{
	for ( int i = 0; i < len; i++ )
		str[i] = '\0';

	int input, length = 0;

	// print the prompt
	mvwaddstr( somewin, y, x, promptString );

	// move the x value to the end
	x += strlen( promptString );

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

			// reset decimal location

			length--;
			x--;
			mvwaddch( somewin, y, x, ' ' );
			move( y, x );
		}
	/* loop if return key is not pressed or
	        if return is pressed but length is zero or */
	} while ( input != KEY_RETURN || 
	        ( input == KEY_RETURN && length == 0 ) );

	str[length] = '\0';
}


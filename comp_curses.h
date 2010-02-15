/******************************************************************************\
 Author: Joshua Gleason

 This header was created to compliment curses and allow user input in form of
 ints, doubles, and strings to be obtained.  A few other functions such as
 startCurses and setColor were defined here to make initialization and color
 changing easier to deal with.
\******************************************************************************/

#ifndef COMP_CURSES
#define COMP_CURSES

#include <curses.h>
#include <cstring>
#include <cmath>

/******************************************************************************\
                                   CONSTANTS
\******************************************************************************/

	const int KEY_RETURN = 13;	// return key
	const int KEY_BS = 127;		// alternate backspace key value
	const int NUM_COLORS = 8;	// the number of colors in curses

	const int CURSOR_INVIS = 0; // the value indicating an invisible cursor
	const int CURSOR_VIS = 1;	// a visible cursor

	const int MAX_INT_LEN = 8;	// maximum allowed length of an integer
	const int MAX_DBL_LEN = 13; // maximum allowed length of a double

/******************************************************************************\
                               FUNCTION PROTOTYPES
\******************************************************************************/

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

	void hideCursor();

	void showCursor();

	// prompt user for an integer at the location
	int promptForInt( WINDOW*, int, int, const char [] );

	// prompt user for a double at that location
	double promptForDouble( WINDOW*, int, int, const char[] );

	// prompt user for a string at the location
	void promptForString( WINDOW *, int, int, const char[], char[], int );

#endif
	

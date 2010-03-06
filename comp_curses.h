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

	// name        : startCurses
	// input       : none
	// output      : starts curses and also sets all possible color pairs
	// assumptions : assumes curses hasn't been initialized
	void startCurses();

	// name        : endCurses
	// input       : none
	// output      : ends curses mode
	// assumptions : assumes that curses has been initialized
	void endCurses();

	// name        : clearScreen
	// input       : none
	// output      : prints spaces across the screen
	// assumptions : assumes that curses has been initialized
	void clearScreen();

	// name        : setColor 
	// input       : (optional WINDOW), foreground color and background colors
	// output      : changes the colors to be used at the given window
	// assumptions : if no WINDOW is passed then stdscr is assumed
	void setColor( WINDOW*, int, int );
	void setColor( int, int );

	// name        : screen<Width/Height>
	// input       : none 
	// output      : returns the terminal width or height
	// assumptions : curses is started
	int screenWidth();
	int screenHeight();

	// name        : <hide/show>Cursor
	// input       : none
	// output      : set the cursor to visible or invisible
	// assumptions : curses is started
	void hideCursor();
	void showCursor();

	// name        : promptForInt
	// input       : a WINDOW, yLoc and xLoc and prompt string
	// output      : prompts user for an integer and returns that value
	// assumptions : curses is started and the prompt is a valid c string
	int promptForInt( WINDOW*, int, int, const char [] );

	// name        : promptForDouble
	// input       : a WINDOW, yLoc and xLoc and prompt string
	// output      : prompts user for a double and returns that value 
	// assumptions : curses is started and the prompt is a valid c string
	double promptForDouble( WINDOW*, int, int, const char[] );

	// name        : promptForString
	// input       : a WINDOW, yLoc and xLoc, a prompt string, a string to store
	//               obtained value and the max length of the string
	// output      : prompt user for a string and return that value
	// assumptions : curses is started and the prompt is a valid c string
	void promptForString( WINDOW *, int, int, const char[], char[], int );

#endif
	

/******************************************************************************\
 Authors              : Josiah Humphrey and Joshua Gleason

 Date Started         : 2/02/2010
 Last Modified        : 3/29/2010
 Date Due For Review  : 03/30/2010
 Version              : 1.2

 This program is designed to be a driver for the ImageType objects.  The
 user interface attempts to allow the objects to be throughly tested in a
 robust, simple environment.

 The ImageType object (defined in image.h) is for manipulating grayscale
 images, it allows the user to easly enlarge, rotate, negate, etc... an image.
 The functions in imageIO.h are used to load and save images of type .pgm.

 We choose to use curses library to make a more visually pleasing main
 driver, it implements our scrolling menu system.

 The dirent.h library is used to scan for files in the appropriate location,
 in our case we only list .pgm files located in the local images folder.  This
 is better understood by examining the findLocalPGM function.

 comp_curses.h was written to make initializing curses easier, it also has
 some functions for obtaining user input as integers, doubles, and strings.
 Many ncurses library functions however are used directly in this program.

 *Change Log*******************************************************************

 Version 1.2
 -added count regions
 Count regions is a function that counts regions of an image and changes the
 image to the thresholded image with each region labeled a different color.
 A message box of the number of regions is displayed after running the
 function.

 Version 1.1
 -added color image support (.ppm)
 This was accomplished by templating the ImageType class to be used
 with either an int or rgb type pixel.  The rgb type pixel has all the
 arithmatic operators overloaded to make this possible.  Anything that
 was done to a single int is now done to all 3 values (red, green, blue).

\******************************************************************************/

#include <string>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <cstring>
#include <stack>
#include "queue.h"
#include "comp_curses.h"
#include "imageIO.h"
#include "image.h"
#include <ctime>

using namespace std;

// This struct is used to store locations of pixels in a stack or queue
struct location
{
	int r;							// holds row value
	int c;							// holds column value
};

/******************************************************************************\
 CONSTANTS
\******************************************************************************/

	// the folder with the images in it, (make it ./ for local) 
	const char IMAGELOC[] = "./images/";

	const int REGS = 5;				// values 1-9
	const int MENU_OPTIONS = 17;	// number of main menu choices
	const int BAD_REG = REGS;		// dont change this
	const int NAME_LEN = 50;		// the max string length of names

	const int MSGBOX_WIDTH = 60;	// message box width (also input box)
	const int MSGBOX_HEIGHT = 4;	// message box height

	const int MENU_WIDTH = 40;		// holds the menu width and height
	const int MENU_HEIGHT = MENU_OPTIONS*2+3;

	const int REGWIN_WIDTH = 36;	// holds the register window width
	const int REGWIN_HEIGHT = REGS*2+5;

	const int FILEWIN_WIDTH = 36;	// file window width and height
	const int FILEWIN_HEIGHT = MENU_HEIGHT-REGWIN_HEIGHT-1;

	const int MAX_IMG = 10000;		// the max size you can enlarge to
	const int MIN_IMG = 4;			// the min size you can reduce to

	const short BG_COLOR = COLOR_BLUE;	// background color
	const short FG_COLOR = COLOR_BLACK;	// doesn't matter(must be dif than BG)

	const short MENU_BACKGROUND = COLOR_CYAN;	// window backgrounds
	const short MENU_FOREGROUND = COLOR_BLACK;	// window foregrounds

/******************************************************************************\
 FUNCTION PROTOTYPES
\******************************************************************************/
	// name        : showMenu
	// input       : an un-initialized window pointer, a string to be the title,
	//               height, width, xLoc, yLoc of the window, list of c-style
	//               strings to be used in the menu, the number of menu options,
	//               and a bool value which says weather the last choice is
	//               left highlighted
	// output      : Display a window with menu options, let user choose and
	//               return the index of that choice
	// assumptions : assumes that window is un-intialized and will be destructed
	//               by calling function
	int showMenu( WINDOW *&, const char[], int, int, int, int, 
	    char[][NAME_LEN], int, bool=false );

	// name        : showMenu
	// input       : same as above function except with dynamic string list
	// output      : same as above function
	// assumptions : assumes the same about window as above, also assums the
	//               dynamic list of strings has been initialized to at least
	//               the number of window options.  The list of strings is not
	//               de-allocated by this function
	int showMenu( WINDOW *&, const char[], int, int, int, int, 
	    char*[], int, bool=false );

	// name        : showRegs
	// input       : an un-initialized window pointer, a title string, and a
	//               list of register names
	// output      : displays a window next to main of all the registers
	// assumptions : allocates but doesn't delete the WINDOW object
	void showRegs( WINDOW *&, const bool[], const char[][NAME_LEN] );	

	// name        : drawWindow
	// input       : an un-intialized window pointer, a title string, and then
	//               the window height, width, yLoc, and xLoc, plus the colors
	//               for the background and foreground which are defaulted
	// output      : displays a empty window with a border and title using the
	//               given parameters
	// assumptions : allocates but doesn't delete the WINDOW object
	void drawWindow( WINDOW *&, const char[], int, int, int, int, 
	    short=MENU_BACKGROUND, short=MENU_FOREGROUND );

	// name        : deleteMenu
	// input       : a WINDOW pointer that is allocated
	// output      : de-allocate memory for the window pointer and refresh the
	//               main screen
	// assumptions : assumes WINDOW object is intialized before calling
	void deleteMenu( WINDOW *& );

	// name        : processEntry
	// input       : List of register images, list of register bools, list of
	//               register names, and a value assumed to be choosen by user
	// output      : depending on the value, call a function to do some image
	//               manipulation
	// assumptions : assumes value >= 0 and < MENU_OPTIONS, not that anything
	//               will crash if its not true, but nothing will happen, also
	//               assumes that names contain valid c strings
	template <class pType>
	void processEntry( ImageType<pType>[], bool[], char[][NAME_LEN], int );

	// name        : stdWindow
	// input       : an un-initalized window and a title string
	// output      : displays a window in the standard text box location with
	//               the title and a border
	// assumptions : the window object is initalized here but not deleted, this
	//               is left up to the calling function
	void stdWindow( WINDOW *&, const char[] );

	// name        : promptForReg
	// input       : list of register bools, list of regist names, a flag that
	//               indicates if registers that have not been loaded can be
	//               choosen, the yLoc, and xLoc of the menu
	// output      : Display a menu with the registers in it, allowing user to
	//               choose a register
	// assumptions : assumes that names are already set to valid c strings
	int promptForReg( bool[], char[][NAME_LEN], const bool = true, 
	    int=1, int=MENU_WIDTH+3 );

	// name        : promptForFilename
	// input       : title string, prompt string and char used to store user
	//               input
	// output      : sets the final parameter equal to the filename the user
	//               chooses and returns the length
	// assumptions : assumes first 2 parameters are valid c strings and that
	//               the final parameter is a string of at least length 16 +
	//               the length of the file path declared as a constant
	int promptForFilename( const char[], const char[], char[] );

	// name        : promptForLoc
	// input       : prompt string, image object, and 2 integers passed by ref
	// output      : sets two reference parameters equal to row and column of
	//               users choice
	// assumptions : assumes image is intialized and has a valid height/width
	//               also that first parameter is a valid c string
	template <class pType>
	void promptForLoc( const char[], ImageType<pType>&, int&, int& );

	// name        : promptFor<Pix/Scale>Value
	// input       : title string, prompt string and max input value
	// output      : prompts user in message window and returns the value when
	//               the user inputs a valid value.  -1 indicates cancel
	// assumptions : for Pix the minimum value is 0 and for Scale its 2, thats
	//               the only difference.  Also assumes that first 2 parameters
	//               are valid c strings
	int promptForPixValue( const char[], const char[], int );
	int promptForScaleValue( const char[], const char[], int );

	// name        : promptForMirror
	// input       : title string, prompt string
	// output      : prompts user for a H, V, or C and doesn't let them cont
	//               until one is choosen, then returns input value to calling
	//               function
	// assumptions : both parameters are valid c strings
	char promptForMirror( const char[], const char[] );

	// name        : promptForAngle
	// input       : title string, prompt string
	// output      : prompts user for an angle 0-360 and returns the value when
	//               a valid number is sent.  -1 indicates cancel
	// assumptions : both parameters are valid c strings
	int promptForAngle( const char[], const char[] );

	// name        : messageBox
	// input       : title string and message string
	// output      : displays a message box in the center of the screen with
	//               the message displayed in it.  Waits for user to press
	//               return before continueing
	// assumptions : assumes both parameters are valid c strings
	void messageBox( const char[], const char[] );

	// name        : fillRegs
	// input       : list of images, bools, and c strings all of length REGS
	//               also the number of arguments passed to main and the array
	//               of strings passed to main
	// output      : sets valid arguments to registers (loading images) and
	//               clears the rest of the registers
	// assumptions : assumes that char** is a valid list of strings with int
	//               rows
	template <class pType>
	void fillRegs( ImageType<pType>[], bool[], char[][NAME_LEN], int, char** );

	// name        : Register manipulation functions
	// input       : List of images of length REGS, list of bools of length
	//               REGS, and list of c strings of length REGS.
	// output      : Each function prompts user for information pertaining
	//               to its manipulation function, these should be pretty
	//               obvious looking at each functions name.  All input is
	//               bounds checked to make sure no bad input is passed to an
	//               ImageType object
	// assumptions : assumes all names in the c string list are valid c
	//               c strings and bools coincide with wether image types are
	//               loaded of the same index
	void loadImage( ImageType<int>[], bool[], char[][NAME_LEN] );
	void loadImage( ImageType<rgb>[], bool[], char[][NAME_LEN] );
	void saveImage( ImageType<int>[], bool[], char[][NAME_LEN] );
	void saveImage( ImageType<rgb>[], bool[], char[][NAME_LEN] );
	void getImageInfo( ImageType<int>[], bool[], char[][NAME_LEN] );
	void getImageInfo( ImageType<rgb>[], bool[], char[][NAME_LEN] );
	void setPixel( ImageType<int>[], bool[], char[][NAME_LEN] );
	void setPixel( ImageType<rgb>[], bool[], char[][NAME_LEN] );
	void getPixel( ImageType<int>[], bool[], char[][NAME_LEN] );
	void getPixel( ImageType<rgb>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void extractSub( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void enlargeImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void shrinkImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void reflectImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void translateImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void rotateImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void sumImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void subtractImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void negateImg( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void clearRegister( ImageType<pType>[], bool[], char[][NAME_LEN] );
	template <class pType>
	void countRegions( ImageType<pType>[], bool[], char[][NAME_LEN] );

	// Functions used for Count Regions	////////////////////////////////////////////

	// Josh's Functions ////////////////////////////////////////////////////////

	template <class pType>
	int computeComponents( ImageType<pType>, ImageType<pType>& );

	template <class pType>
	void findComponentsDFS( ImageType<pType>, ImageType<pType>&, int, int,
	    pType );

	// Josiah's Functions //////////////////////////////////////////////////////

	template <class pType>
	void findComponentsBFS( ImageType<pType>, ImageType<pType>&, int, int,
	    pType );

	// Extra Functions /////////////////////////////////////////////////////////
	template <class pType>
	void findComponentsRec(const ImageType<pType>&, ImageType<pType>&, int,
	    int, pType);

	////////////////////////////////////////////////////////////////////////////////

	// name        : findLocalPGM/PPM
	// input       : one un-intialized double pointer of chars
	// output      : allocates enough memory for a list of all the .pgm or .ppm
	//               files in the local path specified by the FILELOC constant.
	//               It then copys the file names to the array and returns the
	//               number of rows in the array.
	// assumptions : filenames is not initialized, but will be in the function
	//               this means it needs to be de-allocated before it goes out
	//               of scope
	int findLocalPGM( char **&filenames );
	int findLocalPPM( char **&filenames );

/******************************************************************************\
|                                      MAIN                                    |
\******************************************************************************/

int main( int argc, char **argv )
{
	// main menu object pointer
	WINDOW *menu;

	// register window object pointer
	WINDOW *regWin;

	// users menu choice
	int choice;
	bool color;

	// holds the name of the image stored in the register
	char imgName[REGS][NAME_LEN];

	// bool array indicating if registers are loaded with images
	bool imgLoaded[REGS];

	// this is where all the registers are stored
	ImageType<int> grayImage[REGS];
	ImageType<rgb> colorImage[REGS];

	// create main menu strings
	char choices[MENU_OPTIONS][NAME_LEN] = {
		"  Read an image from a file",
		"  Save an image to a file",
		"  Get image info",
		"  Set the value of a pixel",
		"  Get the value of a pixel",
		"  Extract a subimage from an image",
		"  Enlarge image",
		"  Shrink image",
		"  Reflect image",
		"  Translate image",
		"  Rotate image",
		"  Sum two images",
		"  Subtract two images",
		"  Compute negative of an image",
		"  Count Regions",
		"  Clear a register",
		"  Exit" };

	char colorChoice[2][NAME_LEN] = {
		"Grayscale Images", 
		"Color Images" };

	// start
	startCurses();

	// hide that pesky cursor
	hideCursor();

	// set the colors
	setColor( FG_COLOR, BG_COLOR );

	// initialize the bool array
	for ( int i = 0; i < REGS; i++ )
		imgLoaded[i] = false;

	// clear the screen
	clearScreen();

	// ask if grayscale or color
	choice = showMenu( menu, "Choose Color Mode", 7, MENU_WIDTH,
			screenHeight()/2-4, (screenWidth()-MENU_WIDTH)/2, colorChoice, 2 );

	if ( choice == 0 )
		color = false;
	else
		color = true;

	// clear the screen
	clearScreen();

	// read argument parameters
	if ( !color )
		fillRegs( grayImage, imgLoaded, imgName, argc, argv );
	else
		fillRegs( colorImage, imgLoaded, imgName, argc, argv );

	// set the colors again just in case
	setColor( FG_COLOR, BG_COLOR );

	// clear the screen
	clearScreen();

	do {
		// display register window
		showRegs( regWin, imgLoaded, imgName );

		// show and get input from menu
		choice = showMenu( menu, "Main Menu", MENU_HEIGHT, MENU_WIDTH, 1, 1,
				choices, MENU_OPTIONS );

		try
		{
			// this is the main driving function that calls all others
			if ( color )
				processEntry( colorImage, imgLoaded, imgName, choice );
			else
				processEntry( grayImage, imgLoaded, imgName, choice );
		}
		catch( string err )
		{
			// display the message string to user
			messageBox( "Error!", err.c_str() );
		}
		catch( ... )
		{
			// handle errors like this later
			return -1;
		}

		// makes sure everything is reset
		delwin( regWin );
		deleteMenu( menu );
	} while ( choice != MENU_OPTIONS-1 );

	// end curses
	endCurses();

	return 0;
}

/******************************************************************************\
|                           FUNCTION IMPLEMENTATION                            |
\******************************************************************************/

/******************************************************************************\
 Overloaded showMenu that accepts static arrays of strings (length NAME_LEN)
 and then creates a dynamic version and passes it to the other showMenu,
 afterwards the memory is de-allocated
\******************************************************************************/
int showMenu( WINDOW *& menu, const char title[], int height, int width,
		int locY, int locX, char menuStr[][NAME_LEN], int choices, bool erase )
{
	// holds the new dynamic array of strings
	char **menuStrPtr = new char*[choices];

	// will hold the return value from other menu call
	int retVal;

	// count through the static array, allocating memory for the dynamic one
	for ( int i = 0; i < choices; i++ )
	{
		menuStrPtr[i] = new char[NAME_LEN];

		// this is where the string value is copied
		strcpy( menuStrPtr[i], menuStr[i] );
	}

	// call the other menu function
	retVal = showMenu( menu, title, height, width, locY, locX, 
			menuStrPtr, choices, erase );

	// de-allocate memory for temporary string array
	for ( int i = 0; i < choices; i++ )
		delete [] menuStrPtr[i];
	delete [] menuStrPtr;

	// return value obtained by other menu
	return retVal;
}

/******************************************************************************\
 This is the function which builds the scrolling menu system, this simply
 creates a curses window and puts all the options stores in menuStr onto the
 window, it then waits for the user to press UP, DOWN, or RETURN before
 reacting.  The parameters allow menus to be different widths, heights, and
 locations.  A few constants can be changed to change the colors of the window.

 !!!Beware the WINDOW pointer 'menu' is intialized here but is NOT deleted.
 It is up to the calling function to take care of this object.
\******************************************************************************/
int showMenu( WINDOW *& menu, const char title[], int height, int width,
		int locY, int locX, char *menuStr[], int choices, bool eraseHighlight )
{
	/* x, y variables hold location of cursor, choiceLoc is the current choice
	   thats selected, menuLoc is the location on the menu, and input is the
	   value of key input */
	int x, y, choiceLoc, menuLoc, perScreen, input;

	// formatStr is make sure strings are length 40 no matter what
	char formatStr[10] = "%-";

	// draw the window (intializing the WINDOW object as well)
	drawWindow( menu, title, height, width, locY, locX );

	// finish creating the format string
	sprintf( formatStr, "%s%i.%is", formatStr, width-4, width-4 );

	// initialize int values
	x = 2, y = 2;
	choiceLoc = input = menuLoc = 0;

	// this is number of options displayed at a time
	perScreen = (height - 3 ) / 2;

	// turns keypad on for menu (for arrow keys)
	keypad( menu, TRUE );

	// loop for user input
	do {

		// draw the visible menu options
		for ( int i = 0; i < perScreen && i < choices; i++ )
		{
			y = 2+i*2;
			mvwprintw( menu, y, x, formatStr, menuStr[choiceLoc-menuLoc+i] );
		}

		// set mode to highlight
		setColor( menu, MENU_BACKGROUND, MENU_FOREGROUND );
		y = 2 + menuLoc*2;

		// highlight current selection
		mvwprintw( menu, y, x, formatStr, menuStr[choiceLoc] );
		wrefresh( menu );

		// only react to KEY_UP, KEY_DOWN or KEY_RETURN
		do {
			input = wgetch( menu );
		} while ( input != KEY_UP && 
				input != KEY_DOWN && 
				input != KEY_RETURN );

		// dont highlight for now
		setColor( menu, MENU_FOREGROUND, MENU_BACKGROUND );

		// unhighlight current option
		if ( eraseHighlight )
			mvwprintw( menu, y, x, formatStr, menuStr[choiceLoc] );

		// redrawn menu with highlight off in case this is final loop
		wrefresh( menu );

		// test input value
		switch ( input )
		{
			case KEY_UP:	// move up menu

				// choice needs to roll to bottom
				if ( --choiceLoc < 0 )
				{
					choiceLoc = choices-1;

					// set menuLoc to bottom of screen or list
					if ( choices < perScreen )
						menuLoc = choiceLoc;
					else
						menuLoc = perScreen-1;
				}
				else if ( --menuLoc < 0 )
					// only decrement menuLoc if its not 0
					menuLoc++;
				break;
			case KEY_DOWN:	// move down menu

				// choice needs to roll to top
				if ( ++choiceLoc > choices-1 )
				{
					choiceLoc = 0;
					menuLoc = 0;
				}
				else if ( ++menuLoc > perScreen-1 )
					// onlt increment menu if its not at the bottom
					menuLoc--;
				break;
		}
	} while ( input != KEY_RETURN );

	// return the menu choice
	return choiceLoc;
}

/******************************************************************************\
 Display a window of registers next to the main menu (or wherever the constants
 dictate)

 !!!Beware the WINDOW pointer 'regWin' is intialized here but is NOT deleted.
 It is up to the calling function to take care of this object.
\******************************************************************************/
void showRegs( WINDOW *& regWin, const bool loaded[], 
		const char names[][NAME_LEN] )
{
	// draw/initialize the window to display the 
	drawWindow( regWin, "Registers", REGWIN_HEIGHT, REGWIN_WIDTH, 1, 
			MENU_WIDTH+3 );

	// add the register names to the window
	for ( int i = 0; i < REGS; i++ )
		mvwprintw( regWin, i*2+2, 2, "%-32.32s", names[i] );

	// make sure the new characters are printed!
	wrefresh( regWin );
}

/******************************************************************************\
 This basically clears the entire screen after deleting the window that is
 passed.
\******************************************************************************/
void deleteMenu( WINDOW *& menu )
{
	// delete the menu object
	delwin( menu );

	// touch the screen to make sure it knows things have changed then refresh
	touchwin( stdscr );
	refresh();
}

/******************************************************************************\
 This function simply draws an empty window with a given title, height, width,
 x, and y locations.  The colors have default values but can be changed if
 oddly colored windows are wanted.
\******************************************************************************/
void drawWindow( WINDOW *& win, const char title[], int height, int width,
		int y, int x, short bgColor, short fgColor )
{
	// intialize window
	win = newwin( height, width, y, x );

	// set new colors
	setColor( win, fgColor, bgColor );

	// make text bold (brighter)
	wattron( win, A_BOLD );

	// draw border
	box( win, 0, 0 );

	// fill window with spaces
	for ( int i = 1; i < width-1; i++ )
		for ( int j = 1; j < height-1; j++ )
			mvwaddch( win, j, i, ' ' );

	// add title
	mvwaddstr( win, 0, 2, title );
}

/******************************************************************************\
 This is the function that decides where to go depending on the choice in the
 main menu.  The reason it has all the parameters is for passing to the
 subsequent functions that will be using them.
\******************************************************************************/
template <class pType>
void processEntry( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN], 
		int choice )
{
	// enter switch statement evaluating choice
	switch ( choice )
	{
		case 0:	// read image
			loadImage( img, loaded, name );
			break;
		case 1:	// write image
			saveImage( img, loaded, name );
			break;
		case 2:	// image info
			getImageInfo( img, loaded, name );
			break;
		case 3:	// set pixel val
			setPixel( img, loaded, name );
			break;
		case 4:	// get pixel val
			getPixel( img, loaded, name );
			break;
		case 5:	// extract subimage
			extractSub( img, loaded, name );
			break;
		case 6:	// enlarge
			enlargeImg( img, loaded, name );
			break;
		case 7:	// shrink
			shrinkImg( img, loaded, name );
			break;
		case 8:	// reflect
			reflectImg( img, loaded, name );
			break;
		case 9:	// translate
			translateImg( img, loaded, name );
			break;
		case 10:	// rotate
			rotateImg( img, loaded, name );
			break;
		case 11:	// sum
			sumImg( img, loaded, name );
			break;
		case 12:	// difference
			subtractImg( img, loaded, name );
			break;
		case 13:	// negative
			negateImg( img, loaded, name );
			break;
		case 14:
			countRegions( img, loaded, name );
			break;
		case 15:	// clear register
			clearRegister( img, loaded, name );
			break;
		case 16:	// exit
			// do nothing lol ^_^ maybe later add an exit screen
			break;
	}
}

/******************************************************************************\
 Prompt for a register that is filled and then clear it.
\******************************************************************************/
template <class pType>
void clearRegister( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;

	// prompt for register to reset
	index = promptForReg( loaded, name );

	// if back wasn't selected...
	if ( index != BAD_REG )
	{
		// set the register to empty
		loaded[index] = false;

		// reset the register name
		sprintf( name[index], "Register %i: Empty", index+1 );
	}
}

/******************************************************************************\
 This is the function that reads the arguments passed to main by the terminal.
 It compiles an error message for every read/write exception that throws a
 string object.  It stores the values into the registers sequentially, if there
 are no arguments relating to the register it is set to empty
\******************************************************************************/
template <class pType>
void fillRegs( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN], int argc,
		char **argv )
{
	char *msg = new char[1+(argc-1)*40];
	int i, j, k;
	bool f;

	// initalize string
	msg[0] = '\0';

	// set empty values
	for ( int index = 0; index < REGS; index++ )
	{
		sprintf( name[index], "Register %i: Empty", index+1 );
		loaded[index] = false;
	}

	// read arguments from main and attempt to load registers
	for ( int index = 1; index < argc && index <= REGS; index++ )
	{
		try
		{
			// read image header
			readImageHeader( argv[index], i, j, k, f );

			// set image info to header value
			img[index-1].setImageInfo( i, j, k );

			// read the rest of the image
			readImage( argv[index], img[index-1] );

			// set the name of the register to the file path
			sprintf( name[index-1], "Register %i: %s", index, argv[index] );

			// make sure the program knows the register is in use
			loaded[index-1] = true;
		}
		catch ( string s )
		{
			// for every exception string caught compile a list of errors
			strcat( msg, s.c_str() );
			strcat( msg, "\n" );
		}
	}

	// display the message to the back screen (will be behind any windows)
	if ( strlen(msg) > 0 )
	{
		// just white on black
		setColor( COLOR_WHITE, COLOR_BLACK );

		// clear screen with black
		clearScreen();

		// display the message
		mvaddstr( 0, 0, msg );

		// wait for input
		getch();
	}

	// de-allocate memory for message
	delete [] msg;
}

/******************************************************************************\
 Prompt the user for a register to load to, then let them choose from a list
 of the .pgm files in the local images directory (defined as a constant). This
 function is used for loading grayscale images only.
\******************************************************************************/
void loadImage( ImageType<int> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds the file names, menuChoices is a copy with "Back" added at the end
	char **fileNames = NULL;
	char **menuChoices;

	// the window that will hold the file menu
	WINDOW *fileMenu;

	// holds the image values N, M, and Q, the number of local image files, the
	// index of index of the register choosen and index of the file choosen
	int i, j, k, files, index, imageVal;

	// format of file (unused for now) but required for readImageHeader param
	bool f;

	// get a list of local files dynamically allocated
	files = findLocalPGM( fileNames );

	// add one more option to the menu
	menuChoices = new char*[files+1];

	// copy all the pointers from the filenames to the menuChoices
	for ( int a = 0; a < files; a++ )
		menuChoices[a] = fileNames[a];

	// delete the list of string pointers since menuChoices has them now
	if ( fileNames != NULL )
		delete [] fileNames;

	// create the final choice
	menuChoices[files] = new char[5];

	// make the final choice "Exit"
	strcpy( menuChoices[files], "Back" );

	// prompt for a register (false indicated it doesn't need to be full)
	index = promptForReg( loaded, name, false );

	// if exit wasn't choosen then prompt for a file
	if ( index != BAD_REG )
	{	
		// prompt for file
		imageVal = showMenu( fileMenu, "Load Image", FILEWIN_HEIGHT,
				FILEWIN_WIDTH, REGWIN_HEIGHT+2, MENU_WIDTH+3 , menuChoices,
				files+1 );

		// if exit isn't choosen attempt to load image
		if ( imageVal != files )
		{
			// read the image that was choosen
			readImageHeader( menuChoices[imageVal], i, j, k, f );

			// set up the image to store the correct data
			img[index].setImageInfo( i, j, k );

			// read and store image data
			readImage( menuChoices[imageVal], img[index] );

			// make sure that the register is read as full
			loaded[index] = true;

			// remove the file path from the front of the filename
			// exampe: ./images/img.pgm -> img.pgm
			sprintf( name[index], "Register %i: %s", index+1,
					&(menuChoices[imageVal][strlen(IMAGELOC)]) );
		}

		// de-allocate fileMenu
		delwin( fileMenu );
	}

	// de-allocate list of menuChoices
	for ( int a = 0; a < files+1; a++ )
		delete [] menuChoices[a];
	delete [] menuChoices;

}

/******************************************************************************\
 Prompt the user for a register to load to, then let them choose from a list
 of the .pgm files in the local images directory (defined as a constant). This
 function is used for loading color images only.
\******************************************************************************/
void loadImage( ImageType<rgb> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds the file names, menuChoices is a copy with "Back" added at the end
	char **fileNames = NULL;
	char **menuChoices;

	// the window that will hold the file menu
	WINDOW *fileMenu;

	// holds the image values N, M, and Q, the number of local image files, the
	// index of index of the register choosen and index of the file choosen
	int i, j, k, files, index, imageVal;

	// format of file (unused for now) but required for readImageHeader param
	bool f;

	// get a list of local files dynamically allocated
	files = findLocalPPM( fileNames );

	// add one more option to the menu
	menuChoices = new char*[files+1];

	// copy all the pointers from the filenames to the menuChoices
	for ( int a = 0; a < files; a++ )
		menuChoices[a] = fileNames[a];

	// delete the list of string pointers since menuChoices has them now
	if ( fileNames != NULL )
		delete [] fileNames;

	// create the final choice
	menuChoices[files] = new char[5];

	// make the final choice "Exit"
	strcpy( menuChoices[files], "Back" );

	// prompt for a register (false indicated it doesn't need to be full)
	index = promptForReg( loaded, name, false );

	// if exit wasn't choosen then prompt for a file
	if ( index != BAD_REG )
	{	
		// prompt for file
		imageVal = showMenu( fileMenu, "Load Image", FILEWIN_HEIGHT,
				FILEWIN_WIDTH, REGWIN_HEIGHT+2, MENU_WIDTH+3 , menuChoices,
				files+1 );

		// if exit isn't choosen attempt to load image
		if ( imageVal != files )
		{
			// read the image that was choosen
			readImageHeader( menuChoices[imageVal], i, j, k, f );

			// set up the image to store the correct data
			img[index].setImageInfo( i, j, k );

			// read and store image data
			readImage( menuChoices[imageVal], img[index] );

			// make sure that the register is read as full
			loaded[index] = true;

			// remove the file path from the front of the filename
			// exampe: ./images/img.pgm -> img.pgm
			sprintf( name[index], "Register %i: %s", index+1,
					&(menuChoices[imageVal][strlen(IMAGELOC)]) );
		}

		// de-allocate fileMenu
		delwin( fileMenu );
	}

	// de-allocate list of menuChoices
	for ( int a = 0; a < files+1; a++ )
		delete [] menuChoices[a];
	delete [] menuChoices;

}

/******************************************************************************\
 Save image from a register to the local images directory, prompting user for
 register and file name.  This is used only for saving grayscale images.
\******************************************************************************/
void saveImage( ImageType<int> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds the file name
	char strInput[NAME_LEN];

	// used to remove the file path from the front of the filename
	char imageLoc[NAME_LEN];

	// holds the register that the user chooss
	int index;

	// prompt for register
	index = promptForReg( loaded, name );

	// if user doesn't choose 'Back'
	if ( index != BAD_REG )
	{
		// prompt the user for a file name
		promptForFilename( "Save Image", "Enter filename: ", strInput );

		// add .pgm to the filename if it wasnt already
		if ( strlen( strInput ) < 4 )
			strcat( strInput, ".pgm" );
		else if ( strcmp( (strInput+strlen(strInput)-4), ".pgm" ) != 0 )
			strcat( strInput, ".pgm" );

		// add the file path to the filename
		sprintf( imageLoc, "%s%s", IMAGELOC, strInput );

		// save the image to the given filename
		writeImage( imageLoc, img[index] );

		// set register name to match file name
		sprintf( name[index], "Register %i: %s", index+1, strInput );
	}
}

/******************************************************************************\
 Save image from a register to the local images directory, prompting user for
 register and file name.  This is used only for saving color images.
\******************************************************************************/
void saveImage( ImageType<rgb> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds the file name
	char strInput[NAME_LEN];

	// used to remove the file path from the front of the filename
	char imageLoc[NAME_LEN];

	// holds the register that the user chooss
	int index;

	// prompt for register
	index = promptForReg( loaded, name );

	// if user doesn't choose 'Back'
	if ( index != BAD_REG )
	{
		// prompt the user for a file name
		promptForFilename( "Save Image", "Enter filename: ", strInput );

		// add .pgm to the filename if it wasnt already
		if ( strlen( strInput ) < 4 )
			strcat( strInput, ".ppm" );
		else if ( strcmp( (strInput+strlen(strInput)-4), ".ppm" ) != 0 )
			strcat( strInput, ".ppm" );

		// add the file path to the filename
		sprintf( imageLoc, "%s%s", IMAGELOC, strInput );

		// save the image to the given filename
		writeImage( imageLoc, img[index] );

		// set register name to match file name
		sprintf( name[index], "Register %i: %s", index+1, strInput );
	}
}

/******************************************************************************\
 Simply retrieve image information and display to a window below the registers
 The data being displayed is the Register number, Image Height, Width, Q value,
 and average gray value.  This displays the data for a grayscale image.
\******************************************************************************/
void getImageInfo( ImageType<int> img[], bool loaded[], char name[][NAME_LEN] )
{
	// hold image info
	int N, M, Q, index, y, x;

	// the window that holds all the info
	WINDOW *infoWin;

	// prompt for a register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// retrieve image height, width, and color depth
		img[index].getImageInfo( N, M, Q );

		// draw/intialize the info window
		drawWindow( infoWin, name[index], 14, FILEWIN_WIDTH, REGWIN_HEIGHT+2,
				MENU_WIDTH+3 );

		// set the starting x/y values
		x = 2;
		y = 2;

		// print all the information to the window with formating
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Saved in Register",':',
				index+1 );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Image Width(pixels)",':',M );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Image Height(pixels)",':',N );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Color Depth",':',Q );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Mean Gray Value",':',
				img[index].meanColor() );
		y+=2;
		mvwprintw( infoWin, y, x, "Press Enter to continue..." );

		wrefresh( infoWin );

		// wait for input
		while ( wgetch( infoWin ) != KEY_RETURN );

		// de-allocate the window
		delwin( infoWin );
	}
}


/******************************************************************************\
 Simply retrieve image information and display to a window below the registers
 The data being displayed is the Register number, Image Height, Width, Q value,
 and average gray value.  This displays the data for a color image.
\******************************************************************************/
void getImageInfo( ImageType<rgb> img[], bool loaded[], char name[][NAME_LEN] )
{
	// hold image info
	int N, M, Q, index, y, x;

	// the window that holds all the info
	WINDOW *infoWin;

	// prompt for a register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// retrieve image height, width, and color depth
		img[index].getImageInfo( N, M, Q );

		// draw/intialize the info window
		drawWindow( infoWin, name[index], 18, FILEWIN_WIDTH, REGWIN_HEIGHT+2,
				MENU_WIDTH+3 );

		// set the starting x/y values
		x = 2;
		y = 2;

		// print all the information to the window with formating
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Saved in Register",':',
				index+1 );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Image Width(pixels)",':',M );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Image Height(pixels)",':',N );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Color Depth",':',Q );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Mean Red Value",':',
				img[index].meanColor().r );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Mean Green Value",':',
				img[index].meanColor().g );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Mean Blue Value",':',
				img[index].meanColor().b );
		y+=2;
		mvwprintw( infoWin, y, x, "Press Enter to continue..." );

		wrefresh( infoWin );

		// wait for input
		while ( wgetch( infoWin ) != KEY_RETURN );

		// de-allocate the window
		delwin( infoWin );
	}
}

/******************************************************************************\
 Prompt user for a register then a pixel location (row, col) and then the pixel
 value to change that pixel to.  Used for grayscale images.
\******************************************************************************/
void setPixel( ImageType<int> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds various information about image
	int index, row, col, val;
	int N, M, Q;

	// prompt for register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for a pixel location
		promptForLoc( "Set Pixel Value", img[index], row, col );

		// if back isn't choosen
		if ( row != -1 && col != -1 )
		{
			// get image info (just for the Q)
			img[index].getImageInfo( N, M, Q );

			// prompt for the pixel with Q as the max value
			val = promptForPixValue( "Set Pixel Value",
					"Enter new pixel value(-1 to cancel): ", Q );

			// if back isn't choosen
			if ( val != -1 )
			{
				// change pixel value
				img[index].setPixelVal( row, col, val );

				// add modified to end of register name
				if ( name[index][strlen(name[index])-1] != ')' )
					strcat( name[index], " (modified)" );
			}
		}
	}
}

/******************************************************************************\
 Prompt user for a register then a pixel location (row, col) and then the pixel
 value to change that pixel to.  Used for color images.
\******************************************************************************/
void setPixel( ImageType<rgb> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds various information about image
	int index, row, col;
	rgb val;
	int N, M, Q;

	// prompt for register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for a pixel location
		promptForLoc( "Set Pixel Value", img[index], row, col );

		// if back isn't choosen
		if ( row != -1 && col != -1 )
		{
			// get image info (just for the Q)
			img[index].getImageInfo( N, M, Q );

			// prompt for the pixel with Q as the max value
			val.r = promptForPixValue( "Set Pixel Value",
					"Enter new red value(-1 to cancel): ", Q );
			if ( val.r != -1 )
			{
				val.g = promptForPixValue( "Set Pixel Value",
						"Enter new green value(-1 to cancel): ", Q );
				if ( val.g != -1 )
				{
					val.b = promptForPixValue( "Set Pixel Value",
							"Enter new blue value(-1 to cancel): ", Q );
					if ( val.b != -1 )
					{
						// change pixel value
						img[index].setPixelVal( row, col, val );

						// add modified to end of register name
						if ( name[index][strlen(name[index])-1] != ')' )
							strcat( name[index], " (modified)" );
					}
				}
			}
		}
	}
}

/******************************************************************************\
 Return the value of a pixel in a selected image to the user(grayscale only).
\******************************************************************************/
void getPixel( ImageType<int> img[], bool loaded[], char name[][NAME_LEN] )
{
	// self describing variables
	int index, row = -1, col = -1, val;
	WINDOW *infoWin;

	// prompt for the register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for pixel location
		promptForLoc( "Get Pixel Value", img[index], row, col );

		// if back isn't choosen
		if ( row != -1 && col != -1 )
		{
			// create a message box window
			stdWindow( infoWin, "Get Pixel Value" );

			// put the pixel value message in the window
			mvwprintw( infoWin, 1, 2, "The pixel Value at (%i,%i) is %i", 
					col, row, img[index].getPixelVal(row, col) );

			// wait for input
			while ( wgetch( infoWin ) != KEY_RETURN );

			// de-allocate the message window
			delwin( infoWin );

		}
	}
}

/******************************************************************************\
 Return the value of a pixel in a selected image to the user(color only).
\******************************************************************************/
void getPixel( ImageType<rgb> img[], bool loaded[], char name[][NAME_LEN] )
{
	// self describing variables
	int index, row = -1, col = -1;
	rgb val;
	WINDOW *infoWin;

	// prompt for the register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for pixel location
		promptForLoc( "Get Pixel Value", img[index], row, col );

		// if back isn't choosen
		if ( row != -1 && col != -1 )
		{
			// create a message box window
			stdWindow( infoWin, "Get Pixel Value" );
			mvwprintw( infoWin, 1, 2, "The red Value at (%i,%i) is %i", 
					col, row, img[index].getPixelVal(row, col).r );
			while ( wgetch( infoWin ) != KEY_RETURN );
			// create a message box window
			stdWindow( infoWin, "Get Pixel Value" );
			mvwprintw( infoWin, 1, 2, "The green Value at (%i,%i) is %i", 
					col, row, img[index].getPixelVal(row, col).g );
			while ( wgetch( infoWin ) != KEY_RETURN );
			// create a message box window
			stdWindow( infoWin, "Get Pixel Value" );
			mvwprintw( infoWin, 1, 2, "The blue Value at (%i,%i) is %i", 
					col, row, img[index].getPixelVal(row, col).b );
			while ( wgetch( infoWin ) != KEY_RETURN );

			// de-allocate the message window
			delwin( infoWin );

		}
	}
}

/******************************************************************************\
 After getting the image to manipulate, prompt for two corners to make a
 subimage out of, if the lower right corner is above or left of the upper
 right corner re-prompt for valid points
\******************************************************************************/
template <class pType>
void extractSub( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// self documenting variables
	int index;
	int ULr, ULc, LRr = -1, LRc = -1;

	// temporary image to hold the subimage
	ImageType<pType> temp;

	// prompt for image register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for the upper left corner
		promptForLoc( "Upper Left Corner", img[index], ULr, ULc );

		// if back isn't choosen prompt for lower right corner
		if ( ULr != -1 && ULc != -1 )
			promptForLoc( "Lower Right Corner", img[index], LRr, LRc );

		// if invalid re-prompt for corners
		while ( ( LRr <= ULr || LRc <= ULc ) &&
				ULr != -1 && ULc != -1 && LRr != -1 && LRc != -1 )
		{	
			// display message
			messageBox( "Bad Coordinates",
					"Lower Right Corner is Left or Above Upper Left" );

			promptForLoc( "Upper Left Corner", img[index], ULr, ULc );

			if ( ULr != -1 && ULc != -1 )
				promptForLoc( "Lower Right Corner", img[index], LRr, LRc );
		}

		// if corners are good and back wasn't choosen
		if ( ULr != -1 && ULc != -1 && LRr != -1 && LRc != -1 )
		{
			// extract sub image
			temp.getSubImage( ULr, ULc, LRr, LRc, img[index] );

			// set old image equal to subimage
			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
}

/******************************************************************************\
 This function prompts the user for a scale value to enlarge an image by, it
 makes sure the scale value does not make the image larger than MAX_IMG value
 because it may cause a stack overflow.
\******************************************************************************/
template <class pType>
void enlargeImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds image info and maxS value
	int index, N, M, Q, maxS;

	// scale value to be choosen by user
	int s;

	// temporary image used to store enlarged image
	ImageType<pType> temp;

	// prompt user for register value
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// get the basic image info used to determine maxS
		img[index].getImageInfo( N, M, Q );

		// calculate maxS
		maxS = (N > M ? MAX_IMG/N : MAX_IMG/M);

		// prompt for enlarge factor
		s = promptForScaleValue( "Enlarge Image By Factor", 
				"Enter enlargement multiplier(-1 to cancel): ", maxS );

		// if back isn't choosen
		if ( s != -1 )
		{
			// enlarge image by factor s
			temp.enlargeImage( s, img[index] );

			// set register image to the values of temp
			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
}

/******************************************************************************\
 The same as enlarge except it shrinks the image making sure it never gets
 smaller than MIN_IMG.  This is because some image viewers won't open images
 as small as 2x2 (xv for example)
\******************************************************************************/
template <class pType>
void shrinkImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// image info and max s value
	int index, N, M, Q, maxS;

	// scale factor to be reduced by
	int s;

	// holds the reduced image before transfering it to img[index]
	ImageType<pType> temp;

	// prompt for the register to be used
	index = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index != BAD_REG )
	{
		// get image info for calculating maxS
		img[index].getImageInfo( N, M, Q );

		// calculate maxS
		maxS = (N > M ? N/MIN_IMG : M/MIN_IMG);

		// prompt for the scale value
		s = promptForScaleValue( "Shrink Image By Factor",
				"Enter reduction factor(-1 to cancel): ", maxS );

		// if quit isn't choosen
		if ( s != -1 )
		{
			// shrink the image
			temp.shrinkImage( s, img[index] );

			// store back in the register image
			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
}

/******************************************************************************\
 Prompt user for a direction to reflect an image then reflect the image and
 store it back in the original register image.
\******************************************************************************/
template <class pType>
void reflectImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds the index of the register
	int index;

	// users direction choice
	char dir;

	// used to reflect the image before saving to image
	ImageType<pType> temp;

	// prompt for which image to use
	index = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for a valid reflect direction
		dir = promptForMirror("Reflect Image", 
				"Enter mirror direction (H(oriz), V(ert), C(ancel)): ");

		// if cancel isn't choosen
		if ( dir != 'c' && dir != 'C' )
		{
			// reflect horizontally
			if ( dir == 'h' || dir == 'H' ) // horizontal
				temp.reflectImage( false, img[index] );
			else                            // vertical
				temp.reflectImage( true, img[index] );

			// copy temp back to the register image
			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
}

/******************************************************************************\
 This prompts the user for how far to translate the image, then calls the
 translate function which moves the image down to the right 't' number of
 pixels.  Also Won't let user choose t value that would move image totaly off
 the screen.
\******************************************************************************/
template <class pType>
void translateImg(ImageType<pType> img[], bool loaded[], char name[][NAME_LEN])
{
	// holds the image info and maximum t value
	int index, N, M, Q, maxT;

	// the translate value to be choosen by user
	int t;

	// temporary image used as a buffer to the register image
	ImageType<pType> temp;

	// get a valid image register
	index = promptForReg( loaded, name );

	// if back isn't choosen
	if ( index != BAD_REG )
	{
		// get the image info used to calculate maxT
		img[index].getImageInfo( N, M, Q );

		// calculate maxT value
		maxT = (N > M ? N-1 : M-1);

		// prompt for a valid T value (uses Pix because both pix or t can
		// be (0-max$)
		t = promptForPixValue( "Translate Image",
				"Enter translation factor(-1 to cancel): ", maxT );

		// if cancel isn't choosen
		if ( t != -1 )
		{
			// translate the image
			temp.translateImage( t, img[index] );

			// copy back to the image register
			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
}

/******************************************************************************\
 This prompts the user for an angle theta which will rotate the image counter 
 clockwise by theta degrees.  The input is only valid from 0 to 360 which
 should cover all possibilities.
\******************************************************************************/
template <class pType>
void rotateImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// holds the register index and angle theta
	int index, theta;

	// temporary image used as a kind of buffer for register image
	ImageType<pType> temp;

	// prompt for a regiseter that is used
	index = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index != BAD_REG )
	{
		// prompt for valid angle
		theta = promptForAngle( "Rotate Image",
				"Rotate counter-clockwise by angle(-1 to cancel):" );

		// if cancel isn't choosen
		if ( theta != -1 )
		{
			// rotate the image by theta degrees clockwise
			temp.rotateImage( theta, img[index] );

			// set image register equal to buffer temporary image
			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
}

/******************************************************************************\
 Prompt for 2 images and attempt to sum them, there is no size checking because
 operator+ will throw a string which will be handeled by main if sizes of the
 two images are different.
\******************************************************************************/
template <class pType>
void sumImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// index of image 1 and 2
	int index1, index2;

	// 2 temporary images used to sum the images
	ImageType<pType> temp1, temp2;

	// prompt for first image
	index1 = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index1 != BAD_REG )
	{
		// set temp1 equal to the first image
		temp1 = img[index1];

		// prompt for second image but offset a little
		index2 = promptForReg( loaded, name, true, 3, MENU_WIDTH+5 );

		// if quit isn't choosen
		if ( index2 != BAD_REG )
		{
			// set temp2 equal to the second image
			temp2 = img[index2];

			// sum the images
			img[index1] = temp1 + temp2;

			// adds modified to register name
			if ( name[index1][strlen(name[index1])-1] != ')' )
				strcat( name[index1], " (modified)" );
		}
	}
}

/******************************************************************************\
 Prompt for 2 images and attempt to calculate the difference, there's no size
 checking here for the same reason sumImg doesn't do size checking
\******************************************************************************/
template <class pType>
void subtractImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// index of image 1 and 2
	int index1, index2;

	// 2 temporary images used to subtract the images
	ImageType<pType> temp1, temp2;

	// prompt for first image
	index1 = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index1 != BAD_REG )
	{
		// set temp1 equal to the first image
		temp1 = img[index1];

		// prompt for second image but offset a little
		index2 = promptForReg( loaded, name, true, 3, MENU_WIDTH+5 );

		// if quit isn't choosen
		if ( index2 != BAD_REG )
		{
			// set temp2 equal to the second image
			temp2 = img[index2];

			// subtract the images
			img[index1] = temp1 - temp2;

			// adds modified to register name
			if ( name[index1][strlen(name[index1])-1] != ')' )
				strcat( name[index1], " (modified)" );
		}
	}
}

/******************************************************************************\
 Prompt user for which image to negate and negate it, pretty simple function.
\******************************************************************************/
template <class pType>
void negateImg( ImageType<pType> img[], bool loaded[], char name[][NAME_LEN] )
{
	// index of register to use
	int index;

	// prompt for register number
	index = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index != BAD_REG )
	{
		// negate image
		img[index].negateImage();

		// adds modified to register name
		if ( name[index][strlen(name[index])-1] != ')' )
			strcat( name[index], " (modified)" );
	}
}

/******************************************************************************\
 Prompt user for which image to count regions then count them
\******************************************************************************/
template <class pType>
void countRegions(ImageType<pType> img[], bool loaded[], char name[][NAME_LEN])
{
	// index of register to use
	int index, regions;

	// holds the temp image

	// prompt for register number
	index = promptForReg( loaded, name );

	// if quit isn't choosen
	if ( index != BAD_REG )
	{
		char msg[NAME_LEN];

		ImageType<pType> temp;

		// count regions
		regions = computeComponents(img[index], temp);

		// set image to the counted image
		img[index] = temp;

		// display number of regions
		sprintf(msg, "The number of regions is %i", regions);
		messageBox( "Number", msg );

		// adds modified to register name
		if ( name[index][strlen(name[index])-1] != ')' )
			strcat( name[index], " (modified)" );
	}
}

/******************************************************************************\
 This is the function that calls the menu for the register prompt, it can be
 called in different locations (like in addImg and subImg) but has a default
 defined by some global constants.  The function creates a list of registers
 and adds the "Back" option as the final option, this way the user has the
 option to cancel choosing a register.  Although in the program it looks like
 the register display and register choosing window are the same, this menu
 overlaps the other menu to make it seem like control is transfering to another
 window.

 The value check is true by default, if it is false it can return registers
 that have not been loaded.  This feature is needed in functions like
 loadImage.
\******************************************************************************/
int promptForReg( bool loaded[], char name[][NAME_LEN], const bool check,
		int y, int x )
{
	// this is the WINDOW that the menu is stored in
	WINDOW *regMenu;

	// val is the index choice for the menu
	int val;

	// set the flag to continue looping until BAD_REG is true or a register
	// that is not set is choosen which check if true
	bool loop = true;

	// this is the array that will hold all the menu choices including "Back"
	char menuVals[REGS+1][NAME_LEN];

	// copy all the register names to the menu array
	for ( int i = 0; i < REGS; i++ )
		strcpy( menuVals[i], name[i] );

	// add exit to the list of commands
	strcpy( menuVals[REGS], "Back" );

	// prompt for the register
	do {
		val = showMenu( regMenu, "Registers", REGWIN_HEIGHT, REGWIN_WIDTH, y, x,
				menuVals, REGS+1 );

		// set the loop flag depending on val
		if ( val == BAD_REG )
			loop = false;
		else if ( ! loaded[val] && check )
			loop = true;
		else
			loop = false;
	} while ( loop );

	// delete the register menu window
	delwin( regMenu );

	// return the users choice
	return val;
}

/******************************************************************************\
 This just builds the window used for message box, this function is just to
 simplify the plethora of other functions that use this.
\******************************************************************************/
void stdWindow( WINDOW *&newWin, const char title[] )
{
	// simply draw the standard msg box window
	drawWindow( newWin, title, MSGBOX_HEIGHT, MSGBOX_WIDTH, 
			screenHeight()/2-MSGBOX_HEIGHT/2, screenWidth()/2-MSGBOX_WIDTH/2 );
}

/******************************************************************************\
 Create a message box and prompt the user for a string value with given prompt
\******************************************************************************/
int promptForFilename( const char title[], const char prompt[], char str[] )
{
	// holds the prompting window
	WINDOW *fileWin;

	// this is how long the filename can be (some arbitrary value right?...no)
	int len = 16;

	// draw the window
	stdWindow( fileWin, title );

	// prompt for the string
	promptForString( fileWin, 1, 2, prompt, str, len );

	// delete the window
	delwin( fileWin );

	// return the length of the string (not really used in this program)
	return strlen( str );
}

/******************************************************************************\
 Prompt user for a valid angle using a message box, make sure input is between
 0 and 360, if not display a message box and then re-prompt.
\******************************************************************************/
int promptForAngle( const char title[], const char prompt[] )
{
	// holds message box window
	WINDOW *pixWin;

	// user input value
	int val;

	// draw message window
	stdWindow( pixWin, title );

	// get user input
	val = promptForInt( pixWin, 1, 2, prompt );

	// check for valid input
	while ( val < -1 || val > 360 )
	{
		// display error
		messageBox( "Invalid Angle", "Please input an angle (0-360)" );

		// redraw window
		delwin( pixWin );
		stdWindow( pixWin, title );

		// re-prompt user
		val = promptForInt( pixWin, 1, 2, prompt );
	}

	// de-allocate window object
	delwin( pixWin );

	// return users choice
	return val;
}

/******************************************************************************\
 Prompt for a pixel value which is from 0 to maxVal, if not display message
 box and re-prompt user until valid choice is made.
\******************************************************************************/
int promptForPixValue( const char title[], const char prompt[], int maxVal )
{
	// message box window
	WINDOW *pixWin;

	// used in the error message
	char msg[NAME_LEN];

	// user input value
	int val;

	// draw message window
	stdWindow( pixWin, title );

	// get user input
	val = promptForInt( pixWin, 1, 2, prompt );

	// check for valid input
	while ( val < -1 || val > maxVal )
	{
		// display error
		sprintf( msg, "Please input a value (0-%i)", maxVal );
		messageBox( "Invalid Value", msg );

		// redraw window
		delwin( pixWin );
		stdWindow( pixWin, title );

		// re-prompt user
		val = promptForInt( pixWin, 1, 2, prompt );
	}

	// delete this dynamic memory
	delwin( pixWin );

	// return value to calling function
	return val;
}

/******************************************************************************\
 Prompt the user for the characters h, v, or c (not case sensitive) and return
 the value as soon as one of the 3 is pressed.
\******************************************************************************/
char promptForMirror( const char title[], const char prompt[] )
{
	// holds the prompting window
	WINDOW *pixWin;

	// holds user input
	char val;

	// draw message window
	stdWindow( pixWin, title );

	// display prompt message
	mvwaddstr( pixWin, 1, 2, prompt );

	//dont continue untill valid key is pressed
	do {
		// prompt user for character
		val = wgetch( pixWin );
	} while ( val != 'h' && val != 'H' &&
			val != 'v' && val != 'V' &&
			val != 'c' && val != 'C' );

	// delete dynamically allocated window
	delwin( pixWin );

	// return the users input
	return val;
}

/******************************************************************************\
 This function prompts the user for a scale value and checks to make sure it
 is not greater than maxVal and not less than 2.  This is used in the enlarge
 and shrink functions.
\******************************************************************************/
int promptForScaleValue( const char title[], const char prompt[], int maxVal )
{
	// points to the WINDOW that is our prompting window
	WINDOW *pixWin;

	// holds the error message (which needs some formating)
	char msg[NAME_LEN];

	// the users input value
	int val;

	// draw message window
	stdWindow( pixWin, title );

	// prompt user for an integer value
	val = promptForInt( pixWin, 1, 2, prompt );

	// if value is not valid display error message and re-prompt
	while ( val != -1 && ( val < 2 || val > maxVal ) )
	{
		// display error
		sprintf( msg, "Please input a value (2-%i)", maxVal );
		messageBox( "Invalid Value", msg );

		// redraw window
		delwin( pixWin );
		stdWindow( pixWin, title );

		// re-prompt user
		val = promptForInt( pixWin, 1, 2, prompt );
	}

	// delete dynamically allocated window
	delwin( pixWin );

	// return the user's input value
	return val;
}

/******************************************************************************\
 This function prompts the user for a location (both row and column) and sets
 the valid points equal to row or col.  If -1 is returned in either location
 it means user choose to cancel the prompt.  The validity of the points is
 calculated by the image object it is passed.  The image properties are
 calculated and then used to determine the bounds of row and column.
\******************************************************************************/
template <class pType>
void promptForLoc( const char title[], ImageType<pType>& img, int& row, int& col )
{
	// holds various image info
	int N, M, Q;

	// holds the error messages
	char msg[NAME_LEN];

	// this is the WINDOW pointer that points to the prompting window
	WINDOW *pixWin;

	// set default values for row and column it exits early
	row = -1;
	col = -1;

	// retrieve image info
	img.getImageInfo( N, M, Q );

	// draw message window
	stdWindow( pixWin, title );

	// gets user input for row
	row = promptForInt( pixWin, 1, 2, "Enter pixel row(-1 to cancel): " );

	// if row input is not valid display error and re-prompt
	while ( row < -1 || row >= N )
	{
		// show message box
		sprintf( msg, "Invalid Row, must be (0-%i)", N-1 );
		messageBox( "Invalid Row", msg );

		// redraw the window
		delwin( pixWin );
		stdWindow( pixWin, title );

		// re-prompt user
		row = promptForInt( pixWin, 1, 2, "Enter pixel row(-1 to cancel): " );
	}

	// if user didn't choose to cancel
	if ( row != -1 )
	{
		// prompt for column
		col = promptForInt( pixWin, 2, 2,
				"Enter pixel column(-1 to cancel): " );

		// if column input is not valid, display error and re-prompt
		while ( col < -1 || col >= M )
		{
			// show message box warning
			sprintf( msg, "Invalid Column, must be (0-%i)", M-1 );
			messageBox( "Invalid Column", msg );

			// redraw the window
			delwin( pixWin );
			stdWindow( pixWin, title );

			// reprint the upper line
			mvwprintw( pixWin, 1, 2, "Enter pixel row(-1 to cancel): %i",
					row );

			// re-prompt user
			col = promptForInt( pixWin, 2, 2,
					"Enter pixel column(-1 to cancel): " );
		}
	}

	// at this point row and column should be set or should be -1 (cancel)

	// de-allocate memory for WINDOW object
	delwin( pixWin );
}

/******************************************************************************\
 This displays a simple message box to the screen with the given title and msg
 inside of it, it waits for the user to press RETURN before returning to
 calling function
\******************************************************************************/
void messageBox( const char title[], const char msg[] )
{
	// message box window
	WINDOW *msgBox;

	// user
	int input;

	// draw/initialize window
	stdWindow( msgBox, title );

	// add msg value to window
	mvwaddstr( msgBox, 1, 2, msg );

	// wait for return to be pressed
	while ( wgetch( msgBox ) != KEY_RETURN );

	// delete message box window
	delwin( msgBox );
}

/******************************************************************************\
 Couldn't find a good place to put this function, its a not so robust function
 that reads all the .pgm files from a local directory (defined as a constant)
 and places them into a dynamically allocated c style string array.

 !!!Note this function allocates memory for a 2D array and returns the number
 of rows.  This information is REQUIRED to properly de-allocate the memory in
 the calling function.
\******************************************************************************/
int findLocalPGM( char **&filenames )
{
	// namelist holds a list of file names
	struct dirent **namelist;

	// n is the number of files total in the local directory
	int n;

	// holds the length of various strings
	int len;

	// count keeps track of the number of .pgm files found
	int count = 0;

	// store the string values of the local files into namelist in alpha order
	n = scandir( IMAGELOC, &namelist, 0, alphasort );

	// if there are files...
	if ( n > 0 )
	{
		// go through checking for files ending in ".pgm"
		for ( int i = 0; i < n; i++ )
		{
			len = strlen( namelist[i]->d_name );
			if ( len > 5 )
				// compare the last four characters to ".pgm"
				if ( strcmp( ".pgm", &(namelist[i]->d_name[len-4]) ) == 0 )
					// increase count
					count++;
		}

		// if any .pgm files are found
		if ( count > 0 )
		{
			// allocate space for each one
			filenames = new char*[count];

			// j is used as a counter for the filenames
			int j = 0;

			// this loop does the same as the previous except it allocates
			// memory and copies names of .pgm files to filenames
			for ( int i = 0; i < n; i++ )
			{
				// get the length of the filename
				len = strlen( namelist[i]->d_name );
				// compare the suffix to ".pgm" again
				if ( len > 5 )
					if ( strcmp( ".pgm", &(namelist[i]->d_name[len-4]) ) == 0 )
					{
						// this time allocate memory to store the name
						filenames[j] = new char[strlen(namelist[i]->d_name) +
							1 + strlen(IMAGELOC)];
						// store the name with the file path added
						sprintf( filenames[j], "%s%s", IMAGELOC,
								namelist[i]->d_name );
						// increase counter
						j++;
					}
				// de-allocate name list
				delete [] namelist[i];
			}
			// finish de-allocating name list
			delete [] namelist;
		}
	}

	// return the number of rows in filenames
	return count;
}

/******************************************************************************\
 Couldn't find a good place to put this function, its a not so robust function
 that reads all the .pgm files from a local directory (defined as a constant)
 and places them into a dynamically allocated c style string array.

 !!!Note this function allocates memory for a 2D array and returns the number
 of rows.  This information is REQUIRED to properly de-allocate the memory in
 the calling function.
\******************************************************************************/
int findLocalPPM( char **&filenames )
{
	// namelist holds a list of file names
	struct dirent **namelist;

	// n is the number of files total in the local directory
	int n;

	// holds the length of various strings
	int len;

	// count keeps track of the number of .pgm files found
	int count = 0;

	// store the string values of the local files into namelist in alpha order
	n = scandir( IMAGELOC, &namelist, 0, alphasort );

	// if there are files...
	if ( n > 0 )
	{
		// go through checking for files ending in ".pgm"
		for ( int i = 0; i < n; i++ )
		{
			len = strlen( namelist[i]->d_name );
			if ( len > 5 )
				// compare the last four characters to ".pgm"
				if ( strcmp( ".ppm", &(namelist[i]->d_name[len-4]) ) == 0 )
					// increase count
					count++;
		}

		// if any .pgm files are found
		if ( count > 0 )
		{
			// allocate space for each one
			filenames = new char*[count];

			// j is used as a counter for the filenames
			int j = 0;

			// this loop does the same as the previous except it allocates
			// memory and copies names of .pgm files to filenames
			for ( int i = 0; i < n; i++ )
			{
				// get the length of the filename
				len = strlen( namelist[i]->d_name );
				// compare the suffix to ".pgm" again
				if ( len > 5 )
					if ( strcmp( ".ppm", &(namelist[i]->d_name[len-4]) ) == 0 )
					{
						// this time allocate memory to store the name
						filenames[j] = new char[strlen(namelist[i]->d_name) +
							1 + strlen(IMAGELOC)];
						// store the name with the file path added
						sprintf( filenames[j], "%s%s", IMAGELOC,
								namelist[i]->d_name );
						// increase counter
						j++;
					}
				// de-allocate name list
				delete [] namelist[i];
			}
			// finish de-allocating name list
			delete [] namelist;
		}
	}

	// return the number of rows in filenames
	return count;
}

/******************************************************************************\
 Labels each region in the image with a different color.
\******************************************************************************/
template <class pType>
int computeComponents( ImageType<pType> input, ImageType<pType>& output )
{
	// holds the loop values and the regions
	int N, M, Q, regions = 0;
	int labelval=0;

	// label value
	pType lbl;

	// temp image used to count regions initialialy
	ImageType<pType> temp;

	// retrieve image info (rows, columns, color depth)
	input.getImageInfo(N, M, Q);

	// set the output image equal to input image to start
	output = input;

	// run threshold
	output.threshold();

	// dilate then erode image
	output.dilate();
	output.erode();

	// This only changes a temporary image used to count the regions to divide
	// label values evenly
	temp = output;

	//long t;
	//t = clock();

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			if ( temp.getPixelVal(i, j) == Q )    // pixel is white
			{
				regions++;			// count regions
				lbl = Q/2;

				ImageType<pType> temp2;

				//findComponentsDFS(temp, temp, i, j, lbl);
				findComponentsBFS(temp, temp, i, j, lbl);
				//findComponentsRec(temp, temp, i, j, lbl);

			}

	//t = clock() - t;

	//char tmp[50];
	//sprintf(tmp, "Time(seconds): %g", (double)t / CLOCKS_PER_SEC);
	//messageBox("time", tmp);

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < M; j++ )
			if ( output.getPixelVal(i, j) == Q )    // pixel is white
			{
				labelval++;
				// will split label values evenly from 2% of Q to 98% of Q
				lbl = labelval*(Q-(Q*0.02))/regions;
				findComponentsDFS(output, output, i, j, lbl);
			}


	// return number of regions
	return regions;
}

/******************************************************************************\
 Depth first search, fills regions starting at the deepest point and working
 back (uses a stack)
\******************************************************************************/
template <class pType>
void findComponentsDFS(ImageType<pType> inputImg, ImageType<pType>& outputImg,
		int startRow, int startCol, pType label)
{
	// used to hold limits for the loop
	int N, M, Q;

	// set up N, M, and Q
	outputImg.getImageInfo(N, M, Q);

	// declare the stack of locations
	stack<location> stk;

	// a couple temporary location types to hold information
	location loc, loc2;

	// push the first location to the stack
	loc.r = startRow;
	loc.c = startCol;

	stk.push(loc);

	// temporary pixel value used as placeholder value
	pType negOne = -1;

	while (!stk.empty())
	{
		// hold the value on top of the stack
		loc = stk.top();

		// pop the top of the stack off
		stk.pop();

		// set value at location to label value
		outputImg.setPixelVal(loc.r, loc.c, label);

		for ( int i = loc.r-1; i <= loc.r+1; i++ )
			for ( int j = loc.c-1; j <= loc.c+1; j++ )
				if ( i >= 0 && i < N && j >= 0 && j < M )
					if ( inputImg.getPixelVal(i, j) == Q
							&& outputImg.getPixelVal(i, j) == Q )
					{
						// "touch" the location on output
						outputImg.setPixelVal(i, j, negOne);

						// push to stack because location is "untouched"
						loc2.r = i;
						loc2.c = j;
						stk.push( loc2 );
					}
	}
}

/******************************************************************************\
 Breadth first search. searches for regions using enslaved gnomes
\******************************************************************************/
template <class pType>
void findComponentsBFS(ImageType<pType> inputImg, ImageType<pType>& outputImg,
		int startRow, int startCol, pType label)
{

	// used to hold limits for the loop
	int N, M, Q;

	// set up N, M, and Q
	outputImg.getImageInfo(N, M, Q);

	// declare the queue of locations
	queue<location> que;

	// a couple temporary location types to hold information
	location loc, loc2;

	// push the first location to the queue
	loc.r = startRow;
	loc.c = startCol;

	que.push(loc);

	// temporary pixel value used as placeholder value
	pType negOne = -1;

	while (!que.empty())
	{
		// hold the value on top of the queue
		loc = que.front();

		// pop the top of the queue off
		que.pop();

		// set value at location to label value
		outputImg.setPixelVal(loc.r, loc.c, label);

		for ( int i = loc.r-1; i <= loc.r+1; i++ )
			for ( int j = loc.c-1; j <= loc.c+1; j++ )
				if ( i >= 0 && i < N && j >= 0 && j < M )
					if ( inputImg.getPixelVal(i, j) == Q
							&& outputImg.getPixelVal(i, j) == Q )
					{
						// "touch" the location on output
						outputImg.setPixelVal(i, j, negOne);

						// push to queue because location is "untouched"
						loc2.r = i;
						loc2.c = j;
						que.push( loc2 );
					}
	}
}

/******************************************************************************\
 Depth first RECURSIVE search. This recursively floods the current region with
 the value of label.
\******************************************************************************/
template <class pType>
void findComponentsRec(const ImageType<pType>& inputImg,
		ImageType<pType>& outputImg, int startRow, int startCol, pType label)
{
	int N, M, Q;

	inputImg.getImageInfo(N, M, Q);

	// test for valid location
	if ( startRow >= 0 && startCol >= 0 && startRow < N && startCol < M )
		if ( outputImg.getPixelVal(startRow, startCol) != label &&
				outputImg.getPixelVal(startRow, startCol) != 0 )
		{
			// set pixel value
			outputImg.setPixelVal(startRow, startCol, label);

			// recursive calls
			findComponentsRec(inputImg, outputImg, startRow-1, startCol-1,
					label);
			findComponentsRec(inputImg, outputImg, startRow-1, startCol,
					label);
			findComponentsRec(inputImg, outputImg, startRow-1, startCol+1,
					label);
			findComponentsRec(inputImg, outputImg, startRow, startCol-1,
					label);
			findComponentsRec(inputImg, outputImg, startRow, startCol+1,
					label);
			findComponentsRec(inputImg, outputImg, startRow+1, startCol-1,
					label);
			findComponentsRec(inputImg, outputImg, startRow+1, startCol,
					label);
			findComponentsRec(inputImg, outputImg, startRow+1, startCol+1,
					label);
		}
}

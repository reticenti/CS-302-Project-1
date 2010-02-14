#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <cstring>
#include "comp_curses.h"
#include "image.h"
#include "imageIO.h"

using namespace std;

// CONSTANTS
	const bool CUBIC_INTER = true;

	const int REGS = 5;	   		// values 1-9
	const int BAD_REG = REGS;	// dont change this
	const int NAME_LEN = 50;   	// the max string length of names
	const int MSG_LEN = 100;   	// the max string length of messages

	const int MAX_IMG = 10000; 	// the max size you can enlarge to
	const int MIN_IMG = 4;     	// the min size you can reduce to

	const short BG_COLOR = COLOR_BLACK;
	const short WINDOW_COLOR = COLOR_BLUE;
	const short FG_COLOR = COLOR_RED;

	const short MENU_BACKGROUND = COLOR_BLUE;
	const short MENU_FOREGROUND = COLOR_RED;

	const bool BRIGHT = true;
	const int MENU_OPTIONS = 16;

// FUNCTION PROTOTYPES
	// shows menu, parameters in order, height, width, locY, locX
	// make height an odd number for more ballanced windows
	int showMenu( WINDOW *&, const char[], int, int, int, int, char[][NAME_LEN], int );
	int showMenu( WINDOW *&, const char[], int, int, int, int, char*[], int );
	void showRegs( WINDOW *&, const bool[], const char[][NAME_LEN] );

	// this function clears all windows, redraw any you want after this is called
	void deleteMenu( WINDOW *& );
	void deleteWindow( WINDOW *& );

	// processes the choice returned by showMenu
	void processEntry( ImageType[], bool[], char[][NAME_LEN], int );

	// verify input for common prompts
	int promptForReg( bool[], char[][NAME_LEN], const bool = true );
	int promptForFilename( const char[], const char[], char*& );
	void promptForLoc( const char[], ImageType&, int&, int& );
	int promptForPixValue( const char[], const char[], int );

	// fills registers based on parameters
	void fillRegs( ImageType[], bool[], char[][NAME_LEN], int, char** );

	// prompts user for a register to clear
	void clearRegister( ImageType[], bool[], char[][NAME_LEN] );

	// image manip drivers 
	void loadImage( ImageType[], bool[], char[][NAME_LEN] );
	void saveImage( ImageType[], bool[], char[][NAME_LEN] );
	void getImageInfo( ImageType[], bool[], char[][NAME_LEN] );
	void setPixel( ImageType[], bool[], char[][NAME_LEN] );
	void getPixel( ImageType[], bool[], char[][NAME_LEN] );
	void extractSub( ImageType[], bool[], char[][NAME_LEN] );
	void enlargeImg( ImageType[], bool[], char[][NAME_LEN] );
	void shrinkImg( ImageType[], bool[], char[][NAME_LEN] );
	void reflectImg( ImageType[], bool[], char[][NAME_LEN] );
	void translateImg( ImageType[], bool[], char[][NAME_LEN] );
	void rotateImg( ImageType[], bool[], char[][NAME_LEN] );
	void sumImg( ImageType[], bool[], char[][NAME_LEN] );
	void subtractImg( ImageType[], bool[], char[][NAME_LEN] );
	void negateImg( ImageType[], bool[], char[][NAME_LEN] );

	void drawWindow( WINDOW *&, const char[], int, int, int, int, short=WINDOW_COLOR, short=FG_COLOR );
	int findLocalPGM( char **&filenames );

// MAIN PROGRAM

int main( int argc, char **argv )
{
	// main menu object
	WINDOW *menu;
	WINDOW *regWin;
	int choice;

	// holds the name of the image stored in the register
	char imgName[REGS][NAME_LEN];

	// bool array indicating if registers are loaded with images
	bool imgLoaded[REGS];

	// this is where all the registers are stored
	ImageType image[REGS];

	// create main menu strings
	char choices[MENU_OPTIONS][NAME_LEN] = {
		"  Read an image from a file",
		"  Save an image to a file",
		"  Get image info",
		"  Set the value of a pixel",
		"  Get the value of a pixel",
		"  Extract a subimage from an imagei",
		"  Enlarge image",
		"  Shrink image",
		"  Reflect image",
		"  Translate image",
		"  Rotate image",
		"  Sum two images",
		"  Subtract two images",
		"  Compute negative of an image",
		"  Clear a register",
		"  Exit" };

	// start
	startCurses();

	hideCursor();

	setColor( FG_COLOR, BG_COLOR );

	// initialize the bool array
	for ( int i = 0; i < REGS; i++ )
		imgLoaded[i] = false;

	// read argument parameters
	fillRegs( image, imgLoaded, imgName, argc, argv );

	do {
		// display register window
		showRegs( regWin, imgLoaded, imgName );

		// show and get input from menu
		choice = showMenu( menu, "Main Menu", 3+2*MENU_OPTIONS, 40, 1, 1, choices, MENU_OPTIONS );

		try
		{
			// this is the main driving function that calls all others
			processEntry( image, imgLoaded, imgName, choice );
		}
		catch( ... )
		{
			// handle errors like this later
			return -1;
		}

		// makes sure everything is reset
		deleteMenu( menu );
		deleteWindow( regWin );
	} while ( choice != MENU_OPTIONS-1 );

	// end curses
	endCurses();

	return 0;
}

// Supporting functions

// this makes it possible to pass static and dynamic 2d arrays to the menu
int showMenu( WINDOW *& menu, const char title[], int height, int width, int locY, int locX, char menuStr[][NAME_LEN], int choices )
{
	char **menuStrPtr = new char*[choices];
	int retVal;

	for ( int i = 0; i < choices; i++ )
	{
		menuStrPtr[i] = new char[NAME_LEN];
		strcpy( menuStrPtr[i], menuStr[i] );
	}
	retVal = showMenu( menu, title, height, width, locY, locX, menuStrPtr, choices );

	for ( int i = 0; i < choices; i++ )
		delete [] menuStrPtr[i];
	delete [] menuStrPtr;

	return retVal;
}


int showMenu( WINDOW *& menu, const char title[], int height, int width, int locY, int locX, char *menuStr[], int choices )
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
			mvwprintw( menu, y, x, formatStr, menuStr[choiceLoc - menuLoc + i] );
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

void showRegs( WINDOW *& regWin, const bool loaded[], const char names[][NAME_LEN] )
{
	drawWindow( regWin, "Registers", REGS+4, 40, 1, 43 );

	// add the register names to the window
	for ( int i = 0; i < REGS; i++ )
		mvwprintw( regWin, i+2, 2, "%.38s", names[i] );
	
	wrefresh( regWin );
}

void deleteMenu( WINDOW *& menu )
{
	delwin( menu );

	touchwin( stdscr );
	refresh();
}

void deleteWindow( WINDOW *& win )
{
	delwin( win );

	touchwin( stdscr );
	refresh();
}

void drawWindow( WINDOW *& win, const char title[], int height, int width, int y, int x, short bgColor, short fgColor )
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

void processEntry( ImageType img[], bool loaded[], char name[][NAME_LEN], int choice )
{
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
//			enlargeImg( img, loaded, name );
			break;
		case 7:	// shrink
//			shrinkImg( img, loaded, name );
			break;
		case 8:	// reflect
//			reflectImg( img, loaded, name );
			break;
		case 9:	// translate
//			translateImg( img, loaded, name );
			break;
		case 10:	// rotate
//			rotateImg( img, loaded, name );
			break;
		case 11:	// sum
//			sumImg( img, loaded, name );
			break;
		case 12:	// difference
//			subtractImg( img, loaded, name );
			break;
		case 13:	// negative
//			negateImg( img, loaded, name );
			break;
		case 14:	// clear register
			clearRegister( img, loaded, name );
			break;
		case 15:	// exit
			// do nothing lol ^_^ maybe later add an exit screen
			break;
	}
}

void clearRegister( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	index = promptForReg( loaded, name );
	if ( index != BAD_REG )
	{
		loaded[index] = false;
		sprintf( name[index], "Register %i: Empty", index+1 );
	}
}

void fillRegs( ImageType img[], bool loaded[], char name[][NAME_LEN], int argc, char **argv )
{
	char *msg = new char[1+(argc-1)*40];
	int i, j, k;
	bool f;
	
	// initalize string
	msg[0] = '\0';

	for ( int index = 0; index < REGS; index++ )
		sprintf( name[index], "Register %i: Empty", index+1 );

	for ( int index = 1; index < argc && index <= REGS; index++ )
	{
		try
		{
			readImageHeader( argv[index], i, j, k, f );
			img[index-1].setImageInfo( i, j, k );

			readImage( argv[index], img[index-1] );

			sprintf( name[index-1], "Register %i: %s", index, argv[index] );

			loaded[index-1] = true;
		}
		catch ( string s )
		{
			strcat( msg, s.c_str() );
			strcat( msg, "\n" );
		}
	}

	if ( strlen(msg) > 0 )
	{
		setColor( COLOR_WHITE, COLOR_BLACK );

		mvaddstr( 0, 0, msg );
		getch();
		for ( int x = 0; x < screenWidth(); x++ )
			for ( int y = 0; y < screenHeight(); y++ )
				mvaddch( y, x, ' ' );
		refresh();
	}
	
	// de-allocate memory for message
	delete [] msg;
}

void loadImage( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	char strInput[NAME_LEN];
	char **fileNames, **menuChoices;
	WINDOW *fileMenu;
	int i, j, k, files, index, imageVal;
	bool f;
	
	// get a list of local files dynamically allocated
	files = findLocalPGM( fileNames );

	// add one more option to the menu
	menuChoices = new char*[files+1];

	// copy all the pointers from the filenames to the menuChoices
	for ( int a = 0; a < files; a++ )
		menuChoices[a] = fileNames[a];
	
	// delete the list of string pointers since menuChoices has them now
	delete [] fileNames;

	// create the final choice
	menuChoices[files] = new char[5];

	// make the final choice "Exit"
	strcpy( menuChoices[files], "Exit" );

	// prompt for a register (false indicated it doesn't need to be full)
	index = promptForReg( loaded, name, false );

	// if exit wasn't choosen then prompt for a file
	if ( index != BAD_REG )
	{	
		// prompt for file
		imageVal = showMenu( fileMenu, "Load Image", 15, 30, 17, 43, menuChoices, files+1 );

		// if exit isn't choosen attempt to load image
		if ( imageVal != files )
		{
			readImageHeader( menuChoices[imageVal], i, j, k, f );
			img[index].setImageInfo( i, j, k );
	
			readImage( menuChoices[imageVal], img[index] );
	
			loaded[index] = true;

			sprintf( name[index], "Register %i: %s", index+1, menuChoices[imageVal] );
		}

		// de-allocate fileMenu
		delwin( fileMenu );
	}

	// de-allocate list of menuChoices
	for ( int a = 0; a < files+1; a++ )
		delete [] menuChoices[a];
	delete [] menuChoices;

}

void saveImage( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	char *strInput;
	int index, x, y;

	index = promptForReg( loaded, name );
	
	if ( index != BAD_REG )
	{
		promptForFilename( "Save Image", "Enter filename: ", strInput );

		writeImage( strInput, img[index] );

		sprintf( name[index], "Register %i: %s", index+1, strInput );

		delete [] strInput;
	}
}

void getImageInfo( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int N, M, Q, index, x, y;
	WINDOW *infoWin;

	index = promptForReg( loaded, name );
	
	if ( index != BAD_REG )
	{
		img[index].getImageInfo( N, M, Q );

		drawWindow( infoWin, name[index], 14, 40, screenHeight()/2-7, screenWidth()/2-20 );
	
		x = 2;
		y = 2;
		
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Saved in Register", ':', index+1 );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Image Width(pixels)", ':', M );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Image Height(pixels)", ':', N );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %i", "Color Depth", ':', Q );
		y+=2;
		mvwprintw( infoWin, y, x, "%-20s%c %.2f", "Mean Gray Value", ':', img[index].meanGray() );
		y+=2;
		mvwprintw( infoWin, y, x, "Press any key to continue..." );

		wrefresh( infoWin );
		
		// wait for input
		wgetch( infoWin );

		delwin( infoWin );
	}
}

void setPixel( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, row, col, val, x, y;
	int N, M, Q;
	string msg;

	index = promptForReg( loaded, name );

	if ( index != BAD_REG )
	{
		promptForLoc( "Set Pixel Value", img[index], row, col );

		if ( row != -1 && col != -1 )
		{
			img[index].getImageInfo( N, M, Q );
			
			promptForPixValue( "Set Pixel Value", "Enter new pixel value(-1 to cancel): ", Q );

			if ( val != -1 )
			{
				img[index].setPixelVal( row, col, val );
				if ( name[index][strlen(name[index])-1] != ')' )
					strcat( name[index], " (modified)" );
			}
		}
	}
}

void getPixel( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, row = -1, col = -1, val, x, y;
	int N, M, Q;
	WINDOW *infoWin;

	index = promptForReg( loaded, name );

	if ( index != BAD_REG )
	{
		promptForLoc( "Get Pixel Value", img[index], row, col );

		if ( row != -1 && col != -1 )
		{
			img[index].getImageInfo( N, M, Q );
			
			drawWindow( infoWin, "Get Pixel Value", 4, 60, screenHeight()/2-3, screenWidth()/2-30 );
			mvwprintw( infoWin, 1, 2, "The pixel Value at (%i,%i) is %i", 
				col, row, img[index].getPixelVal(row, col) );

			wgetch( infoWin );
			
			delwin( infoWin );
			
		}
	}
}

void extractSub( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, x, y;
	int ULr, ULc, LRr = -1, LRc = -1;
	ImageType temp;

	index = promptForReg( loaded, name );

	if ( index != BAD_REG )
	{
		promptForLoc( "Upper Left Corner", img[index], ULr, ULc );

		if ( ULr != -1 && ULc != -1 )
			promptForLoc( "Lower Right Corner", img[index], LRr, LRc );

		while ( ( LRr <= ULr || LRc <= ULc ) &&
			 ULr != -1 && ULc != -1 && LRr != -1 && LRc != -1 )
		{	
			promptForLoc( "Upper Left Corner", img[index], ULr, ULc );

			if ( ULr != -1 && ULc != -1 )
				promptForLoc( "Lower Right Corner", img[index], LRr, LRc );
		}
		
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
/*
void enlargeImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, x, y;
	int N, M, Q;
	int maxS;
	string msg;
	double s;

	ImageType temp;

	index = promptForReg( "Enlarge Image", loaded );

	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );

		maxS = (N > M ? MAX_IMG/N : MAX_IMG/M);

		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Enlarge Image by factor", x, y, 60, 4 );
		x+=2;
		y+=2;

		s = promptForDoubleAt( x, y, "Enter enlargement multiplier(-1 to cancel): " );

		while ( ( s <= 0 || s > maxS ) && s != -1 )
		{
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Enlarge Image by factor", x, y, 60, 4 );
			x+=2;
			y++;
			
			msg = "Invalid size, factor must be 2-";
			msg += intToString(maxS);
			msg += " for this image";

			printStringAt( x, y, msg, "LEFT" );
			y+=2;
			printStringAt( x, y, "Press any key to continue...", "LEFT" );
			refresh();	

			waitForInput( FIXED_WAIT );

			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Enlarge Image by factor", x, y, 60, 4 );
			x+=2;
			y+=2;

			s = promptForDoubleAt( x, y, "Enter enlargement multiplier(-1 to cancel): " );
		}

		if ( s > -1 )
		{
			temp.enlargeImage( s, img[index], CUBIC_INTER );

			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
	refresh();	
}

void shrinkImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int N, M, Q, s;
	int maxS, x, y;
	string msg;

	ImageType temp;

	index = promptForReg( "Shrink Image", loaded );

	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );

		maxS = (N > M ? N/MIN_IMG : M/MIN_IMG);

		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Reduce Image by factor", x, y, 60, 4 );
		x+=2;
		y+=2;

		s = promptForIntAt( x, y, "Enter reduction factor(-1 to cancel): " );
		
		while ( ( s < 2 || s > maxS ) && s != -1 )
		{
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Shrink Image by factor", x, y, 60, 4 );
			x+=2;
			y++;
			
			msg = "Invalid size, factor must be 2-";
			msg += intToString(maxS);
			msg += " for this image";

			printStringAt( x, y, msg, "LEFT" );
			y+=2;
			printStringAt( x, y, "Press any key to continue...", "LEFT" );

			refresh();	
			waitForInput( FIXED_WAIT );

			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Shrink Image by factor", x, y, 60, 4 );
			x+=2;
			y+=2;

			s = promptForIntAt( x, y, "Enter reduction factor(-1 to cancel): " );
		}

		if ( s > -1 )
		{
			temp.shrinkImage( s, img[index] );

			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
	refresh();	
}

void reflectImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, x, y;
	char dir;
	ImageType temp;

	index = promptForReg( "Reflect Image", loaded );

	if ( index > -1 )
	{
		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Reflect Image", x, y, 60, 4 );
		x+=2;
		y+=2;

		printStringAt(x, y, "Enter mirror direction (H(oriz), V(ert), C(ancel)): ", "LEFT");
		
		do {
			refresh();	
			dir = waitForInput( FIXED_WAIT );
		} while ( dir != 'h' && dir != 'H' &&
		          dir != 'v' && dir != 'V' &&
		          dir != 'c' && dir != 'C' );

		if ( dir != 'c' && dir != 'C' )
		{
			if ( dir == 'h' || dir == 'H' ) // horizontal
				temp.reflectImage( false, img[index] );
			else                            // vertical
				temp.reflectImage( true, img[index] );

			img[index] = temp;

			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
	refresh();	
}

void translateImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, x, y;
	int N, M, Q, t;
	int maxT;
	string msg;

	ImageType temp;

	index = promptForReg( "Translate Image", loaded );

	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );

		maxT = (N > M ? N-1 : M-1);

		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Translate Image", x, y, 60, 4 );
		x+=2;
		y+=2;

		t = promptForIntAt( x, y, "Enter translation factor(-1 to cancel): " );
		
		while ( ( t < 1 || t > maxT ) && t != -1 )
		{
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Translate Image", x, y, 60, 4 );
			x+=2;
			y++;

			msg = "Invalid size, factor must be 1-";
			msg += intToString(maxT);
			msg += " for this image";

			printStringAt( x, y, msg, "LEFT" );
			y+=2;

			printStringAt( x, y, "Press any key to continue...", "LEFT" );
			refresh();	

			waitForInput( FIXED_WAIT );

			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Translate Image", x, y, 60, 4 );
			x+=2;
			y+=2;

			t = promptForIntAt( x, y, "Enter translation factor(-1 to cancel): " );
		}

		if ( t > -1 )
		{
			temp.translateImage( t, img[index] );

			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
	refresh();	
}

void rotateImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, x, y;
	int theta;
	string msg;

	ImageType temp;

	index = promptForReg( "Rotate Image", loaded );

	if ( index > -1 )
	{
		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Translate Image", x, y, 60, 4 );
		x+=2;
		y+=2;

		theta = promptForIntAt( x, y, 
			"Rotate clockwise by angle theta(-1 to cancel):" );

		while ( ( theta < 1 || theta > 360 ) && theta != -1 )
		{
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Translate Image", x, y, 60, 4 );
			x+=2;
			y++;

			printStringAt( x, y, "Invalid angle, factor must be an angle 1-360", "LEFT" );
			y+=2;

			printStringAt( x, y, "Press any key to continue...", "LEFT" );
		}

		if ( theta > -1 )
		{
			temp.rotateImage( theta, img[index] );

			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
	refresh();	

}

void sumImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index1, index2;
	ImageType temp1, temp2;

	index1 = promptForReg( "Sum Image", loaded );
	
	if ( index1 > -1 )
	{
		temp1 = img[index1];

		index2 = promptForReg( "Choose Second Image", loaded );
		
		if ( index2 > -1 )
		{
			temp2 = img[index2];

			img[index1] = temp1 + temp2;

			// adds modified to register name
			if ( name[index1][strlen(name[index1])-1] != ')' )
				strcat( name[index1], " (modified)" );
		}
	}
	refresh();	
}

void subtractImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index1, index2;
	ImageType temp1, temp2;

	index1 = promptForReg( "Subtract Image", loaded );
	
	if ( index1 > -1 )
	{
		temp1 = img[index1];

		index2 = promptForReg( "Choose Second Image", loaded );
		
		if ( index2 > -1 )
		{
			temp2 = img[index2];

			img[index1] = temp1 - temp2;

			// adds modified to register name
			if ( name[index1][strlen(name[index1])-1] != ')' )
				strcat( name[index1], " (modified)" );
		}
	}
	refresh();	
}

void negateImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;

	index = promptForReg( "Negate Image", loaded );

	if ( index > -1 )
	{
		img[index].negateImage();

		// adds modified to register name
		if ( name[index][strlen(name[index])-1] != ')' )
			strcat( name[index], " (modified)" );
	}
	refresh();	
}
*/
// first parameter is array of loaded flags for the register
// if check == true, only allow registers that are loaded
int promptForReg( bool loaded[], char name[][NAME_LEN], const bool check )
{
	WINDOW *regMenu;

	int val;
	bool loop = true;
	char menuVals[REGS+1][NAME_LEN];

	for ( int i = 0; i < REGS; i++ )
		strcpy( menuVals[i], name[i] );

	// add exit to the list of commands
	strcpy( menuVals[REGS], "Exit" );


	do {
		val = showMenu( regMenu, "Registers", 15, 36, 1, 43, menuVals, REGS+1 );
		if ( val == BAD_REG )
			loop = false;
		else if ( ! loaded[val] && check )
			loop = true;
		else
			loop = false;
	} while ( loop );

	delwin( regMenu );

	return val;
}

int promptForFilename( const char title[], const char prompt[], char *&str )
{
	WINDOW *fileWin;

	int len = 16;	// max length of filename
	str = new char[len-1];

	drawWindow( fileWin, title, 5, strlen(prompt)+len+4, 2, 20 );

	promptForString( fileWin, 2, 2, prompt, str, len );

	delwin( fileWin );

	return strlen( str );
}

int promptForPixValue( const char title[], const char prompt[], int maxVal)
{
	WINDOW *pixWin;
	int val;

	drawWindow( pixWin, title, 4, 60, screenHeight()/2-3, screenWidth()/2-30 );
	
	val = promptForInt( pixWin, 1, 2, prompt );
	while ( val < -1 || val > maxVal )
	{
		// redraw window
		delwin( pixWin );
		drawWindow( pixWin, title, 4, 60, screenHeight()/2-3, screenWidth()/2-30 );

		// re-prompt user
		val = promptForInt( pixWin, 1, 2, prompt );
	}

	delwin( pixWin );
}

void promptForLoc( const char title[], ImageType& img, int& row, int& col )
{
	int N, M, Q, x, y;
	WINDOW *pixWin;
	row = -1;
	col = -1;
	img.getImageInfo( N, M, Q );

	drawWindow( pixWin, title, 4, 60, screenHeight()/2-3, screenWidth()/2-30 );

	row = promptForInt( pixWin, 1, 2, "Enter pixel row(-1 to cancel): " );
	while ( row < -1 || row >= N )
	{
		// clear the line
		for ( int i = 1; i < screenWidth()/2-4; i++ )
			mvwaddch( pixWin, 1, i, ' ' );

		// re-prompt user
		row = promptForInt( pixWin, 1, 2, "Enter pixel row(-1 to cancel): " );
	};

	if ( row != -1 )
	{
		col = promptForInt( pixWin, 2, 2, "Enter pixel column(-1 to cancel): " );
		while ( col < -1 || col >= M )
		{
			for ( int i = 1; i < screenWidth()/2-4; i++ )
				mvwaddch( pixWin, 2, i, ' ' );
			col = promptForInt( pixWin, 2, 2, "Enter pixel column(-1 to cancel): " );
		}
	}

	delwin( pixWin );
}

int findLocalPGM( char **&filenames )
{
	struct dirent **namelist;
	int n;
	int count = 0;

	n = scandir( ".", &namelist, 0, alphasort );

	if ( n > 0 )
	{
		for ( int i = 0; i < n; i++ )
		{
			static int len;
			len = strlen( namelist[i]->d_name );
			if ( len > 5 )
				if ( strcmp( ".pgm", &(namelist[i]->d_name[len-4]) ) == 0 )
					count++;
		}

		if ( count > 0 )
		{
			filenames = new char*[count];

			int j = 0;
			for ( int i = 0; i < n; i++ )
			{
				static int len;
				len = strlen( namelist[i]->d_name );
				if ( len > 5 )
					if ( strcmp( ".pgm", &(namelist[i]->d_name[len-4]) ) == 0 )
					{
						filenames[j] = new char[strlen(namelist[i]->d_name)+1];
						strcpy( filenames[j], namelist[i]->d_name );
						j++;
					}
				delete [] namelist[i];
			}
			delete [] namelist;
		}
	}

	return count;
}


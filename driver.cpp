#include <iostream>
#include "curses_io_V13.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "image.h"

using namespace std;

// CONSTANTS
	const int REGS = 3;	   // values 1-9
	const int NAME_LEN = 50;   // the max string length of names
	const int MSG_LEN = 100;   // the max string length of messages

	const int MAX_IMG = 10000; // the max size you can enlarge to
	const int MIN_IMG = 5;     // the min size you can reduce to

	const short BG_COLOR = COLOR_BLACK;
	const short OTHER_COLOR = COLOR_WHITE;	// not displayed, dif than BG_COLOR
	const short WINDOW_COLOR = COLOR_BLUE;
	const short SHADOW_COLOR = COLOR_YELLOW;
	const short FG_COLOR = COLOR_RED;

	const short MENU_BG_COLOR = COLOR_BLUE;
	const short MENU_FG_COLOR = COLOR_RED;
	const short MENU_SHADOW_COLOR = COLOR_YELLOW;

	const bool BRIGHT = true;

// FUNCTION PROTOTYPES
	// read/write functions
	void readImageHeader( const char[], int&, int&, int&, bool& );
	void readImage( const char[], ImageType& );
	void writeImage( const char[], ImageType& );

	// displays the main menu with a message
	char showMenu( const char[], bool[], char[][NAME_LEN] );

	// processes the choice returned by showMenu
	void processEntry( ImageType[], bool[], char[][NAME_LEN], char );

	// verify input for common prompts
	int promptForReg( const string, bool[], const bool = true );
	void promptForLoc( const string, ImageType&, int&, int& );

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

	void drawWindow( const string, int, int, int, int, short=WINDOW_COLOR,
	      short=FG_COLOR, short=SHADOW_COLOR, bool=BRIGHT );

// MAIN PROGRAM

int main( int argc, char **argv )
{
	char choice;
	char menuMsg[MSG_LEN] = {'\0'};

	// holds the name of the image stored in the register
	char imgName[REGS][NAME_LEN];

	// bool array indicating if registers are loaded with images
	bool imgLoaded[REGS];

	// this is where all the registers are stored
	ImageType image[REGS];

	// start
	startCurses();

	// initialize the bool array
	for ( int i = 0; i < REGS; i++ )
		imgLoaded[i] = false;

	// read argument parameters
	fillRegs( image, imgLoaded, imgName, argc, argv );

	do {
		// display main menu
		choice = showMenu( menuMsg, imgLoaded, imgName );

		// reset the menu message
		menuMsg[0] = '\0';

		try
		{
			// this is the main driving function that calls all others
			processEntry( image, imgLoaded, imgName, choice );
		}
		catch ( string s )
		{
			// append the error message to the menu message
			strcpy( menuMsg, s.c_str() );
		}
		catch( ... )
		{
			sprintf( menuMsg, "Encountered Unknown Error!" );

			// this type of error could break the program so exit
			return -1;
		}
	} while ( choice != 'x' && choice != 'X' );

	endCurses();

	return 0;
}

// Supporting functions

void drawWindow( const string title, int x, int y, int width, int height,
			short bgColor, short fgColor, short shadowColor, bool bright )
{
	short temp = COLOR_RED;
	int i;

	if ( x + width + 1 > screenWidth() )
			x = x - (screenWidth() - width);
	if ( y + height + 1 > screenHeight() )
			y = y - (screenHeight() - height);

	if ( x < 0 )
	{
		x = 0;
		width = screenWidth() - 1;
	}
	if ( y < 0 )
	{
		y = 0;
		height = screenWidth() - 1;
	}
	
	if ( shadowColor == temp )
			temp = COLOR_BLACK;
	
	setColor( temp, shadowColor, bright );
	clearScreen( x+1, y+1, x+width+1, y+height+1 );

	setColor( fgColor, bgColor, bright );
	clearScreen( x, y, x+width, y+height );

	printCharAt( x, y, '+' );
	printCharAt( x+width, y, '+' );
	printCharAt( x, y+height, '+' );
	printCharAt( x+width, y+height, '+' );

	for ( i = x+1; i < x + width; i++ )
	{
		printCharAt( i, y, '-' );
		printCharAt( i, y+height, '-' );
	}
	for ( i = y+1; i < y + height; i++ )
	{
		printCharAt( x, i, '|' );
		printCharAt( x+width, i, '|' );
	}

	printCharAt( x+2, y, ' ' );
	printStringAt(x+3, y, title, "LEFT" );
	printCharAt( x+3+title.length(), y, ' ' );
	refresh();
}

char showMenu( const char msg[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int X, Y;
	char input;
	char regStr[REGS * (NAME_LEN + 8)] = "";

	setColor( OTHER_COLOR, BG_COLOR, BRIGHT );
	clearScreen();

	X = 1, Y = 1;
	drawWindow( "Registers", X, Y, 40, REGS+3,
		MENU_BG_COLOR, MENU_FG_COLOR, MENU_SHADOW_COLOR );
	X+=2, Y+=2;

	for ( index = 0; index < REGS; index++ )
	{
		sprintf( regStr, "Register %i: %s", index+1,
		       ( loaded[index] ? name[index] : "" ) );
		printStringAt( X, Y, regStr, "LEFT" ); Y++;
	}

	X = screenWidth() / 2 - 20;
	Y = screenHeight() / 2 - 10;

	if ( msg[0] != '\0' )
	{
		drawWindow( "Main Menu", X, Y, 40, 24,
			MENU_BG_COLOR, MENU_FG_COLOR, MENU_SHADOW_COLOR );
	}
	else
	{
		drawWindow( "Main Menu", X, Y, 40, 22,
			MENU_BG_COLOR, MENU_FG_COLOR, MENU_SHADOW_COLOR );
	}

	X+=2;
	Y+=2;

	printStringAt( X, Y, "a. Read an image from a file.", "LEFT" ); Y++;
	printStringAt( X, Y, "b. Save an image to a file.", "LEFT" ); Y++;
	printStringAt( X, Y, "c. Get image info.", "LEFT" ); Y++;
	printStringAt( X, Y, "d. Set the value of a pixel.", "LEFT" ); Y++;
	printStringAt( X, Y, "e. Get the value of a pixel.", "LEFT" ); Y++;
	printStringAt( X, Y, "f. Extract a subimage from an image.", "LEFT" ); Y++;
	printStringAt( X, Y, "g. Enlarge image.", "LEFT" ); Y++;
	printStringAt( X, Y, "h. Shrink image.", "LEFT" ); Y++;
	printStringAt( X, Y, "i. Reflect image.", "LEFT" ); Y++;
	printStringAt( X, Y, "j. Translate image.", "LEFT" ); Y++;
	printStringAt( X, Y, "k. Rotate image.", "LEFT" ); Y++;
	printStringAt( X, Y, "l. Sum two images.", "LEFT" ); Y++;
	printStringAt( X, Y, "m. Subtract two images.", "LEFT" ); Y++;
	printStringAt( X, Y, "n. Compute negative of an image.", "LEFT" ); Y++;
	printStringAt( X, Y, "o. Clear a register", "LEFT" ); Y+=2;
	printStringAt( X, Y, "x. Exit", "LEFT" ); Y+=2;

	if ( msg[0] != '\0' )
	{
		printStringAt( X, Y, msg, "LEFT" ); Y+=2;
	}
	printStringAt( X, Y, "Enter Choice: ", "LEFT" );

	refresh();
	do {
		input = waitForInput( FIXED_WAIT );
	} while ( ! ( ( input >= 'a' && input <= 'o' ) ||
	              ( input >= 'A' && input <= 'O' ) ||
				  ( input == 'x' || input == 'X' ) ) );

	printCharAt( X+14, Y, input );
	refresh();
	return input;
}

void processEntry( ImageType img[], bool loaded[], char name[][NAME_LEN], char choice )
{
	switch ( choice )
	{
		case 'a':	// read image
		case 'A':
			loadImage( img, loaded, name );
			break;
		case 'b':	// write image
		case 'B':
			saveImage( img, loaded, name );
			break;
		case 'c':	// image info
		case 'C':
			getImageInfo( img, loaded, name );
			break;
		case 'd':	// set pixel val
		case 'D':
			setPixel( img, loaded, name );
			break;
		case 'e':	// get pixel val
		case 'E':
			getPixel( img, loaded, name );
			break;
		case 'f':	// extract subimage
		case 'F':
			extractSub( img, loaded, name );
			break;
		case 'g':	// enlarge
		case 'G':
			enlargeImg( img, loaded, name );
			break;
		case 'h':	// shrink
		case 'H':
			shrinkImg( img, loaded, name );
			break;
		case 'i':	// reflect
		case 'I':
			reflectImg( img, loaded, name );
			break;
		case 'j':	// translate
		case 'J':
			translateImg( img, loaded, name );
			break;
		case 'k':	// rotate
		case 'K':
			rotateImg( img, loaded, name );
			break;
		case 'l':	// sum
		case 'L':
			sumImg( img, loaded, name );
			break;
		case 'm':	// difference
		case 'M':
			subtractImg( img, loaded, name );
			break;
		case 'n':	// negative
		case 'N':
			negateImg( img, loaded, name );
			break;
		case 'o':	// clear register
		case 'O':
			clearRegister( img, loaded, name );
			break;
		case 'x':	// exit
		case 'X':
			// do nothing lol ^_^ maybe later add an exit screen
			break;
		default:	// bad input
			throw (string)"Invalid input";
			break;
	}
}

void clearRegister( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	index = promptForReg( "Clear Register", loaded );
	if ( index > -1 )
	{
		loaded[index] = false;
		name[index][0] = '\0';
	}
}

void fillRegs( ImageType img[], bool loaded[], char name[][NAME_LEN], int argc, char **argv )
{
	string msg = "";
	int i, j, k, index;
	bool f;

	for ( int index = 1; index < argc && index <= REGS; index++ )
	{
		try
		{
			readImageHeader( argv[index], i, j, k, f );
			img[index-1].setImageInfo( i, j, k );

			readImage( argv[index], img[index-1] );

			strcpy( name[index-1], argv[index] );

			loaded[index-1] = true;
		}
		catch ( string s )
		{
			msg += s;
			msg += '\n';
		}
	}

	if ( msg.length() > 0 )
	{
			setColor( COLOR_WHITE, COLOR_BLACK, BRIGHT );
			clearScreen();
			
			printStringAt( 0, 1, msg, "LEFT" );
			waitForInput( FIXED_WAIT );
	}
	refresh();
}

void loadImage( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	string strInput;
	int x, y, i, j, k, index;
	bool f;
	
	index = promptForReg( "Load Image", loaded, false );

	if ( index > -1 )
	{	
		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Load Image", x, y, 60, 4 );
		x+=2, y+=2;

		strInput = promptForStringAt( x, y, "Enter image filename: " );

		readImageHeader( strInput.c_str(), i, j, k, f );
		img[index].setImageInfo( i, j, k );
	
		readImage( strInput.c_str(), img[index] );
	
		loaded[index] = true;
		strcpy( name[index], strInput.c_str() );
	}
	refresh();
}

void saveImage( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	string strInput;
	int index, x, y;

	index = promptForReg( "Save Image", loaded );
	
	if ( index > -1 )
	{
		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( "Load Image", x, y, 60, 4 );
		x+=2, y+=2;

		strInput = promptForStringAt( x, y, "Enter output filename: " );
		writeImage( strInput.c_str(), img[index] );
		strcpy( name[index], strInput.c_str() );
	}
	refresh();
}

void getImageInfo( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int N, M, Q, index, x, y;

	index = promptForReg( "Get Image Info", loaded );
	
	if ( index > -1 )
	{
		setColor( OTHER_COLOR, BG_COLOR, BRIGHT );
		clearScreen();

		img[index].getImageInfo( N, M, Q );

		x = screenWidth() / 2 - 20;
		y = screenHeight() / 2 - 5;

		drawWindow( name[index], x, y, 40, 10 );

		x+=2;
		y+=2;
		
		printStringAt( x, y, "Saved in Register :", "LEFT" );
		printIntAt( x+22, y, index+1, "LEFT" ); y++;
		printStringAt( x, y, "Image Width       :", "LEFT" );
		printIntAt( x+22, y, M, "LEFT" ); y++;
		printStringAt( x, y, "Image Height      :", "LEFT" );
		printIntAt( x+22, y, N, "LEFT" ); y++;
		printStringAt( x, y, "Color Depth       :", "LEFT" );
		printIntAt( x+22, y, Q, "LEFT" ); y++;
		printStringAt( x, y, "Mean Gray value   :", "LEFT" );
		printIntAt( x+22, y, img[index].meanGray(), "LEFT" ); y+=2;
		printStringAt( x, y, "Press any key to continue...", "LEFT" );
		refresh();
		waitForInput( FIXED_WAIT );
	}
}

void setPixel( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, row, col, val, x, y;
	int N, M, Q;
	string msg;

	index = promptForReg( "Set Pixel Value", loaded );

	if ( index > -1 )
	{
		promptForLoc( "Choose a pixel", img[index], row, col );

		if ( row != -1 && col != -1 )
		{
			img[index].getImageInfo( N, M, Q );
			
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Pixel Value", x, y, 60, 4 );
			x+=2;
			y+=2;
			
			val = promptForIntAt( x, y, "Enter new pixel value(-1 to cancel): " );

			while ( val < -1 || val > Q )
			{
				x = screenWidth() / 2 - 30;
				y = screenHeight() / 2 - 3;

				drawWindow( "Pixel Value", x, y, 60, 4 );
				
				x+=2;
				y+=1;
				
				msg = "Invalid pixel value, must be value 0-";
				msg += intToString(Q);
				printStringAt( x, y, msg, "LEFT" );
				y+=2;
				printStringAt( x, y, "Press any key to continue...", "LEFT" );
				
				waitForInput( FIXED_WAIT );
			
				x = screenWidth() / 2 - 30;
				y = screenHeight() / 2 - 3;

				drawWindow( "Pixel Value", x, y, 60, 4 );
				x+=2;
				y+=2;

				val = promptForIntAt( x, y, "Enter new pixel value(-1 to cancel): " );
			}
			if ( val != -1 )
			{
				img[index].setPixelVal( row, col, val );
				if ( name[index][strlen(name[index])-1] != ')' )
					strcat( name[index], " (modified)" );
			}
		}
	}
	refresh();
}

void getPixel( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, row = -1, col = -1, val, x, y;
	int N, M, Q;
	string msg;

	index = promptForReg( "Get Pixel Value", loaded );

	if ( index > -1 )
	{
		promptForLoc( "Choose a pixel", img[index], row, col );

		if ( row > -1 && col > -1 )
		{
			img[index].getImageInfo( N, M, Q );
			val = img[index].getPixelVal( row, col );

			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			msg = "(";
			msg += intToString(row+1);
			msg += ", ";
			msg += intToString(col+1);
			msg += ")";

			drawWindow( msg, x, y, 60, 4 );
			x+=2;
			y++;

			msg = "Pixel value is ";
			msg += intToString(val);
			msg += "/";
			msg += intToString(Q);
			msg += " white";
	
			printStringAt( x, y, msg, "LEFT" );
			y+=2;
			printStringAt( x, y, "Press any key to continue...", "LEFT" );

			waitForInput( FIXED_WAIT );
		}
	}
	refresh();
}

void extractSub( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, x, y;
	int ULr, ULc, LRr = -1, LRc = -1;
	ImageType temp;

	index = promptForReg( "Extract Sub Image", loaded );

	if ( index > -1 )
	{

		promptForLoc( "Upper Left Corner", img[index], ULr, ULc );
		if ( ULr > -1 && ULc > -1 )
		{
			promptForLoc( "Lower Right Corner", img[index], LRr, LRc );
		}

		while ( ( LRr <= ULr || LRc <= ULc ) &&
			 ULr > -1 && ULc > -1 && LRr > -1 && LRc > -1 )
		{	
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;

			drawWindow( "Extract Sub Image", x, y, 60, 4 );
			x+=2;
			y++;

			printStringAt( x, y, "Invalid corners, please enter valid coordinates", "LEFT" );
			
			y+=2;

			printStringAt( x, y, "Press any key to continue...", "LEFT" );
			refresh();

			waitForInput( FIXED_WAIT );

			promptForLoc( "Upper Left Corner", img[index], ULr, ULc );
			if ( ULr > -1 && ULc > -1 )
			{
				promptForLoc( "Lower Right Corner", img[index], LRr, LRc );
			}
		}
		
		if ( ULr > -1 && ULc > -1 && LRr > -1 && LRc > -1 )
		{
			temp.getSubImage( ULr, ULc, LRr, LRc, img[index] );

			img[index] = temp;

			// adds modified to register name
			if ( name[index][strlen(name[index])-1] != ')' )
				strcat( name[index], " (modified)" );
		}
	}
	refresh();
}

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
			temp.enlargeImage( s, img[index] );

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
			"Rotate counter-clockwise by angle theta(-1 to cancel):" );

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

// first parameter is array of loaded flags for the register
// if check == true, only allow registers that are loaded
int promptForReg( const string title, bool loaded[], const bool check )
{
	bool valid;
	char inner_choice;
	int x, y;
	string msg;

	do {
		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		valid = true;

		drawWindow( title, x, y, 60, 4 );
		x+=2, y+=2;

		if ( check )
		{
			msg = "Please choose a loaded register (1-";
			msg += intToString(REGS);
			msg += ", or C): ";
		}
		else
		{
			msg = "Please choose a register (1-";
			msg += intToString(REGS);
			msg += ", or C): ";
		}
		printStringAt( x, y, msg, "LEFT" );

		inner_choice = waitForInput( FIXED_WAIT );

		y++;

		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( title, x, y, 60, 4 );
		x+=2, y++;

		if ( ( inner_choice < '1' || inner_choice > ('0'+REGS) ) &&
		   inner_choice != 'c' && inner_choice != 'C' )
		{
			printStringAt( x, y, "Invalid register please choose again.", "LEFT" );
			y+=2;
			printStringAt( x, y, "Press any key to continue...", "LEFT" );
			refresh();	
			waitForInput( FIXED_WAIT );

			valid = false;
		}
		else if ( check && !loaded[inner_choice - (int)'0' - 1 ] &&
			  inner_choice != 'c' && inner_choice != 'C' )
		{
			msg = "Register ";
			msg += inner_choice;
			msg += " is empty, please choose again.";
			printStringAt( x, y, msg, "LEFT" );
			y+=2;
			printStringAt( x, y, "Press any key to continue...", "LEFT" );
			refresh();	
			waitForInput( FIXED_WAIT );
			valid = false;
		}
		
		refresh();	
	} while ( ! valid );
	
	if ( inner_choice == 'c' || inner_choice == 'C' )
		return -1;
	
	return inner_choice - (int)'0' - 1;
}

void promptForLoc( const string title, ImageType& img, int& row, int& col )
{
	int N, M, Q, x, y;
	row = -1;
	col = -1;
	string msg;
	img.getImageInfo( N, M, Q );

	x = screenWidth() / 2 - 30;
	y = screenHeight() / 2 - 3;

	drawWindow( title, x, y, 60, 4 );

	x+=2;
	y++;

	row = promptForIntAt( x, y, "Enter pixel row(-1 to cancel): " );
	
	while ( ( row < 1 || row > N ) && row != -1 )
	{
		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;
		drawWindow( title, x, y, 60, 4 );

		x+=2;
		y++;
		
		msg = "Invalid row, must be a value 1-";
		msg += intToString(N);
	
		printStringAt( x, y, msg, "LEFT" );
		y+=2;
		printStringAt( x, y, "Press any key to continue...", "LEFT" );
		refresh();	

		waitForInput( FIXED_WAIT );

		x = screenWidth() / 2 - 30;
		y = screenHeight() / 2 - 3;

		drawWindow( title, x, y, 60, 4 );
		x+=2;
		y++;
		row = promptForIntAt( x, y, "Enter pixel row(-1 to cancel): " );
	}

	// adjust for actual value
	if ( row != -1 )
	{
		row--;
		y+=2;
		col = promptForIntAt( x, y, "Enter pixel column(-1 to cancel): " );
		while ( ( col < 1 || col > M ) && col != -1 )
		{
			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;
			drawWindow( title, x, y, 60, 4 );

			x+=2;
			y++;
				
			msg = "Invalid column, must be a value 1-";
			msg += intToString(M);
			
			printStringAt( x, y, msg, "LEFT" );
			y+=2;
			printStringAt( x, y, "Press any key to continue...", "LEFT" );
			refresh();	

			waitForInput( FIXED_WAIT );

			x = screenWidth() / 2 - 30;
			y = screenHeight() / 2 - 3;
			drawWindow( title, x, y, 60, 4 );
			x+=2;
			y++;
			msg = "Enter pixel row(-1 to cancel): ";
			msg += intToString(row+1);
			printStringAt( x, y, msg, "LEFT" );
			y+=2;
			col = promptForIntAt( x, y, "Enter pixel column(-1 to cancel): " );
		}

		// adjust for actual value
		if ( col != -1 )
			col--;
	}
}


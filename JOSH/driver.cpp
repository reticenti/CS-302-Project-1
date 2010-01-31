#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "image.h"

using namespace std;

// CONSTANTS
	const int REGS = 10;	       // must be at least 1
	const int NAME_LEN = 50;   // the max string length of names
	const int MSG_LEN = 100;   // the max string length of messages

	const int MAX_IMG = 10000; // the max size you can enlarge to
	const int MIN_IMG = 5;     // the min size you can reduce to

// FUNCTION PROTOTYPES
	// read/write functions
	void readImageHeader( char[], int&, int&, int&, bool& );
	void readImage( char[], ImageType& );
	void writeImage( char[], ImageType& );

	// displays the main menu with a message
	char showMenu( const char[], bool[], char[][NAME_LEN] );

	// processes the choice returned by showMenu
	void processEntry( ImageType[], bool[], char[][NAME_LEN], char );

	// verify input for common prompts
	int promptForReg( bool[], const bool = true );
	void promptForLoc( ImageType&, int&, int& );

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

// MAIN PROGRAM

int main( int argc, char **argv )
{
	char choice;
	char menuMsg[MSG_LEN] = "";

	// holds the name of the image stored in the register
	char imgName[REGS][NAME_LEN];

	// bool array indicating if registers are loaded with images
	bool imgLoaded[REGS];

	// this is where all the registers are stored
	ImageType image[REGS];

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
			sprintf( menuMsg, "\n%s\n", s.c_str() );
		}
		catch( ... )
		{
			sprintf( menuMsg, "\nEncountered Unknown Error!\n" );

			// this type of error could break the program so exit
			return -1;
		}
	} while ( choice != 'x' && choice != 'X' );

	return 0;
}

// Supporting functions

char showMenu( const char msg[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	char input;
	char regStr[REGS * (NAME_LEN + 8)] = "";

	for ( index = 0; index < REGS; index++ )
		sprintf( regStr, "%sRegister %i: %s\n", regStr, index+1,
		( loaded[index] ? name[index] : "Not loaded" ) );

	system("clear");
	cout << "a. Read an image from a file.\n"
	     << "b. Save an image to a file.\n"
	     << "c. Get image info.\n"
	     << "d. Set the value of a pixel.\n"
	     << "e. Get the value of a pixel.\n"
	     << "f. Extract a subimage from an image.\n"
	     << "g. Enlarge image.\n"
	     << "h. Shrink image.\n"
	     << "i. Reflect image.\n"
	     << "j. Translate image.\n"
	     << "k. Rotate image.\n"
	     << "l. Sum two images.\n"
	     << "m. Subtract two image.\n"
	     << "n. Compute negative of an image.\n"
		 << "o. Clear a register.\n"
	     << "\nx. Exit\n"
	     << endl << regStr << msg << endl
	     << "Enter choice: ";
	
	cin >> input;

	// removes extra characters from the buffer
	cin.ignore( 255, '\n' );

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
			cout << "\nGoodbye!\n\n";
			break;
		default:	// bad input
			throw (string)"Invalid input";
			break;
	}
}

void clearRegister( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	index = promptForReg( loaded );
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
			msg += "\n";
		}
	}

	if ( msg.length() > 0 )
	{
			system("clear");
			cout << msg;
			cout << "\nPress Enter to continue...";
			if ( cin.get() != '\n' )
					cin.ignore( 255, '\n' );
	}
}

void loadImage( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	char strInput[NAME_LEN];
	int i, j, k, index;
	bool f;
	
	index = promptForReg( loaded, false );

	if ( index > -1 )
	{
		cout << "Enter image filename: ";
		cin.getline( strInput, NAME_LEN );	

		readImageHeader( strInput, i, j, k, f );
		img[index].setImageInfo( i, j, k );
	
		readImage( strInput, img[index] );
	
		loaded[index] = true;
		strcpy( name[index], strInput );
	}
}

void saveImage( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	char strInput[NAME_LEN];
	int index;

	index = promptForReg( loaded );
	
	if ( index > -1 )
	{
		cout << "Enter output filename: ";
		cin.getline( strInput, NAME_LEN );
		writeImage( strInput, img[index] );
		strcpy( name[index], strInput );
	}
}

void getImageInfo( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int N, M, Q, index;

	index = promptForReg( loaded );
	
	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );
		system( "clear" );
		cout << "Image info..." << endl << endl
		     << name[index] << endl
		     << "Currently saved in Register #" << index+1 << endl
		     << "Image Width     : " << M << endl
		     << "Image Height    : " << N << endl
		     << "Color Depth     : " << Q << endl
		     << "Mean Gray value : " << img[index].meanGray()
		     << "\n\nPress Enter to continue...";
		if ( cin.get() != '\n' )
			cin.ignore(255, '\n');
	}
}

void setPixel( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index, row, col, val;
	int N, M, Q;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		cout << "Choose a pixel\n\n";
		promptForLoc( img[index], row, col );

		if ( row != -1 && col != -1 )
		{
			img[index].getImageInfo( N, M, Q );
			
			cout << "Enter new pixel value(-1 to cancel): ";
			cin >> val;
			cin.ignore( 255, '\n' );

			while ( val < -1 || val > Q )
			{
				cout << "Invalid pixel value, must be value 0-" << Q << endl
				     << "Enter new pixel value(-1 to cancel): ";
				cin >> val;
				cin.ignore( 255, '\n' );
			}
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
	int index, row = -1, col = -1, val;
	int N, M, Q;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		cout << "Choose a pixel\n\n";
		promptForLoc( img[index], row, col );

		if ( row > -1 && col > -1 )
		{
			img[index].getImageInfo( N, M, Q );
			val = img[index].getPixelVal( row, col );

			system("clear");

			cout << name[index] << endl << endl
			     << "Pixel value at row " << row+1 << ", column " << col+1
			     << " is " << val << "/" << Q << " white" << endl << endl
			     << "Press Enter to continue...";
		
			if ( cin.get() != '\n' )
				cin.ignore( 255, '\n' );
		}
	}
}

void extractSub( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int ULr, ULc, LRr = -1, LRc = -1;
	ImageType temp;

	index = promptForReg( loaded );

	if ( index > -1 )
	{

		cout << "\nEnter location (row, col) of upper left corner.\n\n";
		promptForLoc( img[index], ULr, ULc );
		if ( ULr > -1 && ULc > -1 )
		{
			cout << "\nEnter location (row, col) of lower right corner.\n\n";
			promptForLoc( img[index], LRr, LRc );
		}

		while ( ( LRr <= ULr || LRc <= ULc ) &&
			 ULr > -1 && ULc > -1 && LRr > -1 && LRc > -1 )
		{
			cout << "\nInvalid corners, the upper left values MUST be less than"
			     << "the lower right values.\n"
			     << "Press Enter to continue...";
			if ( cin.get() != '\n' )
				cin.ignore( 255, '\n' );
			
			cout << "\nEnter location (row, col) of upper left corner.\n\n";
			promptForLoc( img[index], ULr, ULc );
			if ( ULr > -1 && ULc > -1 )
			{
				cout << "\nEnter location (row, col) of lower right corner.\n\n";
				promptForLoc( img[index], LRr, LRc );
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
}

void enlargeImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int N, M, Q, s;
	int maxS;

	ImageType temp;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );

		maxS = (N > M ? MAX_IMG/N : MAX_IMG/M);

		cout << "\nEnlarge image by a whole factor (integer value)\n";

		cout << "\nEnter enlargment multiplier(-1 to cancel): ";
		cin >> s;
		cin.ignore( 255, '\n' );

		while ( ( s < 2 || s > maxS ) && s != -1 )
		{
			cout << "\nInvalid size, factor must be 2-" << maxS << " for this image"
			     << "\n\nEnter enlargment factor(-1 to cancel): ";
			cin >> s;
			cin.ignore( 255, '\n' );
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
}

void shrinkImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int N, M, Q, s;
	int maxS;

	ImageType temp;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );

		maxS = (N > M ? N/MIN_IMG : M/MIN_IMG);

		cout << "\nReduce image by a factor (integer)\n";

		cout << "\nEnter reduction factor(-1 to cancel): ";
		cin >> s;
		cin.ignore( 255, '\n' );

		while ( ( s < 2 || s > maxS ) && s != -1 )
		{
			cout << "\nInvalid size, factor must be 2-" << maxS << " for this image"
			     << "\n\nEnter reduction factor(-1 to cancel): ";
			cin >> s;
			cin.ignore( 255, '\n' );
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
}

void reflectImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	char dir;
	ImageType temp;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		cout << "\nReflect image horizontally or vertically?\n\n"
		     << "Enter choice (H(oriz), V(ert), C(ancel)): ";
		cin >> dir;
		cin.ignore( 255, '\n' );

		while ( dir != 'h' && dir != 'H' &&
		        dir != 'v' && dir != 'V' &&
				dir != 'c' && dir != 'C' )
		{
			cout << "\nInvalid input!\n\n"
			     << "Enter choice (H, V, or C): ";
			cin >> dir;
			cin.ignore( 255, '\n' );
		}

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
}

void translateImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int N, M, Q, t;
	int maxT;

	ImageType temp;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		img[index].getImageInfo( N, M, Q );

		maxT = (N > M ? N-1 : M-1);

		cout << "\nTranslate image by a number of pixels (integer)\n";

		cout << "\nEnter translation factor(-1 to cancel): ";
		cin >> t;
		cin.ignore( 255, '\n' );

		while ( ( t < 1 || t > maxT ) && t != -1 )
		{
			cout << "\nInvalid size, factor must be 1-" << maxT << " for this image"
			     << "\n\nEnter translation factor(-1 to cancel): ";
			cin >> t;
			cin.ignore( 255, '\n' );
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
}

void rotateImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;
	int theta;

	ImageType temp;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		cout << "\nRotate image counter-clockwise by some positive angle theta (integer)\n";

		cout << "\nEnter angle theta(-1 to cancel): ";
		cin >> theta;
		cin.ignore( 255, '\n' );

		while ( ( theta < 1 || theta > 360 ) && theta != -1 )
		{
			cout << "\nInvalid angle, factor must be and angle 1-360"
			     << "\n\nEnter angle theta(-1 to cancel): ";
			cin >> theta;
			cin.ignore( 255, '\n' );
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

}

void sumImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index1, index2;
	ImageType temp1, temp2;

	index1 = promptForReg( loaded );
	
	if ( index1 > -1 )
	{
		temp1 = img[index1];

		cout << "\nSum the image with what other image?\n";
		index2 = promptForReg( loaded );
		
		if ( index2 > -1 )
		{
			temp2 = img[index2];

			img[index1] = temp1 + temp2;

			// adds modified to register name
			if ( name[index1][strlen(name[index1])-1] != ')' )
				strcat( name[index1], " (modified)" );
		}
	}
}

void subtractImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index1, index2;
	ImageType temp1, temp2;

	index1 = promptForReg( loaded );
	
	if ( index1 > -1 )
	{
		temp1 = img[index1];

		cout << "\nWhich image do you want to subtract?\n";
		index2 = promptForReg( loaded );
		
		if ( index2 > -1 )
		{
			temp2 = img[index2];

			img[index1] = temp1 - temp2;

			// adds modified to register name
			if ( name[index1][strlen(name[index1])-1] != ')' )
				strcat( name[index1], " (modified)" );
		}
	}
}

void negateImg( ImageType img[], bool loaded[], char name[][NAME_LEN] )
{
	int index;

	index = promptForReg( loaded );

	if ( index > -1 )
	{
		img[index].negateImage();

		// adds modified to register name
		if ( name[index][strlen(name[index])-1] != ')' )
			strcat( name[index], " (modified)" );
	}
}

// first parameter is array of loaded flags for the register
// if check == true, only allow registers that are loaded
int promptForReg( bool loaded[], const bool check )
{
	bool valid;
	char inner_choice;

	do {
		valid = true;
		if ( check )
			cout << "\nPlease choose a loaded register (1-" << REGS << ", or C): ";
		else
			cout << "\nPlease choose a register (1-" << REGS << ", or C): ";

		cin >> inner_choice;

		cin.ignore( 255, '\n' );

		if ( ( inner_choice < '1' || inner_choice > ('0'+REGS) ) &&
		   inner_choice != 'c' && inner_choice != 'C' )
		{
			cout << "Invalid register please choose again.\n";
			valid = false;
		}
		else if ( check && !loaded[inner_choice - (int)'0' - 1 ] &&
			  inner_choice != 'c' && inner_choice != 'C' )
		{
			cout << "Register " << inner_choice << " is empty, please choose again.\n";
			valid = false;
		}
		
	} while ( ! valid );
	
	if ( inner_choice == 'c' || inner_choice == 'C' )
		return -1;
	
	return inner_choice - (int)'0' - 1;
}

void promptForLoc( ImageType& img, int& row, int& col )
{
	int N, M, Q;
	row = -1;
	col = -1;
	img.getImageInfo( N, M, Q );
	cout << "Enter pixel row(-1 to cancel): ";
	cin >> row;
	cin.ignore(255, '\n');
	while ( ( row < 1 || row > N ) && row != -1 )
	{
		cout << "Invalid row, must be a value 1-" << N << endl
		     << "Enter pixel row:(-1 to cancel) ";
		cin >> row;
		cin.ignore(255,'\n');
	}
	// adjust for actual value
	if ( row != -1 )
	{
		row--;
		cout << "Enter pixel column(-1 to cancel): ";
		cin >> col;
		cin.ignore(255, '\n');
		while ( ( col < 1 || col > M ) && col != -1 )
		{
			cout << "Invalid column, must be a value 1-" << M << endl
			     << "Enter pixel column:(-1 to cancel) ";
			cin >> col;
			cin.ignore(255,'\n');
		}

		// adjust for actual value
		if ( col != -1 )
			col--;
	}
}


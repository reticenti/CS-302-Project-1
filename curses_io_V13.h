/*//////////////////////////////////////////////////////////////////////////
  
    Curses I/O header file.

    Copyright (c) 2008, 2009 by Michael E. Leverington

    Code is free for use in whole or in part and may be modified for other
    use as long as the above copyright statement is included.
    
    Code Written:        08/18/2008
    Most Recent Update:  02/11/2010 - 9:00 p.m.
    Date Due for Review: 02/16/2010

	Update by Joshua Gleason 01/31/2010 - 3:30 p.m.
		- added ability to have variable terminal size
		- added screenWidth and screenHeight functions which return the
		  teminals width and height
	
	Update by Josiah Humphrey 02/11/2010 - 9:00 p.m.
		- removed redundent refresh calls, assumed now to be refreshed
		  by main driver program

/////////////////////////////////////////////////////////////////////////*/
/*
INSTRUCTIONS:

    1) This source file contains the functions you may use for input and
       output of assignments when you need to use curses. You only need
       to include this file as a header file in your program. As long as 
       you don't change this file, you can keep using it to develop 
       other programs. It should be noted that header files usually only
       contain function headers, and do not normally contain function 
       implementations. However this file is set up to make it easy for
       you to get started learning C++ programming.
       
    2) For any of the "promptFor..." functions, just place some prompting
       text into the function and call it. The user will be prompted for
       the appropriate value, and the user's response will be assigned
       to the variable you use. These functions are different from the 
       standard functions because you must tell the prompt where on the screen
       (in x and y positions) to be displayed.

       Example:        userAge = promptForInteger( 5, 5, "Enter your age: " );

       Result Displayed (at location x = 5 & y - 5:
                                           Enter your age: {user answers here}

    3) For any of the "print...At" functions, you need to provide the following:
       - the x and y location to print at
       - value to be output
       - the justification; the value will be printed as follows:
         - to the left starting at the x, y location if "RIGHT" justified
         - to the right starting at the x, y location if "LEFT" justified
         - centered on the x, y location if "CENTER" justified
       - in the case of floating point values, the precision

       Example 1:    printStringAt( 5, 5, "This is a string", "CENTER" );
       Result Displayed:        pipe is located at 5, 5 => |
                string is printed here as shown =>  This is a string

       Explanation: "This is a string" is displayed centered
                    on the x, y location

       Example 2:   printDoubleAt( 2, 30, 25.45678, 2, "RIGHT" );
       Result Displayed:     pipe is located at 2, 30 => |
                                                     25.46

       Explanation: The value 25.45678 is displayed right justified ending
                    with the location 2, 30 and with 2 digits after the
                    decimal point (called precision for purposes of
                    this function)

    4) You are provided simple information for all of the functions in a
       standardized format. You will be using this format for your own
       functions in the near future.

END OF INSTRUCTIONS
*/

#ifndef CURSES_IO_H
#define CURSES_IO_H

#include <cstring>
#include <string>
#include <curses.h>

using namespace std;
///////////////////////////////////////////////////////////////////////////
// Global Constants
///////////////////////////////////////////////////////////////////////////

    // keyboard management
    const int ENTER_KEY = 13;
    const int BACKSPACE_KEY = 8;
    const int BACKSPACE_KEY_LINUX = 127;
    const int KP_ENTER_KEY = 459;
    const int KP_MINUS = 464;
    const int KP_PLUS = 465;
    const int KP_SLASH = 458;
    const int KP_SPLAT = 463;
    const int KB_RIGHT_ARROW = 261;
    const int KB_LEFT_ARROW = 260;
    const int KB_DOWN_ARROW = 258;
    const int KB_UP_ARROW = 259;
    const int KB_PAGE_UP = 339;
    const int KB_PAGE_DN = 338;
    const int KB_ESCAPE = 27;

    // string management
    const int MAX_INPUT_LENGTH = 60;
    const int MIN_STRING_SIZE = 15;

    // operational constants
    const bool SET_BRIGHT = true;
    const int FIXED_WAIT = -1;
    const int DEFAULT_WAIT = 1;
    const int NO_RESPONSE = -1;



///////////////////////////////////////////////////////////////////////////
// Function Headers
///////////////////////////////////////////////////////////////////////////

	// name: screen(Width/Height)
	// input: none
	// output (returned): max screen (width/height) is returned to the calling function
	// dependencies: none
	// process: the value of getmax(x/y)( stdscr ) is returned
	int screenWidth();
	int screenHeight();

    // name: clearScreen
    // input: upper left x, y, and lower right x, y locations
    // output (to screen): specified area of screen is cleared
    // output (returned): no value is returned to the calling function
    // dependencies: none
    // process: prints spaces across the given area using the presently
    //          specified color scheme
    void clearScreen( int uprLeftX, int uprLeftY, int lwrRightX, int lwrRightY );

	// name: clearScreen
	// process: same as above clearscreen except it clears entire screen
	void clearScreen();

    // name: promptForCharAt
    // input: x, y location, text to prompt user (string)
    // output: one character (char) is returned to calling function
    // dependencies: uses waitForInput function
    // process: prompts user for character at the specified location, then returns it
    char promptForCharAt( int xPos, int yPos, const string &promptString );

    // name: promptForIntAt
    // input: x, y location, text to prompt user (string)
    // output: one integer (int) is returned to calling function
    // dependencies: uses getInputString function
    // process: prompts user for integer at the specified location, then returns it
    int promptForIntAt( int xPos, int yPos, const string &promptString );

    // name: promptForDoubleAt
    // input: x, y location, text to prompt user (string)
    // output: one floating point value (double) is returned to calling function
    // dependencies: uses getInputString function
    // process: prompts user for floating point value at the specified location, 
    //          then returns it
    double promptForDoubleAt( int xPos, int yPos, const string &promptString );

    // name: promptForStringAt
    // input: x, y location, text to prompt user (string)
    // output: user entered text (string) is returned to calling function
    // dependencies: uses getInputString function
    // process: prompts user for string at the specified location, then returns it
    string promptForStringAt( int xPos, int yPos, const string &promptString );

    // name: promptForStringAt
    // input: x, y location, text to prompt user (string)
    // output: user entered text (string) is returned to calling function
    // dependencies: uses getInputString function
    // process: prompts user for string at the specified location, then returns it
    void promptForStringAt( int xPos, int yPos, 
                              const string &promptString, char resultString[] );

    // name: getLineAt
    // input: x, y location, length of string to capture (ints)
    // output: string extracted from screen
    // dependencies: uses curses function
    // process: acquires string from screen, returns it
    string getLineAt( int xPos, int yPos, int length );

    // name: getCharAt
    // input: x, y location of character to get
    // output: character extracted from screen
    // dependencies: uses curses function
    // process: acquires character from screen, returns it
    char getCharAt( int xPos, int yPos );

    // name: setColor
    // input (for initialization): negative numbers, only accepted once
    // input: color constants representing foreground and background
       // Colors are: COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_MAGENTA, 
       //             COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, COLOR_CYAN
    // output (for initialization): sets a table of all color combinations
    // output: none, function sets colors for use by other functions
    // dependencies: none
    // process: sets foreground & background colors for future operations
    void setColor( short foreGround, short backGround, bool bright );

    // name: printCharAt
    // input: x & y locations
    // input: character value (char)
    // output (to screen): places character at specified location
    // output (returned): no value is returned to the calling function
    // dependencies: none
    // process: places character at specified location
    void printCharAt( int xPos, int yPos, char charVal );

    // name: printIntAt
    // input: x & y locations
    // input: integer value (int)
    // input: justification - "CENTER", "LEFT", "RIGHT"
    // output (to screen): places integer at specified location, 
    //                     centered, left, or right justified with respect
    //                     to given location
    // output (returned): no value is returned to the calling function
    // dependencies: uses printStringAt function
    // process: calculates position for justification, then place
    //          integer at specified location
    void printIntAt( int xPos, int yPos, int intVal, const string &justify );

    // name: printDoubleAt
    // input: x & y locations
    // input: double value (double)
    // input: precision (int), for number of digits right of the decimal
    // input: justification - "CENTER", "LEFT", "RIGHT"
    // output (to screen): places double value at specified location, 
    //                     centered, left, or right justified with respect
    //                     to given location, with specified precision
    // output (returned): no value is returned to the calling function
    // dependencies: uses printStringAt function
    // process: calculates position for justification, then place
    //          double value at specified location
    void printDoubleAt( int xPos, int yPos, double doubleVal, int precision, const string &justify );

    // name: printStringAt
    // input: x & y locations
    // input: string value (string)
    // input: justification - "CENTER", "LEFT", "RIGHT"
    // output (to screen): places string at specified location, 
    //                     centered, left, or right justified with respect
    //                     to given location
    // output (returned): no value is returned to the calling function
    // dependencies: uses intToString and printStringVertical function
    // process: calculates position for justification, then place
    //          string at specified location
    void printStringAt( int xPos, int yPos, const string &outString, const string &justify );

    // name: printStringVertical
    // input: x & y locations
    // input: string value (string)
    // input: justification - "UP", "DOWN" {guaranteed input}
    // output (to screen): places string at specified location, 
    //                     and prints vertically up or down as specified
    // output (returned): no value is returned to the calling function
    // dependencies: none
    // process: calculates position for justification, then place
    //          string at specified location
    void printStringVertical( int xStart, int yStart, const string &text, const string &orient );

    // name: moveToXY
    // input: x and y locations (int)
    // output (returned): none
    // dependencies: none
    // process: moves cursor to x, y location on screen, 
    //          if they are legal positions
    void moveToXY( int xPos, int yPos );

    // name: waitForInput
    // input: integer value to indicate one of three things:
       //     - timedWait < 0: wait for user response: any negative number
       //     - timedWait = 0: no wait, get input immediately
       //     - timedWait > 0: waits for "timedWait" tenths of a second,
       //                      then acquires input
    // output (returned): integer value from input, or -1 if no input
    // dependencies: none
    // process: bases actions on input value as specified above
    int waitForInput( int timedWait );

    // name: startCurses
    // input: none
    // output (returned): Boolean true/false to report success at initialization
    // dependencies: none
    // process: initializes all appropriate components of curses, including
    //          color operations and color table for use by setColor
    bool startCurses();

    // name: endCurses
    // input: none
    // output (returned): none
    // dependencies: none
    // process: shuts down curses operations
    void endCurses();

    // name: getInputString (supporting function)
    // input: x & y locations
    // input: string of characters allowed for input
    // output (to screen): shows input process by user
    // output (returned): string value is returned to calling function
    // dependencies: uses waitForInput and charInString
    // process: allows only specified characters to be input, allows backspace
    //          returns when ENTER key is pressed
    string getInputString( int xPos, int yPos, const string &allowedChars );

    // name: charInString (supporting function)
    // input: a test character (char)
    // input: string of characters to be tested
    // output (to screen): none
    // output (returned): returns true if test character is found to be
    //                    in given string
    // dependencies: none
    // process: searches through given string and tests test character
    //          to see if it is in string; returns true if found, false if not
    bool charInString( char testChar, const string &testString );

    // name: intToString (supporting function)
    // input: value - integer (int) value to be converted to string
    // output: string form of integer value is returned to calling function
    // dependencies: none
    // process: recursively adds individual digits to string (backwards)
    string intToString( int value );

///////////////////////////////////////////////////////////////////////////
// Function Implementations
///////////////////////////////////////////////////////////////////////////

int screenWidth()
{
	return getmaxx( stdscr );
}

int screenHeight()
{
	return getmaxy( stdscr );
}

void clearScreen( int uprLeftX, int uprLeftY, int lwrRightX, int lwrRightY )
   {
    // initialize function
    const char SPACE = ' ';
    int yCtr, xCtr;

    // loop across rows of the screen
    for( yCtr = uprLeftY; yCtr <= lwrRightY; yCtr++ )
       {
        // loop across the columns of the screen
        for( xCtr = uprLeftX; xCtr <= lwrRightX; xCtr++ )
           {
            // output a space
            mvaddch( yCtr, xCtr, SPACE );
           }
       }
   }

void clearScreen()
{
	clearScreen( 0, 0, screenWidth(), screenHeight() );
}

char promptForCharAt( int xPos, int yPos, const string &promptString )
   {
    // initialize function
    const char SPACE = ' ';
    int response, checkForBS = 0;

    // print prompt string
    mvaddstr( yPos, xPos, promptString.c_str() );

    // move cursor to correct location
    xPos += promptString.length();
    move( yPos, xPos );

    do
       {
        // get character response
        response = waitForInput( FIXED_WAIT );
       
        // print character response
        mvaddch( yPos, xPos, response );

        // Wait for ENTER key, check for BACKSPACE key
        if( ( response != ENTER_KEY ) && ( response != KP_ENTER_KEY ) )
           {
            checkForBS = waitForInput( FIXED_WAIT );

            if( checkForBS == BACKSPACE_KEY )
               {
                mvaddch( yPos, xPos, SPACE );
                move( yPos, xPos );
                checkForBS = 0; response = ENTER_KEY;
               }
            }
       }
    while( ( response == ENTER_KEY ) || ( response == KP_ENTER_KEY ) );

    // return character response
    return ( char( response ) );
   }

int promptForIntAt( int xPos, int yPos, const string &promptString )
   {
    // initialize function
    string response;
    int index, numStringLength, answer;
    bool negFlag = false;

    // print prompt string
    mvaddstr( yPos, xPos, promptString.c_str() );

    // move cursor to response point
    xPos += promptString.length();
    move( yPos, xPos );

    // get response from user in string form
    response = getInputString( xPos, yPos, "+-0123456789" );

    // convert string to integer
    numStringLength = response.length();
    index = 0; answer = 0;

    // check for negative sign, if anything entered
    if( ( numStringLength > 0 ) && ( response.at( index ) == '-' ) )
       {
        negFlag = true;
        index++;
       }

    // skip front end zeroes
    while( ( index < numStringLength ) 
             && ( ( response.at( index ) == '0' ) 
                    || ( response.at( index ) == '+' ) ) )
       {
        index ++;
       }

    // verify some number is still there
    if( index >= numStringLength )
       {
        return 0;
       }

    // load number
    while( index < numStringLength )
       {
        answer *= 10;

        answer += int( response.at( index ) - '0' );

        index++;
       }

    if( negFlag )
       {
        answer *= -1;
       }

    // return response
    return answer; 
   }

double promptForDoubleAt( int xPos, int yPos, const string &promptString )
   {
    // initialize function
    string response;
    int index, numStringLength;
    double answer, fractionDigit, multiplier = 1.00;
    bool negFlag = false;

    // print prompt string
    mvaddstr( yPos, xPos, promptString.c_str() );

    // move cursor to response point
    xPos += promptString.length();
    move( yPos, xPos );

    // get response from user in string form
    response = getInputString( xPos, yPos, ".+-0123456789" );

    // convert string to double
    numStringLength = response.length();
    index = 0; answer = 0;

    // check for negative sign, if anything entered
    if( ( numStringLength > 0 ) && ( response.at( index ) == '-' ) )
       {
        negFlag = true;
        index++;
       }

    // skip front end zeroes
    while( ( index < numStringLength ) && ( response.at( index ) == '0' ) )
       {
        index ++;
       }

    // verify some number is still there
    if( index >= numStringLength )
       {
        return 0.0;
       }

    // load major number
    while( ( index < numStringLength ) && ( response.at( index ) != '.' ) )
       {
        answer *= 10.00;

        answer += double( response.at( index ) - '0' );

        index++;
       }

    // skip decimal point if found
    index++;
    multiplier = 0.100;
    
    // add fractional value; protect from second decimal point
    while( ( index < numStringLength ) && ( response.at( index ) != '.' ) )
       {
        fractionDigit = double( response.at( index ) - '0' );
        fractionDigit *= multiplier;

        answer += fractionDigit;

        multiplier /= 10.00; index++;
       }

    if( negFlag )
       {
        answer *= -1.00;
       }

    // return response
    return answer; 
   }

string promptForStringAt( int xPos, int yPos, const string &promptString )
   {
    // initialize function
    string response;

    // print prompt string
    mvaddstr( yPos, xPos, promptString.c_str() );

    // move cursor to correct location
    xPos += promptString.length();
    move( yPos, xPos );

    // takes in all characters except single and double quotes
    response = getInputString( xPos, yPos, 
    "~!@#$%^&*()_+1234567890-=qwertyuiop[]QWERTYUIOP{} |asdfghjkl;ASDFGHJKL:zxcvbnm,./ZXCVBNM<>?" );

    // return response
    return response;
   }

void promptForStringAt( int xPos, int yPos, 
                              const string &promptString, char resultString[] )
   {
    // initialize function
    string response;

    // print prompt string
    mvaddstr( yPos, xPos, promptString.c_str() );

    // move cursor to correct location
    xPos += promptString.length();
    move( yPos, xPos );

    // takes in all characters except single and double quotes
    response = getInputString( xPos, yPos, 
    "~!@#$%^&*()_+1234567890-=qwertyuiop[]QWERTYUIOP{} |asdfghjkl;ASDFGHJKL:zxcvbnm,./ZXCVBNM<>?" );

    // set response
    strcpy( resultString, response.c_str() );   
   }

string getLineAt( int xPos, int yPos, int length )
   {
    string newString = "";
    chtype charVal;
    char newChar;

    while( length > 0 )
       {
        charVal = mvinch( yPos, xPos );

        newChar = ( charVal & A_CHARTEXT );

        newString += newChar;

        xPos++; length--;
       }

    return newString;
   }

char getCharAt( int xPos, int yPos )
   {
    return ( mvinch( yPos, xPos ) & A_CHARTEXT );
   }

void setColor( short foreGround, short backGround, bool bright )
   {
    // initialize function
    static short colorArr[ 8 ] [ 8 ];
    static bool initialized = false;
    short bgInit, fgInit, code = 0;

    // if initialization is called
    if( ( foreGround< 0 ) || ( backGround < 0 ) )
       {
        // protect from re-initialization
        if( initialized )
           {
            attron( COLOR_PAIR( 0 ) );
           }

        // initialize all combinations of fg/bg colors
        for( fgInit = 0; fgInit < 8; fgInit++ )
           {
            for( bgInit = 0; bgInit < 8; bgInit++ )
               {
                if( fgInit == bgInit )
                   {
                    // same fg and bg colors defaults to W on B
                    colorArr[ fgInit ] [ bgInit ] = 0;
                   }
                else
                   {
                    // store code and create a color pair
                    code++;
                    colorArr[ fgInit ] [ bgInit ] = code;
                    init_pair( code, fgInit, bgInit );
                   }
               }
           }

        // trigger one-time initialization flag
        initialized = true;

        // set codes to black on white
        foreGround = backGround = 0;
       }

    // set the color to the specified color code
    if( bright )
       {
        attron( COLOR_PAIR( colorArr[ foreGround ] [ backGround ] ) | A_BOLD );
       }
    else
       {
        attron( COLOR_PAIR( colorArr[ foreGround ] [ backGround ] ) );
       }
   }

void printCharAt( int xPos, int yPos, char charVal )
   {
    // output the character
    mvaddch( yPos, xPos, charVal );

    // update screen
    //refresh();
   }

void printIntAt( int xPos, int yPos, int intVal, const string &justify )
   {
    // initialize function
    char tempString[ MIN_STRING_SIZE ];

    // create string form of number
    sprintf( tempString, "%i", intVal );

    // output in string form
    printStringAt( xPos, yPos, tempString, justify );
   }

void printDoubleAt( int xPos, int yPos, double doubleVal, int precision, const string &justify )
   {
    // initialize function
    char tempString[ MIN_STRING_SIZE ];
    string doubleString = "%0.";

    // set precision in string terms
    doubleString += intToString( precision );
    doubleString += 'f';

    // create string form of decimal number, using precision string
    sprintf( tempString, doubleString.c_str(), doubleVal );

    // output in string form
    printStringAt( xPos, yPos, tempString, justify );
   }

void printStringAt( int xPos, int yPos, const string &outString, const string &justify )
   {
    // initialize function
    int stringLength = outString.length() - 1;

    // check for right justification
    if( justify == "RIGHT" )
       {
        xPos -= stringLength;
       }

    // check for center justification
    else if( justify == "CENTER" )
       {
        xPos -= ( stringLength / 2 );
       }

    // check for vertical justification
    if( ( justify == "UP" ) || ( justify == "DOWN" ) )
       {
        printStringVertical( xPos, yPos, outString, justify );
       }
    else // otherwise horizontal justification
       {
        // protect from printing off screen
        if( xPos < 0 )
           {
            xPos = 0;
           }

        // if no horizontal decisions were made, 
        //   then use "LEFT" justify default
        mvaddstr( yPos, xPos, outString.c_str() );
       }

    //refresh();
   }

void printStringVertical( int xStart, int yStart, const string &text, const string &orient )
   {
    int yLoc = yStart, adder = 1;
    unsigned index = 0;

    if( orient == "UP" )
       {
        adder = -1;
       }

    while( index < text.length() )
       {
        if( ( yLoc >= 0 ) && ( yLoc <= screenHeight() ) )
           {
            printCharAt( xStart, yLoc, text.at( index ) );
           }

        yLoc += adder; 
        index++;
       }
   }

void moveToXY( int xPos, int yPos )
   {
    bool legalXLoc = ( xPos >= 0 && xPos <= screenWidth() );
    bool legalYLoc = ( yPos >= 0 && yPos <= screenHeight() );

    if( legalXLoc && legalYLoc )
       {
        move( yPos, xPos );

        refresh();
       }
   }

int waitForInput( int timedWait )
   {
    // initialize function
    int response;

    // initialize keyboard wait
    halfdelay( DEFAULT_WAIT );

    // sets time constant to wait time
    if( timedWait > 0 )
       {
        halfdelay( timedWait );

        response = getch();

        halfdelay( DEFAULT_WAIT );
       }

    // provides (almost) immediate response
    else if( timedWait == 0 )
       {
        response = getch();
       }

    // waits for user to respond
    else
       {
        do
           {
            response = getch();
           }
        while(  response == NO_RESPONSE );
       }

    // covers KeyPad input
    switch( response )
       {
        case KP_PLUS:
           return '+';
           break;

        case KP_MINUS:
           return '-';
           break;

        case KP_SLASH:
           return '/';
           break;

        case KP_SPLAT:
           return '*';
           break;
       }

    return response;
   }

bool startCurses()
   {
    // Initialize the curses library
    initscr();
    //
    // Enable keyboard mapping
    keypad(stdscr, TRUE);
    //
    // Inhibit converting a newline into a carriage return
    //   and a newline on output
    nonl();
    //
    // Accept input characters without the [ENTER] key
    cbreak();
    //
    // Inhibit input echoing 
    //   (i.e., key pressed will not be shown on the screen)
    noecho();
    //
    // Forces wait until available character
    //   or time delay (of parameter - 0.1 second),
    //   whichever comes first; replaced by function
    //   halfdelay( STANDARD_WAIT );
    halfdelay( DEFAULT_WAIT );

    // test for color problems with console
    if( ( !has_colors() ) || ( start_color() == ERR ) )
       {
        return false;
       }

    // initialize the color set
    setColor( -1, -1, false );

    // if everything worked, return success (true)
    return true;
   }

void endCurses()
   {
    // Shuts down curses interface
    endwin();
   }

string getInputString( int xPos, int yPos, const string &allowedChars )
   {
    // initialize function
    const char SPACE = ' ';
    const char NULL_CHARACTER = '\0';
    int response, index = 0;
    char inputString[ MAX_INPUT_LENGTH ];

    // initialize string in case nothing is entered
    inputString[ 0 ] = '\0';

    // repeatedly capture & process input characters
    // stop when ENTER key is pressed
    do
       {
        // place the cursor
        move( yPos, xPos + index );

        // get the input as an integer
        response = waitForInput( FIXED_WAIT );

        // accept key pad input if used
        switch( response )
           {
            case 465:
               response = '+';
               break;

            case 464:
               response = '-';
               break;

            case 463:
               response = '*';
               break;

            case 458:
               response = '/';
               break;
           }

        // if it is in the allowed list of characters,
        // accept and print it
        if( charInString( response, allowedChars.c_str() ) )
           {
            inputString[ index ] = response;
            inputString[ index + 1 ] = NULL_CHARACTER;

            mvaddch( yPos, xPos + index, response );

            index++;
           }

        // if it is the backspace key, act on it
        else if( response == BACKSPACE_KEY || response == BACKSPACE_KEY_LINUX )
           {
            if( index >= 0 )
               {
                if( index > 0 )
                   {
                    index--;
                   }

                inputString[ index ] = NULL_CHARACTER;

                mvaddch( yPos, xPos + index, SPACE );
               }
           } 
       }
    while( ( response != ENTER_KEY ) & ( response != KP_ENTER_KEY ) );

    // return the generated string
    return string( inputString );
   }

bool charInString( char testChar, const string &testString )
   {
    // initialize function
    int stringLength = testString.length();
    int index = 0;

    // search through string to find test character    
    while( index < stringLength )
       {
        // if character is in string, return true
        if( testChar == testString.at( index ) )
           {
            return true;
           }

        index++;
       }

    // if characater not in string, return false
    return false;
   }

string intToString( int value )
   {
    // if there are digits left, reduce the value by a factor of 10
    // and pick off the next digit
    // repeat until all digits are used
    if( value > 0 )
      {
       return intToString( value / 10 ) 
                + char( ( value % 10 ) + '0' );
      }

    return "";
   }

// End Curses I/O header file
#endif 

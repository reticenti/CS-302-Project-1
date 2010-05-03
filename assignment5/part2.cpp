#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "U_PQType.h"

using namespace std;

enum menuChoice{ENQ, DEL, DEQ, PRINT, UPDATE, EXIT};

bool readFile( string fileName, U_PQType<int>* &tree);
//void storeTree( U_PQType<int>& tree, oType order, string fileName );
menuChoice promptForMenu();

int main()
{
	// holds the menu choice
	menuChoice val;

	// temporary object for getting user input
	int tmp;	
	string fileName;



	system("clear");

	// this is the binary tree that holds all the users
	U_PQType<int>* userTree;


	int size;
	// prompt for valid filename
	do {
		cout << "Please enter a filename: ";
		cin >> fileName;
	} while ( !readFile( fileName, userTree) );
	//

	do {
		val = promptForMenu();

		switch ( val )
		{
			case ENQ:
				cout << "Enter new number: ";
				cin >> tmp;

				userTree->enqueue(tmp);

				cout << "Number successfully added!" << endl;

				cin.get();
				cout << "Press Enter to continue";
				cin.get();

				break;
			case DEQ:
				userTree->dequeue(tmp);
				cout << "The number dequeued is " << tmp << endl;

				cin.get();
				cout << "Press Enter to continue";
				cin.get();
				break;
			case DEL:
				// prompt for a name to remove
				cout << "Enter Number To Remove: ";
				cin >> tmp;

				// remove the name
				userTree->Remove(tmp);

				cout << tmp << " has been removed" << endl;

				cin.get();
				cout << "Press Enter to continue";
				cin.get();

				break;
			case UPDATE:
				// prompt for a name to update
				cout << "Enter Number To update: ";
				cin >> tmp;

				int updated;

				cout << "Enter the Number to have it changed to: ";
				cin >> updated;

				// remove the name
				userTree->Update(tmp, updated);

				cout << tmp << " has been changed to " << updated <<  endl;

				cin.get();
				cout << "Press Enter to continue";
				cin.get();

				break;
			case PRINT:
				system("clear");
				//userTree->printTree( cout );
				cin.get();
				cout << "Press Enter to continue";
				cin.get();
				break;
			case EXIT:
				break;
		}
	} while ( val < EXIT );

	cout << "Goodbye!" << endl;
	cin.get();
	cout << "Press Enter to continue";
	cin.get();

	system("clear");

	return 0;
}

bool readFile( string fileName, U_PQType<int>* &tree)
{
	bool retVal;

	ifstream fin(fileName.c_str());

	// return false if file doesn't open
	retVal = fin.good();

	if ( retVal )
	{
		int tmp;
		int loc;
		int size = 0;

		// count number of users in file
		while ( fin.good() )
		{
			fin >> tmp;
			if ( fin.good() )
				size++;
		}

		tree = new U_PQType<int>(size);

		// close file
		fin.close();

		// make a list of numbers to insert into tree in random order
		int* numbers = new int[size];
		bool* numSet = new bool[size];

		// reopen file to read again
		fin.open(fileName.c_str());

		for ( int i = 0; i < size; i++ )
		{
			fin >> tmp;
			numbers[i] = tmp;
			numSet[i] =  false;
		}

		fin.close();

		// insert into tree in random order
		for ( int i = 0; i < size; i++ )
		{
			// find random locations and insert into tree at random
			do {
				loc = rand() % size;
			} while ( numSet[loc] );
			numSet[loc] = true;
			tree->enqueue( numbers[loc] );	
		}

		// free memory that won't be used again
		delete [] numbers;
		delete [] numSet;
	}		

	// return true if file opened
	return retVal;
}

/*void storeTree( binaryTree<user>& tree, oType order, string fileName )
  {
  user temp;

// reset the tree in all orders and traverse
tree.resetTree( order );

ofstream fout(fileName.c_str());

// traverse tree in selected order
if ( !tree.isEmpty() )
{
while ( !tree.getNextItem( temp, order ) )
fout << temp << endl;
fout << temp << endl;
}

fout.close();
}*/

menuChoice promptForMenu()
{
	int val;
	menuChoice retVal;

	system("clear");

	// display menu
	cout << "1) Enqueue\n2) Dequeue\n3) Delete\n4) Update\n5) Print\n"
		"6) Exit\n\nEnter choice: ";

	// get input value
	cin >> val;

	// loop until val is valid
	while ( val < 1 || val > 6 )
	{
		cout << "Invalid choice!\nEnter choice: ";
		cin >> val;
	}

	switch (val)
	{
		case 1:
			retVal = ENQ;
			break;	
		case 2:
			retVal = DEQ;
			break;	
		case 3:
			retVal = DEL;
			break;	
		case 4:
			retVal = UPDATE;
			break;	
		case 5:
			retVal = PRINT;
			break;	
		case 6:
			retVal = EXIT;
			break;	
	}

	return retVal;
}


#include <fstream>
#include <cstdlib>
#include <ctime>
#include "binaryTree.h"
#include "user.h"

using namespace std;

enum menuChoice{ADD, DEL, VERIFY, PRINT, IN_ORD, PRE_ORD, POST_ORD, EXIT};

bool readFile( string fileName, binaryTree<user>& tree );
void storeTree( binaryTree<user>& tree, oType order, string fileName );
menuChoice promptForMenu();

int main()
{
	// holds the menu choice
	menuChoice val;

	// temporary object for getting user input
	user temp;
	string fileName, name, pass;

	// this is the binary tree that holds all the users
	binaryTree<user> userTree;

	system("clear");

	// prompt for valid filename
	do {
		cout << "Please enter a filename: ";
		cin >> fileName;
	} while ( !readFile( fileName, userTree ) );

	do {
		val = promptForMenu();

		switch ( val )
		{
			case ADD:
				cout << "Enter new username: ";
				cin >> name;
				cout << "Enter new password: ";
				cin >> pass;
				temp.setName(name);
				temp.setPass(pass);		
				
				userTree.insertItem( temp );

				cout << "User successfully added!" << endl;
				
				cin.get();
				cout << "Press Enter to continue";
				cin.get();

				break;
			case DEL:
				// prompt for a name to remove
				cout << "Enter Name To Remove: ";
				cin >> name;

				// remove the name
				temp.setName(name);
				if ( userTree.retrieveItem( temp ) )
				{
					userTree.deleteItem( temp );
					cout << "User successfully removed!" << endl;
				}
				else
					cout << "ERROR! " << name << " doesn't exist in tree" << endl;

				cin.get();
				cout << "Press Enter to continue";
				cin.get();

				break;
			case VERIFY:
				cout << "Enter username: ";
				cin >> name;
				cout << "Enter password: ";
				cin >> pass;

				temp.setName(name);
				if ( userTree.retrieveItem( temp ) )
				{
					if ( temp.getPass().compare(pass) == 0 )
						cout << "User Verified!" << endl;
					else
						cout << "Invalid Password!" << endl;
				}
				else
				cout << "Inalid Username!" << endl;

				cin.get();
				cout << "Press Enter to continue";
				cin.get();
				break;
			case PRINT:
				system("clear");
				userTree.printTree( cout );
				cin.get();
				cout << "Press Enter to continue";
				cin.get();
				break;
			case IN_ORD:
				cout << "Enter output file name: ";
				cin >> fileName;
				storeTree( userTree, IN_ORDER, fileName );
				cout << fileName << " has been written!";
				break;
			case PRE_ORD:
				cout << "Enter output file name: ";
				cin >> fileName;
				storeTree( userTree, PRE_ORDER, fileName );
				cout << fileName << " has been written!";
				break;
			case POST_ORD:
				cout << "Enter output file name: ";
				cin >> fileName;
				storeTree( userTree, POST_ORDER, fileName );
				cout << fileName << " has been written!";
				break;
			case EXIT:
				break;
		}
	} while ( val <= PRINT );

	cout << "Goodbye!" << endl;
	cin.get();
	cout << "Press Enter to continue";
	cin.get();

	system("clear");

	return 0;
}

bool readFile( string fileName, binaryTree<user>& tree )
{
	bool retVal;
	
	ifstream fin(fileName.c_str());

	// return false if file doesn't open
	retVal = fin.good();

	if ( retVal )
	{
		user temp;
		string name, pass;
		int size = 0, loc;

		// count number of users in file
		while ( fin.good() )
		{
			fin >> name >> pass;
			if ( fin.good() )
				size++;
		}

		// close file
		fin.close();

		// make a list of users to insert into tree in random order
		user* allUser = new user[size];
		bool* userSet = new bool[size];

		// reopen file to read again
		fin.open(fileName.c_str());

		for ( int i = 0; i < size; i++ )
		{
			fin >> name >> pass;
			temp.setName( name );
			temp.setPass( pass );
			// read into an array
			allUser[i] = temp;
			userSet[i] = false;
		}

		fin.close();

		// insert into tree in random order
		for ( int i = 0; i < size; i++ )
		{
			// find random locations and insert into tree at random
			do {
				loc = rand() % size;
			} while ( userSet[loc] );
			userSet[loc] = true;
			tree.insertItem( allUser[loc] );	
		}

		// free memory that won't be used again
		delete [] allUser;
		delete [] userSet;
	}		

	// return true if file opened
	return retVal;
}

void storeTree( binaryTree<user>& tree, oType order, string fileName )
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
}

menuChoice promptForMenu()
{
	int val;
	menuChoice retVal;

	system("clear");

	cout << "1) Add new user\n2) Delete user\n3) Verify User\n4) Print user\n5) St"
			"ore data in file using inorder traversal\n6) Store data in file using"
			" preorder traversal\n7) Store data in file using postorder traveral\n"
			"8) Exit without saving\n\nEnter choice: ";

	cin >> val;
	while ( val < 1 || val > 8 )
	{
		cout << "Invalid choice!\nEnter choice: ";
		cin >> val;
	}

	switch (val)
	{
		case 1:
			retVal = ADD;
			break;	
		case 2:
			retVal = DEL;
			break;	
		case 3:
			retVal = VERIFY;
			break;	
		case 4:
			retVal = PRINT;
			break;	
		case 5:
			retVal = IN_ORD;
			break;	
		case 6:
			retVal = PRE_ORD;
			break;	
		case 7:
			retVal = POST_ORD;
			break;	
		case 8:
			retVal = EXIT;
			break;	
	}

	return retVal;
}


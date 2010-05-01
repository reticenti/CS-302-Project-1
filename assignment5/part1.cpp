#include "binaryTree.h"
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

class user
{
public:
	string getName() const
	{ return name; }
	string getPass() const
	{ return pass; }
	void setName( string& rhs )
	{ name = rhs; }
	void setPass( string& rhs )
	{ pass = rhs; }
	bool operator>( const user& rhs )
	{ return (name.compare(rhs.name) > 0); }
	bool operator<( const user& rhs )
	{ return (name.compare(rhs.name) < 0); }
	bool operator>=( const user& rhs )
	{ return (name.compare(rhs.name) >= 0); }
	bool operator<=( const user& rhs )
	{ return (name.compare(rhs.name) <= 0); }
	bool operator==( const user& rhs )
	{ return (name.compare(rhs.name) == 0); }

	friend ostream& operator<<( ostream &out, user& rhs );
private:
	string name;
	string pass;
};

ostream& operator<<( ostream &out, user& rhs )
{
	out << rhs.getName() << ' ' << rhs.getPass(); 
	return out;
}

int main()
{
	binaryTree<user> myTree;

	user temp;
	string name, pass;

	int size = 0, loc;

	ifstream fin("UserIdPasswd");

	// count number of users in file
	while ( fin.good() )
	{
		fin >> name >> pass;
		if ( fin.good() )
			size++;
	}
	fin.close();

	// make a list of users to insert into tree in random order
	user* allUser = new user[size];
	bool* userSet = new bool[size];

	fin.open("UserIdPasswd");

	for ( int i = 0; i < size; i++ )
	{
		fin >> name >> pass;
		temp.setName( name );
		temp.setPass( pass );
		allUser[i] = temp;
		userSet[i] = false;
	}

	fin.close();

	// insert into tree in random order
	for ( int i = 0; i < size; i++ )
	{
		do {
			loc = rand() % size;
		} while ( userSet[loc] );
		userSet[loc] = true;
		myTree.insertItem( allUser[loc] );	
	}

	// free memory that won't be used again
	delete [] allUser;
	delete [] userSet;
	
	// reset the tree in all orders and traverse
	myTree.resetTree( PRE_ORDER );
	myTree.resetTree( POST_ORDER );
	myTree.resetTree( IN_ORDER );
	
	cout << "Pre Order: " << endl;
	while ( !myTree.getNextItem( temp, PRE_ORDER ) )
		cout << temp << " ";
	cout << temp;
	cout << endl << endl;

	cout << "In Order: " << endl;
	while ( !myTree.getNextItem( temp, IN_ORDER ) )
		cout << temp << " ";
	cout << temp;
	cout << endl << endl;

	cout << "Post Order: " << endl;
	while ( !myTree.getNextItem( temp, POST_ORDER ) )
		cout << temp << " ";
	cout << temp;
	cout << endl << endl;
	
	// prompt for a name to remove
	cout << "Enter Name To Remove: ";
	cin >> name;

	// remove the name
	temp.setName(name);
	if ( myTree.retrieveItem( temp ) )
		myTree.deleteItem( temp );
	else
		cout << "ERROR! " << name << " doesn't exist in tree" << endl;

	// print the tree in order using the printTree member function
	myTree.printTree( cout );

	cout << endl;

	return 0;
}

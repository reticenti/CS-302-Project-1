#include "binaryTree.h"
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
	binaryTree<int> myTree;
	int p;

	srand( time( 0 ) );

	for ( int i = 0; i < 100; i++ )
		myTree.insertItem( rand() % 100 );

	myTree.resetTree( PRE_ORDER );
	myTree.resetTree( POST_ORDER );
	myTree.resetTree( IN_ORDER );
	
	cout << "Pre Order: " << endl;
	while ( !myTree.getNextItem( p, PRE_ORDER ) )
		cout << p << " ";
	cout << p;
	cout << endl << endl;

	cout << "In Order: " << endl;
	while ( !myTree.getNextItem( p, IN_ORDER ) )
		cout << p << " ";
	cout << p;
	cout << endl << endl;

	cout << "Post Order: " << endl;
	while ( !myTree.getNextItem( p, POST_ORDER ) )
		cout << p << " ";
	cout << p;
	cout << endl << endl;
	
	cout << "Enter Item Number To Remove: ";
	cin >> p;

	if ( myTree.retrieveItem( p ) )
		myTree.deleteItem( p );
	else
		cout << "ERROR! " << p << " doesn't exist in tree" << endl;

	myTree.printTree( cout );

	cout << endl;

	return 0;
}

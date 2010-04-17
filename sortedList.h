#ifndef SORTEDLIST
#define SORTEDLIST

#ifndef NODE
#define NODE

// node is the node type used in the sorted list class
template <class T>
struct node
{
	T val;
	node<T>* next;
};

#endif // NODE

// used for NULL constant
#include <cstddef>

// simple linked sorted list class
template <class T>
class sortedList
{
public:
	sortedList();
	sortedList( const sortedList<T>& );
	~sortedList() { makeEmpty(); }

	int getLength() { return length; }
	void makeEmpty();
	bool retrieveItem( T& );
	void insertItem( T );
	void deleteItem( T );
	void reset() { currentPos = NULL; }
	bool atEnd();
	T getNextItem();

	sortedList<T>& operator=(const sortedList<T>&);
	
private:
	node<T>* listData;
	int length;
	node<T>* currentPos;
};

template <class T>
sortedList<T>::sortedList()
{
	listData = NULL;
	currentPos = NULL;
	length = 0;
}

template <class T>
sortedList<T>::sortedList( const sortedList<T>& rhs )
{
	// use overloaded operator=
	*this = rhs;
}

template <class T>
void sortedList<T>::makeEmpty()
{
	node<T>* temp;

	while ( listData != NULL )
	{
		temp = listData;
		listData = listData->next;
		delete temp;
	}

	length = 0;
}

template <class T>
bool sortedList<T>::retrieveItem( T& item )
{
	// temp used to traverse list
	node<T>* temp = listData;

	while ( temp != NULL )
	{
		// if equal item found stop here
		if ( item == temp->val )
		{
			item = temp->val;
			return true;
			}
		else if ( item < temp->val )
			return false;	// passed the location where it should be

		temp = temp->next;
	}

	// if it gets here the item was not found
	return false;
}

template <class T>
void sortedList<T>::insertItem( T item )
{
	node<T>* newNode = new node<T>;
	node<T>* predLoc = NULL;
	node<T>* loc = listData;
	bool loop = (loc != NULL);

	while (loop)
	{
		if ( item > loc->val )
		{
			predLoc = loc;
			loc = loc->next;
			loop = (loc != NULL);
		}
		else	// insert item here
			loop = false;
	}

	newNode->val = item;

	if ( predLoc == NULL )	// if insert at first
	{
		newNode->next = listData;
		listData = newNode;
	}
	else
	{
		newNode->next = loc;
		predLoc->next = newNode;
	}

	length++;
}

template <class T>
void sortedList<T>::deleteItem( T item )
{
	// precond: item is in list

	node<T>* loc = listData;
	node<T>* temp;

	if ( item == loc->val )
	{
		temp = loc;
		listData = listData->next;
	}
	else
	{
		while ( !(item == (loc->next)->val) )
			loc = loc->next;

		temp = loc->next;
		loc->next = (loc->next)->next;
	}
	delete temp;
	length--;
}

template <class T>
T sortedList<T>::getNextItem()
{
	if ( currentPos == NULL )
		currentPos = listData;
	else
		currentPos = currentPos->next;

	return currentPos->val;
}

template <class T>
sortedList<T>& sortedList<T>::operator=(const sortedList<T>& rhs)
{
	node<T>* rVal = rhs.listData;
	node<T>* tempNew;
	node<T>* prev;

	// clear old list
	makeEmpty();

	// if list has something in it
	if ( rVal != NULL )
	{
		listData = new node<T>;
		listData->val = rVal->val;

		tempNew = listData;
		prev = tempNew;

		rVal = rVal->next;

		// traverse the right hand list
		while ( rVal != NULL )
		{
			// make a new node
			tempNew = new node<T>;

			// set value of new node
			tempNew->val = rVal->val;

			// make the previous node point to it
			prev->next = tempNew;

			// update the previous node to the current node
			prev = tempNew;

			// go to next node
			rVal = rVal->next;
		}

		// make the end of the list point to NULL
		tempNew->next = NULL;
	}
	else
		listData = NULL;
	
	// set the other stuff
	currentPos = NULL;
	length = rhs.length;

	return *this;
}

template <class T>
bool sortedList<T>::atEnd()
{
	if ( currentPos != NULL )
		return ( currentPos->next == NULL );
	return (length == 0);
}

#endif // SORTEDLIST


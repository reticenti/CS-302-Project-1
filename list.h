#ifndef LIST
#define LIST

#ifndef NODE
#define NODE

// node is the node type used in the list class
template <class T>
struct node
{
	T val;
	node<T>* next;
};

#endif // NODE

// used for NULL constant
#include <cstddef>

// simple linked list class
template <class T>
class list
{
public:
	list();
	list( const list<T>& );
	~list() { makeEmpty(); }

	int getLength() const { return length; }
	void makeEmpty();
	bool retrieveItem( T& );
	void insertItem( T );
	void deleteItem( T );
	void reset() { currentPos = NULL; }
	bool atEnd();
	bool isEmpty() { return (listData == NULL); }
	T getNextItem();

	list<T>& operator=(const list<T>&);
private:
	node<T>* listData;  // head node
	int length;
	node<T>* currentPos;   // current Position
};

template <class T>
list<T>::list()
{
	listData = NULL;
	currentPos = NULL;
	length = 0;
}

template <class T>
list<T>::list( const list<T>& rhs )
{
	// use operator= overload
	*this = rhs;
}

template <class T>
void list<T>::makeEmpty()
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
bool list<T>::retrieveItem( T& item )
{
	// temp used to traverse list
	node<T>* temp = listData;

	while ( temp != NULL )
	{
		// if equal item found, stop here
		if ( item == temp->val )
		{
			item = temp->val;
			return true;
		}

		temp = temp->next;
	}

	// didn't find item by the end of the list
	return false;
}

template <class T>
void list<T>::insertItem( T item )
{
	node<T>* temp;

	temp = new node<T>;

	temp->val = item;
	temp->next = listData;

	listData = temp;
	length++;
}

template <class T>
void list<T>::deleteItem( T item )
{
	// precond: item is in list

	node<T>* temp = listData;
	node<T>* prev;

	if ( item == temp->val )
	{
		prev = temp;
		listData = listData->next;
	}
	else
	{
		while ( !(item == (temp->next)->val))
			temp = temp->next;

		// node found at this point, delete it
		prev = temp->next;
		temp->next = (temp->next)->next;
	}
	
	delete prev;
	length--;
}

template <class T>
T list<T>::getNextItem()
{
	if ( currentPos == NULL )
		currentPos = listData;
	else
		currentPos = currentPos->next;

	return currentPos->val;
}

template <class T>
list<T>& list<T>::operator=(const list<T>& rhs)
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
bool list<T>::atEnd()
{
	if ( currentPos != NULL )
		return ( currentPos->next == NULL );
	return (length == 0);
}

#endif // LIST


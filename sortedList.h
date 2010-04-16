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
private:
	node<T>* frontNode;  // node at the front of the list
	node<T>* rearNode;   // node at the rear of the list
};

template <class T>
sortedList<T>::sortedList()
{
	frontNode = NULL;
	rearNode = NULL;
}

#endif // SORTEDLIST


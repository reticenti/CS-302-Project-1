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
private:
	node<T>* frontNode;  // node at the front of the list
	node<T>* rearNode;   // node at the rear of the list
};

template <class T>
list<T>::list()
{
	frontNode = NULL;
	rearNode = NULL;
}

#endif // LIST


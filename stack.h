#ifndef STACK
#define STACK

#ifndef NODE
#define NODE

// node is the node type used in the stack class
template <class T>
struct node
{
	T val;
	node<T>* next;
};

#endif

// used for NULL constant
#include <cstddef>

// simple linked stack class
template <class T>
class stack
{
public:
	stack();
	~stack();
	bool empty() const;
	int size();
	T& top(); 
	void push(const T&);
	void pop();
	void makeEmpty();

private:
	//int top;
	node<T>* topPtr;

};

template <class T>
stack<T>::stack()
{
	topPtr = NULL;
}

template <class T>
stack<T>::~stack()
{
	makeEmpty();
}

template <class T>
void stack<T>::makeEmpty()
{
	node<T>* tempPtr;

	while ( topPtr != NULL )
	{
		tempPtr = topPtr;
		topPtr = topPtr->next;
		delete tempPtr;
	}
	
	topPtr = NULL;
}

template <class T>
bool stack<T>::empty() const
{
	return (topPtr == NULL);
}

template <class T>
void stack<T>::push( const T& item )
{
	node<T>* newNode = new node<T>;

	newNode->val = item;
	newNode->next = topPtr;
	topPtr = newNode;
}

template <class T>
void stack<T>::pop()
{
	node<T>* tmpPtr;
	tmpPtr = topPtr;
	topPtr = topPtr->next;
	delete tmpPtr;
}

template <class T>
T& stack<T>::top()
{
	return topPtr->val;
}

#endif


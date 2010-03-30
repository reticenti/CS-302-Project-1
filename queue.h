#ifndef QUEUE
#define QUEUE

#ifndef NODE
#define NODE

// node is the node type used in the queue class
template <class T>
struct node
{
	T val;
	node<T>* next;
};

#endif

// used for NULL constant
#include <cstddef>


// simple linked queue class
template <class T>
class queue
{
public:
	queue();
	~queue();
	void makeEmpty();    // clear the queue
	bool empty() const;
	void push(const T&); // push to the rear of the queue
	void pop();          // pop off the front of the queue
	T& front();          // return the value in the front
	const T& front() const;
	T& back();           // return the value in the rear
	const T& back() const;
private:
	node<T>* frontNode;  // node at the front of the queue
	node<T>* rearNode;   // node at the rear of the queue
};

template <class T>
queue<T>::queue()
{
	frontNode = NULL;
	rearNode = NULL;
}

template <class T>
queue<T>::~queue()
{
	makeEmpty();
}

template <class T>
void queue<T>::makeEmpty()
{
	node<T>* tempPtr;

	while ( frontNode != NULL )
	{
		tempPtr = frontNode;
		frontNode = frontNode->next;
		delete tempPtr;
	}
	
	rearNode = NULL;
}

template <class T>
bool queue<T>::empty() const
{
	return (frontNode == NULL);
}

template <class T>
void queue<T>::push( const T& item )
{
	node<T>* newNode = new node<T>;

	newNode->val = item;
	newNode->next = NULL;
	if ( rearNode == NULL )
		frontNode = newNode;
	else
		rearNode->next = newNode;
	rearNode = newNode;
}

template <class T>
void queue<T>::pop()
{
	node<T>* tempPtr = frontNode;
	frontNode = frontNode->next;
	if ( frontNode == NULL )
		rearNode = NULL;
	delete tempPtr;
}

template <class T>
T& queue<T>::front()
{
	return frontNode->val;
}

template <class T>
const T& queue<T>::front() const
{
	return frontNode->val;
}

template <class T>
T& queue<T>::back()
{
	return rearNode->val;
}

template <class T>
const T& queue<T>::back() const
{
	return rearNode->val;
}

#endif


#include "heap.h"

template <class ItemType>
class PQType
{
	public:
		PQType(int);
		~PQType();
		void makeEmpty();
		bool isEmpty() const;
		bool isFull() const;
		void enqueue(ItemType newItem);
		void dequeue(ItemType& item);
	private:
		int numItems;
		HeapType<ItemType> items;
		int maxItems;
};

template <class ItemType>
PQType<ItemType>::PQType(int max)
{
	maxItems = max;
	items.elements = new ItemType[max];
	numItems = 0;
}

template <class ItemType>
PQType<ItemType>::~PQType()
{
	delete [] items.elements;
}

template <class ItemType>
void PQType<ItemType>::makeEmpty()
{
	numItems = 0;
}

template <class ItemType>
void PQType<ItemType>::dequeue(ItemType& item)
{
	item = items.elements[0];
	items.elements[0] = items.elements[numItems-1];
	numItems--;
	items.reheapDown(0, numItems-1);
}

template <class ItemType>
void PQType<ItemType>::enqueue(ItemType newItem)
{
	numItems++;
	items.elements[numItems-1] = newItem;
	items.reheapUp(0, numItems-1);
}

template <class ItemType>
bool PQType<ItemType>::isFull() const
{
	return numItems == maxItems;
}

template <class ItemType>
bool PQType<ItemType>::isEmpty() const
{
	return numItems == 0;
}



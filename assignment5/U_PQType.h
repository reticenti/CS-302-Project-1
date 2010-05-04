#include "pqueue.h"
#include <iostream>

template <class ItemType>
class U_PQType: public PQType<ItemType>
{
	public:
		U_PQType(int);
		void Remove(ItemType);
		void Update(ItemType, ItemType);
		void printTree(std::ostream&);
};

template <class ItemType>
U_PQType<ItemType>::U_PQType(int max) : PQType<ItemType>(max)
{ }

template <class ItemType>
void U_PQType<ItemType>::printTree(std::ostream& out )
{
	U_PQType<ItemType> list(PQType<ItemType>::maxItems);
	ItemType tmp;

	while(!PQType<ItemType>::isEmpty()){
		PQType<ItemType>::dequeue(tmp);
		out << tmp << std::endl;
		list.enqueue(tmp);
	}
	while(!list.isEmpty()){
		list.dequeue(tmp);
		PQType<ItemType>::enqueue(tmp);
	}
}

template <class ItemType>
void U_PQType<ItemType>::Remove(ItemType item)
{
	U_PQType<ItemType> list(PQType<ItemType>::maxItems);
	ItemType tmp;

	while(!PQType<ItemType>::isEmpty()){	// N
		PQType<ItemType>::dequeue(tmp);		// log(N)
		if(tmp != item)
			list.enqueue(tmp);				// log(N)
	}

	while(!list.isEmpty()){					// N
		list.dequeue(tmp);					// log(N)
		PQType<ItemType>::enqueue(tmp);		// log(N)
	}

}

template <class ItemType>
void U_PQType<ItemType>::Update(ItemType item, ItemType newItem)
{

	U_PQType<ItemType> list(PQType<ItemType>::maxItems);
	ItemType tmp;

	while(!PQType<ItemType>::isEmpty()){	// N
		PQType<ItemType>::dequeue(tmp);		// log(N)
		if(tmp == item)
			tmp = newItem;
		list.enqueue(tmp);					// log(N)
	}
	while(!list.isEmpty()){					// N
		list.dequeue(tmp);					// log(N)
		PQType<ItemType>::enqueue(tmp);		// log(N)
	}
}

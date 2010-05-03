#include "pqueue.h"

template <class ItemType>
class U_PQType: public PQType<ItemType>
{
	public:
		U_PQType(int);
		~U_PQType();
		void Remove(ItemType);
		void Update(ItemType, ItemType);

};

template <class ItemType>
U_PQType<ItemType>::U_PQType(int max) : PQType<ItemType>(max)
{ }


template <class ItemType>
U_PQType<ItemType>::~U_PQType()
{
	delete [] PQType<ItemType>::items.elements;
}

template <class ItemType>
void U_PQType<ItemType>::Remove(ItemType item)
{
	U_PQType<ItemType>:: list(PQType<ItemType>::maxItems);
	ItemType tmp;

	while(!PQType<ItemType>::isEmpty()){
		PQType<ItemType>::dequeue(tmp);
		if(tmp != item)
			PQType<ItemType>::list.enqueue(tmp);
	}

	delete PQType<ItemType>::items;
	PQType<ItemType>::items = PQType<ItemType>::list;
}

template <class ItemType>
void U_PQType<ItemType>::Update(ItemType item, ItemType newItem)
{

	U_PQType<ItemType> list(PQType<ItemType>::maxItems);
	ItemType tmp;

	while(!PQType<ItemType>::isEmpty()){
		PQType<ItemType>::dequeue(tmp);
		if(tmp == item)
			tmp = newItem;
		PQType<ItemType>::list.enqueue(tmp);
	}

	delete PQType<ItemType>::items;
	PQType<ItemType>::items = PQType<ItemType>::list;
}

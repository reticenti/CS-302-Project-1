#include "pqueue.h"

template <class ItemType>
class U_PQType: public PQType
{
	public:
		void Remove(ItemType);
		void Update(ItemType, ItemType);

};

template <class ItemType>
void U_PQType<ItemType>::Remove(ItemType item)
{
	
}

template <class ItemType>
void U_PQType<ItemType>::Update(ItemType item, ItemType newItem)
{

	U_PQType list<ItemType> = new U_PQType[PQType::maxItems];
	ItemType tmp;

	while(!isEmpty()){
		dequeue(tmp);
		if(tmp == item)
			tmp = newItem;
		list.enqueue(tmp);
	}

	delete items;
	items = list;
}

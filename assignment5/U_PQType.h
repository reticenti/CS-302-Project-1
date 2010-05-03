#include "pqueue.h"

template <class ItemType>
class U_PQType: protected PQType
{
	public:
		Void Remove(ItemType);
		void Update(ItemType, ItemType);

}

tempalte <class ItemType>
void U_PQType<ItemType>::Remove(ItemType item)
{
	
}

tempalte <class ItemType>
void U_PQType<ItemType>::Update(ItemType item, ItemType newItem)
{

	U_PQType list = new U_PQType[maxItems];
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

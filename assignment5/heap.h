template <class ItemType>
struct HeapType
{

	void reheapDown(int root, int bottom);
	void reheadUp(int root, int bottom);
	void swap(ItemType &a, ItemType &b);
	ItemType * elements;
	int numElements;
}


template <class ItemType>
void swap(ItemTypei &a, ItemType &b)
{

	ItemType c = a;
	a = b;
	b = c;
}

template <class ItemType>
void HeadType<ItemType>::reheapDown(int root, int bottomr)
{

	int maxChild, rightChild, leftChild;

	leftChild = root*2+1;
	rightchild = root*2+2

		if(leftChild <= bottom)
		{
			if(leftChild == bottom)
				maxChild = leftChild;
			else
			{
				if(elements[leftChild] <= elements[rightChild])
					maxChild = rightChild;
				else 
					maxChild = leftChild;
			}
			if (elements[root] < elements[maxChild] )
			{
				swap(elements[root], elements[maxChild]);
				reheapDown(maxChild, bottom);
			}

		}

}

template <class ItemType>
void HeadType<itemType>::reheapUp(int root, int bottom)
{

	int parent;

	if (bottom > root)
	{
		parent = (bottom - 1) / 2;
		if(elements[parent] < elements[bottom])
		{
			swap(elements[parent], elements[bottom]);
			reheapUp(root, parent);
		}
	}
}

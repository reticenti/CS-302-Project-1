#include <iostream>
#include "U_PQType.h"

using namespace std;

int main()
{

	U_PQType<int> test(5);

	int tmp;

	test.enqueue(5);

	test.dequeue(tmp);

	cout << tmp;

	return 0;

}

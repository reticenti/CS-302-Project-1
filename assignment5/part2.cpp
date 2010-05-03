#include <iostream>
#include "U_PQType.h"

using namespace std;

int main()
{

	U_PQType<char> test(5);

	char tmp;
	char b = 'b', c = 'c';

	test.enqueue(b);

	test.Update(b, c);

	test.dequeue(tmp);

	cout << tmp;

	return 0;

}

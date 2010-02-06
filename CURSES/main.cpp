#include "cubicSpline.h"

using namespace std;

int main()
{
	cubicSpline first;

//	int nums[5] = {8, 11, 15, 18, 12};
	int nums[201];
	for ( int i = 0; i < 201; i++ )
		nums[i] = i%2 * 255;

	first.createCubic(nums, 201);
//	first.createCubic( nums, 5 );

	for ( double i = 0.0; i <= 100.0; i+=0.5 )
		cout << first.getCubicVal(i) << " --> " << i << endl;
//	for ( int i = 0; i < 5; i++ )
//		cout << first.a[i] << "[" << i << "], ";

	return 0;
}


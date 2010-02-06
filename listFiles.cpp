#include <dirent.h>
#include <cstring>
#include <iostream>

using namespace std;

int main()
{
	struct dirent **namelist;
	char** filenames;
	int n;
	int count = 0;

	n = scandir( ".", &namelist, 0, alphasort );

	if ( n > 0 )
	{
		for ( int i = 0; i < n; i++ )
		{
			static int len;
			len = strlen( namelist[i]->d_name );
			if ( len > 5 )
				if ( strcmp( ".pgm", &(namelist[i]->d_name[len-4]) ) == 0 )
					count++;
		}

		if ( count > 0 )
		{
			filenames = new char*[count];

			int j = 0;
			for ( int i = 0; i < n; i++ )
			{
				static int len;
				len = strlen( namelist[i]->d_name );
				if ( len > 5 )
					if ( strcmp( ".pgm", &(namelist[i]->d_name[len-4]) ) == 0 )
					{
						filenames[j] = new char[strlen(namelist[i]->d_name)+1];
						strcpy( filenames[j], namelist[i]->d_name );
						j++;
					}
				delete [] namelist[i];
			}
			delete [] namelist;

			for ( int i = 0; i < count; i++ )
			{
				cout << filenames[i] << endl;
				delete [] filenames[i];
			}
			delete [] filenames;

		}
	}

	return 0;
}

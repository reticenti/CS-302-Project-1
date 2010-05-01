#ifndef USER
#define USER

#include <iostream>

using namespace std;

class user
{
public:
	string getName() const
	{ return name; }
	string getPass() const
	{ return pass; }
	void setName( string& rhs )
	{ name = rhs; }
	void setPass( string& rhs )
	{ pass = rhs; }
	bool operator>( const user& rhs )
	{ return (name.compare(rhs.name) > 0); }
	bool operator<( const user& rhs )
	{ return (name.compare(rhs.name) < 0); }
	bool operator>=( const user& rhs )
	{ return (name.compare(rhs.name) >= 0); }
	bool operator<=( const user& rhs )
	{ return (name.compare(rhs.name) <= 0); }
	bool operator==( const user& rhs )
	{ return (name.compare(rhs.name) == 0); }

	friend ostream& operator<<( ostream &out, user& rhs );
private:
	string name;
	string pass;
};

ostream& operator<<( ostream &out, user& rhs )
{
	out << rhs.getName() << ' ' << rhs.getPass(); 
	return out;
}

#endif


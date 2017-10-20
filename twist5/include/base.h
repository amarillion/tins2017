#ifndef BASE_H
#define BASE_H

#include <fstream>
#include <vector>

/**
	Any storable object can be stored in a binary file
*/
class Storable
{
public:
	virtual void store (std::fstream &os);
	virtual void retrieve (std::fstream &is);
	virtual ~Storable();
};

/**
	Base for any data model objects that need to be
	storable, retrievable

	BaseObject has an automatically generated
	unique identifier.
	
	There are some DEBUG-only methods
	to help find memory leaks
*/
class BaseObject : public Storable
{
private:
	static int nextId;
	int id;
public: 
	BaseObject();
	int getId() { return id; }
#ifdef DEBUG
private:
	// list of all allocated objects, to check for memory leaks
	std::vector <BaseObject*> all;
public:
	void printList();
#endif

};

/**
	Typed reference to a BaseObject.

	- use store / retrieve function to store references
	- check reference consistency
	- prevent dangling references with reference counting on baseobject
	- operator overloading makes IdRef look like a pointer.
*/
template <class T>
class IdRef : public Storable
{
private:
	T* ptr;
public:
	T & getObject();
	T* operator-> () { return ptr; }
	T& operator* ()	{ return *ptr; }
	IdRef (const T &t);
	~IdRef ();
	void store();
	void retrieve();
};

#endif

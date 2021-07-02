#pragma once
#include "containers.h"
#include "util.h"
#include <map>

class vmobject
{
public:
	byte** holder;
	int* keys;
	//map<int, Array<byte>> inmap;
	int count;
	int hsize;
	vmobject();
	vmobject(const vmobject& other);
	~vmobject();
	vmobject& operator=(const vmobject& other);
	void add(int key, const Array<byte>& value);
	void remove(int key);
	Array<byte>& get(int key);
	void set(int key, const Array<byte>& val);
	void append(int key, byte b);
	void concat(int key, Array<byte>& a);
	void preallocate(int keys);
	int find(int key);
};
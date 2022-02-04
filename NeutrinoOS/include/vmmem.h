#pragma once
#include "containers.h"
#include "util.h"
#include <map>

enum DefaultType
{
	Null = 0,
	Int = 1,
	String = 2,
	ArrayPointer = 3,
	Byte = 4
};

class vmobject
{
public:
	int* holder;
	int* keys;
	int count;
	int hsize;
	DefaultType type;
	byte* boundValue;
	int bsize;
	int refcount;
	vmobject();
	vmobject(string val);
	vmobject(int val);
	vmobject(Array<byte> val);
	vmobject(int val, DefaultType ty);
	vmobject(const vmobject& other);
	~vmobject();
	vmobject& operator=(const vmobject& other);
	void add(int key, int value);
	void remove(int key);
	int get(int key);
	void set(int key, int val);
	void preallocate(int keys);
	int find(int key);
	void setValue(int v);
	void setValue(string v);
	void setValue(Array<byte> v);
	int getValue();
	static vmobject binaryop(vmobject* a, vmobject* b, byte op);
};

class ObjectMap
{
public:
	  int* keys;
	  vmobject** holder;
	  int size;
	  int hsize;
	  ObjectMap();
	  ~ObjectMap();
	  void add(int key, vmobject value);
	  void remove(int key);
	  vmobject* get(int key);
	  vmobject* operator[](int key);
	  void set(int key, vmobject value);
	  int find(int key);
};
class BufferedStack
{
public:
	  int* holder;
	  bool alive = false;
	  int top;
	  int size;
	  BufferedStack();
	  BufferedStack(int initialSize);
	  BufferedStack(const BufferedStack& c);
	  ~BufferedStack();
	  void operator=(const BufferedStack& c);
	  void reserve(int bytes);
	  void push(int a);
	  int& getTop();
	  void pop();
	  void clear();
	  void moveTop(int dep);
};

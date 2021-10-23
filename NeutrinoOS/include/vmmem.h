#pragma once
#include "containers.h"
#include "util.h"
#include <map>

enum DefaultType
{
	Null = 0,
	Int = 1,
	String = 2,
	ArrayPointer = 3
};

class vmobject
{
public:
	//byte** holder;
	int* holder;
	int* keys;
	//map<int, Array<byte>> inmap;
	int count;
	int hsize;
	DefaultType type; // 0 - generic, 1 - int, 2 - string
	byte* boundValue;
	int bsize;
	vmobject();
	vmobject(string val);
	vmobject(int val);
	vmobject(Array<byte> val);
	vmobject(int val, DefaultType ty);
	vmobject(const vmobject& other);
	~vmobject();
	vmobject& operator=(const vmobject& other);
	//void add(int key, const Array<byte>& value);
	void add(int key, int value);
	void remove(int key);
	//Array<byte>& get(int key);
	int get(int key);
	//void set(int key, const Array<byte>& val);
	void set(int key, int val);
	//void append(int key, byte b);
	//void concat(int key, Array<byte>& a);
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

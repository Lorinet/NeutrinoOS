#include "vmmem.h"
#include "bitconverter.h"

vmobject::vmobject()
{
	holder = new int[0];
	keys = new int[0];
	count = 0;
	hsize = 0;
	boundValue = NULL;
	bsize = 0;
	type = DefaultType::Null;
	refcount = 0;
}

vmobject::vmobject(int val)
{
	holder = new int[0];
	keys = new int[0];
	count = 0;
	hsize = 0;
	boundValue = bitconverter::toarray_p(val);
	bsize = 4;
	type = DefaultType::Int;
	refcount = 0;
}

vmobject::vmobject(int val, DefaultType ty)
{
	holder = new int[0];
	keys = new int[0];
	count = 0;
	hsize = 0;
	boundValue = bitconverter::toarray_p(val);
	bsize = 4;
	type = ty;
	refcount = 0;
}

vmobject::vmobject(string val)
{
	holder = new int[0];
	keys = new int[0];
	count = 0;
	hsize = 0;
	boundValue = bitconverter::toarray_p(val);
	bsize = val.size();
	type = DefaultType::String;
	refcount = 0;
}

vmobject::vmobject(Array<byte> val)
{
	holder = new int[0];
	keys = new int[0];
	count = 0;
	hsize = 0;
	bsize = val.size;
	boundValue = new byte[bsize];
	for (int i = 0; i < bsize; i++)
	{
		boundValue[i] = val.holder[i];
	}
	type = DefaultType::String;
	refcount = 0;
}

vmobject::vmobject(const vmobject& other)
{
	operator=(other);
}

vmobject& vmobject::operator=(const vmobject& other)
{
	hsize = other.hsize;
	count = other.count;
	holder = new int[hsize];
	keys = new int[count];
	for (int i = 0; i < hsize; i++) holder[i] = other.holder[i];
	for (int i = 0; i < count; i++) keys[i] = other.keys[i];
	bsize = other.bsize;
	boundValue = new byte[bsize];
	for(int i = 0; i < bsize; i++) boundValue[i] = other.boundValue[i];
	type = other.type;
	refcount = 0;
	return *this;
}

vmobject::~vmobject()
{
	delete[] holder;
	delete[] keys;
	delete[] boundValue;
}

void vmobject::add(int key, int value)
{
	int i;
	if (count <= key)
	{
		int* nk = new int[key + 1];
		for (int j = 0; j < count; j++) nk[j] = keys[j];
		for (int j = count; j < key; j++) nk[j] = -1;
		nk[key] = -1;
		count = key + 1;
		delete[] keys;
		keys = nk;
	}
	if (keys[key] != -1) i = keys[key];
	i = 0;
	while (i < hsize && holder[i] != NULL)
		i++;
	if (i == hsize)
	{
		int* nh = new int[hsize + 1];
		for (int j = 0; j < hsize; j++) nh[j] = holder[j];
		delete[] holder;
		holder = nh;
		hsize++;
	}
	holder[i] = value;
	keys[key] = i;
}

void vmobject::remove(int key)
{
	keys[key] = -1;
}

int vmobject::get(int key)
{
	return holder[keys[key]];
}

void vmobject::preallocate(int keyc)
{
	int* nk = new int[count + keyc];
	for (int j = 0; j < count; j++) nk[j] = keys[j];
	for (int j = count; j < count + keyc; j++) nk[j] = -1;
	delete[] keys;
	keys = nk;
	int* nh = new int[count + keyc];
	for (int j = 0; j < count; j++) nh[j] = holder[j];
	for (int j = count; j < count + keyc; j++) nh[j] = NULL;
	delete[] holder;
	holder = nh;
	count += keyc;
}

void vmobject::set(int key, int val)
{
	if (count <= key || keys[key] == -1) add(key, val);
	else holder[keys[key]] = val;
}

int vmobject::find(int key)
{
	if(key >= count) return false;
	return keys[key] != -1;
}

void vmobject::setValue(int v)
{
	delete[] boundValue;
	bsize = 4;
	boundValue = new byte[4];
	byte* b = bitconverter::toarray_p(v);
	boundValue[0] = b[0];
	boundValue[1] = b[1];
	boundValue[2] = b[2];
	boundValue[3] = b[3];
	type = DefaultType::Int;
}

void vmobject::setValue(string v)
{
	delete[] boundValue;
	bsize = v.size();
	boundValue = new byte[bsize];
	for (int i = 0; i < bsize; i++)
	{
		boundValue[i] = v[i];
	}
}

void vmobject::setValue(Array<byte> v)
{
	delete[] boundValue;
	bsize = v.size;
	boundValue = new byte[bsize];
	for (int i = 0; i < bsize; i++)
	{
		boundValue[i] = v.holder[i];
	}
}

int vmobject::getValue()
{
	return (type == DefaultType::Int ? bitconverter::toint32(boundValue, 0) : (int)boundValue[0]);
}

vmobject vmobject::binaryop(vmobject* a, vmobject* b, byte op)
{
	if (a->type == DefaultType::Int || a->type == DefaultType::Byte)
	{
		switch(op)
		{
		case 0: // add
			return vmobject(a->getValue() + b->getValue());
		case 1: // sub
			return vmobject(a->getValue() - b->getValue());
		case 2: // mul
			return vmobject(a->getValue() * b->getValue());
		case 3: // div
			return vmobject(a->getValue() / b->getValue());
		case 4: // and
			return vmobject(a->getValue() & b->getValue());
		case 5: // or
			return vmobject(a->getValue() | b->getValue());
		case 6: // xor
			return vmobject(a->getValue() ^ b->getValue());
		case 7: // shl
			return vmobject(a->getValue() << b->getValue());
		case 8: // shr
			return vmobject(a->getValue() >> b->getValue());
		}
	}
	else if (a->type == DefaultType::String)
	{
		return vmobject(bitconverter::tostring(a->boundValue, a->bsize) + bitconverter::tostring(b->boundValue, b->bsize));
	}
	return NULL;
}

ObjectMap::ObjectMap()
{
    keys = new int[0];
    holder = new vmobject*[0];
    size = 0;
    hsize = 0;
}

ObjectMap::~ObjectMap()
{
	//for (int i = 0; i < hsize; i++) delete holder[i];
	delete[] holder;
	delete[] keys;
}

void ObjectMap::add(int key, vmobject value)
{
	int i;
	if (size <= key)
	{
		int* nk = new int[key + 1];
		for (int j = 0; j < size; j++) nk[j] = keys[j];
		nk[key] = -1;
		int oc = size;
		size = key + 1;
		delete[] keys;
		keys = nk;
	}
	if (keys[key] != -1)
	{
		i = keys[key];
		delete holder[i];
		holder[i] = NULL;
	}
	i = 0;
	while (i < hsize && holder[i] != NULL)
		i++;
	if (i == hsize)
	{
		vmobject** nh = new vmobject * [hsize + 1];
		for (int j = 0; j < hsize; j++) nh[j] = holder[j];
		delete[] holder;
		holder = nh;
		hsize++;
	}
	keys[key] = i;
	holder[i] = new vmobject(value);
}

void ObjectMap::remove(int key)
{
	int i = keys[key];
	delete holder[i];
	holder[i] = NULL;
	keys[key] = -1;
}

vmobject* ObjectMap::get(int key)
{
	return holder[keys[key]];
}

vmobject* ObjectMap::operator[](int key)
{
	return holder[keys[key]];
}

void ObjectMap::set(int key, vmobject value)
{
	if (keys[key] == -1) add(key, value);
	else holder[keys[key]] = new vmobject(value);
}

int ObjectMap::find(int key)
{
    if (key >= size) return false;
    return keys[key] != -1;
}

BufferedStack::BufferedStack()
{
	size = 10;
	top = -1;
	holder = new int[size];
	alive = true;
}

BufferedStack::BufferedStack(int initialSize)
{
	size = initialSize;
	top = -1;
	holder = new int[size];
	alive = true;
}

BufferedStack::BufferedStack(const BufferedStack& c)
{
	size = c.size;
	top = c.top;
	holder = new int[size];
	for (int i = 0; i < size; i++) holder[i] = c.holder[i];
}

BufferedStack::~BufferedStack()
{
	if (alive)
	{
		delete[] holder;
		alive = false;
	}
}

void BufferedStack::operator=(const BufferedStack& c)
{
	size = c.size;
	top = c.top;
	holder = new int[size];
	for (int i = 0; i < size; i++) holder[i] = c.holder[i];
}

void BufferedStack::reserve(int bytes)
{
	if (bytes > size)
	{
		int* behold = new int[bytes];
		for (int i = 0; i < size; i++) behold[i] = holder[i];
		size = bytes;
		delete[] holder;
		holder = behold;
	}
	else if (top < bytes)
	{
		int* behold = new int[bytes];
		for (int i = 0; i < bytes; i++) behold[i] = holder[i];
		size = bytes;
		delete[] holder;
		holder = behold;
	}
}

void BufferedStack::push(int a)
{
	if (top >= size - 1)
	{
		reserve(size + 100);
	}
	top++;
	holder[top] = a;
}

int& BufferedStack::getTop()
{
	if (top >= 0) return holder[top];
	else
	{
		push(-1);
		return holder[0];
	}
}

void BufferedStack::pop()
{
	top--;
	if (top < size - 150)
	{
		reserve(size - 100);
	}
}

void BufferedStack::clear()
{
	delete[] holder;
	size = 10;
	holder = new int[size];
}

void BufferedStack::moveTop(int dep)
{
	int v = holder[top - dep];
	for(int i = top - dep; i < top; i++)
	{
		holder[i] = holder[i + 1];
	}
	holder[top] = v;
}

//vmobject::vmobject()
//{
//	holder = new byte * [0];
//	keys = new int[0];
//	count = 0;
//	hsize = 0;
//}
//
//vmobject::vmobject(const vmobject& other)
//{
//	operator=(other);
//}
//
//vmobject& vmobject::operator=(const vmobject& other)
//{
//	hsize = other.hsize;
//	count = other.count;
//	holder = new byte * [hsize];
//	keys = new int[count];
//	for (int i = 0; i < hsize; i++) holder[i] = other.holder[i];
//	for (int i = 0; i < count; i++) keys[i] = other.keys[i];
//	return *this;
//}
//
//vmobject::~vmobject()
//{
//	for (int i = 0; i < hsize; i++) delete[] holder[i];
//	delete[] holder;
//	delete[] keys;
//}
//
//void vmobject::add(int key, const Array<byte>& value)
//{
//	int i;
//	if (count <= key)
//	{
//		int* nk = new int[key + 1];
//		for (int j = 0; j < count; j++) nk[j] = keys[j];
//		for (int j = count; j < key; j++) nk[j] = -1;
//		nk[key] = -1;
//		int oc = count;
//		count = key + 1;
//		delete[] keys;
//		keys = nk;
//	}
//	if (keys[key] != -1)
//	{
//		i = keys[key];
//		//delete[] holder[i];
//		holder[i] = NULL;
//	}
//	i = 0;
//	while (i < hsize && holder[i] != NULL)
//		i++;
//	if (i == hsize)
//	{
//		byte** nh = new byte * [hsize + 1];
//		for (int j = 0; j < hsize; j++) nh[j] = holder[j];
//		delete[] holder;
//		holder = nh;
//		hsize++;
//	}
//	holder[i] = new byte[value.size + 4];
//	Array<byte> byti = bitconverter::toArray(value.size);
//	for (int j = 0; j < 4; j++) holder[i][j] = byti[j];
//	for (int j = 0; j < value.size; j++) holder[i][j + 4] = value.holder[j];
//	keys[key] = i;
//}
//
//void vmobject::remove(int key)
//{
//	int i = keys[key];
//	delete[] holder[i];
//	holder[i] = NULL;
//	keys[key] = -1;
//}
//
//Array<byte>& vmobject::get(int key)
//{
//	byte* i = holder[keys[key]];
//	Array<byte>* ab = new Array<byte>("");
//	ab->size = bitconverter::toint32(i, 0);
//	ab->holder = 4 + i;
//	return *ab;
//}
//
//void vmobject::preallocate(int keyc)
//{
//	int* nk = new int[count + keyc];
//	for (int j = 0; j < count; j++) nk[j] = keys[j];
//	for (int j = count; j < count + keyc; j++) nk[j] = -1;
//	delete[] keys;
//	keys = nk;
//	byte** nh = new byte * [count + keyc];
//	for (int j = 0; j < count; j++) nh[j] = holder[j];
//	for (int j = count; j < count + keyc; j++) nh[j] = NULL;
//	delete[] holder;
//	holder = nh;
//	count += keyc;
//} 
//
//void vmobject::set(int key, const Array<byte>& val)
//{
//	if (count <= key || keys[key] == -1) add(key, val);
//	else
//	{
//		byte* ip = holder[keys[key]];
//		byte* jp = new byte[val.size + 4];
//		Array<byte> sb = bitconverter::toArray(val.size);
//		for (int i = 0; i < 4; i++) jp[i] = sb[i];
//		for (int i = 0; i < val.size; i++) jp[i + 4] = val.holder[i];
//		delete[] ip;
//		holder[keys[key]] = jp;
//	}
//}
//
//void vmobject::append(int key, byte b)
//{
//	byte* ip = holder[keys[key]];
//	int xlen = bitconverter::toint32(ip, 0);
//	byte* jp = new byte[xlen + 5];
//	Array<byte> sb = bitconverter::toArray(xlen + 1);
//	for (int i = 0; i < 4; i++) jp[i] = sb[i];
//	for (int i = 0; i < xlen; i++) jp[i + 4] = ip[i + 4];
//	jp[xlen + 4] = b;
//	delete[] ip;
//	holder[keys[key]] = jp;
//}
//
//void vmobject::concat(int key, Array<byte>& a)
//{
//	byte* ip = holder[keys[key]];
//	int xlen = bitconverter::toint32(ip, 0);
//	byte* jp = new byte[xlen + a.size + 4];
//	Array<byte> sb = bitconverter::toArray(xlen + a.size);
//	for(int i = 0; i < 4; i++) jp[i] = sb[i];
//	for(int i = 0; i < xlen; i++) jp[i + 4] = ip[i + 4];
//	for(int i = 0; i < a.size; i++) jp[i + xlen + 4] = a[i];
//	delete[] ip;
//	holder[keys[key]] = jp;
//}
//
//int vmobject::find(int key)
//{
//	return keys[key] != -1;
//}

//vmobject::vmobject()
//{
//	inmap = map<int, Array<byte>>();
//}
//
//vmobject::vmobject(const vmobject& other)
//{
//	operator=(other);
//}
//
//vmobject::~vmobject()
//{
//	inmap.clear();
//}
//
//vmobject& vmobject::operator=(const vmobject& other)
//{
//	inmap = other.inmap;
//	return *this;
//}
//
//Array<byte>& vmobject::get(int key)
//{
//	return inmap[key];
//}
//
//void vmobject::add(int key, const Array<byte>& value)
//{
//	inmap.emplace(key, value);
//}
//
//int vmobject::find(int key)
//{
//	return inmap.find(key) != inmap.end();
//}
//
//void vmobject::set(int key, const Array<byte>& value)
//{
//	if (!find(key)) add(key, value);
//	else inmap[key] = value;
//}
//
//void vmobject::remove(int key)
//{
//	if (find(key)) inmap.erase(inmap.find(key));
//}

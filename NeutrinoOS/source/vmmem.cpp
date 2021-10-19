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
	return *this;
}

vmobject::~vmobject()
{
	delete[] holder;
	delete[] keys;
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
	return bitconverter::toint32(boundValue, 0);
}

vmobject& vmobject::binaryop(vmobject& a, vmobject& b, byte op)
{
	if (a.type == DefaultType::Int)
	{
		switch(op)
		{
		case 0: // add
			return vmobject(bitconverter::toint32(a.boundValue, 0) + bitconverter::toint32(b.boundValue, 0));
		case 1: // sub
			return vmobject(bitconverter::toint32(a.boundValue, 0) - bitconverter::toint32(b.boundValue, 0));
		case 2: // mul
			return vmobject(bitconverter::toint32(a.boundValue, 0) * bitconverter::toint32(b.boundValue, 0));
		case 3: // div
			return vmobject(bitconverter::toint32(a.boundValue, 0) / bitconverter::toint32(b.boundValue, 0));
		case 4: // and
			return vmobject(bitconverter::toint32(a.boundValue, 0) & bitconverter::toint32(b.boundValue, 0));
		case 5: // or
			return vmobject(bitconverter::toint32(a.boundValue, 0) | bitconverter::toint32(b.boundValue, 0));
		case 6: // xor
			return vmobject(bitconverter::toint32(a.boundValue, 0) ^ bitconverter::toint32(b.boundValue, 0));
		case 7: // shl
			return vmobject(bitconverter::toint32(a.boundValue, 0) << bitconverter::toint32(b.boundValue, 0));
		case 8: // shr
			return vmobject(bitconverter::toint32(a.boundValue, 0) >> bitconverter::toint32(b.boundValue, 0));
		}
	}
	else if (a.type == DefaultType::String)
	{
		return vmobject(bitconverter::tostring(a.boundValue, a.bsize) + bitconverter::tostring(b.boundValue, b.bsize));
	}
	return vmobject();
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

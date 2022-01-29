#pragma once
#include "util.h"
#include <iostream>
#include <map>
template<class T>
class Array
{
public:
	T* holder;
	int size;
	bool alive = false;
	Array()
	{
		size = 0;
		holder = new T[0];
		alive = true;
	}
	Array(const Array<T>& c)
	{
		size = c.size;
		holder = new T[size];
		for (int i = 0; i < size; i++) holder[i] = c.holder[i];
		alive = true;
	}
	Array(const int s)
	{
		size = s;
		holder = new T[size];
		for (int i = 0; i < size; i++) holder[i] = T();
		alive = true;
	}
	Array(T* tc, int s)
	{
		size = s;
		holder = new T[size];
		for (int i = 0; i < size; i++) holder[i] = tc[i];
		alive = true;
	}
	Array(string dummy)
	{
		alive = true;
	}
	~Array()
	{
		if (alive)
		{
			delete[] holder;
			alive = false;
		}
	}
	void removeAt(int index)
	{
		if (index < size && index >= 0)
		{
			size -= 1;
			T* behold = new T[size];
			for (int i = 0; i < index; i++) behold[i] = holder[i];
			for (int i = index + 1; i < size + 1; i++) behold[i - 1] = holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	void remove(T item)
	{
		for (int index = 0; index < size; index++)
		{
			if (holder[index] == item)
			{
				size -= 1;
				T* behold = new T[size];
				for (int i = 0; i < index; i++) behold[i] = holder[i];
				for (int i = index + 1; i < size + 1; i++) behold[i - 1] = holder[i];
				delete[] holder;
				holder = behold;
			}
		}
	}
	int indexOf(T item)
	{
		for (int i = 0; i < size; i++)
		{
			if (holder[i] == item) return i;
		}
		return -1;
	}
	void add(const T& pc)
	{
		push(pc);
	}
	void push(const T& pc)
	{
		size += 1;
		T* behold = new T[size];
		for (int i = 0; i < size - 1; i++) behold[i] = holder[i];
		behold[size - 1] = pc;
		delete[] holder;
		holder = behold;
	}
	void pop()
	{
		if (size > 0)
		{
			size -= 1;
			T* behold = new T[size];
			for (int i = 0; i < size; i++) behold[i] = holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	void erase(int start, int length)
	{
		if (start >= 0 && start < size && start + length <= size && start + length > 0)
		{
			int os = size;
			size -= length;
			T* behold = new T[size];
			for (int i = 0; i < start; i++) behold[i] = holder[i];
			for (int i = start + length; i < os; i++) behold[i - length] = holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	void erase(int length)
	{
		if (length > 0 && length <= size)
		{
			T* behold = new T[size - length];
			for (int i = length; i < size; i++) behold[i - length] = holder[i];
			size -= length;
			delete[] holder;
			holder = behold;
		}
	}
	void pushFront(const T& pc)
	{
		size += 1;
		T* behold = new T[size];
		behold[0] = pc;
		for (int i = 1; i < size; i++) behold[i] = holder[i - 1];
		delete[] holder;
		holder = behold;
	}
	void insert(const Array<T>& other, int index, int start, int length)
	{
		if (start >= 0 && length > 0 && start < other.size && start + length <= other.size && index >= 0 && index <= size)
		{
			int os = size;
			size += length;
			T* behold = new T[size];
			for (int i = 0; i < index; i++) behold[i] = holder[i];
			for (int i = index; i < index + length; i++) behold[i] = other.holder[start + i - index];
			for (int i = index; i < os; i++) behold[i + length] = holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	void insert(Array<T>* other, int index, int start, int length)
	{
		if (start >= 0 && length > 0 && start < other->size && start + length <= other->size && index >= 0 && index <= size)
		{
			int os = size;
			size += length;
			T* behold = new T[size];
			for (int i = 0; i < index; i++) behold[i] = holder[i];
			for (int i = index; i < index + length; i++) behold[i] = other->holder[start + i - index];
			for (int i = index; i < os; i++) behold[i + length] = holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	void insert(int index, T item)
	{
		if (index >= 0 && index <= size)
		{
			int os = size;
			size += 1;
			T* behold = new T[size];
			for (int i = 0; i < index; i++) behold[i] = holder[i];
			behold[index] = item;
			for (int i = index; i < os; i++) behold[i + 1] = holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	T getTop()
	{
		if (size > 0) return holder[size - 1];
		else return 0;
	}
	bool contains(T val)
	{
		return indexOf(val) != -1;
	}
	T& operator[](int index)
	{
		return get(index);
	}
	T& get(int index)
	{
		if (index < size && index >= 0)
		{
			return holder[index];
		}
		//else throw;
		return holder[0];
	}
	T& getSafe(int index)
	{
		if (index < size && index >= 0)
		{
			return holder[index];
		}
		else
		{
			for(int i = 0; i < index - size + 1; i++)
			{
				add(T());
			}
			return holder[index];
		}
	}
	void operator=(const Array<T>& other)
	{
		if (!alive)
		{
			size = other.size;
			holder = new T[size];
			for (int i = 0; i < size; i++) holder[i] = other.holder[i];
			alive = true;
		}
		else
		{
			size = other.size;
			T* behold = new T[size];
			for (int i = 0; i < size; i++) behold[i] = other.holder[i];
			delete[] holder;
			holder = behold;
		}
	}
	bool operator!=(const Array<T> other)
	{
		return !(this == other);
	}
	bool operator==(const Array<T>& other)
	{
		return other.holder == holder;
	}
	bool compareElements(Array<T> other)
	{
		if (other.size != size) return false;
		for (int i = 0; i < size; i++)
		{
			if (holder[i] != other.holder[i]) return false;
		}
		return true;
	}
	void addRange(Array<T> other)
	{
		T* behold = new T[size + other.size];
		for (int i = 0; i < size; i++)
		{
			behold[i] = holder[i];
		}
		delete[] holder;
		int os = size;
		size = size + other.size;
		for (int i = os; i < size; i++)
		{
			behold[i] = other.holder[i - os];
		}
		holder = behold;
	}
	void addRange(Array<T>* other)
	{
		T* behold = new T[size + other->size];
		for (int i = 0; i < size; i++)
		{
			behold[i] = holder[i];
		}
		delete[] holder;
		size = size + other->size;
		for (int i = size - other->size; i < size; i++)
		{
			behold[i] = other->holder[i];
		}
		holder = behold;
	}
	void clear()
	{
		if (alive)
		{
			delete[] holder;
			holder = new T[0];
			size = 0;
		}
	}
};

template<class V>
class IntMap
{
public:
	int* keys;
	V** holder;
	int size;
	int hsize;
	IntMap()
	{
		keys = new int[0];
		holder = new V*[0];
		size = 0;
		hsize = 0;
	}
	IntMap(IntMap<V>& other)
	{
		operator=(other);
	}
	IntMap& operator=(IntMap<V>& other)
	{
		size = other.size;
		hsize = other.hsize;
		holder = new V*[hsize];
		keys = new int[size];
		for (int i = 0; i < size; i++) keys[i] = other.keys[i];
		for (int i = 0; i < hsize; i++) holder[i] = other.holder[i];
		return *this;
	}
	~IntMap()
	{
		//for (int i = 0; i < hsize; i++) delete holder[i];
		delete[] holder;
		delete[] keys;
	}
	void add(int key, const V& value)
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
			V** nh = new V * [hsize + 1];
			for (int j = 0; j < hsize; j++) nh[j] = holder[j];
			delete[] holder;
			holder = nh;
			hsize++;
		}
		keys[key] = i;
		holder[i] = new V(value);
	}
	void remove(int key)
	{
		int i = keys[key];
		delete[] holder[i];
		holder[i] = NULL;
		keys[key] = -1;
	}
	V& get(int key)
	{
		return *holder[keys[key]];
	}
	V& operator[](int key)
	{
		return *holder[keys[key]];
	}
	void set(int key, V& value)
	{
		if (keys[key] == -1) add(key, value);
		else *holder[keys[key]] = value;
	}
	int find(int key)
	{
		if (key >= size) return false;
		return keys[key] != -1;
	}
};


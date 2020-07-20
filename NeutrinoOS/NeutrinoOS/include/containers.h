#pragma once
#include "util.h"
#include <iostream>
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
		holder = new T[size];
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
		if (start >= 0 && start < size && start + length < size && start + length > 0)
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
		if (start >= 0 && length > 0 && start < other.size && start + length < other.size && index >= 0 && index <= size)
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
		size = size + other.size;
		for (int i = size - other.size; i < size; i++)
		{
			behold[i] = other.holder[i];
		}
	}
	void addRange(Array<T>* other)
	{
		T* behold = new T[size + other->size];
		for (int i = 0; i < size; i++)
		{
			behold[i] = holder[i];
		}
		size = size + other->size;
		for (int i = size - other->size; i < size; i++)
		{
			behold[i] = other->holder[i];
		}
	}
	void clear()
	{
		if(alive)
		{
			delete[] holder;
			holder = new T[0];
			size = 0;
		}
	}
};
template<class K, class V>
class Map
{
public:
	Array<K>* keys;
	Array<V>* values;
	Map()
	{
		keys = new Array<K>();
		values = new Array<V>();
	}
	Map(const Map<K, V>& c)
	{
		keys = new Array<K>();
		values = new Array<V>();
		for (int i = 0; i < c.keys->size; i++) keys->add(c.keys->get(i));
		for (int i = 0; i < c.values->size; i++) values->add(c.values->get(i));
	}
	~Map()
	{
		delete keys;
		delete values;
	}
	void emplace(K k, V v)
	{
		set(k, v);
	}
	void set(K k, V v)
	{
		if (!keys->contains(k))
		{
			keys->add(k);
			values->add(v);
		}
		else
		{
			get(k) = v;
		}
	}
	V& get(K& key)
	{
		int index = keys->indexOf(key);
		if (index != -1) return values->get(index);
		//else throw;
		return values->get(0);
	}
	void remove(K& key)
	{
		int index = keys->indexOf(key);
		if (index != -1)
		{
			keys->removeAt(index);
			values->removeAt(index);
		}
	}
	V& operator[](K key)
	{
		return get(key);
	}
	void operator=(const Map<K, V>& c)
	{
		keys = new Array<K>();
		values = new Array<V>();
		for (int i = 0; i < c.keys->size; i++) keys->add(c.keys->get(i));
		for (int i = 0; i < c.values->size; i++) values->add(c.values->get(i));
	}
	bool operator==(const Map<K, V>& c)
	{
		if (keys->size != c.keys->size || values->size != c.values->size) return false;
		for (int i = 0; i < keys->size; i++)
		{
			if (keys->get(i) != c.keys->get(i)) return false;
		}
		for (int i = 0; i < values->size; i++)
		{
			if (values->get(i) != c.valuse->get(i)) return false;
		}
		return true;
	}
	bool operator!=(const Map<K, V>& other)
	{
		return !(this == other);
	}
};
class BufferedStack
{
public:
	Array<byte>* holder;
	bool alive = false;
	int top;
	int size;
	BufferedStack()
	{
		size = 10;
		top = -1;
		holder = new Array<byte>[size];
		alive = true;
	}
	BufferedStack(int initialSize)
	{
		size = initialSize;
		top = -1;
		holder = new Array<byte>[size];
		alive = true;
	}
	BufferedStack(const BufferedStack& c)
	{
		size = c.size;
		top = c.top;
		holder = new Array<byte>[size];
		for (int i = 0; i < size; i++) holder[i] = c.holder[i];
	}
	~BufferedStack()
	{
		if (alive)
		{
			delete[] holder;
			alive = false;
		}
	}
	void operator=(const BufferedStack& c)
	{
		size = c.size;
		top = c.top;
		holder = new Array<byte>[size];
		for (int i = 0; i < size; i++) holder[i] = c.holder[i];
	}
	void reserve(int bytes)
	{
		if (bytes > size)
		{
			Array<byte>* behold = new Array<byte>[bytes];
			for (int i = 0; i < size; i++) behold[i] = holder[i];
			size = bytes;
			delete[] holder;
			holder = behold;
		}
		else if (top < bytes)
		{
			Array<byte>* behold = new Array<byte>[bytes];
			for (int i = 0; i < bytes; i++) behold[i] = holder[i];
			size = bytes;
			delete[] holder;
			holder = behold;
		}
	}
	void push(Array<byte> a)
	{
		if (top >= size - 1)
		{
			reserve(size + 100);
		}
		top++;
		holder[top] = a;
	}
	Array<byte>& getTop()
	{
		if (top >= 0) return holder[top];
		else
		{
			push(Array<byte>());
			return holder[0];
		}
	}
	void pop()
	{
		top--;
		if (top < size - 150)
		{
			reserve(size - 100);
		}
	}
	void clear()
	{
		delete[] holder;
		size = 10;
		holder = new Array<byte>[size];
	}
};
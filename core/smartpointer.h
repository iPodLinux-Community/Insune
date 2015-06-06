#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

#include "exception.h"
#include "hashmap.h"
#include "insune.h"

template<typename T>
class SmartPointer 
{
 public:
	explicit SmartPointer(T *ptr)
	{
		init(ptr);
	}

	SmartPointer(const SmartPointer<T> &src)
	{
		init(src.realPtr);
	}

	~SmartPointer()
	{
		fini();
	}

	SmartPointer<T> &operator=(const SmartPointer<T> &r)
	{
		if (this != &r) {
			fini();
			init(r.realPtr);
		}
		return *this;
	}

	const T &operator*() const
	{ return *realPtr; }

	const T *operator->() const
	{ return realPtr; }

	T &operator*()
	{ return *realPtr; }

	T *operator->()
	{ return realPtr; }

	operator void*() const
	{ return realPtr; }

 protected:
	void fini()
	{
		if (refs->find(realPtr) == refs->end())
			throw Exception("No such pointer");
		(*refs)[realPtr]--;
		if ((*refs)[realPtr] == 0) {
			refs->erase(realPtr);
			if (realPtr != 0)
				delete realPtr;
		}
	}

	void init(T *ptr)
	{
		if (!refs)
			refs = new hashmap<T *, int>();
		realPtr = ptr;
		if (refs->find(realPtr) == refs->end())
			(*refs)[realPtr] = 1;
		else
			(*refs)[realPtr]++;
	}

	T *realPtr;
	static hashmap<T *, int> *refs;
};


template <typename T>
hashmap<T *, int> *SmartPointer<T>::refs = 0;

#endif

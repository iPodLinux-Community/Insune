#ifndef _HASHMAP_H_
#define _HASHMAP_H_
#ifdef __cplusplus
#include "exception.h"
#endif

typedef struct _element_t {
	int key;
	int in_use;
	void *data;
} element_t;

typedef struct _hashmap_t {
	int t_size;
	int size;
	element_t *data;
} hashmap_t;

#ifdef __cplusplus
extern "C" {
#endif

void hashmap_free(hashmap_t *map);
hashmap_t *hashmap_new();
int hashmap_put(hashmap_t *map, long key, void *value);
element_t *hashmap_find(hashmap_t *map, long key);
void *hashmap_get(hashmap_t *map, long key);
int hashmap_remove(hashmap_t *map, long key);
int hashmap_length(hashmap_t *map);

#ifdef __cplusplus
}

template <typename T>
class hashmapIterator
{
 public:
	hashmapIterator() {}
	hashmapIterator(const hashmapIterator<T> &l)
	 : element(l.element) {}
	hashmapIterator(element_t *e)
	 : element(e) {}
	~hashmapIterator() {}

	T operator*() const
	{
		if (!element)
			throw Exception("Can't dereference a non-object");
		return (T)element->data;
	}
	bool operator==(const hashmapIterator<T> &a) const
	{
		return element == a.element;
	}
	bool operator!=(const hashmapIterator<T> &a) const
	{
		return element != a.element;
	}
 protected:
	element_t *element;
};

template<typename Key, typename Data>
class hashmap
{
 public:
 	typedef hashmapIterator<Data> iterator;

	hashmap()
	{
		map = hashmap_new();
		if (!map)
			throw Exception("Unable to create map.");
	}

	~hashmap()
	{
		hashmap_free(map);
	}

	bool empty() const
	{
		return hashmap_length(map) == 0;
	}

	void clear()
	{
		hashmap_free(map);
		map = hashmap_new();
		if (!map)
			throw Exception("Unable to recreate map.");
	}

	void erase(const Key &k)
	{
		hashmap_remove(map, (long)k);
	}

	int size() const
	{
		return hashmap_length(map);
	}

	hashmapIterator< Data > find(const Key &k) const
	{
		return (hashmapIterator< Data >)hashmap_find(map, (long)k);
	}

	hashmapIterator< Data > end() const
	{
		return (hashmapIterator< Data >)0;
	}

	Data &operator[](const Key &k)
	{
		if (find(k) == end())
			hashmap_put(map, (long)k, 0);
		return (Data &)hashmap_find(map, (long)k)->data;
	}

 protected:
	hashmap_t *map;
};

#endif

#endif

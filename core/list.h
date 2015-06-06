#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _list_t {
	void *data;
	struct _list_t *next;
} * list_t;


typedef void (* list_do_t)(void *, void *);
typedef void (* list_foreach_t)(void *);
typedef int (* list_cmp)(void *, void *);

void list_do(list_t *list, void (* func)(void *, void *), void *data);
void list_foreach(list_t *list, void (* func)(void *));
void list_sort(list_t *list, int (* cmp)(void *, void *));

void list_destroy(list_t *list, int dofree);
int list_count(list_t const *list);
list_t list_split(list_t *list, void *start, void *end);
list_t list_next(list_t const *list);

void list_remove(list_t *list, void *data);
void *list_nth(list_t const *list, int n);
void *list_last(list_t const *list);
#define list_first(list) list_nth(list, 0)
void list_insert(list_t *list, void *data, void *before);
void list_append(list_t *list, void *data);
void list_prepend(list_t *list, void *data);

/* lifo */
#define list_peek list_first
#define list_push list_prepend
void *list_pop(list_t *list);

#ifdef __cplusplus
} 

template <typename T>
class listIterator
{
 public:
	listIterator() {};
	listIterator(const listIterator< T > &l)
	 : pos(l.pos), head(l.head) {};
	listIterator(list_t l)
	 : pos(l), head(l) {};
	~listIterator() {};

	listIterator< T > &operator++()
	{
		pos = pos->next;
		return *this;
	}
	listIterator< T > operator++(int)
	{
		listIterator< T > tmp = *this;
		pos = pos->next;
		return tmp;
	}
	listIterator< T > &operator--()
	{
		list_t p = head;
		if (pos == head) {
			pos = 0;
			return *this;
		}
		while (p) {
			if (p->next == pos) {
				pos = p;
				break;
			}
			p = p->next;
		}
		return *this;
	}
	listIterator< T > operator--(int)
	{
		listIterator< T > tmp = *this;
		list_t p = head;
		if (pos == head) {
			pos = 0;
			return tmp;
		}
		while (p) {
			if (p->next == pos) {
				pos = p;
				break;
			}
			p = p->next;
		}
		return tmp;
	}
	T operator*() const
	{
		if (!pos) abort();
		return (T)pos->data;
	}
	bool operator==(const listIterator< T > &a) const
	{
		return pos == a.pos;
	}
	bool operator!=(const listIterator< T > &a) const
	{
		return pos != a.pos;
	}

 protected:
 	list_t pos;
	list_t head;
};


template <typename T>
int list_compare(T a, T b)
{
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}

template <typename T>
int list_compare(T *a, T *b)
{
	return (*a > *b) ? 1 : (*a < *b) ? -1 : 0;
}

template <typename T>
class list
{
 public:
 	typedef listIterator< T > iterator;

	list()
	 : llist(0)
	{ }

	list(const list &ol)
	 : llist(0)
	{
		list_t oll = ol.llist;
		list_t ll = 0;
		while (oll) {
			list_t n = (list_t)malloc(sizeof(struct _list_t));
			n->data = oll->data;
			if (llist == 0)
				llist = n;
			if (ll)
				ll->next = n;
			ll = n;
			oll = oll->next;
		}
	}

	~list()
	{
		destroy();
	}

	void foreach(void (* func)(T))
	{
		list_foreach(&llist, func);
	}
	void foreach(void (* func)(T, void *), void *data)
	{
		list_do(&llist, func, data);
	}
	void sort()
	{
		list_sort(&llist, (list_cmp)static_cast<int (*)(T, T)>(list_compare));
	}
	void destroy()
	{
		list_destroy(&llist, 0);
	}
	void deleteAll()
	{
		iterator it = start();
		for (; it != end(); ++it)
			delete (*it);
		destroy();
	}
	int count() const
	{
		return list_count(&llist);
	}
	iterator next() const
	{
		list_next(&llist);
	}
	void remove(T a)
	{
		list_remove(&llist, (void *)a);
	}
	T nth(int n) const
	{
		return (T)(long)list_nth(&llist, n);
	}
	T last() const
	{
		return (T)(long)list_last(&llist);
	}
	T first() const
	{
		return nth(0);
	}
	void insert(T data, T before)
	{
		list_insert(&llist, (void *)data, (void *)before);
	}
	void append(T data)
	{
		list_append(&llist, (void *)data);
	}
	void prepend(T data)
	{
		list_prepend(&llist, (void *)data);
	}
	T peek() const
	{
		return first();
	}
	void push(T data)
	{
		prepend(data);
	}
	T pop()
	{
		return (T)(long)list_pop(&llist);
	}
	listIterator< T > start() const
	{
		return (listIterator< T >)llist;
	}
	listIterator< T > end() const
	{
		return (listIterator< T >)0;
	}

 protected:
	list_t llist;
};
#endif

#endif

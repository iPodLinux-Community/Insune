#include "stdlib.h"
#include "list.h"

#ifndef safe_free
#define safe_free(x) do { if (x) free(x), x = 0; } while (0)
#endif

list_t list_next(list_t const *list)
{
	return (*list)->next;
}

void list_foreach(list_t *list, void (* func)(void *))
{
	list_t p = *list, o;
	while (p) {
		o = p->next;
		(* func)(p->data);
		p = o;
	}
}

void list_do(list_t *list, void (* func)(void *, void *), void *data)
{
	list_t p = *list, o;
	while (p) {
		o = p->next;
		(* func)(p->data, data);
		p = o;
	}
}

int list_count(list_t const *list)
{
	list_t p;
	int c = 0;
	for (p = *list; p; p = p->next, ++c);
	return c;
}

void *list_last(list_t const *list)
{
	list_t p;
	if (!(p = *list)) return NULL;
	while (p->next) p = p->next;
	return p->data;
}

void list_destroy(list_t *list, int dofree)
{
	list_t p, o;
	p = *list;
	while (p) {
		o = p;
		p = p->next;
		if (dofree) safe_free(o->data);
		free(o);
	}
	*list = 0;
}

list_t list_split(list_t *list, void *start, void *end)
{
	list_t p, n, o = NULL;

	p = *list;
	while (p && p->data != start && p->data != end) {
		o = p;
		p = p->next;
	}

	if (!p) return NULL;

	n = p;
	if (p->data == end) {
		void *t = end;
		end = start;
		start = t;
	}
	while (p && p->data != end)
		p = p->next;
	if (p) {	
		if (o) o->next = p->next;
		else *list = p->next;
		p->next = NULL;
	}
	else {
		if (o) o->next = NULL;
		else *list = NULL;
	}
	return n;
}

void list_append(list_t *list, void *data)
{
	list_t p;
	if ((p = *list)) {
		while (p->next)
			p = p->next;
		p = p->next = calloc(1, sizeof(struct _list_t));
	}
	else p = *list = calloc(1, sizeof(struct _list_t));

	p->data = data;
}

void list_prepend(list_t *list, void *data)
{
	list_t p;

	p = calloc(1, sizeof(struct _list_t));
	p->data = data;
	p->next = *list;
	*list = p;
}

void list_remove(list_t *list, void *data)
{
	list_t p, o = NULL;

	p = *list;
	while (p) {
		if (p->data == data) {
			if (o) o->next = p->next;
			else *list = p->next;
			free(p);
			break;
		}
		o = p;
		p = p->next;
	}
}

void *list_nth(list_t const *list, int n)
{
	list_t p;
	for (p = *list; p && n; n--, p = p->next);
	if (p) return p->data;
	return 0;
	
}

void list_insert(list_t *list, void *data, void *before)
{
	list_t n, p, o = NULL;

	p = *list;
	while (p) {
		if (p->data == before) {
			n = calloc(1, sizeof(struct _list_t));
			n->data = data;
			n->next = p;
			if (o) o->next = n;
			else *list = n;
			break;
		}
		o = p;
		p = p->next;
	}
}

void *list_pop(list_t *list)
{
	void *data = list_peek(list);
	list_remove(list, data);
	return data;
}


void list_sort(list_t *list, int (* cmp)(void *, void *))
{
	list_t p, q, e, tail;
	int insize, nmerges, psize, qsize, i;

	if (!*list) return;

	insize = 1;

	for (;;) {
		p = *list;
		*list = NULL;
		tail = NULL;

		nmerges = 0;

		while (p) {
			nmerges++;
			q = p;
			psize = 0;
			for (i = 0; i < insize; i++) {
				psize++;
				q = q->next;
				if (!q) break;
			}

			qsize = insize;
			while (psize > 0 || (qsize > 0 && q)) {
				if (psize == 0) {
					e = q; q = q->next; qsize--;
				} else if (qsize == 0 || !q) {
					e = p; p = p->next; psize--;
				} else if ((* cmp)(p->data, q->data) <= 0) {
					e = p; p = p->next; psize--;
				} else {
					e = q; q = q->next; qsize--;
				}
				if (tail) {
					tail->next = e;
				} else {
					*list = e;
				}
				tail = e;
			}
			p = q;
		}

		tail->next = NULL;
		if (nmerges <= 1)
			return;
		insize *= 2;
	}
}

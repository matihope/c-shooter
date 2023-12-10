/**
 * @file collection.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#ifndef COLLECTION_H
#define COLLECTION_H

#include <stdlib.h>

typedef struct CNG_CollectionItem     CNG_CollectionItem;
typedef struct CNG_Collection         CNG_Collection;
typedef struct CNG_CollectionIterator CNG_CollectionIterator;

struct CNG_CollectionItem {
	void *data;
};

struct CNG_Collection {
	bool                (*data_cmp_func)(void *, void *);
	size_t              elements;
	size_t              memory_size;
	CNG_CollectionItem *data;
};

struct CNG_CollectionIterator {
	void   *data;
	ssize_t index;
};

void CNG_Collection_init(
	CNG_Collection *collection, bool (*data_cmp_func)(void *, void *)
);

size_t CNG_Collection_size(CNG_Collection *collection);

void CNG_Collection_freeElements(CNG_Collection *collection);

void CNG_Collection_destroy(CNG_Collection *collection);

bool CNG_Collection_insert(CNG_Collection *collection, void *data);

void CNG_CollectionIterator_init(CNG_CollectionIterator *it);

int CNG_CollectionIterator_next(
	CNG_Collection *collection, CNG_CollectionIterator *iterator
);

#endif

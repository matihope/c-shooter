/**
 * @file collection.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#pragma once

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

void CNG_CollectionCreate(
	CNG_Collection *collection, bool (*data_cmp_func)(void *, void *)
);
void CNG_CollectionDestroy(CNG_Collection *collection);

bool CNG_CollectionInsert(CNG_Collection *collection, void *data);

CNG_CollectionIterator CNG_CollectionStartIteration();

int CNG_CollectionNext(
	CNG_Collection *collection, CNG_CollectionIterator *iterator
);

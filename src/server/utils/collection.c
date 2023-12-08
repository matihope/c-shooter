/**
 * @file collection.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "collection.h"

#include <stdio.h>

void CNG_CollectionCreate(
	CNG_Collection *collection, bool (*data_cmp_func)(void *, void *)
) {
	collection->data_cmp_func = data_cmp_func;
	collection->elements      = 0;
	collection->memory_size   = 1;
	collection->data          = malloc(sizeof(CNG_CollectionItem));
}

void CNG_CollectionDestroy(CNG_Collection *collection) {
	free(collection->data);
}

bool CNG_CollectionInsert(CNG_Collection *collection, void *data) {
	bool unique = true;
	for (size_t elem_id = 0; elem_id < collection->elements; ++elem_id)
		if (collection->data_cmp_func(collection->data[elem_id].data, data))
			unique = false;
	if (unique) {
		collection->elements++;
		if (collection->elements > collection->memory_size) {
			collection->memory_size *= 2;
			void *new_data = realloc(
				collection->data,
				sizeof(CNG_CollectionItem) * collection->memory_size
			);
			if (new_data) collection->data = new_data;
		}
		collection->data[collection->elements - 1].data = data;
	}
	return unique;
}

CNG_CollectionIterator CNG_CollectionStartIteration() {
	CNG_CollectionIterator result;
	result.data  = NULL;
	result.index = -1;
	return result;
}

int CNG_CollectionNext(
	CNG_Collection *collection, CNG_CollectionIterator *iterator
) {
	iterator->index++;
	if (iterator->index >= collection->elements) {
		iterator->data = NULL;
		return false;
	}
	iterator->data = collection->data[iterator->index].data;
	return true;
}

/**
 * @file collection.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "CNG/common/collection.h"

#include <stdio.h>

void CNG_Collection_init(
	CNG_Collection *collection, bool (*data_cmp_func)(void *, void *)
) {
	collection->data_cmp_func = data_cmp_func;
	collection->elements      = 0;
	collection->memory_size   = 1;
	collection->data          = malloc(sizeof(CNG_CollectionItem));
}

void CNG_Collection_destroy(CNG_Collection *collection) {
	free(collection->data);
}

bool CNG_Collection_insert(CNG_Collection *collection, void *data) {
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

void CNG_CollectionIterator_init(CNG_CollectionIterator *it) {
	it->data  = NULL;
	it->index = -1;
}

int CNG_CollectionIterator_next(
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

void CNG_Collection_freeElements(CNG_Collection *collection) {
	CNG_CollectionIterator it;
	CNG_CollectionIterator_init(&it);
	while (CNG_CollectionIterator_next(collection, &it)) free(it.data);
}

size_t CNG_Collection_size(CNG_Collection *collection) {
	return collection->elements;
}

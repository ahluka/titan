#include "base.h"
#include "panic.h"
#include "memory.h"
#include "list.h"

struct ListElem {
	struct ListElem *next;
	void *data;
};

struct ListHead {
	ListElem *head;
	uint32_t size;
	bool freeData;
};

/*
 * List_Create
 */
ListHead *List_Create(enum ListFreeType freeType)
{
	ListHead *list = MemAlloc(sizeof(*list));

	list->head = NULL;
	list->size = 0;
	list->freeData = (freeType == LIST_FREE_DATA) ? true : false;

	return list;
}

/*
 * List_Destroy
 */
static void DestroyElem(ListElem *elem, bool freeData)
{
	assert(elem != NULL);

	if (freeData) {
		MemFree(elem->data);
	}

	MemFree(elem);
}

void List_Destroy(ListHead *list)
{
	assert(list != NULL);

	if (list->size == 0) {
		MemFree(list);
		return;
	}

	ListElem *i = list->head;
	ListElem *dead = NULL;

	while (i) {
		dead = i;
		i = i->next;
		DestroyElem(dead, list->freeData);
	}

	MemFree(list);
}

/*
 * List_Add
 */
ecode_t List_Add(ListHead *list, void *data)
{
	assert(list != NULL);

	ListElem *elem = MemAlloc(sizeof(*elem));
	elem->data = data;

	elem->next = list->head;
	list->head = elem;
	list->size++;

	return EOK;
}

/*
 * List_Remove
 */
ecode_t List_Remove(ListHead *list, ListPredFn predFn)
{
	assert(list != NULL);
	assert(predFn != NULL);

	for (ListElem *i = list->head, *prev = i; i; prev = i, i = i->next) {
		if (predFn(i->data)) {
			/* if it's the first elem just destroy and reseat the
			 * list's head. */
			if (i == list->head) {
				list->head = i->next;
				DestroyElem(i, list->freeData);
				list->size--;
				return EOK;
			}

			prev->next = i->next;
			DestroyElem(i, list->freeData);
			list->size--;
			return EOK;
		}
	}

	return EFAIL;
}

/*
 * List_ForEach
 */
ecode_t List_ForEach(ListHead *list, ListCallbackFn callback, void *user)
{
	assert(list != NULL);
	assert(callback != NULL);

	for (ListElem *i = list->head; i; i = i->next) {
		callback(i->data, user);
	}

	return EOK;
}

/*
 * List_At
 */
void *List_At(ListHead *list, uint32_t idx)
{
	assert(list != NULL);

	if (idx > list->size) {
		Panic(Fmt("Invalid index %u, list has %u elements", idx,
			list->size));
	}

	for (ListElem *iter = list->head; iter; iter = iter->next, idx--) {
		if (idx == 0) {
			return iter->data;
		}
	}

	Panic("This shouldn't happen...");
	return NULL;	/* not reached; shuts up warning */
}

/*
 * List_GetSize
 */
size_t List_GetSize(ListHead *list)
{
	assert(list != NULL);

	return list->size;
}

/*
 * List_Contains
 */
bool List_Contains(ListHead *list, void *data, DeepCmpFn deepCmp,
		uint32_t *_idx)
{
	assert(list != NULL);
	assert(data != NULL);
	assert(deepCmp != NULL);

	uint32_t index = 0;
	for (ListElem *i = list->head; i; i = i->next, index++) {
		if (deepCmp != NULL) {
			if (deepCmp(i->data, data)) {
				if (_idx != NULL) {
					*_idx = index;
				}
				return true;
			}
		} else {
			if (i->data == data) {
				if (_idx != NULL) {
					*_idx = index;
				}
				return true;
			}
		}
	}

	return false;
}

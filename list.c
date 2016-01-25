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
	uint32_t count;
	bool freeData;
};

/*
 * List_Create
 */
ListHead *List_Create(enum ListFreeType freeType)
{
	ListHead *list = MemAlloc(sizeof(*list));

	list->head = NULL;
	list->count = 0;
	list->freeData = (freeType == LIST_FREE_DATA) ? true : false;

	return list;
}

/*
 * List_Destroy
 */
static void DestroyElem(ListElem *elem, bool freeData)
{
	if (freeData) {
		MemFree(elem->data);
	}

	MemFree(elem);
}

void List_Destroy(ListHead *list)
{
	if (!list) {
		Panic("invalid list");
	}

	if (list->count == 0) {
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

	//MemFree(i);
	MemFree(list);
}

/*
 * List_Add
 */
ecode_t List_Add(ListHead *list, void *data)
{
	if (!list) {
		Panic("invalid list");
	}

	ListElem *elem = MemAlloc(sizeof(*elem));
	elem->data = data;

	elem->next = list->head;
	list->head = elem;
	list->count++;

	return EOK;
}

/*
 * List_Remove
 */
ecode_t List_Remove(ListHead *list, ListPredFn predFn)
{
	if (!list) {
		Panic("invalid list");
	}

	if (!predFn) {
		Panic("invalid predicate function");
	}

	for (ListElem *i = list->head, *prev = i; i; prev = i, i = i->next) {
		if (predFn(i->data)) {
			/* if it's the first elem just destroy and reseat the
			 * list's head. */
			if (i == list->head) {
				list->head = i->next;
				DestroyElem(i, list->freeData);
				list->count--;
				return EOK;
			}

			prev->next = i->next;
			DestroyElem(i, list->freeData);
			list->count--;
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
	if (!list) {
		Panic("invalid list");
	}

	if (!callback) {
		Panic("invalid callback function");
	}

	for (ListElem *i = list->head; i; i = i->next) {
		callback(i->data, user);
	}

	return EOK;
}

/*
 * List_GetSize
 */
size_t List_GetSize(ListHead *list)
{
	if (!list) {
		Panic("invalid list");
	}

	return list->count;
}

#pragma once

typedef struct ListHead ListHead;
typedef struct ListElem ListElem;

/* A function pointer that receives a pointer to user data and returns
 * true or false. Used for list element deletion.
 */
typedef bool (*ListPredFn)(void *data);
typedef void (*ListCallbackFn)(void *data);

/* Return a pointer to a new list head, ready for use.
 * If freeType is LIST_FREE_DATA, List_Destroy will free the user data
 * of each ListElem so you don't have to do it yourself.
 */
enum ListFreeType {
	LIST_MANUAL,
	LIST_FREE_DATA
};

ListHead *List_Create(enum ListFreeType freeType);
void List_Destroy(ListHead *list);

/* add given data pointer to the list */
ecode_t List_Add(ListHead *list, void *data);

/* Given a list and a pointer to a predicate function, remove the list
 * element when the predicate returns true.
 */
ecode_t List_Remove(ListHead *list, ListPredFn predFn);

/* Given a list, iterate over it and pass each element to the given
 * callback function.
 */
ecode_t List_ForEach(ListHead *list, ListCallbackFn callback);

size_t List_GetSize(ListHead *list);

// #pragma once
//
// /*
//  * list.h
//  *	Implements a simple singly-linked list that supports insertion,
//  *	removal, and iteration with a callback. It stores all data as void
//  *	pointers.
//  *
//  *	Use List_Create() to get a pointer to a ListHead, which you will need
//  *	for all other operations. Pass it to List_Destroy() when you're done.
//  *	If you specified LIST_FREE_DATA when calling List_Create(), then
//  *	List_Destroy() will pass your pointers to MemFree() for you.
//  *
//  *	List_Add() appends the given data pointer to the head of the list.
//  *	List_Remove() requires a predicate callback function from you, then
//  *	linearly searches the list, passing each element to said callback.
//  *	When your callback returns true on a given element, that element is
//  *	removed from the list (and its data pointer passed to MemFree() if you
//  *	specified LIST_FREE_DATA).
//  *	List_ForEach() linearly iterates over the list, passing each data
//  *	pointer to a specified callback function.
//  *	List_GetSize() is obvious.
//  */
// typedef struct ListHead ListHead;
// typedef struct ListElem ListElem;
//
// /* A function pointer that receives a pointer to user data and returns
//  * true or false. Used for list element deletion.
//  */
// typedef bool (*ListPredFn)(void *data);
// typedef void (*ListCallbackFn)(void *listItem, void *user);
//
// /* Return a pointer to a new list head, ready for use.
//  * If freeType is LIST_FREE_DATA, List_Destroy will free the user data
//  * of each ListElem so you don't have to do it yourself.
//  */
// enum ListFreeType {
// 	LIST_MANUAL,
// 	LIST_FREE_DATA
// };
//
// ListHead *List_Create(enum ListFreeType freeType);
// void List_Destroy(ListHead *list);
//
// size_t List_GetSize(ListHead *list);
//
// /* add given data pointer to the list */
// ecode_t List_Add(ListHead *list, void *data);
//
// /* Given a list and a pointer to a predicate function, remove the list
//  * element when the predicate returns true.
//  */
// ecode_t List_Remove(ListHead *list, ListPredFn predFn);
// ecode_t List_RemoveAt(ListHead *list, uint32_t idx);
//
// /* Given a list, iterate over it and pass each element to the given
//  * callback function, as well as the given user data (which can be NULL).
//  *
//  */
// ecode_t List_ForEach(ListHead *list, ListCallbackFn callback, void *user);
//
// /* Returns the element at the given index within the list. Indices start at
//  * zero.
//  */
// void *List_At(ListHead *list, uint32_t idx);
//
// /* Check if the given list contains the given datum. If deepCmp is non-NULL
//  * then it is used to perform the comparison, and should return true or false
//  * appropriately.
//  * Returns true if the list contains data. If _idx is non-NULL, it will contains
//  * the index of data within the list when true is returned.
//  */
// typedef bool (*DeepCmpFn)(void *a, void *b);
// bool List_Contains(ListHead *list, void *data, DeepCmpFn deepCmp,
// 		uint32_t *_idx);

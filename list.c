#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

/* private functions */
bool List_is_modifiable(const List *list);
ListNode *List_get_node(const List *list, const void *value);
ListNode *ListNode_new(void *value);
void ListNode_delete(ListNode *node);

/* [private]
 * Allocates memory for the new List.
 * Returns: A pointer to the allocated memory if it succeed to allocate; otherwise NULL.
 */
List *List_alloc() {
    List *list = (List *)malloc(sizeof(List));
    return list;
}

/* [public]
 * Initializes the new List instance.
 * Returns: A pointer to the new list if it succeed to allocate memory for it; otherwise NULL.
 */
List *List_new(VALUE_COMPARER comparer) {
    assert(comparer != NULL);

    List *list = List_alloc();
    if (!list) {
        return NULL;
    }
    list->head = NULL;
    list->traversing = 0;
    return list;
}

/* [public]
 * Deletes the List instance.
 * Remarks: This function will do nothing if NULL is given.
 */
void List_delete(List *list) {
    if (list) {
        assert(List_is_modifiable(list));
        for (ListNode *p = list->head; p != NULL;) {
            ListNode *next = p->next;
            ListNode_delete(p);
            p = next;
        }
        list->head = NULL;
        list->comparer = NULL;
        free(list);
    }
}

/* [public]
 * Adds the value to the list.
 * Returns: true if succeeded to add; otherwise, false.
 */
bool List_add(List *list, void *value) {
    assert(list != NULL);
    assert(List_is_modifiable(list));

    /* Allocate new node */
    ListNode *new = ListNode_new(value);
    if (!new) {
        return false;
    }
    /* Add to the list. */
    if (list->head) {
        ListNode *p = list->head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = new;
    } else {
        list->head = new;
    }
    return true;
}

/* [private]
 * Gets the element of the given list which has the value that equals to `value`.
 * Returns: The node which has the value that equals to `value` if found; otherwise, NULL.
 */
ListNode *List_get_node(const List *list, const void *value) {
    assert(list != NULL);

    VALUE_COMPARER comparer = list->comparer;
    for (ListNode *p = list->head; p != NULL; p = p->next) {
        if (comparer(p->value, value) == 0) {
            return p;
        }
    }
    return NULL;
}

/* [public]
 * Gets the value from the given list.
 * Returns: The value that equals to `value` if found; otherwise, NULL.
 */
void *List_get(const List *list, const void *value) {
    ListNode *p = List_get_node(list, value);
    if (p) {
        return p->value;
    }
    return NULL;
}

void *List_get_at(const List *list, const unsigned long index) {
    assert(list != NULL);
    assert(index >= 0);

    unsigned long i = 0;
    ListNode *p;
    for (p = list->head; p != NULL && i < index; p = p->next, ++i) { }
    if (p) {
        return p->value;
    }
    return NULL;
}

/* [public]
 * Removes the value from the given list.
 * Returns: true if succeeded to remove; otherwise, false.
 */
bool List_remove(List *list, void *value) {
    assert(list != NULL);
    assert(List_is_modifiable(list));

    ListNode *prev = NULL, *p;
    VALUE_COMPARER comparer = list->comparer;
    for (p = list->head; p != NULL; prev = p, p = p->next) {
        if (comparer(p->value, value)) {
            if (prev) {
                prev->next = p->next;
            }
            ListNode_delete(p);
            return true;
        }
    }
    return false;
}

/* [public]
 * Removes the value at the given index in the list.
 * Returns: true if succeeded to remove; otherwise, false.
 */
bool List_remove_at(List* list, unsigned long index) {
    assert(list != NULL);
    assert(index >= 0);
    assert(List_is_modifiable(list));

    unsigned long i = 0;
    ListNode *prev = NULL, *p;
    for (p = list->head; p != NULL && i < index; prev = p, p = p->next, ++i) { }
    if (p != NULL) {
        if (prev != NULL) {
            prev->next = p->next;
        }
        ListNode_delete(p);
        return true;
    }
    return false;
}

inline bool List_is_modifiable(const List *list) {
    return 0 == list->traversing;
}

/* [public]
 * Iterates for each elements in the list.
 * Remarks:
 * * When the given iterator returns false, iteration will be stopped.
 * * While iterating, any functions that make any changes to the list MUST NOT be called.
 */
void List_each(List *list, LIST_ITERATOR iter) {
    assert(list != NULL);
    assert(iter != NULL);

    ++list->traversing;
    unsigned long i = 0;
    for (ListNode *p = list->head; p != NULL; p = p->next, ++i) {
        if (!iter(i, p->value))
            break;
    }
    --list->traversing;
}

/****************************************
 *                                      *
 *               ListNode               *
 *                                      *
 ****************************************/

/*
 * Allocates memory for the new ListNode.
 * Returns: A pointer to the allocated memory if it succeed to allocate; otherwise NULL.
 */
ListNode *ListNode_alloc() {
    return (ListNode *)malloc(sizeof(ListNode));
}

/*
 * Initializes the new ListNode instance.
 * Returns: A pointer to the new list if it succeed to allocate memory for it; otherwise NULL.
 */
ListNode *ListNode_new(void *value) {
    ListNode *p = ListNode_alloc();
    if (!p) {
        return NULL;
    }
    p->value = value;
    p->next = NULL;
    return p;
}

/*
 * Deletes the ListNode instance.
 * Remarks: This function will do nothing if NULL is given.
 */
void ListNode_delete(ListNode *node) {
    if (node) {
        node->value = NULL;
        node->next = NULL;
        free(node);
    }
}

/* vim: set et ts=4 sw=4 sts=4: */

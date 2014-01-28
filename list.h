#ifndef ylaciexpr_list_h
#define ylaciexpr_list_h

#include <stdbool.h>

typedef int (*VALUE_COMPARER)(const void *, const void *);

typedef struct list_node ListNode;

typedef struct list_ {
    ListNode* head;
    unsigned long size;
    VALUE_COMPARER comparer;
    unsigned int traversing;
} List;

struct list_node {
    void *value;
    ListNode *next;
};

typedef enum list_iteration_state_t {
    LITST_UNKNOWN = 0,
    LITST_BEFORE,
    LITST_RUNNING,
    LITST_AFTER
} ListIterationState;

typedef struct {
    List *list;
    ListNode *current;
    ListIterationState state;
} ListIterator;

List *List_new(VALUE_COMPARER comparer);
void List_delete(List *list);
const unsigned long List_size(const List *list);
const bool List_is_empty(const List *list);
bool List_add(List *list, void *value);
void *List_get(const List *list, const void *value);
void *List_get_at(const List *list, const unsigned long index);
bool List_remove(List *list, void *value);
bool List_remove_at(List *list, unsigned long index);
ListIterator *List_iterator(List *list);

ListIterator *ListIter_new(List *list);
void ListIter_delete(ListIterator *iter);
void *ListIter_current(const ListIterator *iter);
bool ListIter_move_next(ListIterator *iter);

#endif

/* vim: set et ts=4 sw=4 sts=4: */

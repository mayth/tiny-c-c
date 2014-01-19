/************************************************
    Trie, from rb_trie (Trie for Ruby)
    Original is available on github:
        https://github.com/mayth/rb_trie
 ************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "trie.h"

TrieNode *trie_search_leaf_node(Trie *trie, const char *key);

/***** TrieNode Operation *****/
TrieNode *
trienode_alloc(void)
{
    TrieNode *p = (TrieNode *)malloc(sizeof(TrieNode));
    if (!p) {
        fprintf(stderr, "Cannot allocate memory for TrieNode.\n");
        abort();
    }
    return p;
}

TrieNode *
trienode_new(void)
{
    TrieNode *node = trienode_alloc();
    if (!node) {
        return NULL;
    }
    node->c = 0;
    node->next = NULL;
    node->child = NULL;
    node->value = EMPTY_VALUE;
    return node;
}

void
trienode_free(TrieNode *node)
{
    if (node) {
        if (node->next) {
            trienode_free(node->next);
        }
        if (node->child) {
            trienode_free(node->child);
        }
        free(node);
    }
}

TrieNode *
trienode_add_child(TrieNode *node, TrieNode *child)
{
    if (node->child) {
        child->next = node->child;
        node->child = child;
    } else {
        node->child = child;
    }
    return child;
}

TrieNode *
trienode_find(TrieNode *start, const char key)
{
    TrieNode *p;
    for (p = start; p != NULL && p->c != key; p = p->next);
    return p;
}

TrieNode *
trienode_create(TrieNode *node, const char *key)
{
    const size_t key_len = strlen(key);
    TrieNode *p = node;
    for (size_t i = 0; i < key_len; ++i) {
        const char k = key[i];
        TrieNode *child = trienode_find(p->child, k);
        if (!child) {
            child = trienode_new();
            child->c = k;
            trienode_add_child(p, child);
        }
        p = child;
    }
    return p;
}

TrieNode *
trienode_search(TrieNode* node, const char *key)
{
    TrieNode *p = node;
    const size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; ++i) {
        const char k = key[i];
        if (p->c == k) {
            p = trienode_find(p->next, k);
        } else {
            p = trienode_find(p->child, k);
        }
        if (!p) {
            return NULL;
        }
    }
    return p;
}

TrieNode *
trienode_search_leaf(TrieNode* node, const char *key)
{
    TrieNode *result = trienode_search(node, key);
    if (!result || !result->value)
        return NULL;
    return result;
}

void
trienode_traverse(TrieNode* node, TRAVERSE_FUNC func)
{
    if (node) {
        if (node->value) {
            func(node->value);
        }
        for (TrieNode *p = node->child; p != NULL; p = p->next) {
            trienode_traverse(p, func);
        }
    }
}

void
trienode_traverse_with_key(TrieNode *node, const char *key, TRAVERSE_WITH_KEY_FUNC func)
{
    if (node) {
        const size_t key_len = strlen(key);
        char *tmp_key = (char *)malloc(sizeof(char) * (key_len + 2));
        strcpy(tmp_key, key);
        tmp_key[key_len] = node->c;
        tmp_key[key_len+1] = '\0';
        if (node->value) {
            func(node->value, tmp_key);
        }
        for (TrieNode *p = node->child; p != NULL; p = p->next) {
            trienode_traverse_with_key(p, tmp_key, func);
        }
        free(tmp_key);
    }
}

unsigned long
trienode_count(TrieNode *node)
{
    unsigned long count = 0;
    if (node->value) {
        ++count;
    }
    for (TrieNode *p = node->child; p != NULL; p = p->next) {
        count += trienode_count(p);
    }
    return count;
}

/***** Trie Operation *****/
Trie *
trie_alloc(void)
{
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    if (!trie) {
        fprintf(stderr, "Cannot allocate memory for Trie\n");
        abort();
    }
    return trie;
}

Trie *
trie_new(void)
{
    Trie *trie = trie_alloc();
    trie->root = trienode_new();
    trie->traversing = 0;
    return trie;
}

void
trie_free(Trie *trie)
{
    if (trie && trie->root) {
        trienode_free(trie->root);
    }
}

void
trie_store(Trie *trie, const char *key, VALUE value)
{
    if (IS_MODIFIABLE(trie)) {
        TrieNode *node = trienode_create(trie->root, key);
        node->value = value;
    }
}

VALUE
trie_delete(Trie *trie, const char *key)
{
    TrieNode *result = trie_search_leaf_node(trie, key);
    if (result) {
        VALUE v = result->value;
        result->value = EMPTY_VALUE;
        return v;
    }
    return EMPTY_VALUE;
}

VALUE
trie_search(Trie *trie, const char *key)
{
    TrieNode *p = trienode_search(trie->root, key);
    if (p) {
        return p->value;
    }
    return EMPTY_VALUE;
}

TrieNode *
trie_search_leaf_node(Trie *trie, const char *key)
{
    TrieNode *p = trienode_search_leaf(trie->root, key);
    if (p) {
        return p;
    }
    return NULL;
}

VALUE
trie_search_leaf(Trie *trie, const char *key)
{
    TrieNode *p = trie_search_leaf_node(trie, key);
    if (p) {
        return p->value;
    }
    return EMPTY_VALUE;
}

void
trie_traverse_with_key(Trie *trie, char *key, TRAVERSE_WITH_KEY_FUNC func)
{
    ++trie->traversing;
    trienode_traverse_with_key(trie->root, key, func);
    --trie->traversing;
}

void
trie_traverse(Trie *trie, TRAVERSE_FUNC func)
{
    ++trie->traversing;
    trienode_traverse(trie->root, func);
    --trie->traversing;
}

void
trie_common_prefix_traverse(Trie *trie, char *prefix, TRAVERSE_WITH_KEY_FUNC func)
{
    char* sub_prefix;
    size_t len;
    TrieNode *sub = trie_search(trie, prefix);
    if (!sub) {
        return;
    }
    len = strlen(prefix);
    sub_prefix = (char *)malloc(sizeof(char) * len);
    if (!sub_prefix) {
        fprintf(stderr, "Cannot allocate memory for sub-prefix string.\n");
        abort();
    }
    strncpy(sub_prefix, prefix, len - 1);
    trienode_traverse_with_key(sub, sub_prefix, func);
    free(sub_prefix);
}

unsigned long
trie_count(Trie *trie)
{
    return trienode_count(trie->root);
}

/* vim: set expandtab ts=4 sw=4 sts=4: */

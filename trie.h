/************************************************
    Trie, from rb_trie (Trie for Ruby)
    Original is available on github:
        https://github.com/mayth/rb_trie
 ************************************************/

#ifndef ylaciexpr_trie_h
#define ylaciexpr_trie_h

#define NUM_CHILDREN 256
#define INITIAL_STR_LEN 32
#define INCREMENT_STR_LEN 32

#define IS_MODIFIABLE(trie) (0 == trie->traversing)

typedef struct trie_ Trie;
typedef struct trienode_ TrieNode;
typedef void* VALUE;

#define EMPTY_VALUE (NULL)

struct trie_ {
    TrieNode *root;
    int traversing;
};

struct trienode_ {
    char c;
    TrieNode *next;
    TrieNode *child;
    VALUE value;
};

typedef void (*TRAVERSE_FUNC)(const VALUE value);
typedef void (*TRAVERSE_WITH_KEY_FUNC)(const VALUE node, const char *key);

Trie *trie_new(void);
void trie_free(Trie *trie);
void trie_store(Trie *trie, const char *key, VALUE value);
VALUE trie_delete(Trie *trie, const char *key);
VALUE trie_search(Trie *trie, const char *key);
VALUE trie_search_leaf(Trie *trie, const char *key);
void trie_traverse(Trie *trie, TRAVERSE_FUNC func);
void trie_traverse_with_key(Trie *trie, char *key, TRAVERSE_WITH_KEY_FUNC func);
void trie_common_prefix_traverse(Trie *trie, char *prefix, TRAVERSE_WITH_KEY_FUNC func);
unsigned long trie_count(Trie *trie);

#endif

/* vim: set expandtab ts=4 sw=4 sts=4: */

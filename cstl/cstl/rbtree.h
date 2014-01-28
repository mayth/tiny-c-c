/* 
 * Copyright (c) 2006-2010, KATO Noriaki
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*! 
 * \file rbtree.h
 * \brief 赤黒木
 * \author KATO Noriaki <katono@users.sourceforge.jp>
 * \date 2006-10-29
 * $URL: http://svn.sourceforge.jp/svnroot/cstl/tags/rel-0.7.0/cstl/rbtree.h $
 * $Id: rbtree.h 328 2010-01-24 03:49:15Z katono $
 *
 * このファイルを直接インクルードしないこと
 */
#ifndef CSTL_RBTREE_H_INCLUDED
#define CSTL_RBTREE_H_INCLUDED

#include <stdlib.h>
#include "common.h"


#define CSTL_LESS(x, y)		((x) == (y) ? 0 : (x) < (y) ? -1 : 1)
#define CSTL_GREATER(x, y)	((x) == (y) ? 0 : (x) > (y) ? -1 : 1)


#define CSTL_RBTREE_IS_HEAD(node, Name)		((node)->color == Name##_COLOR_HEAD)
#define CSTL_RBTREE_IS_ROOT(node, Name)		CSTL_RBTREE_IS_HEAD((node)->parent, Name)
#define CSTL_RBTREE_IS_NIL(node, Name)		((node) == (Name##RBTree *) &Name##RBTree_nil)


#define CSTL_RBTREE_IMPLEMENT(Name, KeyType, ValueType, Compare)	\
\
/*! \
 * \brief 赤黒木の色\
 */\
enum {\
	Name##_COLOR_RED,\
	Name##_COLOR_BLACK,\
	Name##_COLOR_HEAD\
};\
\
static const Name##RBTree Name##RBTree_nil = {\
	(Name##RBTree *) &Name##RBTree_nil, \
	(Name##RBTree *) &Name##RBTree_nil, \
	(Name##RBTree *) &Name##RBTree_nil, \
	Name##_COLOR_BLACK\
};\
\
static Name##RBTree *Name##RBTree_new(void);\
static void Name##RBTree_delete(Name##RBTree *self);\
static void Name##RBTree_clear(Name##RBTree *self);\
static int Name##RBTree_empty(Name##RBTree *self);\
static void Name##RBTree_insert(Name##RBTree *self, Name##RBTree *node);\
static void Name##RBTree_erase(Name##RBTree *self, Name##Iterator pos);\
static size_t Name##RBTree_count(Name##RBTree *self, KeyType key);\
static Name##Iterator Name##RBTree_find(Name##RBTree *self, KeyType key);\
static Name##Iterator Name##RBTree_lower_bound(Name##RBTree *self, KeyType key);\
static Name##Iterator Name##RBTree_upper_bound(Name##RBTree *self, KeyType key);\
static Name##Iterator Name##RBTree_begin(Name##RBTree *self);\
static Name##Iterator Name##RBTree_end(Name##RBTree *self);\
static Name##Iterator Name##RBTree_rbegin(Name##RBTree *self);\
static Name##Iterator Name##RBTree_rend(Name##RBTree *self);\
static Name##Iterator Name##RBTree_next(Name##Iterator pos);\
static Name##Iterator Name##RBTree_prev(Name##Iterator pos);\
\
static void Name##RBTree_set_left(Name##RBTree *node, Name##RBTree *t);\
static void Name##RBTree_set_right(Name##RBTree *node, Name##RBTree *t);\
static Name##RBTree *Name##RBTree_get_root(Name##RBTree *self);\
static void Name##RBTree_set_root(Name##RBTree *self, Name##RBTree *t);\
static Name##RBTree *Name##RBTree_find_node(Name##RBTree *t, KeyType key);\
static Name##RBTree *Name##RBTree_replace_subtree(Name##RBTree *node, Name##RBTree *t);\
static void Name##RBTree_swap_parent_child(Name##RBTree *p, Name##RBTree *c);\
static void Name##RBTree_swap(Name##RBTree *s, Name##RBTree *t);\
static void Name##RBTree_rotate_right(Name##RBTree *node);\
static void Name##RBTree_rotate_left(Name##RBTree *node);\
static Name##RBTree *Name##RBTree_get_sibling(Name##RBTree *node);\
static Name##RBTree *Name##RBTree_get_uncle(Name##RBTree *node);\
static void Name##RBTree_balance_for_insert(Name##RBTree *n);\
static void Name##RBTree_balance_for_erase(Name##RBTree *n, Name##RBTree *p_of_n);\
\
\
static void Name##RBTree_set_left(Name##RBTree *node, Name##RBTree *t)\
{\
	CSTL_ASSERT(node && "RBTree_set_left");\
	node->left = t;\
	if (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		t->parent = node;\
	}\
}\
\
static void Name##RBTree_set_right(Name##RBTree *node, Name##RBTree *t)\
{\
	CSTL_ASSERT(node && "RBTree_set_right");\
	node->right = t;\
	if (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		t->parent = node;\
	}\
}\
\
static Name##RBTree *Name##RBTree_get_root(Name##RBTree *self)\
{\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_get_root");\
	return self->right;\
}\
\
static void Name##RBTree_set_root(Name##RBTree *self, Name##RBTree *t)\
{\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_set_root");\
	self->right = t;\
	if (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		t->parent = self;\
	}\
}\
\
static Name##RBTree *Name##RBTree_new(void)\
{\
	Name##RBTree *self;\
	self = (Name##RBTree *) malloc(sizeof(Name##RBTree));\
	if (!self) return 0;\
	self->left = (Name##RBTree *) &Name##RBTree_nil;\
	self->right = (Name##RBTree *) &Name##RBTree_nil;\
	self->parent = (Name##RBTree *) &Name##RBTree_nil;\
	self->color = Name##_COLOR_HEAD;\
	CSTL_MAGIC(self->magic = self);\
	return self;\
}\
\
static void Name##RBTree_clear(Name##RBTree *self)\
{\
	register Name##RBTree *t;\
	register Name##RBTree *tmp;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_clear");\
	t = Name##RBTree_get_root(self);\
	if (CSTL_RBTREE_IS_NIL(t, Name)) return;\
	while (1) {\
		if (!CSTL_RBTREE_IS_NIL(t->left, Name)) {\
			t = t->left;\
			continue;\
		}\
		if (!CSTL_RBTREE_IS_NIL(t->right, Name)) {\
			t = t->right;\
			continue;\
		}\
		if (t == t->parent->left) {\
			t->parent->left = (Name##RBTree *) &Name##RBTree_nil;\
		} else {\
			t->parent->right = (Name##RBTree *) &Name##RBTree_nil;\
		}\
		tmp = t->parent;\
		CSTL_MAGIC(t->magic = 0);\
		free(t);\
		t = tmp;\
		if (CSTL_RBTREE_IS_HEAD(t, Name)) break;\
	}\
}\
\
static void Name##RBTree_delete(Name##RBTree *self)\
{\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_delete");\
	Name##RBTree_clear(self);\
	CSTL_MAGIC(self->magic = 0);\
	free(self);\
}\
\
static int Name##RBTree_empty(Name##RBTree *self)\
{\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_empty");\
	return CSTL_RBTREE_IS_NIL(Name##RBTree_get_root(self), Name);\
}\
\
static Name##RBTree *Name##RBTree_find_node(Name##RBTree *t, KeyType key)\
{\
	register int cmp;\
	while (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		cmp = Compare(key, t->key);\
		if (cmp < 0) {\
			t = t->left;\
		} else if (cmp > 0) {\
			t = t->right;\
		} else {\
			break;\
		}\
	}\
	return t;\
}\
\
static Name##Iterator Name##RBTree_find(Name##RBTree *self, KeyType key)\
{\
	Name##RBTree *t;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_find");\
	t = Name##RBTree_find_node(Name##RBTree_get_root(self), key);\
	return CSTL_RBTREE_IS_NIL(t, Name) ? Name##RBTree_end(self) : t;\
}\
\
static size_t Name##RBTree_count(Name##RBTree *self, KeyType key)\
{\
	register size_t count = 0;\
	register Name##Iterator pos;\
	register Name##Iterator first;\
	register Name##Iterator last;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_count");\
	first = Name##RBTree_lower_bound(self, key);\
	last = Name##RBTree_upper_bound(self, key);\
	for (pos = first; pos != last; pos = Name##RBTree_next(pos)) {\
		count++;\
	}\
	return count;\
}\
\
static Name##Iterator Name##RBTree_lower_bound(Name##RBTree *self, KeyType key)\
{\
	register Name##RBTree *t;\
	register Name##RBTree *tmp;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_lower_bound");\
	tmp = Name##RBTree_end(self);\
	t = Name##RBTree_get_root(self);\
	while (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		if (Compare(key, t->key) <= 0) {\
			tmp = t;\
			t = t->left;\
		} else {\
			t = t->right;\
		}\
	}\
	return tmp;\
}\
\
static Name##Iterator Name##RBTree_upper_bound(Name##RBTree *self, KeyType key)\
{\
	register Name##RBTree *t;\
	register Name##RBTree *tmp;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_upper_bound");\
	tmp = Name##RBTree_end(self);\
	t = Name##RBTree_get_root(self);\
	while (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		if (Compare(key, t->key) < 0) {\
			tmp = t;\
			t = t->left;\
		} else {\
			t = t->right;\
		}\
	}\
	return tmp;\
}\
\
static Name##RBTree *Name##RBTree_replace_subtree(Name##RBTree *node, Name##RBTree *t)\
{\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(node, Name) && "RBTree_replace_subtree");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(t, Name) && "RBTree_replace_subtree");\
	if (node->parent->left == node) {\
		Name##RBTree_set_left(node->parent, t);\
	} else {\
		Name##RBTree_set_right(node->parent, t);\
	}\
	return node;\
}\
\
static void Name##RBTree_swap_parent_child(Name##RBTree *p, Name##RBTree *c)\
{\
	Name##RBTree *cl;\
	Name##RBTree *cr;\
	if (p->parent->left == p) {\
		Name##RBTree_set_left(p->parent, c);\
	} else {\
		Name##RBTree_set_right(p->parent, c);\
	}\
	cl = c->left;\
	cr = c->right;\
	if (p->left == c) {\
		Name##RBTree_set_left(c, p);\
		Name##RBTree_set_right(c, p->right);\
	} else {\
		Name##RBTree_set_right(c, p);\
		Name##RBTree_set_left(c, p->left);\
	}\
	Name##RBTree_set_left(p, cl);\
	Name##RBTree_set_right(p, cr);\
}\
\
static void Name##RBTree_swap(Name##RBTree *s, Name##RBTree *t)\
{\
	Name##RBTree *tp;\
	Name##RBTree *tl;\
	Name##RBTree *tr;\
	int c;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(s, Name) && "RBTree_swap");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(t, Name) && "RBTree_swap");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(s, Name) && "RBTree_swap");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(t, Name) && "RBTree_swap");\
	if (t->parent == s) {\
		Name##RBTree_swap_parent_child(s, t);\
	} else if (s->parent == t) {\
		Name##RBTree_swap_parent_child(t, s);\
	} else {\
		tp = t->parent;\
		tl = t->left;\
		tr = t->right;\
		if (s->parent->left == s) {\
			Name##RBTree_set_left(s->parent, t);\
		} else {\
			Name##RBTree_set_right(s->parent, t);\
		}\
		Name##RBTree_set_left(t, s->left);\
		Name##RBTree_set_right(t, s->right);\
		if (tp->left == t) {\
			Name##RBTree_set_left(tp, s);\
		} else {\
			Name##RBTree_set_right(tp, s);\
		}\
		Name##RBTree_set_left(s, tl);\
		Name##RBTree_set_right(s, tr);\
	}\
	c = s->color;\
	s->color = t->color;\
	t->color = c;\
}\
\
static void Name##RBTree_rotate_right(Name##RBTree *node)\
{\
	Name##RBTree *p;\
	Name##RBTree *n;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(node, Name) && "RBTree_rotate_right");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(node, Name) && "RBTree_rotate_right");\
	p = node->parent;\
	n = node->left;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(n, Name) && "RBTree_rotate_right");\
	Name##RBTree_set_left(node, n->right);\
	Name##RBTree_set_right(n, node);\
	if (p->left == node) {\
		Name##RBTree_set_left(p, n);\
	} else {\
		Name##RBTree_set_right(p, n);\
	}\
}\
\
static void Name##RBTree_rotate_left(Name##RBTree *node)\
{\
	Name##RBTree *p;\
	Name##RBTree *n;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(node, Name) && "RBTree_rotate_left");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(node, Name) && "RBTree_rotate_left");\
	p = node->parent;\
	n = node->right;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(n, Name) && "RBTree_rotate_left");\
	Name##RBTree_set_right(node, n->left);\
	Name##RBTree_set_left(n, node);\
	if (p->left == node) {\
		Name##RBTree_set_left(p, n);\
	} else {\
		Name##RBTree_set_right(p, n);\
	}\
}\
\
static Name##RBTree *Name##RBTree_get_sibling(Name##RBTree *node)\
{\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(node, Name) && "RBTree_get_sibling");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(node, Name) && "RBTree_get_sibling");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_ROOT(node, Name) && "RBTree_get_sibling");\
	return (node->parent->left == node) ? \
		node->parent->right : node->parent->left;\
}\
\
static Name##RBTree *Name##RBTree_get_uncle(Name##RBTree *node)\
{\
	Name##RBTree *g;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(node, Name) && "RBTree_get_uncle");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(node, Name) && "RBTree_get_uncle");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_ROOT(node, Name) && "RBTree_get_uncle");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_ROOT(node->parent, Name) && "RBTree_get_uncle");\
	g = node->parent->parent;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(g, Name) && "RBTree_get_uncle");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(g, Name) && "RBTree_get_uncle");\
	return (g->left == node->parent) ?\
		g->right : g->left;\
}\
\
static void Name##RBTree_balance_for_insert(Name##RBTree *n)\
{\
	Name##RBTree *p;\
	Name##RBTree *g;\
	Name##RBTree *u;\
	Name##RBTree *tmp;\
	while (1) {\
		p = n->parent;\
		if (CSTL_RBTREE_IS_ROOT(n, Name)) {\
			/* case 1 nがroot */\
			n->color = Name##_COLOR_BLACK;\
			break;\
		}\
		if (p->color == Name##_COLOR_BLACK) {\
			/* case 2 pが黒 */\
			break;\
		}\
		/* 以下、pは赤、gは黒 */\
		g = p->parent;\
		u = Name##RBTree_get_uncle(n);\
		if (u->color == Name##_COLOR_RED) {\
			/* case 3 uが赤 */\
			g->color = Name##_COLOR_RED;\
			p->color = Name##_COLOR_BLACK;\
			u->color = Name##_COLOR_BLACK;\
			n = g;\
			/* gをnにして再調整。最悪の場合、根まで伝播 */\
			continue;\
		}\
		/* 以下、uは黒 */\
		if (g->left == p && p->right == n) {\
			/* case 4 pがgのleft */\
			Name##RBTree_rotate_left(p);\
			tmp = p;\
			p = n;\
			n = tmp;\
			/* case 5 leftへ */\
		} else if (g->right == p && p->left == n) {\
			/* case 4 pがgのright */\
			Name##RBTree_rotate_right(p);\
			tmp = p;\
			p = n;\
			n = tmp;\
			/* case 5 rightへ */\
		}\
		if (g->left == p) {\
			/* case 5 left */\
			Name##RBTree_rotate_right(g);\
		} else if (g->right == p) {\
			/* case 5 right */\
			Name##RBTree_rotate_left(g);\
		} else {\
			CSTL_ASSERT(0 && "RBTree_balance_for_insert");\
		}\
		p->color = Name##_COLOR_BLACK;\
		g->color = Name##_COLOR_RED;\
		break;\
	}\
}\
\
static void Name##RBTree_insert(Name##RBTree *self, Name##RBTree *node)\
{\
	register Name##RBTree *n;\
	register Name##RBTree *tmp;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_insert");\
	CSTL_MAGIC(node->magic = self);\
	n = Name##RBTree_get_root(self);\
	if (CSTL_RBTREE_IS_NIL(n, Name)) {\
		/* rootになる */\
		node->color = Name##_COLOR_BLACK;\
		Name##RBTree_set_root(self, node);\
		return;\
	}\
	/* 2分探索木の挿入 */\
	do {\
		tmp = n;\
		if (Compare(node->key, n->key) < 0) {\
			n = n->left;\
		} else {\
			/* 同じ値なら右へ */\
			n = n->right;\
		}\
	} while (!CSTL_RBTREE_IS_NIL(n, Name));\
	if (Compare(node->key, tmp->key) < 0) {\
		Name##RBTree_set_left(tmp, node);\
	} else {\
		Name##RBTree_set_right(tmp, node);\
	}\
	Name##RBTree_balance_for_insert(node);\
}\
\
static void Name##RBTree_balance_for_erase(Name##RBTree *n, Name##RBTree *p_of_n)\
{\
	Name##RBTree *p;\
	Name##RBTree *s;\
	Name##RBTree *sl;\
	Name##RBTree *sr;\
	int c;\
	if (n->color == Name##_COLOR_RED) {\
		/* case 0 nが赤 */\
		n->color = Name##_COLOR_BLACK;\
		return;\
	}\
	while (1) {\
		if (CSTL_RBTREE_IS_ROOT(n, Name)) {\
			/* case 1 nがroot */\
			n->color = Name##_COLOR_BLACK;\
			break;\
		}\
		if (CSTL_RBTREE_IS_NIL(n, Name)) {\
			CSTL_ASSERT(!(CSTL_RBTREE_IS_NIL(p_of_n->left, Name) && CSTL_RBTREE_IS_NIL(p_of_n->right, Name)) && "RBTree_balance_for_erase");\
			p = p_of_n;\
			s = (n == p_of_n->left) ? p_of_n->right : p_of_n->left;\
		} else {\
			p = n->parent;\
			s = Name##RBTree_get_sibling(n);\
		}\
		CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(s, Name) && "RBTree_balance_for_erase");\
		if (s->color == Name##_COLOR_RED) {\
			/* case 2 sが赤 */\
			p->color = Name##_COLOR_RED;\
			s->color = Name##_COLOR_BLACK;\
			if (p->left == n) {\
				Name##RBTree_rotate_left(p);\
				s = p->right;\
			} else {\
				Name##RBTree_rotate_right(p);\
				s = p->left;\
			}\
		}\
		/* 以下、sは黒 */\
		CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(s, Name) && "RBTree_balance_for_erase");\
		sl = s->left;\
		sr = s->right;\
		if (p->color == Name##_COLOR_BLACK && sl->color == Name##_COLOR_BLACK && sr->color == Name##_COLOR_BLACK) {\
			/* case 3 */\
			s->color = Name##_COLOR_RED;\
			n = p;\
			/* pをnにして再調整。最悪の場合、根まで伝播 */\
			continue;\
		}\
		if (p->color == Name##_COLOR_RED && sl->color == Name##_COLOR_BLACK && sr->color == Name##_COLOR_BLACK) {\
			/* case 4 */\
			p->color = Name##_COLOR_BLACK;\
			s->color = Name##_COLOR_RED;\
			break;\
		}\
		if (p->left == n && sl->color == Name##_COLOR_RED && sr->color == Name##_COLOR_BLACK) {\
			/* case 5 nがpのleft */\
			sl->color = Name##_COLOR_BLACK;\
			s->color = Name##_COLOR_RED;\
			Name##RBTree_rotate_right(s);\
			sr = s;\
			s = sl;\
			/* case 6 leftへ */\
		} else if (p->right == n && sl->color == Name##_COLOR_BLACK && sr->color == Name##_COLOR_RED) {\
			/* case 5 nがpのright */\
			sr->color = Name##_COLOR_BLACK;\
			s->color = Name##_COLOR_RED;\
			Name##RBTree_rotate_left(s);\
			sl = s;\
			s = sr;\
			/* case 6 rightへ */\
		}\
		if (p->left == n && sr->color == Name##_COLOR_RED) {\
			/* case 6 left */\
			sr->color = Name##_COLOR_BLACK;\
			Name##RBTree_rotate_left(p);\
		} else if (p->right == n && sl->color == Name##_COLOR_RED) {\
			/* case 6 right */\
			sl->color = Name##_COLOR_BLACK;\
			Name##RBTree_rotate_right(p);\
		} else {\
			CSTL_ASSERT(0 && "RBTree_balance_for_erase");\
		}\
		c = p->color;\
		p->color = s->color;\
		s->color = c;\
		break;\
	}\
}\
\
static void Name##RBTree_erase(Name##RBTree *self, Name##Iterator pos)\
{\
	register Name##RBTree *n;\
	register Name##RBTree *x;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_erase");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(pos, Name) && "RBTree_erase");\
	n = pos;\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(n, Name) && "RBTree_erase");\
	if (CSTL_RBTREE_IS_NIL(n->left, Name) && CSTL_RBTREE_IS_NIL(n->right, Name)) {\
		if (CSTL_RBTREE_IS_ROOT(n, Name)) {\
			/* 最後の一つを削除 */\
			Name##RBTree_set_root(self, (Name##RBTree *) &Name##RBTree_nil);\
		} else {\
			n = Name##RBTree_replace_subtree(n, (Name##RBTree *) &Name##RBTree_nil);\
			if (n->color == Name##_COLOR_BLACK) {\
				Name##RBTree_balance_for_erase((Name##RBTree *) &Name##RBTree_nil, n->parent);\
			}\
		}\
		goto end;\
	}\
	if (CSTL_RBTREE_IS_NIL(n->left, Name)) {\
		n = Name##RBTree_replace_subtree(n, n->right);\
		if (n->color == Name##_COLOR_BLACK) {\
			CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(n->right, Name) && "RBTree_erase");\
			Name##RBTree_balance_for_erase(n->right, 0);\
		}\
		goto end;\
	}\
	if (CSTL_RBTREE_IS_NIL(n->right, Name)) {\
		n = Name##RBTree_replace_subtree(n, n->left);\
		if (n->color == Name##_COLOR_BLACK) {\
			CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(n->left, Name) && "RBTree_erase");\
			Name##RBTree_balance_for_erase(n->left, 0);\
		}\
		goto end;\
	}\
	CSTL_ASSERT(!(CSTL_RBTREE_IS_NIL(n->left, Name) || CSTL_RBTREE_IS_NIL(n->right, Name)) && "RBTree_erase");\
	x = n->left;\
	while (!CSTL_RBTREE_IS_NIL(x->right, Name)) {\
		x = x->right;\
	}\
	Name##RBTree_swap(n, x);\
	n = Name##RBTree_replace_subtree(n, n->left);\
	if (n->color == Name##_COLOR_BLACK) {\
		CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(n, Name) && "RBTree_erase");\
		Name##RBTree_balance_for_erase(n->left, n->parent);\
	}\
end:\
	CSTL_MAGIC(n->magic = 0);\
	free(n);\
}\
\
static Name##Iterator Name##RBTree_begin(Name##RBTree *self)\
{\
	register Name##RBTree *t;\
	register Name##RBTree *tmp;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_begin");\
	tmp = Name##RBTree_end(self);\
	t = Name##RBTree_get_root(self);\
	while (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		tmp = t;\
		t = t->left;\
	}\
	return tmp;\
}\
\
static Name##Iterator Name##RBTree_end(Name##RBTree *self)\
{\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_end");\
	return self;\
}\
\
static Name##Iterator Name##RBTree_rbegin(Name##RBTree *self)\
{\
	register Name##RBTree *t;\
	register Name##RBTree *tmp;\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_rbegin");\
	tmp = Name##RBTree_rend(self);\
	t = Name##RBTree_get_root(self);\
	while (!CSTL_RBTREE_IS_NIL(t, Name)) {\
		tmp = t;\
		t = t->right;\
	}\
	return tmp;\
}\
\
static Name##Iterator Name##RBTree_rend(Name##RBTree *self)\
{\
	CSTL_ASSERT(CSTL_RBTREE_IS_HEAD(self, Name) && "RBTree_rend");\
	return self;\
}\
\
static Name##Iterator Name##RBTree_next(Name##Iterator pos)\
{\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(pos, Name) && "RBTree_next");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(pos, Name) && "RBTree_next");\
	/* 下位検索 */\
	if (!CSTL_RBTREE_IS_NIL(pos->right, Name)) {\
		pos = pos->right;\
		while (!CSTL_RBTREE_IS_NIL(pos->left, Name)) {\
			pos = pos->left;\
		}\
		return pos;\
	}\
	/* 上位検索 */\
	while (!CSTL_RBTREE_IS_ROOT(pos, Name) && pos == pos->parent->right) {\
		pos = pos->parent;\
	}\
	/* 引数のposがrbegin()の時、pos->parentはend()となる。 */\
	return pos->parent;\
}\
\
static Name##Iterator Name##RBTree_prev(Name##Iterator pos)\
{\
	CSTL_ASSERT(!CSTL_RBTREE_IS_HEAD(pos, Name) && "RBTree_prev");\
	CSTL_ASSERT(!CSTL_RBTREE_IS_NIL(pos, Name) && "RBTree_prev");\
	/* 下位検索 */\
	if (!CSTL_RBTREE_IS_NIL(pos->left, Name)) {\
		pos = pos->left;\
		while (!CSTL_RBTREE_IS_NIL(pos->right, Name)) {\
			pos = pos->right;\
		}\
		return pos;\
	}\
	/* 上位検索 */\
	while (!CSTL_RBTREE_IS_ROOT(pos, Name) && pos == pos->parent->left) {\
		pos = pos->parent;\
	}\
	/* 引数のposがbegin()の時、pos->parentはrend()となる。 */\
	return pos->parent;\
}\
\



#define CSTL_RBTREE_WRAPPER_INTERFACE(Name, KeyType, ValueType)	\
\
typedef struct Name Name;\
typedef struct Name##RBTree *Name##Iterator;\
\
Name *Name##_new(void);\
void Name##_delete(Name *self);\
void Name##_clear(Name *self);\
int Name##_empty(Name *self);\
size_t Name##_size(Name *self);\
int Name##_insert_range(Name *self, Name##Iterator first, Name##Iterator last);\
Name##Iterator Name##_erase(Name *self, Name##Iterator pos);\
Name##Iterator Name##_erase_range(Name *self, Name##Iterator first, Name##Iterator last);\
size_t Name##_erase_key(Name *self, KeyType key);\
size_t Name##_count(Name *self, KeyType key);\
Name##Iterator Name##_find(Name *self, KeyType key);\
Name##Iterator Name##_lower_bound(Name *self, KeyType key);\
Name##Iterator Name##_upper_bound(Name *self, KeyType key);\
void Name##_equal_range(Name *self, KeyType key, Name##Iterator *first, Name##Iterator *last);\
Name##Iterator Name##_begin(Name *self);\
Name##Iterator Name##_end(Name *self);\
Name##Iterator Name##_rbegin(Name *self);\
Name##Iterator Name##_rend(Name *self);\
Name##Iterator Name##_next(Name##Iterator pos);\
Name##Iterator Name##_prev(Name##Iterator pos);\
void Name##_swap(Name *self, Name *x);\


#define CSTL_RBTREE_WRAPPER_IMPLEMENT(Name, KeyType, ValueType, Compare)	\
\
typedef struct Name##RBTree Name##RBTree;\
/*! \
 * \brief set/map構造体\
 */\
struct Name {\
	Name##RBTree *tree;\
	size_t size;\
	CSTL_MAGIC(Name *magic;)\
};\
\
CSTL_RBTREE_IMPLEMENT(Name, KeyType, ValueType, Compare)\
\
Name *Name##_new(void)\
{\
	Name *self;\
	self = (Name *) malloc(sizeof(Name));\
	if (!self) return 0;\
	self->tree = Name##RBTree_new();\
	if (!self->tree) {\
		free(self);\
		return 0;\
	}\
	self->size = 0;\
	CSTL_MAGIC(self->magic = self);\
	return self;\
}\
\
void Name##_delete(Name *self)\
{\
	if (!self) return;\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_delete");\
	Name##RBTree_delete(self->tree);\
	CSTL_MAGIC(self->magic = 0);\
	free(self);\
}\
\
void Name##_clear(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_clear");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_clear");\
	Name##RBTree_clear(self->tree);\
	self->size = 0;\
}\
\
int Name##_empty(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_empty");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_empty");\
	return Name##RBTree_empty(self->tree);\
}\
\
size_t Name##_size(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_size");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_size");\
	return self->size;\
}\
\
Name##Iterator Name##_erase(Name *self, Name##Iterator pos)\
{\
	Name##Iterator tmp;\
	CSTL_ASSERT(self && "(Set|Map)_erase");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_erase");\
	CSTL_ASSERT(pos && "(Set|Map)_erase");\
	CSTL_ASSERT(pos != self->tree && "(Set|Map)_erase");\
	CSTL_ASSERT(pos->magic == self->tree && "(Set|Map)_erase");\
	tmp = Name##_next(pos);\
	Name##RBTree_erase(self->tree, pos);\
	self->size--;\
	return tmp;\
}\
\
Name##Iterator Name##_erase_range(Name *self, Name##Iterator first, Name##Iterator last)\
{\
	register Name##Iterator pos;\
	CSTL_ASSERT(self && "(Set|Map)_erase_range");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_erase_range");\
	CSTL_ASSERT(first && "(Set|Map)_erase_range");\
	CSTL_ASSERT(last && "(Set|Map)_erase_range");\
	CSTL_ASSERT(first->magic == self->tree && "(Set|Map)_erase_range");\
	CSTL_ASSERT(last->magic == self->tree && "(Set|Map)_erase_range");\
	pos = first;\
	while (pos != last) {\
		CSTL_ASSERT(!Name##_empty(self) && "(Set|Map)_erase_range");\
		pos = Name##_erase(self, pos);\
	}\
	return pos;\
}\
\
size_t Name##_erase_key(Name *self, KeyType key)\
{\
	register size_t count = 0;\
	register Name##Iterator pos;\
	register Name##Iterator last;\
	CSTL_ASSERT(self && "(Set|Map)_erase_key");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_erase_key");\
	pos = Name##_lower_bound(self, key);\
	last = Name##_upper_bound(self, key);\
	while (pos != last) {\
		CSTL_ASSERT(!Name##_empty(self) && "(Set|Map)_erase_key");\
		pos = Name##_erase(self, pos);\
		count++;\
	}\
	return count;\
}\
\
size_t Name##_count(Name *self, KeyType key)\
{\
	CSTL_ASSERT(self && "(Set|Map)_count");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_count");\
	return Name##RBTree_count(self->tree, key);\
}\
\
Name##Iterator Name##_find(Name *self, KeyType key)\
{\
	CSTL_ASSERT(self && "(Set|Map)_find");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_find");\
	return Name##RBTree_find(self->tree, key);\
}\
\
Name##Iterator Name##_lower_bound(Name *self, KeyType key)\
{\
	CSTL_ASSERT(self && "(Set|Map)_lower_bound");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_lower_bound");\
	return Name##RBTree_lower_bound(self->tree, key);\
}\
\
Name##Iterator Name##_upper_bound(Name *self, KeyType key)\
{\
	CSTL_ASSERT(self && "(Set|Map)_upper_bound");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_upper_bound");\
	return Name##RBTree_upper_bound(self->tree, key);\
}\
\
void Name##_equal_range(Name *self, KeyType key, Name##Iterator *first, Name##Iterator *last)\
{\
	CSTL_ASSERT(self && "(Set|Map)_equal_range");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_equal_range");\
	CSTL_ASSERT(first && "(Set|Map)_equal_range");\
	CSTL_ASSERT(last && "(Set|Map)_equal_range");\
	*first = Name##RBTree_lower_bound(self->tree, key);\
	*last = Name##RBTree_upper_bound(self->tree, key);\
}\
\
Name##Iterator Name##_begin(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_begin");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_begin");\
	return Name##RBTree_begin(self->tree);\
}\
\
Name##Iterator Name##_end(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_end");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_end");\
	return Name##RBTree_end(self->tree);\
}\
\
Name##Iterator Name##_rbegin(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_rbegin");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_rbegin");\
	return Name##RBTree_rbegin(self->tree);\
}\
\
Name##Iterator Name##_rend(Name *self)\
{\
	CSTL_ASSERT(self && "(Set|Map)_rend");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_rend");\
	return Name##RBTree_rend(self->tree);\
}\
\
Name##Iterator Name##_next(Name##Iterator pos)\
{\
	CSTL_ASSERT(pos && "(Set|Map)_next");\
	CSTL_ASSERT(pos->magic && "(Set|Map)_next");\
	return Name##RBTree_next(pos);\
}\
\
Name##Iterator Name##_prev(Name##Iterator pos)\
{\
	CSTL_ASSERT(pos && "(Set|Map)_prev");\
	CSTL_ASSERT(pos->magic && "(Set|Map)_prev");\
	return Name##RBTree_prev(pos);\
}\
\
void Name##_swap(Name *self, Name *x)\
{\
	Name##RBTree *tmp_tree;\
	size_t tmp_size;\
	CSTL_ASSERT(self && "(Set|Map)_swap");\
	CSTL_ASSERT(x && "(Set|Map)_swap");\
	CSTL_ASSERT(self->magic == self && "(Set|Map)_swap");\
	CSTL_ASSERT(x->magic == x && "(Set|Map)_swap");\
	tmp_tree = self->tree;\
	tmp_size = self->size;\
	self->tree = x->tree;\
	self->size = x->size;\
	x->tree = tmp_tree;\
	x->size = tmp_size;\
}\
\


#endif /* CSTL_RBTREE_H_INCLUDED */

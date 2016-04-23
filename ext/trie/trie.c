#include <stdlib.h>
#include <stdio.h>
#include <ruby.h>
#include <assert.h>

#include "trie.h"

/*
 * This structure holds the size of the pointer (to make cleanup easier) and
 * the pointer to the allocated memory.  Data_Make_Struct() and
 * Data_Get_Struct will extract this structure from the wrapped object so you
 * can work with the pointer.
 */

struct trie {
    size_t size;
    void *ptr;
};

/*
 * This frees the internal pointer at garbage-collection time.  Ruby will free
 * the struct trie for us.
 */

static void
trie_free(void *p) {
    struct trie *ptr = p;

    if (ptr->size > 0)
	free(ptr->ptr);
}

/*
 * This allocates the object and default values for the struct trie.
 */

static VALUE
trie_alloc(VALUE klass) {
    VALUE obj;
    struct trie *ptr;

    /* there is no mark function as struct trie does not store ruby
     * objects
     */

    obj = Data_Make_Struct(klass, struct trie, NULL, trie_free, ptr);

    ptr->size = 0;
    ptr->ptr  = NULL;

    return obj;
}

/*
 * This is #initialize which allocates some memory and stores the pointer in
 * struct trie.  (There's not anything useful you can do with the memory,
 * though.)
 */

static VALUE
trie_init(VALUE self, VALUE size) {
    struct trie *ptr;
    size_t requested = NUM2SIZET(size);

    if (0 == requested)
	rb_raise(rb_eArgError, "unable to allocate 0 bytes");

    Data_Get_Struct(self, struct trie, ptr);

    ptr->ptr = malloc(requested);

    if (NULL == ptr->ptr)
	rb_raise(rb_eNoMemError, "unable to allocate %ld bytes", requested);

    ptr->size = requested;

    return self;
}

/*
 * This frees the internal pointer so you can explicitly control the lifetime
 * of the held memory (instead of waiting for GC).  If you don't free the
 * memory the GC will do it for us.
 */

static VALUE
trie_release(VALUE self) {
    struct trie *ptr;

    Data_Get_Struct(self, struct trie, ptr);

    if (0 == ptr->size)
	return self;

    ptr->size = 0;
    free(ptr->ptr);

    return self;
}

/*
 * This defines the C functions as extensions.
 *
 * The name of the Init_ function is important.  What follows Init_ must match
 * the file name (including case) so ruby knows what method to call to define
 * the extensions.
 */

void
Init_trie(void) {
    VALUE cTrie;

    cTrie = rb_const_get(rb_cObject, rb_intern("Trie"));

    rb_define_alloc_func(cTrie, trie_alloc);
    rb_define_method(cTrie, "initialize", trie_init, 1);
    rb_define_method(cTrie, "free", trie_release, 0);
}

/*
 * Older functions from the original trie C repo (based on #TODO: find original github repo)
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <assert.h>

 #include "trie.h"

 inline trie_t *trie_init(void) {
     trie_t *t = (trie_t *) malloc(sizeof(trie_t));
     t->node = 0;
     return t;
 }

 void trie_add(trie_t *t, char *word) {
     int c;
     while ((c = *word++)) {
         assert(c >= 97 && c < 123);
         if (t->chars[c-TRIE_OFFSET] == NULL) {
             t->chars[c-TRIE_OFFSET] = trie_init();
         }
         t->node = 1;
         t = t->chars[c-TRIE_OFFSET];
     }
     t->node = 1;
     t->chars[TRIE_SENTINEL] = trie_init();
 }

 int trie_exists(trie_t *t, char *word) {
     int c;
     while ((c = *word++)) {
         if (t->chars[c-TRIE_OFFSET] == NULL) {
             return 0;
         }
         t = t->chars[c-TRIE_OFFSET];
     }
     return t->chars[TRIE_SENTINEL] != NULL ? 1 : 0;
 }

 int trie_prefix(trie_t *t, char *prefix) {
     int c;
     while ((c = *prefix++)) {
         if (t->chars[c-TRIE_OFFSET] == NULL) {
             return 0;
         }
         t = t->chars[c-TRIE_OFFSET];
     }
     return t->node;
 }

 int trie_load(trie_t *t, char *file) {
     FILE *stream = fopen(file, "r");
     if (stream == NULL) {
         return -1;
     }

     trie_t *root = t;
     int c, words = 0, word_len = 0;
     while ((c = getc(stream)) != EOF) {
         if (c == '\n' || c == '\r') {
             if (word_len > 0) {
                 t->chars[TRIE_SENTINEL] = trie_init();
                 words++;
                 word_len = 0;
                 t = root;
             }
         } else {
             word_len++;
             assert(c >= 97 && c < 123);
             if (t->chars[c-TRIE_OFFSET] == NULL) {
                 t->chars[c-TRIE_OFFSET] = trie_init();
             }
             t->node = 1;
             t = t->chars[c-TRIE_OFFSET];
         }
     }
     if (t != root && word_len > 0) {
         t->chars[TRIE_SENTINEL] = trie_init();
     }
     return words;
 }

 void trie_strip(trie_t *t, char *src, char *dest) {
     if (src == NULL) {
         return;
     }
     if (dest == NULL) {
         dest = src;
     }
     int c, i = 0, last_break = 0, in_trie = 1;
     trie_t *root = t;

     while ((c = dest[i++] = *src++)) {
         if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
             t = root;
             if (in_trie) {
                 i = last_break;
             } else {
                 in_trie = 1;
                 last_break = i;
             }
             continue;
         }
         if (!in_trie) {
             continue;
         }
         if (t->chars[c-TRIE_OFFSET] == NULL) {
             in_trie = 0;
         } else {
             t = t->chars[c-TRIE_OFFSET];
             in_trie = 1;
         }
     }
 }

 void trie_free(trie_t *t) {
   int i;
     for (i = 0; i < TRIE_SIZE; i++) {
         if (t->chars[i] != NULL) {
             trie_free(t->chars[i]);
         }
     }
     free(t);
 }

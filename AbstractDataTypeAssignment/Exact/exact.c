#include "../dict.h"
#include "specific.h"

void dict_free(dict* x)
{
   word* chain;
   word* last;
   for (int i = 0; i < x->capacity; i++) {
      chain = x->arr[i].next;
      while (chain) {
         last = chain;
         chain = chain->next;
         free(last);
      }
   }
   free(x->arr);
   free(x);
}

bool dict_spelling(dict* x, const char* s)
{
   if (!x || !s) { return false; }
   int i = _b_hash(s, x->capacity);
   word* chain = &x->arr[i];
   if (chain->str[0] == '\0') { return false; }
   while (chain) {
      if(strcmp(chain->str,s) == 0) { return true; }
      chain = chain->next;
   }
   return false;
}

bool dict_add(dict* x,  const char* s)
{  
   if (!x || !s) { return false; }
   int hash = _b_hash(s, x->capacity);
   word* chain = &x->arr[hash];
   if(chain->str[0] != '\0') { 
      word* last;
      while (chain) {
         if(strcmp(chain->str,s) == 0) { return true; }
         last = chain;
         chain = chain->next;
      }
      chain = _alloc_wrd();
      last->next = chain;
   }
   strcpy(chain->str, s);
   return true;
}

dict* dict_init(unsigned int maxwords)
{
   _test();
   dict* d = (dict*) ncalloc(1, sizeof(dict));
   d->arr = (word*) ncalloc(DBL(maxwords), sizeof(word));
   d->capacity = DBL(maxwords);
   return d;
}

word* _alloc_wrd(void)
{
   word* w = (word*) ncalloc(1, sizeof(word));
   return w;
}

int _b_hash (const char* str, int cap)
{
   unsigned long hash = B2;
   int c;
   while ((c = (*str++)))
   { hash = (hash * B1) ^ c; }
   hash %= cap;
   return (int) hash;
}

void _test (void)
{  
   assert(_b_hash("a", TEST) != _b_hash("b", TEST));
   assert(_b_hash("abc", TEST) != _b_hash("bcd", TEST));
   assert(_b_hash("alligator", TEST) != _b_hash("foxes", TEST));
   assert(_b_hash("halloumi", TEST) == _b_hash("halloumi", TEST));
   assert(_b_hash("anagram", TEST) != _b_hash("manarag", TEST));
   assert(_b_hash("reverse", TEST) != _b_hash("esrever", TEST));

   dict* d = (dict*) ncalloc(1, sizeof(dict));
   d->arr = (word*) ncalloc(DBL(TEST), sizeof(word));
   d->capacity = DBL(TEST);
   assert(d);
   assert(d->arr);

   assert(dict_add(d, "ab"));
   assert(dict_add(d, "ab"));
   assert(dict_add(d, "abc"));
   assert(dict_add(d, "abc"));
   assert(dict_add(d, "ada"));
   assert(dict_add(d, "halloumi"));

   assert(dict_spelling(d, "ab"));
   assert(dict_spelling(d, "abc"));
   assert(!dict_spelling(d, "bcd"));
   assert(dict_spelling(d, "ada"));
   assert(!dict_spelling(d, "eda"));
   assert(dict_add(d, "eda"));
   assert(dict_spelling(d, "eda"));
   assert(dict_spelling(d, "halloumi"));
   assert(!dict_spelling(d, "halloum"));
   assert(!dict_spelling(d, "halloume"));

   word* w = _alloc_wrd();
   assert(w);
   d->arr[_b_hash("ab", d->capacity)].next = w;
   strcpy(w->str, "TEST1");
   w->next = _alloc_wrd();
   word* x = w->next;
   strcpy(x->str, "TEST2");
   assert(_b_hash(w->str, TEST) != _b_hash(x->str, TEST));
   assert(strcmp(w->str, x->str) != 0);
   x->next = _alloc_wrd();
   strcpy(x->next->str, "TEST3");

   assert(strcmp(d->arr[_b_hash("ab", d->capacity)].next->str, \
                 "TEST1") == 0);

   assert(strcmp(d->arr[_b_hash("ab", d->capacity)].next->next->str, \
                 "TEST2") == 0);

   assert(strcmp(d->arr[_b_hash("ab", d->capacity)].next->next->next->str, \
                 "TEST3") == 0);
   dict_free(d);
}

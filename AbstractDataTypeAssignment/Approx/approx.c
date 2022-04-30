/*
Approx (Full Boolean Array) implementation
Please see ../Packed/packed.c for packed BA implementation
*/
#include "../dict.h"
#include "specific.h"

#define TEST 50

void dict_free(dict* x)
{
   free(x->arr);
   free(x);
}

bool dict_spelling(dict* x, const char* s)
{
   if (!x || !s) { return false; }
   unsigned long* hashes = _hashes(s, x->capacity);
   for (int i = 0; i < K; i++) {
      if(!x->arr[hashes[i]]) {
      free(hashes);
      return false;
      }
   }
   free(hashes);
   return true;
}

bool dict_add(dict* x,  const char* s)
{
   if (!x || !s) { return false; }
   if (x->size >= x->capacity) {
      fprintf(stderr, "WARNING: CAPACITY REACHED!");
   }
   unsigned long* hashes = _hashes(s, x->capacity);
   for (int i = 0; i < K; i++) {
      if (!x->arr[hashes[i]]) { x->size++; }
      x->arr[hashes[i]] = true;
   }
   free(hashes);
   return true;
}

dict* dict_init(unsigned int maxwords)
{
   _test();
   dict* d = (dict*) ncalloc(1, sizeof(dict));
   d->capacity = (unsigned long) SCALE(maxwords);
   d->arr = (bool*) ncalloc(d->capacity, sizeof(bool));
   return d;
}

unsigned long* _hashes(const char* str, unsigned long cap)
{
   unsigned long* hashes = ncalloc(K, sizeof(unsigned long));
   unsigned long bhash = _b_hash(str, cap);
   int len = (int) strlen(str);
   srand(bhash*(len*str[0] + str[DEC(len)]));
   unsigned long nu_hash = bhash;
   for (int i = 0; i < K; i++) {
      nu_hash = (B1 * nu_hash) ^ rand();
      nu_hash %= cap;
      hashes[i] = nu_hash;
   }
   return hashes;
}

unsigned long _b_hash(const char* str, unsigned long cap)
{
   unsigned long hash = B2;
   int c;
   while ((c = (*str++)))
   { hash = (hash * B1) ^ c; }
   hash %= cap;
   return hash;
}

void _test (void)
{
   assert(_b_hash("a", TEST) != _b_hash("b", TEST));
   assert(_b_hash("abc", TEST) != _b_hash("bcd", TEST));
   assert(_b_hash("alligator", TEST) != _b_hash("foxes", TEST));
   assert(_b_hash("halloumi", TEST) == _b_hash("halloumi", TEST));
   assert(_b_hash("anagram", TEST) != _b_hash("manarag", TEST));
   assert(_b_hash("reverse", TEST) != _b_hash("esrever", TEST));

   unsigned long* hashes1 = _hashes("fettuccine", TEST * TEST * TEST);
   unsigned long* hashes2 = _hashes("fettuccine", TEST * TEST * TEST);
   unsigned long* hashes3 = _hashes("carbonara", TEST * TEST * TEST);
   for (int i = 0; i < K; i++)
   {
      assert(hashes1[i] == hashes2[i]);
      assert(hashes1[i] != hashes3[i]);
   }
   free(hashes1);
   free(hashes2);
   free(hashes3);

   dict* d = (dict*) ncalloc(1, sizeof(dict));
   d->capacity = (unsigned long) SCALE(TEST);
   d->arr = (bool*) ncalloc(d->capacity, sizeof(bool));
   assert(d);

   assert(!dict_add(NULL, "abc"));
   assert(!dict_add(d, NULL));
   assert(!dict_add(NULL, NULL));
   assert(dict_add(d, "ab"));
   assert(dict_add(d, "ab"));
   assert(dict_add(d, "abc"));
   assert(dict_add(d, "abc"));
   assert(dict_add(d, "ada"));
   assert(dict_add(d, "halloumi"));

   assert(!dict_spelling(NULL, "abc"));
   assert(!dict_spelling(d, NULL));
   assert(!dict_spelling(NULL, NULL));
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

   dict_free(d);
}

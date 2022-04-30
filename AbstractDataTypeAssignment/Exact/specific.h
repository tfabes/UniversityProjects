#define BIGSTRING 50
#define B1 33
#define B2 5381
#define TEST 50
#define DBL(X) (X * 2)
#define INC(X) (X + 1)

typedef struct word {
   char str[BIGSTRING];
   struct word* next;
} word;

struct dict {
   word* arr;
   int capacity;
};

int _b_hash (const char* str, int cap);
word* _alloc_wrd(void);
void _test(void);



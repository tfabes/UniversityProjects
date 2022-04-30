#define B1 33
#define B2 5381
#define K 11
#define SCALE(X) (X * 20)
#define DEC(X) (X - 1)

struct dict {
   bool* arr;
   unsigned long capacity;
   unsigned long size;
};

unsigned long _b_hash(const char* str, unsigned long cap);
unsigned long* _hashes(const char* str, unsigned long cap);
void _test (void);

#define B1 33
#define B2 5381
#define K 11
#define SCALE(X) (X * 20)
#define DEC(X) (X - 1)

struct dict {
   unsigned char* arr;
   unsigned long capacity;
};

int _bit(unsigned long index);
int _cell(unsigned long index);
unsigned long _b_hash(const char* str, unsigned long cap);
unsigned long* _hashes(const char* str, unsigned long cap);
void _test (void);

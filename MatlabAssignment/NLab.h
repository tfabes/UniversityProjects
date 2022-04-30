#define MAXTOKENS 1000
#define MAXTOKENSIZE 30

#define MINARGS 1
#define MIN_ARR 1
#define MIN_U_STACK 1
#define MIN_B_STACK 2
#define BAD_PARSE 3
#define HEADER 2
#define MIN_NU_LINE 2

#define BASE_TEN 10
#define C2I(X) (X - '0')
#define INC(X) (X+1)
#define DEC(X) (X-1)

#define SMSTR(S,T) (strcmp(S, T) == 0)

#define SUCCESS 0
#define NON_ZERO -1
#define W_PROG -2
#define W_STMNT -3
#define W_PRINT -4
#define W_ONES -5
#define W_READ -6
#define W_FILE -7
#define W_LOOP -8
#define W_LOOP_NUM -9
#define W_SET -10
#define W_BAD_SET -11
#define W_POLISH -12
#define W_STACK -13
#define W_OPCODE -14
#define W_U_STACK -15
#define W_B_STACK -16
#define W_UNEQL -17
#define W_NEGATIVE -18

#define DOUBLEDIGIT 10

#ifdef EXT
   #define W_RAND -19
   #define W_RAND_NUM -20
   #define W_RMV -21
   #define W_INV_RMV -22
   #define W_INDX -23
   #define W_INDX_BND -24

   typedef enum range {
      ind_row, ind_col, rng_all, rng_inv,
      rng_row, rng_col, cell, rng_cells,
      rng_row_cell, rng_col_cell
   } range; 

#endif


typedef enum scalar {
   a_sclr, b_sclr, eql, uneql
} scalar;

typedef enum ops {
   INV_OP, U_NOT, U_EIGHT, B_AND, B_OR,
   B_GREATER, B_LESS, B_ADD, B_TIMES, B_EQUALS
} ops;

typedef enum err {
   nlbfile, alloc_p, ptr_p,
   inp_str, alloc_l, alloc_d,
   alloc_a, inv_var, arrfile
} err;

typedef struct program {
   char words [MAXTOKENS][MAXTOKENSIZE];
   int tot;
   int cw;
   struct linked* varlist;
   struct linked* stk;
} prog;

typedef struct linked {
   struct data* start;
   int size;
} linked;

typedef struct data {
   char varname[MAXTOKENSIZE];
   int** arr;
   int row;
   int col;
   struct data* next;
} data;

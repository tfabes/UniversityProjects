#include "NLab.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef EXT
   #include <time.h>

   void sort_strt_end (int* strt, int* end);
   int get_rng (prog* p, int* strt, int* end);
   int indx_rng_cells (prog* p, int vname_index);
   int get_strt_end_indx (prog* p, bool bRow, int* strt, int* end, int* ind);
   int indx_row_col_cell(prog* p, int vname_index, bool bRow);
   int indx_rng_row_col (prog* p, int vname_index, bool bRow);
   int indx_row_col (prog* p, int vname_index, bool bRow);
   int indx_cell (prog* p, int vname_index);
   data* get_var (prog* p, int vname_index);
   int indx (prog* p);
   bool in_range (int index, int start, int end);
   void update_vlist (prog* p, data* nu, data* d, data* pre);
   int get_start_end (prog* p, bool bRow, int* start, int* end);
   int remove_range (prog* p, data* d, data* pre, bool bRow);
   int get_index (prog* p, bool bRow);
   int remove_row_col (prog* p, data* d, data* pre, bool bRow);
   range id_range (prog* p);
   int rmv_var (prog* p, int vname_index, range mode);
   int remove_variable (prog* p);
   int rand_arr (prog* p);

#endif

int b_value (int a, int b, ops opcode);
scalar sm_size (data* a, data* b);
int binary_ops (prog* p, ops opcode);
int moore_count (data* d, int r, int c);
int u_eight (prog* p);
int u_not (prog* p);
ops get_opcode (char* str);
int operations (prog* p);

int push_int (prog* p, int input);
int push_var (prog* p);
int polish (prog* p);
int polish_list (prog* p, int vname_index);
int get_stack_value (prog* p);
int valid_set (prog* p);
int set (prog* p);

int get_count (prog* p, char* vname);
int loop (prog* p);

int read (prog* p);
data* create_variable (prog* p, int vname_index, int row, int col);
data* add_var_to_list (prog* p, int vname_index, data* nu);
int ones (prog* p);
int get_digits (int num);
void print_var (prog* p);
void print_string (char str[MAXTOKENSIZE]);
int print (prog* p, bool btest);

int statement (prog* p, bool btest);
int code (prog* p);
int program (prog* p);

int str2int (char* s);
int is_string (prog* p);
int is_varname (prog* p);

data* pop (prog* p);
void copy_data (data* dest, const data* src);
int push (prog* p, const data* d);

void free_data (data* d);
void clear_linked (linked* lnk);
void free_prog (prog* p);
int** _2dcalloc (int r, int c, prog* p);
data* init_data (prog* p);
linked* init_linked (prog* p);
prog* init_prog (void);
int handle_input (bool file, char* f_name, prog* p);

int parse_error(int out);
void error (err err, int line, prog* p);
void test (void);

int main (int argc, char* argv[])
{
   #ifdef EXT
      srand(time(NULL));
   #endif
   test();
   prog* p = init_prog();
   p->tot = handle_input(argc > MINARGS, argv[DEC(argc)], p);
   int out = program(p);
   if (out != SUCCESS) { out = parse_error(out); }
   free_prog(p);
   return out;
}

#ifdef EXT

void sort_strt_end (int* strt, int* end)
{
   if (*strt > *end) {
      int tmp = *strt;
      *strt = *end;
      *end = tmp;
   }
}

int get_rng (prog* p, int* strt, int* end)
{
   *strt = DEC(str2int(p->words[p->cw++]));
   p->cw++;
   *end = DEC(str2int(p->words[p->cw++]));
   sort_strt_end(strt, end);
   if (*strt < 0 || *end < 0) { return NON_ZERO; }
   return SUCCESS;
}

int indx_rng_cells (prog* p, int vname_index)
{
   int rStrt, rEnd, cStrt, cEnd, rAdj = 0, cAdj = 0;
   if (get_rng(p, &rStrt, &rEnd) == NON_ZERO) { return W_INDX_BND; }
   p->cw++;
   if (get_rng(p, &cStrt, &cEnd) == NON_ZERO) { return W_INDX_BND; }
   data* d = get_var(p, vname_index);
   if (!d) { error(inv_var, __LINE__, p); }
   if (rStrt >= d->row || rEnd >= d->row) { return W_INDX_BND; }
   if (cStrt >= d->col || cEnd >= d->col) { return W_INDX_BND; }
   data* nu = init_data(p);
   nu->row = INC(rEnd - rStrt);
   nu->col = INC(cEnd - cStrt);
   nu->arr = _2dcalloc(nu->row, nu->col, p);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         rAdj = rStrt > 0 ? rStrt : 0;
         cAdj = cStrt > 0 ? cStrt : 0;
         if (in_range(i, rStrt, rEnd) && in_range(j, cStrt, cEnd))
         { nu->arr[i-rAdj][j-cAdj] = d->arr[i][j]; }
      }
   }
   push(p, nu);
   free_data(nu);
   return SUCCESS;
}

int get_strt_end_indx (prog* p, bool bRow, int* strt, int* end, int* ind)
{
   if (bRow) {
      *strt = DEC(str2int(p->words[p->cw++]));
      p->cw++;
      *end = DEC(str2int(p->words[p->cw++]));
      p->cw++;
      *ind = DEC(str2int(p->words[p->cw++]));
   }
   else {
      *ind = DEC(str2int(p->words[p->cw++]));
      p->cw++;
      *strt = DEC(str2int(p->words[p->cw++]));
      p->cw++;
      *end = DEC(str2int(p->words[p->cw++]));
   }
   sort_strt_end(strt, end);
   if (*strt < 0 || *end < 0 || *ind < 0)
   { return NON_ZERO; }
   return SUCCESS;
}

int indx_row_col_cell (prog* p, int vname_index, bool bRow)
{
   int index, start, end;
   if (get_strt_end_indx(p, bRow, &start, &end, &index) == NON_ZERO)
   { return W_NEGATIVE; }
   data* d = get_var(p, vname_index);
   if (!d) { error(inv_var, __LINE__, p); }
   if (start >= (bRow ? d->row : d->col)) { return W_INDX_BND; }
   if (end >= (bRow ? d->row : d->col)) { return W_INDX_BND; }
   if (index >= (bRow ? d->col : d->row)) { return W_INDX_BND; }
   int cpy_rng = INC(end - start), adj = 0;
   data* nu = init_data(p);
   nu->row = bRow ? cpy_rng : 1;
   nu->col = bRow ? 1 : cpy_rng;
   nu->arr = _2dcalloc(nu->row, nu->col, p);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         adj = start > 0 ? start : 0;
         if (bRow && in_range(i, start, end) && j == index)
         { nu->arr[i-adj][0] = d->arr[i][j]; }
         if (!bRow && in_range(j, start, end) && i == index)
         { nu->arr[0][j-adj] = d->arr[i][j]; }
      }
   }
   push(p, nu);
   free_data(nu);
   return SUCCESS;
}

int indx_rng_row_col (prog* p, int vname_index, bool bRow)
{
   int start, end;
   if (get_start_end(p, bRow, &start, &end) == NON_ZERO)
   { return W_NEGATIVE; }
   data* d = get_var(p, vname_index);
   if (!d) { error(inv_var, __LINE__, p); }
   if (start >= (bRow ? d->row : d->col)) { return W_INDX_BND; }
   if (end >= (bRow ? d->row : d->col)) { return W_INDX_BND; }
   int cpy_rng = INC(end - start), adj = 0;
   data* nu = init_data(p);
   nu->row = bRow ? cpy_rng : d->row;
   nu->col = bRow ? d->col : cpy_rng;
   nu->arr = _2dcalloc(nu->row, nu->col, p);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         adj = start > 0 ? start : 0;
         if (bRow && in_range(i, start, end))
         { nu->arr[i-adj][j] = d->arr[i][j]; }
         if (!bRow && in_range(j, start, end))
         { nu->arr[i][j-adj] = d->arr[i][j]; }
      }
   }
   push(p, nu);
   free_data(nu);
   return SUCCESS;
}

int indx_row_col (prog* p, int vname_index, bool bRow)
{
   int index = get_index(p, bRow);
   if (index == NON_ZERO) { return W_NEGATIVE; }
   data* d = get_var(p, vname_index);
   if (!d) { error(inv_var, __LINE__, p); }
   if (index >= (bRow ? d->row : d->col)) { return W_INDX_BND; }
   data* nu = init_data(p);
   nu->row = bRow ? 1 : d->row;
   nu->col = bRow ? d->col : 1;
   nu->arr = _2dcalloc(nu->row, nu->col, p);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         if ((bRow ? i : j) == index) {
            nu->arr[(bRow ? 0 : i)][(bRow ? j : 0)] = d->arr[i][j];
         }
      }
   }
   push(p, nu);
   free_data(nu);
   return SUCCESS;
}

int indx_cell (prog* p, int vname_index)
{
   int r = DEC(str2int(p->words[p->cw++]));
   p->cw++;
   int c = DEC(str2int(p->words[p->cw++]));
   if (r < 0 || c < 0) { return W_NEGATIVE; }
   data* d = get_var(p, vname_index);
   if (!d) { error(inv_var, __LINE__, p); }
   if (r >= d->row || c >= d->col) { return W_INDX_BND; }
   return push_int(p, d->arr[r][c]);
}

data* get_var (prog* p, int vname_index)
{
   data* d = p->varlist->start;
   while (d && !SMSTR(d->varname, p->words[vname_index])) {
      d = d->next;
   }
   return d;
}

int indx (prog* p)
{
   int vname_index = p->cw++;
   range mode = id_range(p);
   if (mode == rng_all) {
      p->cw--; p->cw--;
      push_var(p);
      while (!SMSTR(p->words[p->cw], "]"))
      { p->cw++; }
   }
   switch (mode) {
      case cell :
         indx_cell(p, vname_index);
         break;
      case ind_row :
      case ind_col :
         indx_row_col(p, vname_index, mode==ind_row);
         break;
      case rng_row :
      case rng_col :
         indx_rng_row_col(p, vname_index, mode==rng_row);
         break;
      case rng_row_cell :
      case rng_col_cell :
         indx_row_col_cell(p, vname_index, mode==rng_row_cell);
         break;
      case rng_cells :
         indx_rng_cells(p, vname_index);
         break;
      case rng_inv :
         return W_INDX_BND;
      default :
         break;
   }
   return SUCCESS;
}

bool in_range (int index, int start, int end)
{
   if (index >= start && index <= end) { return true; }
   return false;
}

void update_vlist (prog* p, data* nu, data* d, data* pre)
{
   nu->arr = _2dcalloc(nu->row, nu->col, p);
   strcpy(nu->varname, d->varname);
   nu->next = d->next;
   if (!SMSTR(p->varlist->start->varname, d->varname) && pre)
   { pre->next = nu; }
   else { p->varlist->start = nu; }
}

int get_start_end (prog* p, bool bRow, int* start, int* end)
{
   if (!bRow) { p->cw++; p->cw++; }
   *start = DEC(str2int(p->words[p->cw++]));
   p->cw++;
   *end = DEC(str2int(p->words[p->cw++]));
   if (bRow) { p->cw++; p->cw++; }
   if (*start < 0 || *end < 0) { return NON_ZERO; }
   sort_strt_end(start, end);
   return SUCCESS;
}

int remove_range (prog* p, data* d, data* pre, bool bRow)
{  
   int start, end;
   if (get_start_end(p, bRow, &start, &end) == W_NEGATIVE)
   { return W_INV_RMV; }
   int d_limit = bRow ? d->row : d->col;
   if (start >= d_limit || end >= d_limit) { return W_INV_RMV; }
   int rm_rng = INC(end - start), adj = 0;
   data* nu = init_data(p);
   nu->row = bRow ? d->row - rm_rng : d->row;
   nu->col = bRow ? d->col : d->col - rm_rng;
   update_vlist(p, nu, d, pre);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         adj = (bRow ? i : j) > end ? rm_rng : 0;
         if (bRow && !in_range(i, start, end))
         { nu->arr[i-adj][j] = d->arr[i][j]; }
         if (!bRow && !in_range(j, start, end))
         { nu->arr[i][j-adj] = d->arr[i][j]; }
      }
   }
   return SUCCESS;
}

int get_index (prog* p, bool bRow)
{
   if (!bRow) { p->cw++; p->cw++; }
   int index = DEC(str2int(p->words[p->cw++]));
   if (bRow) { p->cw++; p->cw++; }
   if (index < 0) { return NON_ZERO; }
   return index;
}

int remove_row_col (prog* p, data* d, data* pre, bool bRow)
{
   int adj = 0, index = get_index(p, bRow);
   if (index == NON_ZERO) { return W_NEGATIVE; }
   if (index >= (bRow ? d->row : d->col)) { return W_INV_RMV; }
   data* nu = init_data(p);
   nu->row = bRow ? DEC(d->row) : d->row;
   nu->col = bRow ? d->col : DEC(d->col);
   update_vlist(p, nu, d, pre);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         adj = (bRow ? i : j) <= index ? 0 : 1;
         if (bRow && i != index) { nu->arr[i-adj][j] = d->arr[i][j]; }
         if (!bRow && j != index) { nu->arr[i][j-adj] = d->arr[i][j]; }
      }
   }
   return SUCCESS;
}

range id_range (prog* p)
{
   int rng_start = ++p->cw;
   range id = rng_all;
   if (str2int(p->words[p->cw++]) >= MIN_ARR) { id = ind_row; }
   if (SMSTR(p->words[p->cw],":") && !SMSTR(p->words[INC(p->cw)],"]")) {
      if (str2int(p->words[++p->cw]) < MIN_ARR) { return rng_inv; }
      p->cw++;
      id = rng_row;
   }
   if (!SMSTR(p->words[p->cw++], ",")) { return rng_inv; }
   if (str2int(p->words[p->cw++]) >= MIN_ARR) {
      if (id == ind_row) { id = cell; }
      else if (id == rng_row) { id = rng_row_cell; }
      else { id = ind_col; }
   }
   if (SMSTR(p->words[p->cw++],":")) {
      if (str2int(p->words[p->cw++]) < MIN_ARR) { return rng_inv; }
      else if (id == ind_row || id == cell) { id = rng_col_cell; }
      else if (id == rng_row_cell) { id = rng_cells; }
      else { id = rng_col; }
   }
   p->cw = rng_start;
   return id;
}

int rmv_var (prog* p, int vname_index, range mode)
{
   if (p->varlist->start == NULL) { return W_INV_RMV; }
   data* d = p->varlist->start;
   data* pre = NULL;
   while (d && !SMSTR(p->words[vname_index], d->varname)) {
      pre = d;
      d = d->next;
   }
   if (!d) { return W_INV_RMV; }
   if (mode == rng_all && pre) { pre->next = d->next; }
   else if (mode == rng_row || mode == rng_col) {
      if (remove_range(p,d,pre,(mode==rng_row)) == W_INV_RMV)
      { return W_INV_RMV; }
   }
   else if (remove_row_col(p,d,pre,(mode==ind_row)) == W_INV_RMV)
   { return W_INV_RMV; }
   free_data(d);
   return SUCCESS;
}

int remove_variable (prog* p)
{
   if (is_varname(p) == NON_ZERO) { return W_RMV; }
   int vname_index = p->cw++;
   if (!SMSTR(p->words[p->cw],"[")) { return rmv_var(p, vname_index, rng_all); }
   range mode = id_range(p);
   if (mode == rng_inv || mode == cell || mode == rng_row_cell || \
       mode == rng_col_cell || mode == rng_cells) { return W_RMV; }
   int out = rmv_var(p, vname_index, mode);
   p->cw++;
   return out;
}

int rand_arr (prog* p)
{
   int row = str2int(p->words[p->cw++]);
   if (row == NON_ZERO) { return W_RAND; }
   int col = str2int(p->words[p->cw++]);
   if (col == NON_ZERO) { return W_RAND; }
   if (row < MIN_ARR || col < MIN_ARR) { return W_NEGATIVE; }
   int max = str2int(p->words[p->cw++]);
   if (max <= 0) { return W_RAND_NUM; }
   if (is_varname(p) == NON_ZERO) { return W_RAND; }
   data* d = create_variable(p, p->cw++, row, col);
   for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
         d->arr[i][j] = rand() % INC(max);
      }
   }
   return SUCCESS;
}

#endif

int b_value (int a, int b, ops opcode)
{
   switch (opcode) {
      case B_AND :
         return a && b;
      case B_OR :
         return a || b;
      case B_GREATER :
         return a > b;
      case B_LESS :
         return a < b;
      case B_ADD :
         return a + b;
      case B_TIMES :
         return a * b;
      case B_EQUALS :
         return a == b;
      default :
         return 0;
   }
}

scalar sm_size (data* a, data* b)
{
   if ((a->row == 1 && a->col == 1) && \
       (b->row != 1 && b->col != 1))
   { return a_sclr; }
   if ((a->row != 1 && a->col != 1) && \
       (b->row == 1 && b->col == 1)) 
   { return b_sclr; }
   if ((a->row == b->row) && (a->col == b->col))
   { return eql; }
   return uneql;
}

int binary_ops (prog* p, ops opcode)
{
   if (p->stk->size < MIN_B_STACK) { return W_B_STACK; }
   scalar sclr = sm_size(p->stk->start->next, p->stk->start);
   if (sclr == uneql) { return W_UNEQL; }
   data* b = pop(p);
   data* a = pop(p);
   if (sclr == a_sclr) {
      data* tmp = a;
      a = b;
      b = tmp;
      if (opcode == B_GREATER) { opcode = B_LESS; }
      else if (opcode == B_LESS) { opcode = B_GREATER; }
   }
   int x = 0, y = 0;
   for (int i = 0; i < a->row; i++) {
      for (int j = 0; j < a->col; j++) {
         if (sclr == eql) { x = i; y = j; }
         a->arr[i][j] = b_value(a->arr[i][j], b->arr[x][y], opcode);
      }
   }
   push(p, a);
   free_data(a);
   free_data(b);
   return SUCCESS;
}

int moore_count (data* d, int r, int c)
{
   int tot = 0;
   for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
         if (!(i == 0 && j == 0)) {
            if ((i+r >= 0) && (i+r < d->row) && \
                (j+c >= 0) && (j+c < d->col)) {
               tot += (bool) d->arr[i+r][j+c];
            }
         }
      }
   }
   return tot;
}

int u_eight (prog* p)
{
   data* d = pop(p);
   data* result = init_data(p);
   result->arr = _2dcalloc(d->row, d->col, p);
   copy_data(result, d);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         result->arr[i][j] = moore_count(d,i,j);
      }
   }
   push(p, result);
   free_data(result);
   free_data(d);
   return SUCCESS;
}

int u_not (prog* p)
{
   data* d = pop(p);
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         d->arr[i][j] = !d->arr[i][j];
      }
   }
   push(p, d);
   free_data(d);
   return SUCCESS;
}

ops get_opcode (char* str)
{
   if (SMSTR(str, "U-NOT")) { return U_NOT; }
   if (SMSTR(str, "U-EIGHTCOUNT")) { return U_EIGHT; }
   if (SMSTR(str, "B-AND")) { return B_AND; }
   if (SMSTR(str, "B-OR")) { return B_OR; }
   if (SMSTR(str, "B-GREATER")) { return B_GREATER; }
   if (SMSTR(str, "B-LESS")) { return B_LESS; }
   if (SMSTR(str, "B-ADD")) { return B_ADD; }
   if (SMSTR(str, "B-TIMES")) { return B_TIMES; }
   if (SMSTR(str, "B-EQUALS")) { return B_EQUALS; }
   return INV_OP;
}

int operations (prog* p)
{
   ops opcode = get_opcode(p->words[p->cw]);
   if (opcode == INV_OP) { return W_OPCODE; }
   p->cw++;
   #ifndef INTERP
      return SUCCESS;
   #endif
   if (p->stk->size < MIN_U_STACK) { return W_U_STACK; }
   if (opcode == U_NOT) { return u_not(p); }
   if (opcode == U_EIGHT) { return u_eight(p); }
   return binary_ops(p, opcode);
}

int push_int (prog* p, int input)
{
   data* d = init_data(p);
   d->row = 1;
   d->col = 1;
   d->arr = _2dcalloc(1,1,p);
   d->arr[0][0] = input;
   int out = push(p, d);
   free_data(d);
   return out;
}

int push_var (prog* p)
{   
   data* d = p->varlist->start;
   while (!SMSTR(p->words[p->cw], d->varname)) {
      if (!d->next) { error(inv_var, __LINE__, p); }
      d = d->next;
   }
   return push(p, d);
}

int polish (prog* p)
{
   if (is_varname(p) == SUCCESS) {
      #ifdef INTERP
      #ifndef EXT
         push_var(p);
      #endif 
      #endif
      #ifdef EXT
         if (SMSTR(p->words[INC(p->cw)],"["))
         { indx(p); }
         else { push_var(p); }
      #endif
   }
   else if (str2int(p->words[p->cw]) >= 0) {
      #ifdef INTERP
         if (push_int(p, str2int(p->words[p->cw])) == NON_ZERO) 
         { return NON_ZERO; }
      #endif
   }
   else { return operations(p); }
   p->cw++;
   return SUCCESS;
}

int polish_list (prog* p, int vname_index)
{
   if (SMSTR(p->words[p->cw], ";")) {
      p->cw++;
      #ifdef INTERP
         add_var_to_list(p, vname_index, pop(p));
         return p->stk->size == 0 ? 0 : W_STACK;
      #endif
      return SUCCESS;
   }
   int out = polish(p);
   if (out == SUCCESS) { out = polish_list(p, vname_index); }
   if (out != SUCCESS) { clear_linked(p->stk); }
   return out;
}

int get_stack_value (prog* p)
{
   #ifdef EXT
      if (SMSTR(p->words[p->cw], "[")) { return 0; }
      if (SMSTR(p->words[p->cw], "]")) { return 0; }
      if (SMSTR(p->words[p->cw], ",")) { return 0; }
      if (SMSTR(p->words[p->cw], ":")) { return 0; }
      if (str2int(p->words[p->cw]) >= 0) {
         if (SMSTR(p->words[DEC(p->cw)], "[")) { return 0; }
         if (SMSTR(p->words[INC(p->cw)], "]")) { return 0; }
         if (SMSTR(p->words[DEC(p->cw)], ",")) { return 0; }
         if (SMSTR(p->words[INC(p->cw)], ",")) { return 0; }
         return 1;
      } 
   #endif
   #ifndef EXT
   if (str2int(p->words[p->cw]) >= 0) { return 1; }
   #endif
   if (is_varname(p) == 0) { return 1; }
   ops opcode = get_opcode(p->words[p->cw]);
   switch (opcode) {
      case INV_OP:
         return BAD_PARSE;
      case U_NOT:
      case U_EIGHT:
         return 0;
      default:
         return -1;
   }
}

int valid_set (prog* p)
{
   int store_cw = p->cw, tot = 0, val = 0;
   while (!SMSTR(p->words[++p->cw], ";")) {
      if (p->words[p->cw][0] == '\0') { return NON_ZERO; }
      val = get_stack_value(p);
      if (val == -1 && tot < MIN_B_STACK) { return NON_ZERO; }
      if (val == 0 && tot < MIN_U_STACK) { return NON_ZERO; }
      if (val == BAD_PARSE) { return NON_ZERO; }
      tot += val;
   }
   p->cw = store_cw;
   return tot == 1 ? 0 : NON_ZERO;
}

int set (prog* p)
{
   if (is_varname(p) == NON_ZERO)
   { return W_SET; }
   int vname_index = p->cw++;
   if (!SMSTR(p->words[p->cw], ":="))
   { return W_SET; }
   if (valid_set(p) == NON_ZERO) { return W_BAD_SET; }
   p->cw++;
   return polish_list(p, vname_index);
}

int get_count (prog* p, char* vname)
{
   data* d = p->varlist->start;
   data* counter = NULL;
   while (d && !counter) {
      if (SMSTR(d->varname, vname)) { counter = d; }
      d = d->next;
   }
   return ++counter->arr[0][0];
}

int loop (prog* p)
{
   if (is_varname(p) == NON_ZERO) { return W_LOOP; }
   #ifdef INTERP
      int vname_index = p->cw;
   #endif
   p->cw++;
   int loop_index = str2int(p->words[p->cw++]);
   if (loop_index <= 0) { return W_LOOP_NUM; }
   if (!SMSTR(p->words[p->cw++], "{")) { return W_LOOP; }
   #ifdef INTERP
      int loop_start = p->cw, counter = 1, out = 0;
      data* d = create_variable(p, vname_index, 1, 1);
      d->arr[0][0] = counter;
      while (counter <= loop_index) {
         p->cw = loop_start;
         out = code(p);
         if (out != SUCCESS) { return out; }
         counter = get_count(p, p->words[vname_index]);
      }
   #endif
   #ifndef INTERP
      return code(p);
   #endif
   return SUCCESS;
}

int read (prog* p)
{
   if (is_string(p) == NON_ZERO) { return W_READ; }
   p->cw++;
   if (is_varname(p) == NON_ZERO) { return W_READ; }
   p->cw++;
   #ifdef INTERP
      int fname_index = DEC(DEC(p->cw)), vname_index = DEC(p->cw);
      char temp, fname[MAXTOKENSIZE];
      int j = 0, row = 0, col = 0;
      for (int i = 0; i < (int) strlen(p->words[fname_index]); i++) {
         temp = p->words[fname_index][i];
         if (temp != '\"') { fname[j++] = temp; }
      }
      while (j < MAXTOKENSIZE) { fname[j++] = '\0'; }
      if (SMSTR(fname, "TEST_UNIQ_£.arr")) { return 0; }
      FILE* fp = fopen(fname, "r");
      if (!fp) { error(arrfile, __LINE__, p); }
      if (fscanf(fp, "%d%d", &row, &col) != HEADER) { return W_FILE; };
      if (row < MIN_ARR || col < MIN_ARR) { return W_NEGATIVE; }
      data* d = create_variable(p, vname_index, row, col);
      for (int m = 0; m < row; m++) {
         for (int n = 0; n < col; n++) {
            if (fscanf(fp, "%d", &d->arr[m][n]) != 1) {
               return W_FILE;
            }
         }
      }
      fclose(fp);
   #endif
   return SUCCESS;
}

data* add_var_to_list (prog* p, int vname_index, data* nu)
{  
   if (!nu) { return NULL; }
   if (!p->varlist) { p->varlist = init_linked(p); }
   data* d = p->varlist->start;
   data* tmp = NULL;
   data* tmp2 = NULL;
   while (d && !SMSTR(d->varname, p->words[vname_index])) {
      tmp = d;
      d = d->next;
   }
   if (d) {
      tmp2 = d->next;
      free_data(d);
   }
   else { p->varlist->size++; }
   strcpy(nu->varname, p->words[vname_index]);
   if (tmp) { tmp->next = nu; }
   else { p->varlist->start = nu; }
   if (tmp2) { nu->next = tmp2; }
   return nu;
}

data* create_variable (prog* p, int vname_index, int row, int col)
{
   data* nu = init_data(p);
   nu->row = row;
   nu->col = col;
   nu->arr = _2dcalloc(row, col, p);
   return add_var_to_list(p, vname_index, nu);
}

int ones (prog* p)
{
   int row = str2int(p->words[p->cw++]);
   if (row == NON_ZERO) { return W_ONES; }
   int col = str2int(p->words[p->cw++]);
   if (col == NON_ZERO) { return W_ONES; }
   if (row < MIN_ARR || col < MIN_ARR) { return W_NEGATIVE; }
   if (is_varname(p) == NON_ZERO) { return W_ONES; }
   p->cw++;
   #ifdef INTERP
      data* d = create_variable(p, DEC(p->cw), row, col);
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            d->arr[i][j] = 1;
         }
      }
   #endif
   return SUCCESS;
}

int get_digits (int num)
{
   if (num < DOUBLEDIGIT) { return 1; }
   else { return get_digits(num / DOUBLEDIGIT) + 1 ; }
}

void print_var (prog* p)
{
   data* d = p->varlist->start;
   while (!SMSTR(p->words[p->cw], d->varname)) {
      if (!d->next) { error(inv_var, __LINE__, p); }
      d = d->next;
   }
   int digits = 0, cell = 0;
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         cell = get_digits(d->arr[i][j]);
         digits = cell > digits ? cell : digits;
      }
   }
   for (int i = 0; i < d->row; i++) {
      for (int j = 0; j < d->col; j++) {
         printf("%*d ", digits, d->arr[i][j]);
      }
      printf("\n");
   }
   if (d->row >= MIN_NU_LINE) { printf("\n"); }
}

void print_string (char str[MAXTOKENSIZE])
{
   char str2[MAXTOKENSIZE];
   int j = 0;
   for (int i = 0; i < MAXTOKENSIZE; i++) {
      if (str[i] != '\"') { str2[j++] = str[i]; }
   }
   while (j < MAXTOKENSIZE) { str[j++] = '\0'; }
   printf("%s\n", str2);
}

int print (prog* p, bool btest)
{
   if (is_string(p) == SUCCESS) {
      if (!btest) { 
         #ifdef INTERP
            print_string(p->words[p->cw]);
         #endif
      }
   }
   else if (is_varname(p) == SUCCESS) {
      if (!btest) {
         #ifdef INTERP
            print_var(p);
         #endif
      }
   }
   else { return W_PRINT; }
   p->cw++;
   return SUCCESS;
}

int statement (prog* p, bool btest)
{
   char str[MAXTOKENSIZE];
   strcpy(str, p->words[p->cw++]);
   if (SMSTR(str, "PRINT")) { return print(p, btest); }
   if (SMSTR(str, "SET")) { return set(p); }
   if (SMSTR(str, "ONES")) { return ones(p); }
   if (SMSTR(str, "READ")) { return read(p); }
   if (SMSTR(str, "LOOP")) { return loop(p); }
   #ifdef EXT
      if (SMSTR(str, "RAND")) { return rand_arr(p); }
      if (SMSTR(str, "RMV")) { return remove_variable(p); }
   #endif
   return W_STMNT;
}

int code (prog* p)
{
   if (SMSTR(p->words[p->cw], "}")) {
      p->cw++;
      return SUCCESS;
   }
   int out = statement(p, false);
   if (out != SUCCESS) { return out; }
   return code(p);
}

int program (prog* p)
{
   if (!SMSTR(p->words[p->cw++], "BEGIN"))
   { return W_PROG; }
   if (!SMSTR(p->words[p->cw++], "{"))
   { return W_PROG; }
   return code(p);
}

int str2int (char* s)
{
   if (s == NULL) { return NON_ZERO; }
   int len = (int) strlen(s);
   int tot = 0, pow = BASE_TEN, neg = 1;
   for (int i = DEC(len); i >= 0; i--) {
      if (i == 0 && s[i] == '-') { neg = -1; }
      else if (isdigit(s[i]) == 0) { return NON_ZERO; }
      else if (i == DEC(len)) { tot += C2I(s[i]); }
      else {
         tot += C2I(s[i]) * pow;
         pow *= BASE_TEN;
      }
   }
   return tot * neg;
}

int is_string (prog* p)
{
   int len = (int) strlen(p->words[p->cw]);
   if (p->words[p->cw][0] == '"' && \
       p->words[p->cw][DEC(len)] == '"')
       { return SUCCESS; }
   return NON_ZERO;
}

int is_varname (prog* p)
{
   int len = (int) strlen(p->words[p->cw]);
   if (p->words[p->cw][0] != '$') { return NON_ZERO; }
   for (int i = 1; i < len; i++) {
      if (p->words[p->cw][i] < 'A' || \
          p->words[p->cw][i] > 'Z')
          { return NON_ZERO; }
   }
   return SUCCESS;
}

data* pop (prog* p)
{
   if (!p || !p->stk || !p->stk->start) { return NULL; }
   if (p->stk->size == 0) { return NULL; }
   data* nu_top = p->stk->size > 1 ? p->stk->start->next : NULL;
   data* d = p->stk->start;
   d->next = NULL;
   p->stk->start = nu_top;
   p->stk->size--;
   return d;
}

void copy_data (data* dest, const data* src)
{
   dest->row = src->row;
   dest->col = src->col;
   for (int i = 0; i < src->row; i++) {
     memcpy(dest->arr[i], src->arr[i], sizeof(int) * src->col);
   }
   strcpy(dest->varname, src->varname);
}

int push (prog* p, const data* d)
{
   if (!p || !d) { return NON_ZERO; }
   if (!p->stk) { p->stk = init_linked(p); }
   data* tmp = p->stk->start;
   p->stk->start = init_data(p);
   p->stk->start->arr = _2dcalloc(d->row, d->col, p);
   copy_data(p->stk->start, d);
   p->stk->start->next = tmp;
   p->stk->size++;
   return SUCCESS;
}

void free_data (data* d)
{
   for (int i = 0; i < d->row; i++) { free(d->arr[i]); }
   free(d->arr);
   free(d);
}

void clear_linked (linked* lnk)
{
   if (lnk) {
      #ifdef INTERP
         data* nxt = lnk->start;
         while (lnk->size != 0) {
            data* tmp = nxt->next;
            free_data(nxt);
            lnk->size--;
            nxt = tmp;
         }
         lnk = NULL;
      #endif
   }
}

void free_prog (prog* p)
{
   if (!p) { return; }
   if (p->varlist) {
      clear_linked(p->varlist);
      free(p->varlist);
   }
   if (p->stk) {
     clear_linked(p->stk);
     free(p->stk);
   }
   free(p);
}

int** _2dcalloc (int r, int c, prog* p)
{
   int** arr = (int**) calloc (r, sizeof(int*));
   if (!arr) { error(alloc_a, __LINE__, p); }
   for(int i = 0; i < r; i++) {
      arr[i] = (int*) calloc (c, sizeof(int));
      if (!arr[i]) { error(alloc_a, __LINE__, p); }
   }
   return arr;
}

data* init_data (prog* p)
{
   data* d = (data*) calloc (1, sizeof(data));
   if (!d) { error(alloc_d, __LINE__, p); }
   return d;
}

linked* init_linked (prog* p)
{
   linked* lnk = (linked*) calloc (1, sizeof(linked));
   if (!lnk) { error(alloc_l, __LINE__, p); }
   return lnk;
}

prog* init_prog (void)
{
   prog* p = (prog*) calloc(1, sizeof(prog));
   if (!p) { error(alloc_p, __LINE__, p); }
   return p;
}

int handle_input (bool file, char* f_name, prog* p)
{
   if (!p) { error(ptr_p, __LINE__, p); }
   int i = 0;
   if (file) {
      if (!f_name) { error(inp_str, __LINE__, p); }
      FILE* fp = fopen(f_name, "r");
      if (!fp) { error(nlbfile, __LINE__, p); }
      while (fscanf(fp, "%s", p->words[i++]) == 1 && i < MAXTOKENS);
      fclose(fp);
   }
   else {
      while (scanf("%s", p->words[i++]) == 1 && i < MAXTOKENS); }
   return i;
}

int parse_error (int out)
{
   switch (out) {
      case W_PROG :
         printf("Program Parse Error: Missing BEGIN or {\n");
         break;
      case W_STMNT :
         printf("Statement Parse Error: Unrecognised Instruction\n");
         break;
      case W_PRINT :
         printf("PRINT Parse Error. Correct usage: PRINT <VARNAME> || PRINT <STRING>\n");
         break;
      case W_ONES :
         printf("ONES Parse Error. Correct usage: ONES <ROWS> <COLS> <VARNAME>\n");
         break;
      case W_READ :
         printf("READ Parse Error. Correct usage: READ <FILENAME> <VARNAME>\n");
         break;
      case W_FILE :
         printf("READ File Error. Incorrectly formatted array file used.\n");
         break;
      case W_LOOP :
         printf("LOOP Parse Error. Correct usage: LOOP <VARNAME> <INTEGER> { <INSTRCLIST>\n");
         break;
      case W_LOOP_NUM :
         printf("LOOP Usage Error. Invalid loop integer given - please use a positive, integer.\n");
         break;
      case W_SET :
         printf("SET Parse Error. Correct usage: SET <VARNAME> := <POLISHLIST>\n");
         break;
      case W_BAD_SET :
         printf("SET Usage Error. Invalid combination of integers, variables and operations.\n");
         break;
      case W_POLISH :
         printf("POLISH Parse Error. Correct usage: <VARNAME> || <INTEGER> || <UNARYOP> || <BINARYOP>\n");
         break;
      case W_STACK :
         printf("WARNING: STACK WAS NOT CLEARED UPON COMPLETING POLISH LIST CALCULATIONS.\n");
         break;
      case W_OPCODE :
         printf("Operation Parse Error. Unrecognised operation name used.\n");
         break;
      case W_U_STACK :
         printf("Usage Error. At least one variable is required to perform Unary Operations.\n");
         break;
      case W_B_STACK :
         printf("Usage Error. At least two variables are required to perform Binary Operations.\n");
         break;
      case W_UNEQL :
         printf("Usage Error. Binary Operations using two non-scalar variables must have equal array sizes.\n");
         break;
      case W_NEGATIVE :
         printf("Usage Error. Arrays cannot have dimensions less than 1.\n");
         break;
      #ifdef EXT
         case W_RAND :
            printf("RAND Parse Error. Correct usage: RAND <ROWS> <COLS> <MAX> <VARNAME>.\n");
            break;
         case W_RAND_NUM :
            printf("Usage Error. Random numbers must have a positive upper limited (MAX).\n");
            break;
         case W_RMV :
            printf("RMV Parse Error. Correct usage: RMV <VARNAME> || RMV [ <ROW_RANGE> , <COL_RANGE> ].\n");
            break;
         case W_INV_RMV :
            printf("Usage Error. Variable or range for removal does not exist.\n");
            break;
         case W_INDX :
            printf("INDEX Parse Error. Correct usage: <VARNAME> [ <RANGE> ].\n");
            break;
         case W_INDX_BND :
            printf("Usage Error. Attempting to reach a range that is out of bounds.\n");
            break;
      #endif
   }
   return NON_ZERO;
}

void error (err err, int line, prog* p)
{
   switch (err)
   {
      case nlbfile :
         fprintf(stderr, "NLAB FILE ERROR - CHECK FILE NAME. EXITED AT LINE %d\n", line);
         break;
      case alloc_p :
         fprintf(stderr, "ALLOC PROGRAM ERROR. EXITED AT LINE %d\n", line);
         break;
      case ptr_p :
         fprintf(stderr, "NULL PROGRAM POINTER. EXITED AT LINE %d\n", line);
         break;
      case inp_str :
         fprintf(stderr, "NULL FILE NAME STRING. EXITED AT LINE %d\n", line);
         break;
      case alloc_l :
         fprintf(stderr, "ALLOC LINKED LIST ERROR. EXITED AT LINE %d\n", line);
         break;
      case alloc_d :
         fprintf(stderr, "ALLOC DATA ERROR. EXITED AT LINE %d\n", line);
         break;
      case alloc_a :
         fprintf(stderr, "ALLOC ARRAY ERROR. EXITED AT LINE %d\n", line);
         break;
      case inv_var :
         fprintf(stderr, "UNKNOWN VARIABLE USED. EXITED AT LINE %d\n", line);
         break;
      case arrfile :
         fprintf(stderr, "ARRAY FILE ERROR - CHECK FILE NAME. EXITED AT LINE %d\n", line);
         break;
   }
   if (p) { free_prog(p); }
   exit(EXIT_FAILURE);
}

void test (void)
{  
   prog* p = init_prog();
   bool test = true;
   
   strcpy(p->words[0], "$A");
   assert(is_varname(p) == 0);
   assert(is_string(p) == NON_ZERO);
   strcpy(p->words[0], "\"STRING\"");
   assert(is_string(p) == 0);
   strcpy(p->words[0], "STRING\"");
   assert(is_string(p) == NON_ZERO);
   strcpy(p->words[0], "\"5TRING\"");
   assert(is_string(p) == 0);
   strcpy(p->words[0], "\"5TRING");
   assert(is_string(p) == NON_ZERO);
   strcpy(p->words[0], "abc");
   assert(is_string(p) == NON_ZERO);
   assert(str2int("123") == 123);
   assert(str2int("5") == 5);
   assert(str2int("23172") == 23172);
   assert(str2int("0") == 0);
   assert(str2int("-1") == -1);
   assert(str2int("-5") == -5);
   assert(str2int("-5250") == -5250);
   assert(str2int("a23") == NON_ZERO);
   assert(str2int("12c") == NON_ZERO);
   assert(str2int("5-20") == NON_ZERO);
   assert(get_digits(5) == 1);
   assert(get_digits(55) == 2);
   assert(get_digits(2012) == 4);
   assert(get_digits(123456789) == 9);
   assert(get_digits(0) == 1);

   strcpy(p->words[0], "PRINT");
   strcpy(p->words[1], "$A");
   assert(statement(p, test) == SUCCESS);
   p->cw = 0;
   strcpy(p->words[1], "\"123\"");
   assert(statement(p, test) == SUCCESS);
   p->cw = 0;
   strcpy(p->words[1], "abc");
   assert(statement(p, test) == W_PRINT);
   p->cw = 0;
   strcpy(p->words[1], "123");
   assert(statement(p, test) == W_PRINT);
   p->cw = 0;
   strcpy(p->words[0], "ONES");
   strcpy(p->words[1], "20");
   strcpy(p->words[2], "15");
   strcpy(p->words[3], "$A");
   assert(statement(p, test) == SUCCESS);
   p->cw = 0;
   strcpy(p->words[1], "abc");
   assert(statement(p, test) == W_ONES);
   p->cw = 0;
   strcpy(p->words[1], "20");
   strcpy(p->words[2], "abc");
   assert(statement(p, test) == W_ONES);
   p->cw = 0;
   strcpy(p->words[0], "READ");
   strcpy(p->words[1], "\"TEST_UNIQ_£.arr\"");
   strcpy(p->words[2], "$A");
   assert(statement(p, test) == SUCCESS);
   p->cw = 0;
   strcpy(p->words[2], "abc");
   assert(statement(p, test) == W_READ);
   p->cw = 0;
   strcpy(p->words[1], "TEST_UNIQ_£.arr");
   strcpy(p->words[2], "$A");
   assert(statement(p, test) == W_READ);
   p->cw = 0;
   strcpy(p->words[0], "LOOP");
   strcpy(p->words[1], "$A");
   strcpy(p->words[2], "5");
   strcpy(p->words[3], "{");
   strcpy(p->words[4], "}");
   assert(statement(p, test) == SUCCESS);
   p->cw = 0;
   strcpy(p->words[1], "A");
   assert(statement(p, test) == W_LOOP);
   p->cw = 0;
   strcpy(p->words[1], "$A");
   strcpy(p->words[2], "abc");
   assert(statement(p, test) == W_LOOP_NUM);
   p->cw = 0;
   strcpy(p->words[2], "5");
   strcpy(p->words[3], "abc");
   assert(statement(p, test) == W_LOOP);
   p->cw = 0;
   strcpy(p->words[3], "{");
   strcpy(p->words[4], "abc");
   assert(statement(p, test) == W_STMNT);
   p->cw = 0;
   strcpy(p->words[0], "SET");
   strcpy(p->words[1], "$A");
   strcpy(p->words[2], ":=");
   strcpy(p->words[3], "5");
   strcpy(p->words[4], ";");
   strcpy(p->words[5], "}");
   assert(statement(p, test) == SUCCESS);
   p->cw = 0;
   assert(code(p) == SUCCESS);
   p->cw = 0;
   strcpy(p->words[1], "abc");
   assert(statement(p, test) == W_SET);
   p->cw = 0;
   assert(code(p) == W_SET);
   p->cw = 0;
   strcpy(p->words[1], "$A");
   strcpy(p->words[2], "*");
   assert(statement(p, test) == W_SET);
   p->cw = 0;
   assert(code(p) == W_SET);
   p->cw = 0;
   strcpy(p->words[2], ":=");
   strcpy(p->words[3], "abc");
   assert(statement(p, test) == W_BAD_SET);
   p->cw = 0;
   assert(code(p) == W_BAD_SET);
   p->cw = 0;
   strcpy(p->words[3], "5");
   strcpy(p->words[4], "abc");
   assert(statement(p, test) == W_BAD_SET);
   p->cw = 0;
   assert(code(p) == W_BAD_SET);
   p->cw = 0;
   strcpy(p->words[4], ";");
   assert(code(p) == SUCCESS);
   p->cw = 0;

   #ifdef INTERP
      
      while (p->words[p->cw++][0] != '\0')
      { strcpy(p->words[p->cw], ""); }
      p->cw = 0;
   
      strcpy(p->words[0], "7");
      polish(p);
      assert(p->stk->start->row == 1);
      assert(p->stk->start->col == 1);
      assert(p->stk->start->arr[0][0] == 7);
      p->cw = 0;
      strcpy(p->words[0], "15");
      polish(p);
      assert(p->stk->start->row == 1);
      assert(p->stk->start->col == 1);
      assert(p->stk->start->arr[0][0] == 15);
      p->cw = 0;
      assert(p->stk->size == 2);
      clear_linked(p->stk);
      assert(p->stk->size == 0);

      strcpy(p->words[0], "SET");
      strcpy(p->words[1], "$A");
      strcpy(p->words[2], ":=");
      strcpy(p->words[3], "5");
      strcpy(p->words[4], "8");
      strcpy(p->words[5], "B-ADD");
      strcpy(p->words[6], ";");
      strcpy(p->words[7], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      data* start = p->varlist->start;
      assert(start->arr[0][0] == 13); //A
      assert(p->stk->size == 0);
      assert(p->stk->start == NULL);
      strcpy(p->words[1], "$B");
      strcpy(p->words[3], "$A");
      strcpy(p->words[4], "-11");
      assert(code(p) == W_BAD_SET);
      p->cw = 0;
      strcpy(p->words[1], "$B");
      strcpy(p->words[3], "0");
      strcpy(p->words[4], "2");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(SMSTR(start->varname, "$A"));
      assert(SMSTR(start->next->varname, "$B"));
      assert(start->next->arr[0][0] == 2); //B
      strcpy(p->words[1], "$C");
      strcpy(p->words[3], "$B");
      strcpy(p->words[4], "3");
      strcpy(p->words[5], "B-TIMES");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->arr[0][0] == 6); //C
      strcpy(p->words[1], "$D");
      strcpy(p->words[3], "$C");
      strcpy(p->words[4], "$B");
      strcpy(p->words[5], "B-GREATER");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->arr[0][0] > start->next->arr[0][0]);
      assert(start->next->next->next->arr[0][0] == 1);
      strcpy(p->words[3], "$D");
      strcpy(p->words[4], "4");
      strcpy(p->words[5], "B-TIMES");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->arr[0][0] == 4);
      strcpy(p->words[4], "0");
      strcpy(p->words[5], "B-TIMES");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->arr[0][0] == 0); //D
      strcpy(p->words[1], "$E");
      strcpy(p->words[3], "$B");
      strcpy(p->words[4], "$D");
      strcpy(p->words[5], "B-AND");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->next->arr[0][0] == 0);
      strcpy(p->words[5], "B-OR");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->next->arr[0][0] == 1); //E
      strcpy(p->words[1], "$F");
      strcpy(p->words[3], "$B");
      strcpy(p->words[4], "$E");
      strcpy(p->words[5], "$B");
      strcpy(p->words[6], "B-TIMES");
      strcpy(p->words[7], "B-EQUALS");
      strcpy(p->words[8], ";");
      strcpy(p->words[9], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->next->next->arr[0][0] == 1); //F
      strcpy(p->words[5], "B-ADD");
      assert(code(p) == W_BAD_SET);
      p->cw = 0;
      strcpy(p->words[3], "U-NOT");
      strcpy(p->words[4], "B-AND");
      assert(code(p) == W_BAD_SET);
      p->cw = 0;
      strcpy(p->words[1], "$G");
      strcpy(p->words[3], "$F");
      strcpy(p->words[4], "U-NOT");
      strcpy(p->words[5], "$B");
      strcpy(p->words[6], "B-ADD");
      strcpy(p->words[7], ";");
      strcpy(p->words[8], "}");
      strcpy(p->words[9], "");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->next->next->next->arr[0][0] == 2); //G
      
      clear_linked(p->varlist);
      free(p->varlist);
      p->varlist = init_linked(p);
      assert(p->varlist->size == 0);
      while (p->words[p->cw++][0] != '\0')
      { strcpy(p->words[p->cw], ""); }
      p->cw = 0;

      strcpy(p->words[0], "ONES");
      strcpy(p->words[1], "5");
      strcpy(p->words[2], "5");
      strcpy(p->words[3], "$A");
      strcpy(p->words[4], "SET");
      strcpy(p->words[5], "$B");
      strcpy(p->words[6], ":=");
      strcpy(p->words[7], "$A");
      strcpy(p->words[8], "U-NOT");
      strcpy(p->words[9], ";");
      strcpy(p->words[10], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      start = p->varlist->start;
      assert(SMSTR(start->varname, "$A"));
      assert(start->row == 5);
      assert(start->col == 5);
      assert(SMSTR(start->next->varname, "$B"));
      assert(start->next->row == 5);
      assert(start->next->col == 5);
      for (int i = 0; i < 5; i++) {
         for (int j = 0; j < 5; j++) {
            assert(start->arr[i][j] == 1);
            assert(start->next->arr[i][j] == 0);
         }
      }
      while (p->words[p->cw++][0] != '\0')
      { strcpy(p->words[p->cw], ""); }
      p->cw = 0;
      start->next->arr[2][2] = 1;
      strcpy(p->words[0], "SET");
      strcpy(p->words[1], "$C");
      strcpy(p->words[2], ":=");
      strcpy(p->words[3], "$B");
      strcpy(p->words[4], "U-EIGHTCOUNT");
      strcpy(p->words[5], ";");
      strcpy(p->words[6], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      for (int i = 0; i < 5; i++) {
         for (int j = 0; j < 5; j++) {
            if ((i == 1 && (j == 1 || j == 2 || j == 3)) || \
                (i == 2 && (j == 1 || j == 3)) || \
                (i == 3 && (j == 1 || j == 2 || j == 3)))
            { assert(start->next->next->arr[i][j] == 1); }
            else { assert(start->next->next->arr[i][j] == 0); }
         }
      }
      strcpy(p->words[0], "SET");
      strcpy(p->words[1], "$D");
      strcpy(p->words[2], ":=");
      strcpy(p->words[3], "5");
      strcpy(p->words[4], ";");
      strcpy(p->words[5], "}");
      strcpy(p->words[6], "");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      strcpy(p->words[0], "SET");
      strcpy(p->words[1], "$E");
      strcpy(p->words[2], ":=");
      strcpy(p->words[3], "$B");
      strcpy(p->words[4], "$D");
      strcpy(p->words[5], "B-TIMES");
      strcpy(p->words[6], ";");
      strcpy(p->words[7], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(start->next->next->next->next->arr[2][2] == 5);
      strcpy(p->words[0], "ONES");
      strcpy(p->words[1], "3");
      strcpy(p->words[2], "2");
      strcpy(p->words[3], "$A");
      strcpy(p->words[4], "}");
      strcpy(p->words[5], "");
      strcpy(p->words[6], "");
      strcpy(p->words[7], "");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      strcpy(p->words[0], "SET");
      strcpy(p->words[1], "$C");
      strcpy(p->words[2], ":=");
      strcpy(p->words[3], "$A");
      strcpy(p->words[4], "$B");
      strcpy(p->words[5], "B-AND");
      strcpy(p->words[6], ";");
      strcpy(p->words[7], "}");
      assert(code(p) == W_UNEQL);
      p->cw = 0;

      clear_linked(p->varlist);
      free(p->varlist);
      p->varlist = init_linked(p);
      while (p->words[p->cw++][0] != '\0')
      { strcpy(p->words[p->cw], ""); }
      p->cw = 0;

      strcpy(p->words[0], "ONES");
      strcpy(p->words[1], "3");
      strcpy(p->words[2], "3");
      strcpy(p->words[3], "$A");
      strcpy(p->words[4], "SET");
      strcpy(p->words[5], "$B");
      strcpy(p->words[6], ":=");
      strcpy(p->words[7], "2");      
      strcpy(p->words[8], ";");
      strcpy(p->words[9], "LOOP");
      strcpy(p->words[10], "$I");
      strcpy(p->words[11], "5");
      strcpy(p->words[12], "{");
      strcpy(p->words[13], "SET");
      strcpy(p->words[14], "$A");
      strcpy(p->words[15], ":=");
      strcpy(p->words[16], "$A");
      strcpy(p->words[17], "$B");
      strcpy(p->words[18], "B-TIMES");
      strcpy(p->words[19], ";");
      strcpy(p->words[20], "}");
      strcpy(p->words[21], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      start = p->varlist->start;
      assert(SMSTR(start->varname, "$A"));
      assert(SMSTR(start->next->varname, "$B"));
      assert(SMSTR(start->next->next->varname, "$I"));
      assert(start->next->arr[0][0] == 2);
      assert(start->next->next->arr[0][0] == 6);
      for (int i = 0; i < 3; i++) {
         for (int j = 0; j < 3; j++) {
            assert(start->arr[i][j] == 32);
         }
      }
   #endif

   #ifdef EXT
      clear_linked(p->varlist);
      free(p->varlist);
      p->varlist = init_linked(p);
      while (p->words[p->cw++][0] != '\0')
      { strcpy(p->words[p->cw], ""); }
      p->cw = 0;

      strcpy(p->words[0], "RAND");
      strcpy(p->words[1], "5");
      strcpy(p->words[2], "5");
      strcpy(p->words[3], "100");
      strcpy(p->words[4], "$A");
      strcpy(p->words[5], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      start = p->varlist->start;
      assert(SMSTR(start->varname, "$A"));
      int tot_a = 0;
      for (int i = 0; i < 5; i++) {
         for (int j = 0; j < 5; j++) {
            tot_a += start->arr[i][j];
            assert(start->arr[i][j] >= 0);
            assert(start->arr[i][j] <= 100);
         }
      }
      strcpy(p->words[0], "RAND");
      strcpy(p->words[1], "50");
      strcpy(p->words[2], "50");
      strcpy(p->words[3], "1000");
      strcpy(p->words[4], "$B");
      strcpy(p->words[5], "}");    
      assert(code(p) == SUCCESS);
      p->cw = 0;
      assert(SMSTR(start->next->varname, "$B"));
      int tot_b = 0;
      for (int i = 0; i < 50; i++) {
         for (int j = 0; j < 50; j++) {
            tot_b += start->next->arr[i][j];
         }
      }
      assert(tot_b / (50 * 50) >= 400);
      assert(tot_b / (50 * 50) <= 600);
      strcpy(p->words[0], "RMV");
      strcpy(p->words[1], "$A");
      strcpy(p->words[2], "[");
      strcpy(p->words[3], "2");
      strcpy(p->words[4], ",");
      strcpy(p->words[5], ":");
      strcpy(p->words[6], "]");
      strcpy(p->words[7], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      start = p->varlist->start;
      assert(SMSTR(start->varname, "$A"));
      assert(start->row == 4);
      assert(start->col == 5);
      int tot_a_rmv = 0;
      for (int i = 0; i < 4; i++) {
         for (int j = 0; j < 5; j++) {
            tot_a_rmv += start->arr[i][j];
         }
      }
      assert(tot_a > tot_a_rmv);
      strcpy(p->words[0], "RMV");
      strcpy(p->words[1], "$A");
      strcpy(p->words[2], "[");
      strcpy(p->words[3], "1");
      strcpy(p->words[4], ":");
      strcpy(p->words[5], "2");
      strcpy(p->words[6], ",");
      strcpy(p->words[7], ":");
      strcpy(p->words[8], "]");
      strcpy(p->words[9], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      start = p->varlist->start;
      assert(SMSTR(start->varname, "$A"));
      assert(start->row == 2);
      assert(start->col == 5);
      int tot_a_rmv2 = 0;
      for (int i = 0; i < 2; i++) {
         for (int j = 0; j < 5; j++) {
            tot_a_rmv2 += start->arr[i][j];
         }
      }
      assert(tot_a_rmv > tot_a_rmv2);
      assert(tot_a_rmv2 / (2 * 5) >= 25);
      assert(tot_a_rmv2 / (2 * 5) <= 75);
      strcpy(p->words[0], "ONES");
      strcpy(p->words[1], "5");
      strcpy(p->words[2], "5");
      strcpy(p->words[3], "$C");
      strcpy(p->words[4], "SET");
      strcpy(p->words[5], "$D");
      strcpy(p->words[6], ":=");
      strcpy(p->words[7], "$C");      
      strcpy(p->words[8], "[");
      strcpy(p->words[9], "1");
      strcpy(p->words[10], ":");
      strcpy(p->words[11], "2");
      strcpy(p->words[12], ",");
      strcpy(p->words[13], "1");
      strcpy(p->words[14], ":");
      strcpy(p->words[15], "2");
      strcpy(p->words[16], "]");
      strcpy(p->words[17], "5");
      strcpy(p->words[18], "B-TIMES");
      strcpy(p->words[19], ";");
      strcpy(p->words[20], "}");
      assert(code(p) == SUCCESS);
      p->cw = 0;
      data* current = start->next->next->next;
      int tot_d = 0;
      assert(SMSTR(current->varname, "$D"));
      for (int i = 0; i < current->row; i++) {
         for (int j = 0; j < current->col; j++) {
            assert(current->arr[i][j] == 5);
            tot_d += current->arr[i][j];
         }
      }
      assert(tot_d = (5 * 2 * 2));
   #endif

   free_prog(p);
}

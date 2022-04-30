#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST 0
#define SINGLE 1
#define FILE_ARGS 2
#define MIN 2
#define MAX 6
#define LONG 100
#define INCR(X) (X+1)
#define DECR(X) (X-1)
#define DBL_DECR(X) DECR(DECR(X))
#define GRID board->grid

typedef struct board {
   int rows;
   int cols;
   int grid[MAX][MAX];
   struct board* parent;
   struct board* next;
   int top_row;
} board;

typedef struct inp_data {
   int height;
   int width;
   int grid[MAX][MAX];
} inp_data;

typedef enum inp_mde {
   manual, file
} inp_mde;

typedef enum roll {
   up, down, left, right
} roll;

typedef enum err {
   brd_err, word, data_ptr, file_name,
   file_error, ones, no_input, child_err,
   brd_alo, numbs, usage
} err;

int sum_top (board* brd);

void free_all (board* start);
void v_print (board* end);
int print_solve (board* start);
board* solve (board* start);
board* new_child (board* parent, int num, roll roll);
bool unique_grid (board* brd, board* init);
bool is_final (board* board);
void roll_board (board* board, int num, roll roll);
void fill_first_brd (inp_data* data, board* brd);
board* alloc_brd (void);

void handle_input (inp_data* data, char* f_name);
bool file_input (inp_data* data);
bool manual_input (inp_data* data);
char* numbers (int num);
void run_input (inp_data* data);
inp_data* alloc_data (void);
inp_mde check_input (void);
void print_board (board* board);
bool verbose (char* str);
void error (err err);
void test (void);

int main (int argc, char* argv[])
{
   test();
   inp_data* data = alloc_data();
   run_input(data);
   board* first_board = alloc_brd();
   fill_first_brd(data, first_board);
   free(data);
   if (argc > 1 && verbose(argv[1]))
   { v_print(solve(first_board)); }
   else { print_solve(first_board); }
   free_all(first_board);
}

int sum_top (board* board)
{
   int count = 0;
   for (int i = 0; i < board->cols; i++)
   {
      count += GRID[FIRST][i];
   }
   return count;
}


void free_all (board* start)
{ 
   if (start->next)
   { free_all(start->next); }
   free(start);
}

void v_print (board* end)
{
   if (end->parent)
   {
      v_print(end->parent);
   }
   print_board(end);
}

int print_solve (board* start)
{
   int count = 0;
   board* ptr = solve(start);
   while (ptr->parent)
   {
      ptr = ptr->parent;
      count++;
   }
   printf("%d moves\n", count);
   return count;
}

board* solve (board* start)
{
   if (!start) { error(brd_err); }
   if (is_final(start)) { return start; }
   board* ptr = start;
   board* child = NULL;
   while (ptr->next)
   { ptr = ptr->next; }
   for (int i = 0; i < start->cols; i++)
   {
      child = new_child(start, i, up);
      if (unique_grid(child, start) && (child->top_row >= ptr->top_row))
      {
         ptr->next = child;
         ptr = ptr->next;
      }
      else { free(child); }      
      child = new_child(start, i, down);
      if (unique_grid(child, start) && (child->top_row >= ptr->top_row))
      {
         ptr->next = child;
         ptr = ptr->next;
      }
      else { free(child); }
   }
   for (int i = 0; i < start->cols; i++)
   {
      child = new_child(start, i, left);
      if (unique_grid(child, start) && (child->top_row >= ptr->top_row))
      {
         ptr->next = child;
         ptr = ptr->next;
      }
      else { free(child); }
      child = new_child(start, i, right);
      if (unique_grid(child, start) && (child->top_row >= ptr->top_row))
      {
         ptr->next = child;
         ptr = ptr->next;
      }
      else { free(child); }
   }
   return solve(start->next);
}

board* new_child (board* parent, int num, roll roll)
{
   board* child = alloc_brd();
   if (child == NULL) { error(brd_alo); }
   *child = *parent;
   child->parent = parent;
   child->next = NULL;
   roll_board(child, num, roll);
   child->top_row = sum_top(child);
   return child;
}

bool unique_grid (board* brd, board* init)
{
   board current;
   if (!brd || !init) { error(brd_err); }
   current = *init;
   int count;
   while (current.next)
   {
      count = 0;
      for (int i = 0; i < brd->rows; i++)
      {
         for (int j = 0; j < brd->cols; j++)
         {
            if (current.grid[i][j] == brd->grid[i][j])
            { count++; }
         }
      }
      if (count == brd->rows * brd->cols)
      { return false; }
      current = *current.next;
   }
   return true;
}

bool is_final (board* board)
{
   if (board == NULL) { error(brd_err); }
   int count = 0;
   for (int i = 0; i < board->cols; i++)
   {
      count += GRID[FIRST][i];
   }
   return (count == board->cols);
}

void roll_board (board* board, int num, roll roll)
{
   if (board == NULL) { error(brd_err); }
   int store = 0;
   switch (roll)
   {
      case left :
         store = GRID[num][FIRST];
         for (int i = INCR(FIRST); i < board->cols; i++)
         { GRID[num][DECR(i)] = GRID[num][i]; }
         GRID[num][DECR(board->cols)] = store;
         break;
      case right :
         store = GRID[num][DECR(board->cols)];
         for (int j = DBL_DECR(board->cols); j >= 0; j--)
         { GRID[num][INCR(j)] = GRID[num][j]; }
         GRID[num][FIRST] = store;
         break;
      case up :
         store = GRID[FIRST][num];
         for (int m = INCR(FIRST); m < board->rows; m++)
         { GRID[DECR(m)][num] = GRID[m][num]; }
         GRID[DECR(board->rows)][num] = store;
         break;
      case down :
         store = GRID[DECR(board->rows)][num];
         for (int n = DBL_DECR(board->rows); n >= 0; n--)
         { GRID[INCR(n)][num] = GRID[n][num]; }
         GRID[FIRST][num] = store;
         break;
   }
}

void fill_first_brd (inp_data* data, board* brd)
{
   if (brd == NULL) { error(brd_err); }
   if (data == NULL) { error(data_ptr); }
   brd->rows = data->height;
   brd->cols = data->width;
   for (int rows = 0; rows < data->height; rows++)
   {
      for (int cols = 0; cols < data->width; cols++)
      {
         brd->grid[rows][cols] = data->grid[rows][cols];
      }
   }
   brd->top_row = sum_top(brd);
}

board* alloc_brd (void)
{
   board* brd;
   brd = (board*) calloc(SINGLE, sizeof(board));
   if (brd == NULL) { error(brd_alo); }
   return brd;
}

void handle_input (inp_data* data, char* f_name)
{
   FILE* fp = fopen(f_name, "r");
   if (fp == NULL) { error (file_error); }
   int height, width, input, count = 0;
   if (fscanf(fp, "%d %d", &height, &width) != FILE_ARGS)
   { error(file_error); }
   if (height > MAX || width > MAX || \
       height < MIN || width < MIN)
   { error(file_error); }
   data->height = height;
   data->width = width;
   for (int rows = 0; rows < height; rows++)
   {
      for (int cols = 0; cols < width; cols++)
      {
         if (fscanf(fp, "%1d", &input) != SINGLE)
         { error(file_error); }
         switch (input) {
            case 0 :
               break;
            case 1 :
               count++;
               break;
            default :
               error(file_error);
         }
         data->grid[rows][cols] = input;
      }
   }
   if (count != width) { error(ones); }
   if (fscanf(fp, "%1d", &input) == SINGLE)
   { error(file_error); }
   fclose(fp);
   return;
}

bool file_input (inp_data* data)
{
   if (data == NULL) { error(data_ptr); }
   printf("Please type the name of the file you would like to use: ");
   char str[LONG];
   if (scanf("%s", str) != SINGLE)
   {
      printf("\nIncorrect usage! Please input one file name.\n");
      return file_input(data);
   }
   handle_input(data, str);
   return true;
}

bool manual_input (inp_data* data)
{
   int hght, wdth, input, count = 0;
   printf("\nThe maximum allowed Grid Height or Width is 6.\n");
   printf("What height would you like for your grid? ");
   if (scanf("%d", &hght) != SINGLE || hght > MAX || hght < MIN)
   {
      if (hght > 9) { error(numbs); }
      printf("Incorrect Input Format - Please use numbers only!\n");
   }
   printf("What width would you like for your grid?  ");
   if (scanf("%d", &wdth) != SINGLE || wdth > MAX || wdth < MIN)
   {
      if (wdth > 9) { error(numbs); }
      printf("Incorrect Input Format - Please use numbers only!\n");
   }
   data->height = hght;
   data->width = wdth;
   printf("\nEnter numbers below for each row. Additional input per row will be ignored!\n");
   printf("Please Only input 0's or 1's!\n");
   for (int i = 0; i < hght; i++)
   {
      printf("Please enter %d numbers for the %s Row: ", wdth, numbers(i));
      for (int j = 0; j < wdth; j++)
      {
         scanf("%1d", &input);
         if (input > 9) { error(numbs); }
         switch (input)
         {
            case 0 :
            case 1 :
               data->grid[i][j] = input;
               count += input;
               break;
            default :
               error(usage);
         }
      }
   }
   if (count > wdth)
   {
      printf("You have input too many one's - Please re-do data input!\n");
      return manual_input(data);
   }
   return true;
}

char* numbers (int num)
{
   switch (num)
   {
      case 0 :
         return "1st";
      case 1 :
         return "2nd";
      case 2 :
         return "3rd";
      case 3 :
         return "4th";
      case 4 :
         return "5th";
      case 5 :
         return "6th";
      default :
         error(word);
   }
   return "UNKNOWN";
}

inp_mde check_input (void)
{
   char c;
   printf("Please choose an input method: Manual (M) or from a File (F)\n");
   printf("Input Choice: ");
   if (scanf("%1c", &c) != 1)
   {
      error(usage);
   }
   switch (c)
   {
      case 'm' :
      case 'M' :
         return manual;
      case 'f' :
      case 'F' :
         return file;
      default :
         error(usage);
   }
   return manual;
}

void run_input (inp_data* data)
{
   switch (check_input())
   {
      case manual :
         manual_input(data);
         break;
      case file :
         file_input(data);
         break;
   }
}

inp_data* alloc_data (void)
{
   inp_data* data;
   data = (inp_data*) calloc (1, sizeof(inp_data));
   if (data == NULL) { error(data_ptr); }
   return data;
}

bool verbose (char* arg)
{
   if (strcmp(arg, "-v") == 0)
   { return true; }
   if (strcmp(arg, "--verbose") == 0)
   { return true; }
   return false;
}

void error (err err)
{
   switch (err)
   {
      case brd_alo :
         fprintf(stderr, "\nSpace could not be allocated for board!\n");
         break;
      case brd_err :
         fprintf(stderr, "\nNull Board Pointer!\n");
         break;
      case word :
         fprintf(stderr, "\nInput Error!\n");
         break;
      case data_ptr :
         fprintf(stderr, "\nData Pointer Error!\n");
         break;
      case file_name :
         fprintf(stderr, "\nFile Not Found!\n");
         break;
      case file_error :
         fprintf(stderr, "\nIncorrect File Grid Format!\n");
         break;
      case ones :
         fprintf(stderr, "\nIncorrect number of 1's in Grid!\n");
         break;
      case no_input :
         fprintf(stderr, "\nNo valid input received!\n");
         break;
      case child_err :
         fprintf(stderr, "\nSpace could not be allocated for Children!\n");
         break;
      case numbs :
         fprintf(stderr, "\nIncorrect Usage! Please use numbers only!\n");
         break;
      case usage :
         fprintf(stderr, "\nIncorrect Usage!\n");
         break;
    }
    exit(EXIT_FAILURE);
}

void print_board (board* board)
{
   static int count = 0;
   printf("%d:\n", count);
   for (int row = 0; row < board->rows; row++)
   {
      for (int col = 0; col < board->cols; col++)
      {
         printf("%d", GRID[row][col]);
      }
      printf("\n");
   }
   printf("\n");
   count++;
}

void test (void)
{
   int test_1[MAX][MAX] = {{1, 0, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0}};
   inp_data* test_data = alloc_data();
   assert(test_data);
   test_data->height = 2;
   test_data->width = 2;
   for (int row = 0; row < MAX; row++)
   {
      for (int col = 0; col < MAX; col++)
      {
         test_data->grid[row][col] = test_1[row][col];
      }
   }
   board* test_brd = alloc_brd();
   assert(test_brd);
   fill_first_brd(test_data, test_brd);
   test_brd->next = new_child(test_brd, 0, up);
   assert(unique_grid(test_brd->next, test_brd));
   test_brd->next->next = new_child(test_brd->next, 0, up);
   assert(!unique_grid(test_brd->next->next, test_brd));
   free_all(test_brd);
   free(test_data);
}

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN 2
#define MAX 6
#define BIG 100000
#define NO_PARENT -1
#define NO_SOLVE -1
#define FIRST 0
#define INC_GEN 1
#define SINGLE_DIGIT 1
#define FILE_ARGS 2
#define MIN_ARGS 2
#define MAX_ARGS 3
#define DECR(X) (X - 1)
#define INCR(X) (X + 1)
#define DBL_DECR(X) DECR(DECR(X))
#define DBL_INCR(X) INCR(INCR(X))
#define ROLLS(X, Y) (X + Y) * 2
#define GRID board->grid
#define OUT_OF_BOUNDS_CHECK if (index + children >= BIG) { return NO_SOLVE; }

typedef enum errors {
   input, in_data, alloc, array,
   data, file, scan, size,
   grid_scan, ones, board_ptr
} errors;

typedef enum roll {
   up, down, left, right
} roll;

typedef struct board {
   int rows;
   int cols;
   int grid[MAX][MAX];
   int index;
   int parent;
} board;

typedef struct input_data {
   int height;
   int width;
   int grid[MAX][MAX];
} input_data;

int print_solve (board* arr, int solution, bool verbose);
int find_solve_index (board* arr);
int create_children (board* arr, int parent, int index);

bool is_final (board* board);
bool unique_grid (board* arr, int index);
void roll_board (board* board, int num, roll roll);
void new_board (board* arr, int parent, int index);
void create_first_board (board* arr, input_data* input);

bool verbose (char* arg);
void handle_input (input_data* data, char* f_name);
board* alloc_arr (void);
input_data* alloc_data (void);
void error (errors err);
void print_board (board* board);
void test (void);

int main (int argc, char* argv[])
{
   test();
   
   if (argc < MIN_ARGS || argc > MAX_ARGS) { error(input); }
   input_data* data = alloc_data();
   handle_input(data, argv[DECR(argc)]);
   board* arr = alloc_arr();
   create_first_board (arr, data);
   free(data);
   if (argc == MAX_ARGS && verbose(argv[DBL_DECR(argc)]))
   { print_solve(arr, find_solve_index(arr), true); }
   else { print_solve(arr, find_solve_index(arr), false); }
   free(arr);
}

int print_solve (board* arr, int solution, bool verbose)
{
   static int gen = 0;
   if (solution == NO_SOLVE)
   { printf("No Solution?\n"); }
   else if (verbose)
   {
      if (arr[solution].parent != NO_PARENT)
      { gen += print_solve(arr, arr[solution].parent, verbose); }
      printf("%d:\n", gen);
      print_board(&arr[solution]);
   }
   else
   {
      int count = 0;
      while (arr[solution].parent != NO_PARENT)
      {
         solution = arr[solution].parent;
         count++;
      }
      printf("%d moves\n", count);
   }
   return INC_GEN;
}

int find_solve_index (board* arr)
{
   if (is_final(arr)) { return arr->index; }
   int parent = arr->index;
   int index = INCR(parent);
   int children = 0;
   while (!is_final(&arr[parent]))
   {
      if (index == DECR(BIG)) { return NO_SOLVE; }
      children = create_children(arr, parent, index);
      if (children == NO_SOLVE) { return NO_SOLVE; }
      index += children;
      parent++;
   }
   return parent;
}

int create_children (board* arr, int parent, int index)
{
   int children = 0;
   new_board(arr, parent, index);
   for (int i = 0; i < arr[parent].rows; i++)
   {
      OUT_OF_BOUNDS_CHECK
      roll_board(&arr[index + children], i, left);
      children += unique_grid(arr, index + children);
      OUT_OF_BOUNDS_CHECK
      new_board(arr, parent, index + children);
      roll_board(&arr[index + children], i, right);
      children += unique_grid(arr, index + children);
      OUT_OF_BOUNDS_CHECK
      new_board(arr, parent, index + children);
   }
   for (int j = 0; j < arr[parent].cols; j++)
   {
      OUT_OF_BOUNDS_CHECK
      roll_board(&arr[index + children], j, up);
      children += unique_grid(arr, index + children);
      OUT_OF_BOUNDS_CHECK
      new_board(arr, parent, index + children);
      roll_board(&arr[index + children], j, down);
      children += unique_grid(arr, index + children);
      OUT_OF_BOUNDS_CHECK
      new_board(arr, parent, index + children);
   }
   return children;
}

bool is_final (board* board)
{
   if (board == NULL) { error(board_ptr); }
   int count = 0;
   for (int i = 0; i < board->cols; i++)
   { count += GRID[FIRST][i]; }
   return (count == board->cols);
}

bool unique_grid (board* arr, int index)
{
   if (arr == NULL) { error(array); }
   int height = arr[index].rows;
   int width = arr[index].cols;
   int count;
   for (int i = 0; i < index; i++)
   {
      count = 0;
      for (int row = 0; row < height; row++)
      {
         for (int col = 0; col < width; col++)
         {
            if (arr[i].grid[row][col] == arr[index].grid[row][col])
            { count++; }
         }
      }
      if (count == height * width)
      { return false; }
   }
   return true;
}

void roll_board (board* board, int num, roll roll)
{
   if (board == NULL) { error(board_ptr); }
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

void new_board (board* arr, int parent, int index)
{
   arr[index] = arr[parent];
   arr[index].parent = parent;
   arr[index].index = index;
}

void create_first_board (board* arr, input_data* input)
{
   if (arr == NULL) { error(array); }
   if (input == NULL) { error(data); }
   arr->rows = input->height;
   arr->cols = input->width;
   arr->parent = NO_PARENT;
   for (int rows = 0; rows < input->height; rows++)
   {
      for (int cols = 0; cols < input->width; cols++)
      {
         arr->grid[rows][cols] = input->grid[rows][cols];
      }
   }
}

bool verbose (char* arg)
{
   if (strcmp(arg, "-v") == 0)
   { return true; }
   if (strcmp(arg, "--verbose") == 0)
   { return true; }
   else { return false; }
}

void handle_input (input_data* data, char* f_name)
{
   FILE* fp = fopen(f_name, "r");
   if (fp == NULL) { error (file); }
   int height, width, input, count = 0;
   if (fscanf(fp, "%d %d", &height, &width) != FILE_ARGS)
   { error(scan); }
   if (height > MAX || width > MAX || \
       height < MIN || width < MIN)
   { error(size); }
   data->height = height;
   data->width = width;
   for (int rows = 0; rows < height; rows++)
   {
      for (int cols = 0; cols < width; cols++)
      {
         if (fscanf(fp, "%1d", &input) != SINGLE_DIGIT)
         { error(grid_scan); }
         switch (input) {
            case 0 :
               break;
            case 1 :
               count++;
               break;
            default :
               error(grid_scan);
         }
         data->grid[rows][cols] = input;
      }
   }
   if (count != width) { error(ones); }
   if (fscanf(fp, "%1d", &input) == SINGLE_DIGIT)
   { error(grid_scan); }
   fclose(fp);
   return;
}

board* alloc_arr (void)
{
   board* arr;
   arr = (board*) calloc(BIG, sizeof(board));
   if (arr == NULL) { error(array); }
   return arr;
}

input_data* alloc_data (void)
{
   input_data* data;
   data = (input_data*) calloc (1, sizeof(input_data));
   if (data == NULL) { error(in_data); }
   return data;
}

void error (errors err)
{
   switch (err)
   {
      case input :
         fprintf(stderr, "Incorrect command line usage!\n");
         break;
      case in_data :
         fprintf(stderr, "Memory could not be allocated for Input Data.\n");
         break;
      case alloc :
         fprintf(stderr, "Memory could not be allocated for Board Array.\n");
         break;
      case array :
         fprintf(stderr, "Null Array\n");
         break;
      case data :
         fprintf(stderr, "Null Data\n");
         break;
      case file :
         fprintf(stderr, "Null File Pointer\n");
         break;
      case scan :
         fprintf(stderr, "Incorrect file header format.\n");
         break;
      case size :
         fprintf(stderr, "Height and / or Width inputs are invalid.\n");
         break;
      case grid_scan :
         fprintf(stderr, "Incorrect file grid format.\n");
         break;
      case ones :
         fprintf(stderr, "Incorrect number of 1's in file grid.\n");
         break;
      case board_ptr :
         fprintf(stderr, "Board pointer was NULL.\n");
         break;
   }
   exit(EXIT_FAILURE);
}

void print_board (board* board)
{
   for (int row = 0; row < board->rows; row++)
   {
      for (int col = 0; col < board->cols; col++)
      {
         printf("%d", GRID[row][col]);
      }
      printf("\n");
   }
   printf("\n");
}

void test (void)
{
   input_data* test_data = alloc_data();
   assert(test_data);
   test_data->height = MAX;
   test_data->width = MAX;
   int test_grd[MAX][MAX] = {{1, 0, 0, 0, 0, 0},
                             {0, 1, 0, 0, 0, 0},
                             {0, 0, 1, 0, 0, 0},
                             {0, 0, 0, 1, 0, 0},
                             {0, 0, 0, 0, 1, 0},
                             {0, 0, 0, 0, 0, 1}};
   for (int row = 0; row < MAX; row++)
   {
      for (int col = 0; col < MAX; col++)
      {
         test_data->grid[row][col] = test_grd[row][col];
      }
   }
   board* test_arr = alloc_arr();
   assert(test_arr);
   create_first_board(test_arr, test_data);
   for (int i = 0; i < MAX; i++)
   {
      for (int j = 0; j < MAX; j++)
      { assert(test_arr->grid[i][j] == test_data->grid[i][j]); }
   }
   free(test_data);
   int index = test_arr->index;
   new_board(test_arr, index, INCR(index));
   index++;

   assert(!unique_grid(test_arr, index));
   roll_board(&test_arr[index], 0, left);
   assert(unique_grid(test_arr, index));
   roll_board(&test_arr[index], 0, right);
   assert(!unique_grid(test_arr, index));

   roll_board(&test_arr[index], 0, left);
   for (int i = 0; i < DECR(MAX); i++)
   {
      assert(unique_grid(test_arr, index));
      roll_board(&test_arr[index], 0, left);
   }
   assert(!unique_grid(test_arr, index));

   roll_board(&test_arr[index], 0, right);
   for (int i = 0; i < DECR(MAX); i++)
   {
      assert(unique_grid(test_arr, index));
      roll_board(&test_arr[index], 0, right);
   }
   assert(!unique_grid(test_arr, index));

   roll_board(&test_arr[index], 0, up);
   for (int i = 0; i < DECR(MAX); i++)
   {
      assert(unique_grid(test_arr, index));
      roll_board(&test_arr[index], 0, up);
   }
   assert(!unique_grid(test_arr, index));

   roll_board(&test_arr[index], 0, down);
   for (int i = 0; i < DECR(MAX); i++)
   {
      assert(unique_grid(test_arr, index));
      roll_board(&test_arr[index], 0, down);
   }
   assert(!unique_grid(test_arr, index));

   index = 0;
   int children = create_children(test_arr, index, INCR(index));
   assert(children == ROLLS(MAX, MAX));

   int test_2[MAX][MAX] = {{1, 1, 1, 1, 1, 1},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0}};
   for (int row = 0; row < MAX; row++)
   {
      for (int col = 0; col < MAX; col++)
      {
         test_arr[index].grid[row][col] = test_2[row][col];
      }
   }
   assert(is_final(&test_arr[index]));
   assert(find_solve_index(test_arr) == 0);
   free(test_arr);

   int test_3[MAX][MAX] = {{1, 0, 0, 0, 0, 0},
                           {1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0}};
   test_data = alloc_data();
   assert(test_data);
   test_data->height = 2;
   test_data->width = 2;
   for (int row = 0; row < MAX; row++)
   {
      for (int col = 0; col < MAX; col++)
      {
         test_data->grid[row][col] = test_3[row][col];
      }
   }
   test_arr = alloc_arr();
   assert(test_arr);
   create_first_board(test_arr, test_data);
   free(test_data);
   assert(!is_final(test_arr));
   int parent = 0;
   index = INCR(parent);
   children = create_children(test_arr, parent, index);
   assert(children == 2);
   index += children;
   parent++;
   children = create_children(test_arr, parent, index);
   assert(children == 3);
   assert(is_final(&test_arr[INCR(index)]));
   assert(find_solve_index(test_arr) == 4);
   free(test_arr);

   assert(verbose("-v"));
   assert(verbose("--verbose"));
   assert(!verbose("abc"));
   assert(!verbose("123"));

}

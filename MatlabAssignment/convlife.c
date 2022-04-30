#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int c2i (char c);

int main (int argc, char* argv[])
{
   if (argc != 2) { return 1; }
   FILE* fp = fopen(argv[argc-1], "r");
   char c;
   while (fscanf(fp, "%c", &c) == 1) {
      if (c == '\n') { printf("\n"); }
      else { printf("%d ", c2i(c)); }
   }
}

int c2i (char c)
{
   if (c == '.') { return 0; }
   if (c == 'O') { return 1; }
   return -1;
}

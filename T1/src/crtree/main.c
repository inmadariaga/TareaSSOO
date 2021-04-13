#include <stdio.h>
#include "../file_manager/manager.h"

int main(int argc, char **argv)
{ 
  InputFile* file = read_file(argv[1]);
  printf("ARCG: %i", argc);
  printf("Length: %i\n", file->len);
  printf("First Line: %s\n", file->lines[0][0]);
}

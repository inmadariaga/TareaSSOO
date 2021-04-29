#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    float suma = 0;
    for (int i = 1; i < argc; i++){
        suma += atoi(argv[i]);
    }
    float avg = suma / (argc - 1);
    return avg;
}
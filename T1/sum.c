#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int suma = 0;
    for (int i = 1; i < argc; i++){
        suma += atoi(argv[i]);
    }
    return suma;
}
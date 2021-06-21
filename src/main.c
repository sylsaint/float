#include<stdio.h>

#include "sfloat.h"


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("\n float number must be the second argument \n");
    }
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            struct Float32 f32 = parse_float32(argv[i]);
            print_float32(f32); 
        }
    }
    return 0;
}
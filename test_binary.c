#include<stdio.h>
#include<stdlib.h>

#include "sfloat.h"


int main()
{
    Binary b1 = {.bit_cnt = 4, .bits = (u_int8_t[4]){1, 0, 1, 1}};
    Binary b2 = {.bit_cnt = 5, .bits = (u_int8_t[5]){1, 0, 0, 0, 1}};
    Binary bs = plus_binary(b1, b2);
    printf("bs.bit_cnt = %d\n", bs.bit_cnt);
    u_int8_t *bits = bs.bits;
    for (u_int32_t i = 0; i < bs.bit_cnt; i++)
    {
        printf("%d", *(bits++));
    }
    printf("\n");
    free(bs.bits);

    Binary b3 = {.bit_cnt = 0, .bits = NULL};
    Binary bs1 = plus_binary(b1, b3);
    printf("bs1.bit_cnt = %d\n", bs1.bit_cnt);
    u_int8_t *bits1 = bs1.bits;
    for (u_int32_t i = 0; i < bs1.bit_cnt; i++)
    {
        printf("%d", *(bits1++));
    }
    printf("\n");
    free(bs1.bits);
    Binary bs2 = multiply_binary(b1, b2);
    printf("bs2.bit_cnt = %d\n", bs2.bit_cnt);
    u_int8_t *bits2 = bs2.bits;
    for (u_int32_t i = 0; i < bs2.bit_cnt; i++)
    {
        printf("%d", *(bits2++));
    }
    printf("\n");
    free(bs2.bits);
    return 0;
}
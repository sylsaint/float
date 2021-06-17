#include<stdio.h>
#include<stdlib.h>

#include "sfloat.h"

int main()
{
    u_int8_t *n1 = (u_int8_t[3]){3, 1, 2};
    Binary b = convert2binary(n1, 3);
    printf("b.bit_cnt = %d\n", b.bit_cnt);
    for (u_int32_t i = 0; i < b.bit_cnt; i++)
    {
        printf("%d", *(b.bits + i));
    }
    printf("\n");
    revoke(b);
    return 0;
}
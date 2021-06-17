#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sfloat.h"

Binary BI = {.bit_cnt = 1, .bits = (u_int8_t[1]){1}};
Binary BII = {.bit_cnt = 2, .bits = (u_int8_t[2]){0, 1}};
Binary BIII = {.bit_cnt = 2, .bits = (u_int8_t[2]){1, 1}};
Binary BIV = {.bit_cnt = 3, .bits = (u_int8_t[3]){0, 0, 1}};
Binary BV = {.bit_cnt = 3, .bits = (u_int8_t[3]){1, 0, 1}};
Binary BVI = {.bit_cnt = 3, .bits = (u_int8_t[3]){1, 1, 0}};
Binary BVII = {.bit_cnt = 3, .bits = (u_int8_t[3]){1, 1, 1}};
Binary BVIII = {.bit_cnt = 4, .bits = (u_int8_t[4]){0, 0, 0, 1}};
Binary BIX = {.bit_cnt = 4, .bits = (u_int8_t[4]){1, 0, 0, 1}};
Binary BX = {.bit_cnt = 4, .bits = (u_int8_t[4]){0, 1, 0, 1}};

Binary plus_binary(Binary b1, Binary b2)
{
    u_int32_t bit_cnt = MIN(b1.bit_cnt, b2.bit_cnt);
    u_int32_t max_cnt = MAX(b1.bit_cnt, b2.bit_cnt);
    u_int8_t carry = 0;
    u_int8_t *bits1 = b1.bits;
    u_int8_t *bits2 = b2.bits;
    Binary bf = {.bit_cnt = max_cnt, .bits = malloc(sizeof(u_int8_t) * (max_cnt + 1))};
    u_int8_t *bits = bf.bits;
    for (uint32_t i = 0; i < bit_cnt; i++)
    {
        u_int8_t bit = *(bits1++) + *(bits2++);
        *(bits++) = (carry + bit) % 2;
        carry = (carry + bit) / 2;
    }
    u_int8_t *rbits = b1.bit_cnt > b2.bit_cnt ? bits1 : bits2;
    for (u_int32_t i = bit_cnt; i < max_cnt; i++)
    {
        u_int8_t bit = carry + *(rbits++);
        *(bits++) = bit % 2;
        carry = bit / 2;
    }
    if (carry)
    {
        bf.bit_cnt++;
        *(bits++) = carry;
    }
    return bf;
}

Binary multiply_binary(Binary b1, Binary b2)
{
    // find the shorter binary
    u_int8_t *low_bits = b1.bit_cnt < b2.bit_cnt ? b1.bits : b2.bits;
    u_int8_t *high_bits = b1.bit_cnt >= b2.bit_cnt ? b1.bits : b2.bits;
    u_int32_t low_cnt = MIN(b1.bit_cnt, b2.bit_cnt);
    u_int32_t high_cnt = MAX(b1.bit_cnt, b2.bit_cnt);
    Binary bf = {.bit_cnt = 0, .bits = NULL};
    for (u_int32_t i = 0; i < low_cnt; i++)
    {
        u_int32_t bit = *(low_bits + i);
        // if bit == 1 then shift high_bits by i
        if (bit)
        {
            // should be freed after used
            Binary bt = {.bit_cnt = high_cnt + i, .bits = malloc(sizeof(u_int8_t) * (high_cnt + i))};

            // initialize lower bits to 0
            for (u_int32_t k = 0; k < i; k++)
            {
                *(bt.bits + k) = 0;
            }
            // copy high_bits to bt
            for (u_int32_t j = i; j < high_cnt + i; j++)
            {
                *(bt.bits + j) = *(high_bits + j - i);
            }
            u_int8_t *to_free = bf.bits;
            bf = plus_binary(bf, bt);
            // free previous bf and bt to avoid memory leak
            free(to_free);
            free(bt.bits);
        }
    }
    return bf;
}

Binary convert2binary(u_int8_t *cs, u_int32_t len)
{
    // radix binary represent 10^n
    Binary b0 = BI;
    Binary bx = {.bit_cnt = 1, .bits = malloc((sizeof(u_int8_t)))};
    *(bx.bits) = 1;
    Binary b = {.bit_cnt = 0, .bits = NULL};
    for (u_int32_t i = 0; i < len; i++)
    {
        u_int8_t datum = *(cs + i);
        switch (datum)
        {
        case 1:
            b0 = BI;
            break;
        case 2:
            b0 = BII;
            break;
        case 3:
            b0 = BIII;
            break;
        case 4:
            b0 = BIV;
            break;
        case 5:
            b0 = BV;
            break;
        case 6:
            b0 = BVI;
            break;
        case 7:
            b0 = BVII;
            break;
        case 8:
            b0 = BVIII;
            break;
        case 9:
            b0 = BIX;
            break;
        default:
            break;
        }
        u_int8_t *to_free_base_bits = bx.bits;
        Binary b1 = multiply_binary(b0, bx);
        u_int8_t *to_free_bits = b.bits;
        b = plus_binary(b, b1);
        bx = multiply_binary(bx, BX);
        free(b1.bits);
        free(to_free_bits);
        free(to_free_base_bits);
    }
    return b;
}

u_int32_t get_exp_num(u_int8_t *exps, u_int32_t len)
{
    u_int32_t n = 0;
    u_int8_t radix = 10;
    u_int32_t base = 1;
    for(u_int32_t i = 0; i < len; i++)
    {
        n = *(exps + i) + n * base;
        base = base * radix;
    }
    return n;
}

void parse_float(char *cs)
{
    struct Float16Decimal d = {.sign = 0, .integers = NULL, .decimals = NULL, .exponents = NULL, .exp_sign = 0};

    printf("sign: ");
    if (*cs == SIGN_MINUS)
    {
        printf("%c", *cs);
        cs++;
        d.sign = 1;
    }
    if (*cs == SIGN_PLUS)
    {
        printf("%c", *cs);
        cs++;
    }
    u_int8_t *ints = malloc(sizeof(u_int8_t) * 20);
    u_int32_t int_cnt = 0;
    printf(" integer: ");
    while (*cs != CHAR_END && *cs != DOT && *cs != EXPONENT_LOWER && *cs != EXPONENT_UPPER)
    {
        printf("%c", *cs);
        *(ints + int_cnt) = *(cs++) - '0';
        int_cnt++;
    }
    d.integers = ints;
    d.int_cnt = int_cnt;
    // skip the dot(.) char
    *cs != CHAR_END &&*cs != EXPONENT_LOWER &&*cs != EXPONENT_UPPER &&cs++;
    u_int8_t *decimals = malloc(sizeof(u_int8_t) * 20);
    u_int32_t dec_cnt = 0;
    printf(" decimal: ");
    while (*cs != EXPONENT_LOWER && *cs != EXPONENT_UPPER && *cs != CHAR_END)
    {
        printf("%c", *cs);
        *(decimals + dec_cnt) = *cs++ - '0';
        dec_cnt++;
    }
    d.decimals = decimals;
    d.dec_cnt = dec_cnt;
    // skip the exp(e|E) char
    *cs != CHAR_END &&cs++;
    printf(" exponent: ");
    if (*cs == SIGN_MINUS)
    {
        printf("%c", *cs);
        cs++;
        d.exp_sign = 1;
    }
    if (*cs == SIGN_PLUS)
    {
        printf("%c", *cs);
        cs++;
    }
    u_int8_t *exps = malloc(sizeof(u_int8_t) * 20);
    u_int32_t exp_cnt = 0;
    while (*cs != CHAR_END)
    {
        printf("%c", *cs);
        *(exps + exp_cnt) = *cs++ - '0';
        exp_cnt++;
    }
    d.exponents = exps;
    d.exp_cnt = exp_cnt;
    printf("\n");
    struct Float16 f = {.sign = d.sign, .exponent = 0, .significand = NULL};
    printf("decimal count = %d\n", d.dec_cnt);
    // count how many significant decmials, excluding values like 1.000000000
    u_int32_t exp_num = get_exp_num(d.exponents, d.exp_cnt);
    printf("exp = %d\n", exp_num); 
    if (d.exp_sign)
    {
        // negative

    } else {
        // positive
    }
    d.integers;
    d.decimals; 
}

// dont call it more than once
void revoke(Binary b)
{
    free(b.bits);
}

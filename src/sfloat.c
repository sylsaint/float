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

void print_binary(Binary b)
{
    if (b.dot) printf("0.");
    for (int i = b.bit_cnt; i > 0; i--)
    {
        printf("%d", *(b.bits + i - 1));
    }
    printf("\n");
}

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

Binary decimal2binary(u_int8_t *cs, u_int32_t len, u_int32_t precision)
{
    // radix binary represent 10^n
    Binary b = { .bit_cnt = precision, .bits = malloc(sizeof(u_int8_t) * precision), .dot = 1 };
    while (precision--)
    {
        u_int8_t carry = 0;
        for (u_int32_t i = len; i > 0; i--)
        {
            *(cs + i - 1) = *(cs + i - 1) * 2 % 10 + carry;
            carry = *(cs + i - 1) / 10;
        }
        *(b.bits + precision) = carry;
    }
    return b;
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
    for (u_int32_t i = 0; i < len; i++)
    {
        n = *(exps + i) + n * base;
        base = base * radix;
    }
    return n;
}

void parse_float(char *cs)
{
    struct Float16Decimal d = {.sign = 0, .numbers = NULL, .exponents = NULL, .exp_sign = 0};

    // detect the length of cs
    u_int32_t cs_len = 0;
    while (*(cs + cs_len) != CHAR_END)
        cs_len++;
    printf("cs len = %d\n", cs_len);
    u_int8_t *numbers = malloc(sizeof(u_int8_t) * cs_len);
    u_int8_t *numbers_p = numbers;
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
    u_int32_t int_cnt = 0;
    printf(" integer: ");
    while (*cs != CHAR_END && *cs != DOT && *cs != EXPONENT_LOWER && *cs != EXPONENT_UPPER)
    {
        printf("%c", *cs);
        *(numbers_p++) = *(cs++) - '0';
        int_cnt++;
    }
    d.int_cnt = int_cnt;
    // skip the dot(.) char
    *cs != CHAR_END &&*cs != EXPONENT_LOWER &&*cs != EXPONENT_UPPER &&cs++;
    u_int32_t dec_cnt = 0;
    printf(" decimal: ");
    while (*cs != EXPONENT_LOWER && *cs != EXPONENT_UPPER && *cs != CHAR_END)
    {
        printf("%c", *cs);
        *(numbers_p++) = *cs++ - '0';
        dec_cnt++;
    }
    d.dec_cnt = dec_cnt;
    d.numbers = numbers;
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
    u_int8_t *exps = malloc(sizeof(u_int8_t) * (cs_len - dec_cnt - int_cnt));
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
    printf("decimal count = %d\n", d.dec_cnt);
    // count how many significant decmials, excluding values like 1.000000000
    u_int32_t exp_num = get_exp_num(d.exponents, d.exp_cnt);
    printf("exp = %d\n", exp_num);
    u_int8_t *integers = malloc(sizeof(u_int8_t) * (d.dec_cnt + d.int_cnt + exp_num));
    u_int8_t *decimals = malloc(sizeof(u_int8_t) * (d.dec_cnt + d.int_cnt + exp_num));
    u_int32_t int_len = 0, dec_len = 0; 
    if (d.exp_sign && exp_num)
    {
        // negative
        int32_t offset = (int32_t)exp_num - d.int_cnt;
        if (offset > 0)
        {
            int_len = 0;
            for (int i = 0; i < offset; i++)
            {
                *(decimals + i) = 0;
            }
            for (int i = 0; i < d.dec_cnt + d.int_cnt; i++)
            {
                *(decimals + i + offset) = *(d.numbers + i);
            }
            dec_len = exp_num + d.dec_cnt;
        } else {
            int_len = offset;
            for (int i = 0; i < offset; i++)
            {
                *(integers + i) = *(d.numbers + i);
            }
            for (int i = offset; i < d.dec_cnt + d.int_cnt; i++)
            {
                *(decimals + i) = *(d.numbers + i);
            }
            dec_len = exp_num + d.dec_cnt;
        }
    } else if (exp_num) {
        // positive
        int32_t offset = (int32_t)exp_num - d.dec_cnt;
        printf("offset = %d\n", offset);
        if (offset >= 0)
        {
            int_len = d.int_cnt + exp_num;
            for(int i = 0; i < d.dec_cnt + d.int_cnt; i++)
            {
                *(integers + i) = *(d.numbers + i);
            }
            for (int i = d.dec_cnt + d.int_cnt; i < int_len; i++)
            {
                *(integers + i) = 0;
            }
            dec_len = 0;
        } else {
            int_len = d.int_cnt + exp_num;
            for (int i = 0; i < int_len; i++)
            {
                *(integers + i) = *(d.numbers + i);
            }
            dec_len = d.dec_cnt - exp_num;
            for (int i = 0; i< dec_len; i++)
            {
                *(decimals + i) = *(d.numbers + int_len + i);
            }
        }
    } else {
        int_len = d.int_cnt;
        for(int i = 0; i < int_len; i++)
        {
            *(integers + i) = *(d.numbers + i);
        }
        dec_len = d.dec_cnt;
        for (int i = 0; i < dec_len; i++)
        {
            *(decimals + i) = *(d.numbers + i + int_len);
        }
    }
    printf("new integers: ");
    for(int i = 0; i < int_len; i++)
    {
        printf("%d", *(integers + i));
    }
    printf("\nnew decimals: ");
    for(int i = 0; i < dec_len; i++)
    {
        printf("%d", *(decimals + i));
    }
    printf("\n");
    // should be freed when used
    Binary int_b = convert2binary(integers, int_len);
    print_binary(int_b);
    Binary dot_b = decimal2binary(decimals, dec_len, FLOAT16_PRECISION);
    print_binary(dot_b);
    // count exponent
    struct Float16 f = {.sign = d.sign, .exponent = 0, .significand = NULL};
    Binary fb = { .bit_cnt = FLOAT16_TRAILING, .bits = (u_int8_t[FLOAT16_TRAILING]){0} };
    f.significand = fb;
    if (int_b.bit_cnt > 0)
    {
        u_int32_t exponent = int_b.bit_cnt - 1;
        u_int8_t exp_sign = 0;
        // infty
        if (exponent + FLOAT16_BIAS >= 2 * FLOAT16_EMAX + 1)
        {
            f.exponent = 2 * FLOAT16_EMAX + 1;
        } else if (exponent + FLOAT16_BIAS == 2 * FLOAT16_EMAX)  {
            bool exceed = true;
            u_int32_t trail = FLOAT16_TRAILING;
            // msb has been encoded in the exponent
            for (int i = int_b.bit_cnt - 1; i > 0, trail > 0; i--, trail--)
            {
                if (*(int_b.bits + i - 1) == 0) {
                    exceed = false;
                    break;
                };
            }
            for (int i = dot_b.bit_cnt; i > 0, trail > 0; i--, trail--)
            {
                if (*(dot_b.bits + i - 1) == 0) {
                    exceed = false;
                    break;
                };
            }
            if (trail == 0)
            {
                if (FLOAT16_TRAILING + 1 <= int_b.bit_cnt && *(int_b.bits + FLOAT16_TRAILING) == 1) exceed = true;
                if (FLOAT16_TRAILING + 1 > int_b.bit_cnt && *(dot_b.bits + FLOAT16_TRAILING - int_b.bit_cnt) == 1) exceed = true;
            }
            if (exceed)
            {
                f.exponent = 2 * FLOAT16_EMAX + 1;
            } else {
                f.exponent = exponent + FLOAT16_BIAS;
                u_int32_t offset = FLOAT16_TRAILING;
                for (int i = int_b.bit_cnt - 1; i > 0, offset > 0; i--, offset--)
                {
                    *(f.significand.bits + offset - 1) = *(int_b.bits + i - 1);
                }
                for (int i = dot_b.bit_cnt; i > 0, offset > 0; i--, offset--)
                {
                    *(f.significand.bits + offset - 1) = *(dot_b.bits + i - 1);
                }
            }
        }
        // 
    } else {
        u_int32_t exponent = int_len - 1;
        u_int8_t exp_sign = 1;
    }
}

// dont call it more than once
void revoke(Binary b)
{
    free(b.bits);
}

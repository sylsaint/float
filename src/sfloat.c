#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>

#include "sfloat.h"

Binary B0 = {.bit_cnt = 1, .bits = (u_int8_t[1]){0}};
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


void print_float32(Float32 f32)
{
    printf("{ sign = %d, exponent = %d, significand = ", f32.sign, f32.exponent);
    print_binary(f32.significand);
    printf(" }\n");
}

void print_binary(Binary b)
{
    printf("{ bit_cnt = %d, bits = '", b.bit_cnt);
    if (b.dot)
        printf("0.");
    for (int i = b.bit_cnt; i > 0; i--)
    {
        printf("%d", *(b.bits + i - 1));
    }
    printf("' }");
}

void copy_bit(Binary from, Binary to, u_int32_t len, u_int32_t offset)
{
    assert(len + offset <= from.bit_cnt && len <= to.bit_cnt);
    for (int i = 0; i < len; i++)
    {
        *(to.bits + i) = *(from.bits + i + offset);
    }
}

void reset_binary(Binary b)
{
    for(int i = 0; i < b.bit_cnt; i++)
    {
        *(b.bits + i) = 0;
    }
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

Binary decimal2binary(u_int8_t *cs, u_int32_t len, u_int32_t precision, u_int32_t max_zeros)
{
    // TODO: should consider 0.0000000
    bool all_zero = true;
    for (int i = 0; i < len; i++)
    {
        if (*(cs + i)) all_zero = false;
    }
    if (all_zero) {
        Binary bf = {.bit_cnt = 0, .bits = malloc(sizeof(u_int8_t) * precision), .dot = 1, .leading_zeros = 0};
        return bf;
    }
    // radix binary represent 10^n
    Binary b = {.bit_cnt = precision, .bits = malloc(sizeof(u_int8_t) * precision), .dot = 1};
    u_int32_t zero_cnt = 0;
    // count leading zeros
    while (zero_cnt < max_zeros)
    {
        u_int8_t carry = 0;
        for (u_int32_t i = len; i > 0; i--)
        {
            u_int8_t sum = *(cs + i - 1) * 2 + carry;
            *(cs + i - 1) = sum % 10; 
            carry = sum / 10;
        }
        if (carry == 1)
        {
            precision--;
            *(b.bits + precision) = carry;
            break;
        }
        zero_cnt++;
    }
    while (precision--)
    {
        u_int8_t carry = 0;
        for (u_int32_t i = len; i > 0; i--)
        {
            u_int8_t sum = *(cs + i - 1) * 2 + carry;
            *(cs + i - 1) = sum % 10; 
            carry = sum / 10;
        }
        *(b.bits + precision) = carry;
    }
    Binary bf = {.bit_cnt = zero_cnt + b.bit_cnt, .bits = malloc(sizeof(u_int8_t) * (zero_cnt + b.bit_cnt)), .dot = 1, .leading_zeros = zero_cnt};
    reset_binary(bf);
    for (int i = 0; i < b.bit_cnt; i++)
    {
        *(bf.bits + i) = *(b.bits + i);
    }
    for (int i = 0; i < zero_cnt; i++)
    {
        *(bf.bits + i + b.bit_cnt) = 0;
    }
    // clean up
    revoke(b);
    return bf;
}

Binary convert2binary(u_int8_t *cs, u_int32_t len)
{
    // TODO: should consider 0.0000000
    bool all_zero = true;
    for (int i = 0; i < len; i++)
    {
        if (*(cs + i)) all_zero = false;
    }
    if (all_zero) {
        Binary bf = { .bit_cnt = 0, .bits = malloc(sizeof(u_int8_t) * len) };
        return bf;
    }
    // radix binary represent 10^n
    Binary b0 = BI;
    Binary b = {.bit_cnt = 1, .bits = malloc((sizeof(u_int8_t)))};
    *(b.bits) = 0;
    for (u_int32_t i = 0; i < len; i++)
    {
        u_int8_t datum = *(cs + i);
        switch (datum)
        {
        case 0:
            b0 = B0;
            break;
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
        u_int8_t *to_free_b = b.bits;
        b = multiply_binary(b, BX);
        free(to_free_b);
        to_free_b = b.bits;
        b = plus_binary(b, b0);
        free(to_free_b);
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

Float32 parse_float32(char *cs)
{
    struct FloatDecimal d = {.sign = 0, .numbers = NULL, .exponents = NULL, .exp_sign = 0};

    // detect the length of cs
    u_int32_t cs_len = 0;
    while (*(cs + cs_len) != CHAR_END)
        cs_len++;
    u_int8_t *numbers = malloc(sizeof(u_int8_t) * cs_len);
    u_int8_t *numbers_p = numbers;
    if (*cs == SIGN_MINUS)
    {
        cs++;
        d.sign = 1;
    }
    if (*cs == SIGN_PLUS)
    {
        cs++;
    }
    u_int32_t int_cnt = 0;
    while (*cs != CHAR_END && *cs != DOT && *cs != EXPONENT_LOWER && *cs != EXPONENT_UPPER)
    {
        *(numbers_p++) = *(cs++) - '0';
        int_cnt++;
    }
    d.int_cnt = int_cnt;
    // skip the dot(.) char
    *cs != CHAR_END &&*cs != EXPONENT_LOWER &&*cs != EXPONENT_UPPER &&cs++;
    u_int32_t dec_cnt = 0;
    while (*cs != EXPONENT_LOWER && *cs != EXPONENT_UPPER && *cs != CHAR_END)
    {
        *(numbers_p++) = *cs++ - '0';
        dec_cnt++;
    }
    d.dec_cnt = dec_cnt;
    d.numbers = numbers;
    // skip the exp(e|E) char
    *cs != CHAR_END &&cs++;
    if (*cs == SIGN_MINUS)
    {
        cs++;
        d.exp_sign = 1;
    }
    if (*cs == SIGN_PLUS)
    {
        cs++;
    }
    u_int8_t *exps = malloc(sizeof(u_int8_t) * (cs_len - dec_cnt - int_cnt));
    u_int32_t exp_cnt = 0;
    while (*cs != CHAR_END)
    {
        *(exps + exp_cnt) = *cs++ - '0';
        exp_cnt++;
    }
    d.exponents = exps;
    d.exp_cnt = exp_cnt;
    // count how many significant decmials, excluding values like 1.000000000
    u_int32_t exp_num = get_exp_num(d.exponents, d.exp_cnt);
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
        }
        else
        {
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
    }
    else if (exp_num)
    {
        // positive
        int32_t offset = (int32_t)exp_num - d.dec_cnt;
        if (offset >= 0)
        {
            int_len = d.int_cnt + exp_num;
            for (int i = 0; i < d.dec_cnt + d.int_cnt; i++)
            {
                *(integers + i) = *(d.numbers + i);
            }
            for (int i = d.dec_cnt + d.int_cnt; i < int_len; i++)
            {
                *(integers + i) = 0;
            }
            dec_len = 0;
        }
        else
        {
            int_len = d.int_cnt + exp_num;
            for (int i = 0; i < int_len; i++)
            {
                *(integers + i) = *(d.numbers + i);
            }
            dec_len = d.dec_cnt - exp_num;
            for (int i = 0; i < dec_len; i++)
            {
                *(decimals + i) = *(d.numbers + int_len + i);
            }
        }
    }
    else
    {
        int_len = d.int_cnt;
        for (int i = 0; i < int_len; i++)
        {
            *(integers + i) = *(d.numbers + i);
        }
        dec_len = d.dec_cnt;
        for (int i = 0; i < dec_len; i++)
        {
            *(decimals + i) = *(d.numbers + i + int_len);
        }
    }
    // new float 16
    Float32 f32 = {.sign = d.sign, .exponent = 0, .significand = NULL};
    Binary fb = {.bit_cnt = FLOAT32_TRAILING, .bits = malloc(sizeof(u_int8_t) * FLOAT32_TRAILING)};
    reset_binary(fb);
    f32.significand = fb;
    // should be freed when used
    Binary int_b = convert2binary(integers, int_len);
    // add additional bit for rounding
    Binary dot_b = decimal2binary(decimals, dec_len, FLOAT32_PRECISION + 1, FLOAT32_BIAS + FLOAT32_TRAILING - 1);
    // combine int and dot
    u_int32_t total_bit_cnt = int_b.bit_cnt + dot_b.bit_cnt;
    Binary total_b = {.bit_cnt = total_bit_cnt, .bits = malloc(sizeof(u_int8_t) * total_bit_cnt)};
    reset_binary(total_b);
    // copy int_b and dot_b to total_b
    for (int i = 0; i < dot_b.bit_cnt; i++)
    {
        *(total_b.bits + i) = *(dot_b.bits + i);
    }
    for (int i = 0; i < int_b.bit_cnt; i++)
    {
        *(total_b.bits + i + dot_b.bit_cnt) = *(int_b.bits + i);
    }
    if (int_b.bit_cnt == 0)
    {
        // zero
        if (dot_b.bit_cnt == 0)
        {
            f32.exponent = 0;
        }
        // subnormal
        else if (dot_b.leading_zeros >= FLOAT32_BIAS - 1)
        {
            f32.exponent = 0;
            u_int32_t offset = dot_b.bit_cnt + 1 - FLOAT32_BIAS - FLOAT32_TRAILING;
            u_int8_t carry = *(dot_b.bits + offset - 1);
            if (carry)
            {
                Binary carry_bit = {.bit_cnt = 1, .bits = (u_int8_t[1]){1}};
                Binary dot_sub_b = {.bit_cnt = FLOAT32_TRAILING, .bits = malloc(sizeof(u_int8_t) * (FLOAT32_TRAILING))};
                copy_bit(dot_b, dot_sub_b, FLOAT32_TRAILING, offset);
                Binary carried_sub_b = plus_binary(carry_bit, dot_sub_b);
                if (carried_sub_b.bit_cnt > dot_sub_b.bit_cnt)
                {
                    f32.exponent++;
                    copy_bit(carried_sub_b, f32.significand, FLOAT32_TRAILING, 0);
                } else {
                    copy_bit(carried_sub_b, f32.significand, FLOAT32_TRAILING, 0);
                }
                // clean up
                revoke(dot_sub_b);
                revoke(carried_sub_b);
            } else {
                copy_bit(dot_b, f32.significand, FLOAT16_TRAILING, offset);
            }
        }
        // normal
        else
        {
            f32.exponent = FLOAT32_BIAS - dot_b.leading_zeros - 1;
            u_int8_t carry = *dot_b.bits;
            if (carry)
            {
                Binary carry_bit = {.bit_cnt = 1, .bits = (u_int8_t[1]){1}};
                Binary dot_sub_b = {.bit_cnt = FLOAT32_TRAILING + 1, .bits = malloc(sizeof(u_int8_t) * (FLOAT32_TRAILING + 1))};
                copy_bit(dot_b, dot_sub_b, FLOAT32_TRAILING + 1, 1);
                Binary carried_sub_b = plus_binary(carry_bit, dot_sub_b);
                if (carried_sub_b.bit_cnt > dot_sub_b.bit_cnt)
                {
                    f32.exponent++;
                    copy_bit(carried_sub_b, f32.significand, FLOAT32_TRAILING, 1);
                } else {
                    copy_bit(carried_sub_b, f32.significand, FLOAT32_TRAILING, 0);
                }
                // clean up
                revoke(dot_sub_b);
                revoke(carried_sub_b);
            } else {
                for (int i = 0; i < FLOAT32_TRAILING; i++)
                {
                    *(f32.significand.bits + i) = *(dot_b.bits + i + 1);
                }
            }
        }
    }
    // calc exponent and binary part, round to nearest
    else if (FLOAT32_TRAILING + 1 < int_b.bit_cnt)
    {
        u_int32_t offset = int_b.bit_cnt - 1 - FLOAT32_TRAILING;
        u_int8_t carry = *(int_b.bits + offset - 1);
        // test if infty
        if (int_b.bit_cnt - 1 + FLOAT32_BIAS >= 2 * FLOAT32_EMAX + 1)
        {
            f32.exponent = 2 * FLOAT32_EMAX + 1;
        }
        else if (carry)
        {
            Binary carry_bit = {.bit_cnt = 1, .bits = (u_int8_t[1]){1}};
            Binary int_sub_b = {.bit_cnt = FLOAT32_TRAILING + 1, .bits = malloc(sizeof(u_int8_t) * (FLOAT32_TRAILING + 1))};
            copy_bit(int_b, int_sub_b, FLOAT32_TRAILING + 1, offset);
            Binary carried_int_b = plus_binary(carry_bit, int_sub_b);
            if (carried_int_b.bit_cnt > int_sub_b.bit_cnt)
            {
                f32.exponent = int_b.bit_cnt + FLOAT32_BIAS;
                // if rounding up to infty
                if (f32.exponent != 2 * FLOAT32_EMAX + 1)
                {
                    copy_bit(carried_int_b, f32.significand, FLOAT32_TRAILING, 1);
                }
            }
            else
            {
                f32.exponent = int_b.bit_cnt - 1 + FLOAT32_BIAS;
                copy_bit(carried_int_b, f32.significand, FLOAT32_TRAILING, 0);
            }
            // clean up
            revoke(int_sub_b);
            revoke(carried_int_b);
        }
        else
        {
            f32.exponent = int_b.bit_cnt - 1 + FLOAT32_BIAS;
            copy_bit(int_b, f32.significand, FLOAT32_TRAILING, int_b.bit_cnt - 1 - FLOAT32_TRAILING);
        }
    }
    else if (FLOAT32_TRAILING + 1 >= int_b.bit_cnt && FLOAT32_TRAILING + 1 < total_bit_cnt)
    {
        u_int8_t carry = *(total_b.bits + total_bit_cnt - FLOAT32_TRAILING - 2);
        if (carry)
        {
            Binary carry_bit = {.bit_cnt = 1, .bits = (u_int8_t[1]){1}};
            Binary total_sub_b = {.bit_cnt = FLOAT32_TRAILING + 1, .bits = malloc(sizeof(u_int8_t) * (FLOAT32_TRAILING + 1))};
            reset_binary(total_sub_b);
            copy_bit(total_b, total_sub_b, FLOAT32_TRAILING, total_b.bit_cnt - FLOAT32_TRAILING - 1);
            Binary carried_sub_b = plus_binary(carry_bit, total_sub_b);
            if (carried_sub_b.bit_cnt > total_sub_b.bit_cnt)
            {
                f32.exponent = int_b.bit_cnt + FLOAT32_BIAS;
                copy_bit(carried_sub_b, f32.significand, FLOAT32_TRAILING, 1);
            }
            else
            {
                f32.exponent = int_b.bit_cnt - 1 + FLOAT32_BIAS;
                copy_bit(carried_sub_b, f32.significand, FLOAT32_TRAILING, 0);
            }
            // clean up
            revoke(total_sub_b);
            revoke(carried_sub_b);
        }
        else
        {
            f32.exponent = int_b.bit_cnt - 1 + FLOAT32_BIAS;
            copy_bit(total_b, f32.significand, FLOAT32_TRAILING, total_b.bit_cnt - FLOAT32_TRAILING - 1);
        }
    }
    else if (FLOAT32_TRAILING + 1 >= total_bit_cnt)
    {
        f32.exponent = int_b.bit_cnt - 1 + FLOAT32_BIAS;
        for (int i = 0; i < FLOAT32_TRAILING + 1 - total_b.bit_cnt; i++)
        {
            *(f32.significand.bits + i) = 0;
        }
        for (int i = 0; i < total_b.bit_cnt - 1; i++)
        {
            *(f32.significand.bits + i + FLOAT32_TRAILING + 1 - total_b.bit_cnt) = *(total_b.bits + i);
        }
    }
    // clean up
    revoke(int_b);
    revoke(dot_b);
    revoke(total_b);
    return f32;
}

// dont call it more than once
void revoke(Binary b)
{
    free(b.bits);
}

#include<stdint.h>
#include<stdbool.h>

#define INFTY '∞'
#define sNaN "sNaN"
#define qNaN "qNaN"

#define FLOAT16_PRECISION 11

#define ROUND_TIES_TO_EVEN 0
#define ROUND_TIES_TO_AWAY 1
#define ROUND_TO_POSITIVE 2
#define ROUND_TO_ZERO 3
#define ROUND_TO_NEGATIVE 4
#define FLOAT16_BIAS 15
#define FLOAT16_EMAX 15
#define FLOAT16_TRAILING 10

#define SIGN_PLUS '+'
#define SIGN_MINUS '-'
#define DOT '.'
#define EXPONENT_UPPER 'E'
#define EXPONENT_LOWER 'e'
#define CHAR_END '\0'

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// big endian
typedef struct {
    u_int8_t *bits;
    u_int32_t bit_cnt;
    u_int8_t dot;
} Binary;

struct Float16 {
    u_int8_t sign;
    u_int32_t exponent;
    Binary significand;
};

// [+-]?(0|[1, 9][0, 9]*)?[.][0, 9]+([eE][+-]?[0, 9]+)?
struct Float16Decimal {
    u_int8_t sign;
    u_int32_t int_cnt;
    u_int32_t dec_cnt;
    // exponent part, do not modify, used for free memory
    u_int8_t *exponents;
    u_int32_t exp_cnt;
    u_int8_t exp_sign;
    // numbers including integers and decimals, dont modify
    u_int8_t *numbers;
};

void show_float(struct Float16 f);

void parse_float(char *chars);

bool is_normal(struct Float16 f);
bool is_subnormal(struct Float16 f);

Binary plus_binary(Binary b1, Binary b2);
Binary multiply_binary(Binary b1, Binary b2);
Binary convert2binary(u_int8_t *ns, u_int32_t len);
Binary decimal2binary(u_int8_t *ns, u_int32_t len, u_int32_t precision);
void print_binary(Binary b);

void revoke(Binary b);

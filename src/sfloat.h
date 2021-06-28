#include<stdint.h>
#include<stdbool.h>
#include <sys/types.h>

#define INFTY '∞'
#define sNaN "sNaN"
#define qNaN "qNaN"

#define ROUND_TIES_TO_EVEN 0
#define ROUND_TIES_TO_AWAY 1
#define ROUND_TO_POSITIVE 2
#define ROUND_TO_ZERO 3
#define ROUND_TO_NEGATIVE 4
#define FLOAT16_BIAS 15
#define FLOAT16_EMAX 15
#define FLOAT16_PRECISION 11
#define FLOAT16_TRAILING 10
#define FLOAT32_BIAS 127
#define FLOAT32_EMAX 127
#define FLOAT32_TRAILING 23
#define FLOAT32_PRECISION 24

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
    u_int32_t leading_zeros;
} Binary;

struct Float16 {
    u_int8_t sign;
    u_int32_t exponent;
    Binary significand;
};

typedef struct {
    u_int8_t sign;
    u_int32_t exponent;
    Binary significand;
    char* raw;
} Float32;

// [+-]?(0|[1, 9][0, 9]*)?[.][0, 9]+([eE][+-]?[0, 9]+)?
struct FloatDecimal {
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

// float32
Float32 parse_float32(char *chars);
void print_float32(Float32 f);

bool is_normal(struct Float16 f);
bool is_subnormal(struct Float16 f);

// binary related
Binary plus_binary(Binary b1, Binary b2);
Binary multiply_binary(Binary b1, Binary b2);
Binary convert2binary(u_int8_t *ns, u_int32_t len);
Binary decimal2binary(u_int8_t *ns, u_int32_t len, u_int32_t precision, u_int32_t max_zeros);
void print_binary(Binary b);
void reset_binary(Binary b);
void copy_bit(Binary from, Binary dest, u_int32_t len, u_int32_t offset);

// memory related
void revoke(Binary b);

/**
 * @brief homogeneous general computational operations
 * 
 */

Float32 round_to_integral_tie_to_even(Float32);
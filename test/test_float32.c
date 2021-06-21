#include<stdio.h>

#include "../src/sfloat.h"


int main()
{
    char *cs1 = "2";
    char *cs2 = "2.";
    char *cs4 = "+2";
    char *cs3 = "-2";
    char *cs5 = "+2.";
    char *cs6 = "2.5";
    char *cs7 = "+2.5";
    char *cs8 = "-2.5";
    char *cs9 = ".5";
    char *cs10 = "+.5";
    char *cs11 = "-.5";
    char *cs12 = "1.5e10";
    char *cs13 = "+1.5e+10";
    char *cs14 = "-1.5e-10";
    char *cs15 = "1.5E10";
    char *cs16 = "+1.5E+10";
    char *cs17 = "-1.5E-10";
    char *cs18 = "-12312.1237701e-6";
    char *cs19 = "1e10";
    char *cs20 = "0.3";
    char *cs21 = "2.3";
    char *cs22 = "+2.33";
    char *cs23 = "2.999999999999";
    char *cs24 = "1e24";
    char *cs25 = "0";
    char *cs26 = "1.1754943508222875e-38";  // exactly normal
    char *cs27 = "1.1754943508222875e-39"; // subnormal
    char *cs28 = "1e50"; // positive infty
    char *cs29 = "-1e50"; // negative infty
    char *cs30 = "3.4028234664e38"; // max normal number
    // parse_float32(cs1);
    // parse_float32(cs2);
    // parse_float32(cs3);
    // parse_float32(cs4);
    // parse_float32(cs5);
    parse_float32(cs6);
    parse_float32(cs7);
    // parse_float32(cs8);
    // parse_float32(cs9);
    // parse_float32(cs10);
    // parse_float32(cs11);
    // parse_float32(cs12);
    // parse_float32(cs13);
    // parse_float32(cs14);
    // parse_float32(cs15);
    // parse_float32(cs16);
    // parse_float32(cs17);
    // parse_float32(cs18);
    // parse_float32(cs19);
    // parse_float32(cs20);
    parse_float32(cs21);
    parse_float32(cs22);
    parse_float32(cs23);
    parse_float32(cs24);
    parse_float32(cs25);
    parse_float32(cs26);
    parse_float32(cs27);
    parse_float32(cs28);
    parse_float32(cs29);
    parse_float32(cs30);
    return 0;
}

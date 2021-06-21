#include<stdio.h>

#include "sfloat.h"


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
    parse_float(cs1);
    parse_float(cs2);
    parse_float(cs3);
    parse_float(cs4);
    parse_float(cs5);
    parse_float(cs6);
    parse_float(cs7);
    parse_float(cs8);
    parse_float(cs9);
    parse_float(cs10);
    parse_float(cs11);
    parse_float(cs12);
    parse_float(cs13);
    parse_float(cs14);
    parse_float(cs15);
    parse_float(cs16);
    parse_float(cs17);
    parse_float(cs18);
    parse_float(cs19);
    return 0;
}
#ifndef UTILS_H
#define UTILS_H

#include "Main.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

void strnset(char *str, int ch, int length);

float deg2radf(float deg);

bool anchorsearch(byte *str1, byte *str2, int len);

// 0: equal, negative: str1 < str2, positive: str1 > str2
int compare(byte *str1, int len1, byte *str2, int len2);

void copybytes(byte *src, byte *dst, int length);
void copybytes0(byte *src, byte *dst, int length);

void pause();

#endif
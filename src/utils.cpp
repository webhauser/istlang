#include "utils.h"

void strnset(char *str, int ch, int length) {
	while(length-- > 0) {
		*str++ = ch;
	}
}

float deg2radf(float deg) {return deg * M_PI / 180.0f;}	

bool anchorsearch(byte *str1, byte *str2, int length) {
	int x;
	for(x=0; x<length; x++) {
		if(*str1++ != *str2++)
			return false;
	}
	return true;
}

int compare(byte *str1, int len1, byte *str2, int len2) {
	int x;
	for(x=0; x<len1 && x<len2 ;x++) {
		if(str1[x] != str2[x])
			return str1[x] < str2[x] ? -1 : 1;
	}
	if(len1==len2) return 0;
	return (len1 < len2 ? -1 : 1);
}

void copybytes(byte *src, byte *dst, int length) {
	while(length-- > 0) {
		*dst++ = *src++;
	}
}

void copybytes0(byte *src, byte *dst, int length) {
	copybytes(src,dst,length);
	dst[length] = 0;
}

void pause() {
	printf("Press enter to continue...\n");
	fgetc(stdin);
}

/* EOF */
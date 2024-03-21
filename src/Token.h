#ifndef Token_H
#define Token_H

#include "Main.h"
#include "Error.h"

struct Token {
    enum LexToken {
		LEX_EOF,
		LEX_ERROR,
		LEX_INT, LEX_REAL,
		LEX_STRING,        LEX_HEXSTRING, LEX_STRING85,
		LEX_NAME_IE,       LEX_NAME_LIT,  LEX_NAME,
		LEX_ARRAY='[',     LEX_ARRAY_END=']',
		LEX_ARRAY_EXE='{', LEX_ARRAY_EXE_END='}',
		LEX_DICT=0x3C3C,   LEX_DICT_END=0x3E3E
	} token_type;

	union {
		int intValue;
		float realValue;
		char strValue[TOKEN_BUFLEN];
    } uni;

	Token() {token_type=LEX_ERROR;uni.strValue[0]='\0';}
    Token(LexToken type) {token_type=type;}
    Token(LexToken type, int i) {token_type=type;uni.intValue=i;}
    Token(LexToken type, float x) {token_type=type;uni.realValue=x;}
    Token(LexToken type, char *s) {
		token_type=type;
		if(strlen(s) >= TOKEN_BUFLEN) throw Error::LimitCheck();
		strcpy(uni.strValue,s);
	}
    inline bool eof(){return token_type==LexToken::LEX_EOF;}
    void print(const char *title) {
		printf(title);
		switch(token_type) {
			case Token::LEX_ERROR:			printf("ERROR");break;
			case Token::LEX_EOF:			printf("EOF");  break;
			case Token::LEX_REAL:			printf("REAL %.4f",uni.realValue);break;
			case Token::LEX_INT:			printf("INT %d",uni.intValue);break;
			case Token::LEX_HEXSTRING:		printf("HEXSTRING %s",uni.strValue);break;
			case Token::LEX_STRING85:		printf("STRING85 %s",uni.strValue);break;
			case Token::LEX_STRING:			printf("STRING %s",uni.strValue);break;
			case Token::LEX_NAME:			printf("NAME %s",uni.strValue);break;
			case Token::LEX_NAME_IE:		printf("IENAME %s",uni.strValue);break;
			case Token::LEX_NAME_LIT:		printf("NAME LITERAL %s",uni.strValue);break;
			case Token::LEX_DICT:			printf("DICTIONARY");break;
			case Token::LEX_DICT_END:		printf("END DICTIONARY");break;
			case Token::LEX_ARRAY:			printf("ARRAY [");break;
			case Token::LEX_ARRAY_EXE:		printf("PROC {");break;
			case Token::LEX_ARRAY_EXE_END:	printf("END-PROC }");break;
			case Token::LEX_ARRAY_END:		printf("END-ARRAY ]");break;
			default:				    	printf("INVALID TOKEN: %d", token_type);
		}
		printf("\n");
	}
};

#endif
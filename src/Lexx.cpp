#include "Lexx.h"

Lexx::Lexx() {
	total = 0;	
	stream = new InputStream();
	buffer = new char[TOKEN_BUFLEN*2];
	lexch = 0;
}

Lexx::Lexx(char *script) {
	total = 0;	
	stream = new InputStream(script);
	buffer = new char[TOKEN_BUFLEN*2];
	lexch = stream->getNextChar();//The pre pread character!
}

Lexx::Lexx(char *fname, const char *mode) {
	total = 0;
	stream = new InputStream(fname,mode);
	buffer = new char[TOKEN_BUFLEN*2];
	lexch = stream->getNextChar();//The pre pread character!
}

Lexx::~Lexx() { 
	fprintf(stderr, "DEBUG: ~Lexx()\n");
	if(buffer) {
		delete [] buffer;
		buffer = 0;
	}
	if(stream) {
		delete stream;
		stream = 0;
	}
	fprintf(stderr, "DEBUG: ~Lexx(OK)\n");
}

void Lexx::SkipWhiteSpace() {
	do {
		while(isWhiteSpace(lexch))
			lexch=stream->getNextChar();

		if(lexch=='%') {	// Comment
            do lexch=stream->getNextChar();
			while(lexch!=ISEOF && lexch!='\n');
		}
	} while (lexch=='%' || isWhiteSpace(lexch));
}

/**
Token::LexToken Lexx::getNumberValue(Token& token) {
	int intValue=0, sign=+1;
	Token::LexToken ret;	
	if (lexch=='-' || lexch=='+') {
		if (lexch=='+') sign=+1; else sign=-1;
		lexch=stream->getNextChar();
	}
	while (lexch>='0' && lexch<='9') {
		intValue=intValue*10+lexch-'0';
		lexch=stream->getNextChar();
	}
	if (lexch=='.')	{
		lexch=stream->getNextChar();
		float power=1.0;
		float realValue=(float)intValue;
		while (lexch>='0' && lexch<='9') {
			realValue=realValue*10.0+lexch-'0';
			lexch=stream->getNextChar();
			power*=10.0;
		}
		realValue = sign*realValue / power;        
        ret=Token::LEX_REAL;
		token=Token(ret, realValue);
	} else {
		intValue*=sign;
        ret=Token::LEX_INT;
		token=Token(ret, intValue);
	}
    return ret;
} */

bool Lexx::isRadixNumber(char *p, int *intVal) {	
int base,num;

	// max 2 decimal digits base
	if(isDigit(*p)) {
		base=*p++ - '0';
		if(isDigit(*p))
			base=base*10 + *p++ - '0';
	} 
	else return false;	
	if(*p!='#' || base<2 || base>36)
		return false;
	
	// process radix digits
	char *s = ++p;
	while(isBaseDigit(base,*p))
		p++;
	
	if(isLexSeparator0(*p)) {
		// The radix number has correct syntax
		for(num = 0; s < p; s++) {
			// @TODO: if(overflow) throw Error::LimitCheck();
			num=num*base + getBaseDigitVal(*s);			
		}
		*intVal = num;
		return true;
	}
	return false;
}

bool Lexx::isSignedInteger(char *p, int *intVal) {
int num, sign=+1;
	
	if (*p=='-' || *p=='+') {
		if (*p=='+') sign=+1; else sign=-1;
		p++;
	}
	for(num=0; isDigit(*p); p++) {
		// @TODO: if(overflow) throw Error::LimitCheck();
		num=num*10 + *p-'0';
	}
	if(isLexSeparator0(*p)) {
		*intVal = num * sign;
		return true;
	}
	return false;
}

bool Lexx::isFloatDefault(char *p, float *floatVal) {
float fval, power; int sign=+1; 
	
	if (*p=='-' || *p=='+') {
		if (*p=='+') sign=+1; else sign=-1;
		p++;
	}
	for(fval=0.0f; isDigit(*p); p++) {
		fval=fval*10 + *p-'0';	// @TODO: if(overflow) throw Error::LimitCheck();
	}
	if (*p=='.') {
		p++;		
		for(power=1.0f; isDigit(*p); p++) {
			fval=fval*10.0f + *p-'0';	// @TODO: if(overflow) throw Error::LimitCheck();
			power*=10.0f;
		}
		fval = sign*fval / power;
	}
	if(isLexSeparator0(*p)) {
		*floatVal = fval;
		return true;
	}
	return false;
}

bool Lexx::isFloatScientific(char *p, float *floatVal) {
	return false;
}

int Lexx::getOctal888() {
	int maxd=3, num=0;
	while (maxd-->0 && (lexch>='0' && lexch<='7')) {
		num=num*8+lexch-'0';
		lexch=stream->getNextChar();
	}
	stream->ungetNextChar(lexch);	// Azert kell, hogy majd a kov hivas beolvashassa..
	return num;
}

void Lexx::getNameValue(char *buf,int bufsize) {
	do {
		if(--bufsize > 0) 
			*buf++=lexch;
		else throw Error::LimitCheck();		
		lexch=stream->getNextChar();
	} while (lexch!=ISEOF && !isLexSeparator(lexch));
	*buf='\0';
}

void Lexx::getStringValue(char *buf,int bufsize) {
	char *s=buf;
	int level=0;
	bool skip=false;
	while (lexch!=ISEOF) {
		switch (lexch) {
			case '\\':
				lexch=stream->getNextChar();
				switch(lexch) {
					case '\n': 
					case '\r': skip=true; break;
					case 'n': lexch='\n'; break;
					case 'r': lexch='\r'; break;
					case 't': lexch='\t'; break;
					case 'b': lexch='\b'; break;
					case 'f': lexch='\f'; break;
					case '\\':lexch='\\'; break;
					case '(': lexch='(';  break;
					case ')': lexch=')';  break;			
					default:
						skip=true;
						// Oktalis beagyazott karakter
						if(lexch>='0' && lexch<='7') {
							if(s<buf+bufsize-1) *s++ = getOctal888(); else throw Error::LimitCheck();
						} else stream->ungetNextChar(lexch);
						// igy is ugyis ungetch lesz..de minek?
						// mi lesz ezutan a lexch sorsa? beolvasodik!
						// 2023.03.28 -Azert kell mert getNextChar lesz a next hivas!
					}
				break;
			case '(': ++level; break;
			case ')': 
				if(--level<0) {
					lexch=stream->getNextChar();					
					*s='\0'; return; // Don't forget to close the string!
				}
				break;
		}
		if (!skip) {
			if(s<buf+bufsize-1) *s++=lexch; else throw Error::LimitCheck();
		} else skip=false;
		lexch=stream->getNextChar();
	}	
	*s='\0'; // Don't forget to close the string!
}

/* <~ Ascii base-85 string literal ~> */
void Lexx::getString85Value(char *buf, int bufsize) {
	char *s=buf;
	int prech=stream->getNextChar();
	if(prech!=ISEOF) {
		lexch=stream->getNextChar();
		while(lexch!=ISEOF && (prech!='~' || lexch!='>')) {
			if(!isWhiteSpace(prech)) {
				if(s<buf+bufsize-1) *s++=prech; else throw Error::LimitCheck();
			}
			prech=lexch;
			lexch=stream->getNextChar();
		}
		if(lexch!=ISEOF) lexch=stream->getNextChar();
	}
	*s='\0';
}

int Lexx::getHexValue(char *src) {
	int hi=src[0], lo=src[1];
	if(isDigit(lo)) lo-='0'; else
	if(isHexDigitLo(lo)) lo-='a'; else
	if(isHexDigitHi(lo)) lo-='A';
	if(isDigit(hi)) hi-='0'; else
	if(isHexDigitLo(hi)) hi-='a'; else
	if(isHexDigitHi(hi)) hi-='A';	
	return 16*hi+lo;
}

/** White-space chars are ignored. If the hexstring contains chars outside the allowed, 
syntaxerror occurs. if there is an odd number of digits—the final digit is assumed to be 0 */
void Lexx::getHexStringValue(char *startbuf, int bufsize, int *retlen) {
	char *buf = startbuf;
	while (lexch!=ISEOF && lexch!='>') {
		if(!isWhiteSpace(lexch)) {
			if (isDigit(lexch) || isHexDigit(lexch)) {
				if(--bufsize > 0) *buf++=lexch;	else throw Error::LimitCheck();
			}
			else throw Error::SyntaxError();
		}	
		lexch=stream->getNextChar();
	}
	if(lexch!=ISEOF) lexch=stream->getNextChar();
	*retlen = buf - startbuf;
	if(*retlen & 1) {
		*buf = '0';
		(*retlen)++;
	}
}

void Lexx::putHexStringValue(byte *dst, char *src, int length) {
	while(length > 0) {
		*dst++ = getHexValue(src);
		src+=2;
		length-=2;
	}
	*dst = '\0';
}

/* @TODO: SyntaxError when encountering ~> >> ) */
Token::LexToken Lexx::getToken(Token& token) {
	int i; float x; bool slash;
	char *s=token.uni.strValue;
	++total;//Ervenyes, hacsak nem azonnal EOF
	SkipWhiteSpace();
    switch (lexch) {
    case ISEOF: return token.token_type=Token::LEX_EOF;
    case '[': *s++=lexch;*s='\0';lexch=stream->getNextChar();return token.token_type=Token::LEX_ARRAY;
    case ']': *s++=lexch;*s='\0';lexch=stream->getNextChar();return token.token_type=Token::LEX_ARRAY_END;
    case '{': *s++=lexch;*s='\0';lexch=stream->getNextChar();return token.token_type=Token::LEX_ARRAY_EXE;
    case '}': *s++=lexch;*s='\0';lexch=stream->getNextChar();return token.token_type=Token::LEX_ARRAY_EXE_END;
	case ')': throw Error::SyntaxError(); 
    case '(': // String
		lexch=stream->getNextChar();
		getStringValue(s, TOKEN_BUFLEN);
		return token.token_type=Token::LEX_STRING;
    case '<':
		lexch=stream->getNextChar();
		switch(lexch) {
			case '<':	// Dictionary
				*s++='<';
				*s++='<';
				*s='\0';
				lexch=stream->getNextChar();
				return token.token_type=Token::LEX_DICT;
			case '~':	// Ascii base-85 string literal
				getString85Value(s, TOKEN_BUFLEN);
				return token.token_type=Token::LEX_STRING85;
			default: {	// Hex string
				int length;
				//getHexStringValue(s, TOKEN_BUFLEN);
				getHexStringValue(buffer, 2*TOKEN_BUFLEN, &length);
				putHexStringValue((byte *)s, buffer, length);
				return token.token_type=Token::LEX_HEXSTRING;
			}
		}
	case '>':
		lexch=stream->getNextChar();
		if (lexch=='>')	{
			lexch=stream->getNextChar();
			*s++='>';
			*s++='>';
			*s='\0';
			return token.token_type=Token::LEX_DICT_END;
		}
		throw Error::SyntaxError();
	default:
		slash = (lexch=='/');
		if (slash)	{ // Literal name			
			lexch=stream->getNextChar();
			if (lexch=='/')	{ // Immediate evaluated name
				getNameValue(s, TOKEN_BUFLEN);
				for(int i=0;i<(int)strlen(s);i++) s[i]=s[i+1];
				return token.token_type=Token::LEX_NAME_IE;
			}
			else if(lexch==ISEOF) throw Error::SyntaxError();
		}
		getNameValue(s, TOKEN_BUFLEN);
		if (slash) {
			return token.token_type=Token::LEX_NAME_LIT;
		}
		// Check for Signed integers, Real numbers, Radix numbers
		if(isRadixNumber(s, &i) || isSignedInteger(s, &i)) {
			token.uni.intValue=i;
			return token.token_type=Token::LEX_INT;
		}
		else if(isFloatDefault(s, &x) || isFloatScientific(s, &x)) {
			token.uni.realValue=x;
			return token.token_type=Token::LEX_REAL;
		}
		return token.token_type=Token::LEX_NAME;
    }
}

int Lexx::getRest(int *plength) {
	if(lexch!=ISEOF) stream->ungetNextChar(lexch);
	return stream->getBufferPos(plength);
}

void Lexx::initFile(char *filename) {
}

void Lexx::initString(char *str, int len) {
	stream->setBuffer(str, len);
	lexch = stream->getNextChar();//The pre pread character!
}

bool Lexx::getStringBuffer(byte *buff, int maxlength, int *retlength) {
	*retlength=0;
	while (maxlength-- > 0) {
		if(lexch==ISEOF) return false;
		*buff++ = lexch;
		(*retlength)++;
		lexch=stream->getNextChar();		
	}
	return true;
}

// -1=filled before new line, 1=normal, 0=eof before new line
int Lexx::getLineBuffer(byte *buff, int maxlength, int *retlength) {
	*retlength=0;
	while (maxlength-- > 0) {
		switch(lexch) {
			case ISEOF: return 0;
			case '\n':
			case '\r':	// Skip newline if first
				lexch=stream->getNextChar();
				if(*retlength > 0) return 1;
				break;
			default:
				*buff++ = lexch;
				(*retlength)++;
				lexch=stream->getNextChar();
				break;
		}
	}
	return -1;
}

int Lexx::getNextChar(){ 
	int ch=lexch; 
	
	if(lexch!=ISEOF)
		lexch=stream->getNextChar(); 

	return ch;
}

/* EOF */
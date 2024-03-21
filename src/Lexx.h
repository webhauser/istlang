#ifndef Lexx_H
#define Lexx_H

#include "InputStream.h"
#include "Token.h"

/*** Ezt az osztalyt eredetileg azert csinaltam, hogy a file vagy string 
elemenkenti bemenetet int InputStream::getNextChar() azonos modon, 
lexikalis tokenenkent egymas utan lehessen beolvasni.

Ezutan felhigult, mert a FileObject erre mutat (Lexx *lexp) es a metodusait 
kellett atvezetni az InputStream osztalyba, ahol a FILE *fp van. ***/

class Lexx {
public:

	Lexx();
    Lexx(char *script);
    Lexx(char *fname, const char *mode);
    ~Lexx();

	void initFile(char *filename);
	void initString(char *str, int length);	
	Token::LexToken getToken(Token& token);
	int getRest(int *length);
	bool getStringBuffer(byte *buff, int maxlength, int *retlength);	
	int  getLineBuffer(byte *buff, int maxlength, int *retlength); // -1=filled before new line, 1=normal, 0=eof before new line	
	int  getNextChar();
	
	inline int  getBytesAvailable() {return stream->getBytesAvailable();}
	inline int  getFilePos() {return stream->getFilePos();}
	inline void setFilePos(int pos) {stream->setFilePos(pos);}
	inline void resetBuffer() {stream->drop();}
	inline void flushFile() {stream->flush();}

private:
	int total;
   	int lexch;		// preread char
	char *buffer;	// dynamic hex string buffer size is 2*TOKEN_BUFLEN
	InputStream *stream;

    /* private methods */

	inline bool isDigit(int c)     {return c>='0' && c<='9';}
	inline bool isHexDigitLo(int c){return c>='a' && c<='f';}
	inline bool isHexDigitHi(int c){return c>='A' && c<='F';}
	inline bool isHexDigit(int c)  {return isHexDigitLo(c) || isHexDigitHi(c);}
	inline bool isLetterBig(int c) {return c>='A' && c<='Z';}
	inline bool isLetterLit(int c) {return c>='a' && c<='z';}
	inline bool isValidAsciiChar(int c) {return c>' ' && c<128;}
	//bool isWhiteSpace0(int c) {return (c==0 || c=='\t' || c=='\f' || c=='\n' || c=='\r' || c==' ');}
	inline bool isWhiteSpace(int c) {return (c=='\t' || c=='\f' || c=='\n' || c=='\r' || c==' ');}    
	inline bool isLexSeparator(int c) {return isWhiteSpace(c) || strchr("/()<>[]{}%", c)!=0;}
	inline bool isLexSeparator0(int c){return c=='\0' || isLexSeparator(c);}
    void SkipWhiteSpace();
    //Token::LexToken getNumberValue(Token &);
	
	/**
	// base(0..36)
	// base(10) [0..9]
	// base(16) [0..9] - [A..Fa..f]
	// base(32) [0..9] - [A..Za..z] */
	bool isBaseDigit(int b, int c) {
		if(b < 11) return isDigit(c);
		return isDigit(c) || (c>='A' && c<='A'+b-11) || (c>='a' && c<='a'+b-11);
	}
	int getBaseDigitVal(int c) {
		if(isDigit(c)) return c-'0';
		if(isLetterBig(c)) return c-'A'+10;
		if(isLetterLit(c)) return c-'a'+10;
		/* notreached */ return -1;
	}
	// base#digits
	bool isRadixNumber(char *str, int *intVal);
	bool isSignedInteger(char *str, int *intVal);
	bool isFloatDefault(char *str, float *floatVal);
	bool isFloatScientific(char *str, float *floatVal);

    int  getOctal888();
    void getNameValue(char *buf,int bufsize);
    void getStringValue(char *buf,int bufsize);
    void getString85Value(char *buf,int bufsize);
	int  getHexValue(char *src);
    void getHexStringValue(char *buf,int bufsize,int *retlength);
	void putHexStringValue(byte *dst, char *src, int srclen);
};

#endif
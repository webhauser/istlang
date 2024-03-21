#ifndef InputStream_H
#define InputStream_H

#include "Main.h" 
#include "Error.h"

const int ISEOF = 0xCAFEFFFF;

class InputStream {
public:

	enum StreamType { STRING, STDIN, STREAM, SOCKET } streamtype;

	InputStream();
    InputStream(char *script);
	InputStream(char *fname, const char *mode);
	~InputStream();

	int getNextChar();//EOF
	void ungetNextChar(int c);
    void drop();//follow from the next line
	
	int  getBufferPos(int *length);
	void setBuffer(char *str, int length);

	// @TODO: remove later
	void flush();
	int  getBytesAvailable();	
	int getFilePos();
	void setFilePos(int pos);

private:	

	FILE *file;
	char *buffer;	// sor merete
	int pos,length;
	bool alloc;
	// ez igazabol most InputStream
	char mode;	// 'i','o'
};

#endif
#ifndef FileHandler_H
#define FileHandler_H

#include "Lexx.h"
#include "Token.h"

class FileHandler {
public:

	FileHandler();
	~FileHandler();
	
	int  getBytesAvailable();
	int  getFilePos();
	void setFilePos(int pos);
	void resetBuffer();
	void flushFile();
	
private:
	Lexx *lexp;
};

#endif
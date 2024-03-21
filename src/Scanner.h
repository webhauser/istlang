#ifndef Scanner_H
#define Scanner_H

#include "Token.h"
#include "Object.h"

namespace Scanner {
	
	extern Lexx lexx;
	
	bool GetStreamToken(FileObject *file, Token *token);
	bool GetStreamString(FileObject *file, StringObject *dest, int *bytesread);
	bool GetStreamLine(FileObject *file, StringObject *dest, int *bytesread);
	bool GetStringToken(StringObject *obj, StringObject *post, Token *token);
};

#endif
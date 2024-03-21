#include "Scanner.h"

namespace Scanner {
	
	Lexx lexx;	// GetStringToken
	
	bool GetStreamToken(FileObject *file, Token *token) {
		return file->getToken(*token);
	}
	
	bool GetStreamString(FileObject *file, StringObject *dest, int *bytesread) {
		return file->getString(dest->getStringStartPtr(), dest->length, bytesread);
	}
	
	bool GetStreamLine(FileObject *file, StringObject *dest, int *bytesread) {
		return file->getLine(dest->getStringStartPtr(), dest->length, bytesread);
	}

	bool GetStringToken(StringObject *obj, StringObject *post, Token *token) {
		int length;
		lexx.initString((char *)obj->getStringStartPtr(), obj->length);
		Token::LexToken ret = lexx.getToken(*token);
		post->token = obj->token;
		post->start = obj->start + lexx.getRest(&length);
		post->length = (word)length;
		post->what.memIndex = obj->what.memIndex;
		return ret!=Token::LEX_EOF;
	}
};
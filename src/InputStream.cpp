
/* 2007 (c) webHauser
 * Itt az lenne a lenyeg, hogy
 * a scanner / beolvaso rutinokat
 * egy helyen lehessen kezelni es hogy
 * tobb kulonfele bemenetrol fogadhasson
 * adatokat egyazon fuggveny, megnyitastol fuggoen.
 
 * TStream is good example of another type of class in this branch.
 * TStream is the base class type for stream objects that can read from or
 * write to various kinds of storage media, such as disk files, dynamic memory,
 * and so on.
 */

#include "InputStream.h"

InputStream::InputStream() {
	streamtype = STRING;
	pos = 0;
	length = 0;
	file = NULL;
	buffer = 0;
	alloc=false;
	mode='i';
}

InputStream::InputStream(char *script) {
	streamtype = STRING;
	pos = 0;
    if((length = strlen(script)) > STREAM_BUFSIZE-1)
		throw Error::LimitCheck();
	
	file = NULL;
	alloc = true;
    buffer = new char[length+1];	// buffer[0] .. buffer[length-1]
    strcpy(buffer,script);
	mode='i';
}

InputStream::InputStream(char *fname, const char *fmode) {
	if(fname) {
		streamtype = STREAM;
		switch(fmode[0]) {			
			case 'a':
			case 'w': mode='o'; break;
			case 'r': mode='i'; break;
			default : throw Error::IO();
		}
		if((file=fopen(fname, fmode))==NULL)
			throw Error::FileNotFound();
	}
	else {
		streamtype = STDIN;
		file=stdin;
		mode='i';
	}
	alloc = true;
    buffer = new char[STREAM_BUFSIZE];
    drop();//igy fgets lesz eloszor
}

InputStream::~InputStream() {
	fprintf(stderr, "DEBUG: ~InputStream()\n");
	if(buffer && alloc) {
		delete[] buffer;
		buffer = 0;
	}	
	if (streamtype==STREAM) {
		if(file) {
			fclose(file);
			file = 0;
		}
	}	
	fprintf(stderr, "DEBUG: ~InputStream(OK)\n");
}

void InputStream::drop() {
	length = 0;
    pos = 0;
}

int InputStream::getNextChar(void) {
	if(pos >= length) {
		if (streamtype==STDIN || streamtype==STREAM) {
			if((fgets(buffer, STREAM_BUFSIZE, file))==NULL)
				return ISEOF;

			length = strlen(buffer);
			pos = 0;
        }
		else if (streamtype==STRING)
        	return ISEOF;
	}
    return buffer[pos++];
}

void InputStream::ungetNextChar(int c) {
	if (pos-1 >= 0)
		buffer[--pos] = c;
}

void InputStream::setBuffer(char *str, int len) {
	pos=0;
	buffer = str;
	length = len;
}

/* char *InputStream::getBuffer(int *rest_length) {
	*rest_length = length - pos;
	return buffer + pos;
} */

int InputStream::getBufferPos(int *rest_length) {
	*rest_length = length - pos;
	return pos;
}

int InputStream::getBytesAvailable() {
	if(!file) throw Error::IO();
	long pos = ftell(file);
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, pos, SEEK_SET);
	return (int)(size - pos);
}

int InputStream::getFilePos() {
	if(!file) throw Error::IO();
	return (int)ftell(file);
}

void InputStream::setFilePos(int pos) {
	if(!file) throw Error::IO();
	fseek(file, pos, SEEK_SET);
}

void InputStream::flush() {
	if(!file) throw Error::IO();
	if(mode=='i') {
		while (fgetc(file) != EOF) ;
	}
	else fflush(file);

}
/* EOF */
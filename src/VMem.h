#ifndef VMem_H
#define VMem_H

#include "Main.h"
#include "Error.h"
#include "utils.h"

class Object;

struct VMEntry {
	
	enum { UNUSED=0xFFFF };	// VMDesc
	
	char ttype;	// 0 O S N
	byte level;	
	word count;
	word refcount;
	unsigned ts;		// timestamp
	union {
		byte *strValue;
		Object *objValue;
	} uni;
	
	VMEntry() { zero(); }
	VMEntry(char type, word length);
	
	inline bool isFree()   {return ttype==0;}
	void zero();
	int free();
};

class VMem {
private:

	VMEntry *vmem;
	int num_bytes, max_bytes;
	int num_elems, max_elems;
	
	VMDesc getFreeDesc();
	VMDesc getFreeDescCheck(int count, size_t bytes_need);	

	VMDesc FindName(byte *name, int length);
	VMDesc AllocName(byte *name, int length);
	
	void freedups(void *ptr);
	
public:

	VMem(int total_size, int total_bytes);
	~VMem();
	
	byte   *getStringPtr(word index);
	Object *getObjectPtr(word index);
	unsigned getTimeStamp(word index);
		
	VMDesc AllocNameGet(byte *name, int length);
	VMDesc AllocString(byte *name, int length);
	VMDesc AllocObject(int count);
	VMDesc GetSetString(byte *name);
	
	void FreeEntry(word index);	
	
	void print_free();
};

#endif
#include "VMem.h"
#include "Object.h"	/* resolve circular dependency */

/*** VMentry ***/

VMEntry::VMEntry(char type, word length) {
	ttype = type;
	level = 0;
	count = length;
	refcount = 0;
	ts = Timer::time();
}

int VMEntry::free() {
	int bytes = 0;	
	switch(ttype) {
		case 'O':
			if(count > 0) 
				 delete [] uni.objValue;
			else delete uni.objValue;
			bytes = count * (int)sizeof(Object);
			break;
		case 'N':
		case 'S':
			bytes = count;
			if(count > 0) delete [] uni.strValue;
			break;
	}
	zero();
	return bytes;
}

void VMEntry::zero() {
	ttype=0;
	level=0;
	count=0;
	refcount=0;
	ts=0;
	uni.strValue=0;
}

/*** VMem ***/

VMem::VMem(int total_size, int total_bytes) {
	num_elems = 0;
	num_bytes = 0;
	max_bytes = total_bytes;
	max_elems = total_size;
	vmem = new VMEntry[total_size];
	for(int ix=0; ix<max_elems; ix++) {
		vmem[ix].zero();
	}
}

VMem::~VMem() {
	fprintf(stderr, "DEBUG: ~VMem()\n");
	for(int ix=0; ix<max_elems; ix++) {
		FreeEntry(ix);
	}
	delete [] vmem;
}

void VMem::freedups(void *ptr) {
	for(int ix=0; ix<max_elems; ix++) {
		VMEntry *entry = &vmem[ix];
		switch(entry->ttype) {
			case 'O':
			case 'N':
			case 'S':
				if(ptr==(void *)entry->uni.strValue) {
					entry->zero();
					num_elems--;
				}
				break;
		}
	}
}

void VMem::FreeEntry(word index) {
	VMEntry *entry = &vmem[index];
	if(!entry->isFree() && entry->uni.strValue!=NULL) {
		fprintf(stderr, "DEBUG: FreeEntry(%d)\n", index);
		void *ptr = (void *)entry->uni.strValue;		
		num_bytes -= entry->free();
		num_elems--;
		freedups(ptr);
	}
}

VMDesc VMem::getFreeDesc() {
	for(int ix=0; ix<max_elems; ix++) {
		if(vmem[ix].isFree())
			return (VMDesc)ix;
	}
	return (VMDesc)VMEntry::UNUSED;
}

VMDesc VMem::getFreeDescCheck(int count, size_t bytes_need) {
	if(num_elems + count >= max_elems || num_bytes + (int)bytes_need * count > max_bytes) {
		fprintf(stderr, "DEBUG: VMem::getFreeDescCheck(count=%d, bytes_need=%d)\n", count, (int)bytes_need);
		fprintf(stderr, "DEBUG: VMem::num_elems=%d, num_bytes=%d, max_elems=%d, max_bytes=%d\n", num_elems, num_bytes, max_elems, max_bytes);
		throw Error::OutOfMemory();
	}
	num_elems += count;
	num_bytes += (int)bytes_need * count;
	return getFreeDesc();
}

VMDesc VMem::AllocObject(int count) {
	VMDesc ix = getFreeDescCheck(count, sizeof(Object));
	fprintf(stderr, "DEBUG: AllocObject(%d) : %d\n", count, ix);
	VMEntry *entry = &vmem[ix];	
	entry->ts = Timer::time();
	entry->count = count;
	entry->ttype = 'O';	
	if(count > 0) 
		entry->uni.objValue = new Object[count];
	else entry->uni.objValue = 0;
	return ix;
}

VMDesc VMem::AllocString(byte *name, int length) {
	VMDesc ix = getFreeDescCheck(1, length);
	fprintf(stderr, "DEBUG: AllocString(\"%s\",%d): %d\n", name ? (char *)name : "NULL", length, ix);
	VMEntry *entry = &vmem[ix];
	entry->ts = Timer::time();
	entry->count = length;
	entry->ttype = 'S';	
	if(length > 0) {
		entry->uni.strValue = new byte[length];
		byte *dst=entry->uni.strValue;
		if(name) copybytes(name, dst, length);
		else while(length-- > 0) *dst++ = 0;
	}
	else entry->uni.strValue = 0;
	return ix;
}

VMDesc VMem::AllocName(byte *name, int length) {
	VMDesc ix = getFreeDescCheck(1, length);
	fprintf(stderr, "DEBUG: AllocName(\"%s\",%d): %d\n", name ? (char *)name : "NULL", length, ix);
	VMEntry *entry = &vmem[ix];
	entry->ts = Timer::time();
	entry->count = length;
	entry->ttype = 'N';
	entry->uni.strValue = new byte[length];
	byte *dst=entry->uni.strValue;
	copybytes(name, dst, length);
	return ix;
}

VMDesc VMem::FindName(byte *name, int length) {
	for(int i=0; i<max_elems; i++) {
		if(vmem[i].ttype=='N' && !compare(vmem[i].uni.strValue, vmem[i].count, name, length))
			return (VMDesc)i;
	}
	return (VMDesc)VMEntry::UNUSED;
}

VMDesc VMem::AllocNameGet(byte *name, int length) {
	VMDesc index = FindName(name, length);	// GVmem
	if(index!=VMEntry::UNUSED)
		return index;
	return AllocName(name, length);
}

VMDesc VMem::GetSetString(byte *name) {
	VMDesc ix = FindName(name, strlen((char *)name));	// GVmem
	if(ix!=VMEntry::UNUSED)
		return ix;
	ix = getFreeDescCheck(1,0);
	VMEntry *entry = &vmem[ix];
	entry->ts = Timer::time();
	entry->count = 0;
	entry->ttype = 'S';
	entry->uni.strValue = name;
	return ix;
}

byte    *VMem::getStringPtr(word index) {return vmem[index].uni.strValue;}
Object  *VMem::getObjectPtr(word index) {return vmem[index].uni.objValue;}
unsigned VMem::getTimeStamp(word index) {return vmem[index].ts;}

void VMem::print_free() {
	printf("Free cells: %10d of %10d\n"
	       "Free bytes: %10d of %10d\n", num_elems, max_elems, num_bytes, max_bytes);
}

/* EOF */
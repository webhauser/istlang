#include "Object.h"
#include "SystemTable.h"

Object::Object() {
	make_null(this);
}

Object::~Object() {}

bool operator ==(const Object& a, const Object& b) {
	return( a.token==b.token && a.pad==b.pad && a.start==b.start && a.length==b.length && a.what.intValue==b.what.intValue );
}

bool operator !=(const Object& a, const Object& b) {
	return( a.token!=b.token || a.pad!=b.pad || a.start!=b.start || a.length!=b.length || a.what.intValue!=b.what.intValue );
}

int  Object::size(){return length;}
int  Object::GetType(void){return(token & (EXEC_BIT-1));}
bool Object::isBool() {return GetType()==TOK_BOOL;}
bool Object::isInt()  {return GetType()==TOK_INTEGER;}
bool Object::isReal() {return GetType()==TOK_REAL;} 
bool Object::isPositive() {if(isReal()) return what.realValue > 0.0f; return what.intValue > 0;}
bool Object::isNegative() {if(isReal()) return what.realValue < 0.0f; return what.intValue < 0;}
void Object::CheckExArray() {if(token!=EXEC_BIT+TOK_ARRAY) throw Error::TypeCheck();}
void Object::CheckNumber() {if(!isInt() && !isReal()) throw Error::TypeCheck();}
void Object::CheckType(int needType) {if(GetType()!=needType) throw Error::TypeCheck();}
void Object::CheckInt() {CheckType(TOK_INTEGER);}
void Object::CheckName() {CheckType(TOK_NAME);}
void Object::CheckString() {CheckType(TOK_STRING);}
void Object::CheckFile(int acc){CheckType(TOK_FILE); if(acc && pad!=acc) throw Error::InvalidAccess();}
bool Object::GetExecFlag(void) {return (token & EXEC_BIT)!=0;}
void Object::SetExecFlag(bool x){token = (token & (EXEC_BIT-1)) | ((byte)x<<7);}	

bool Object::isGreaterThan(Object *obj) {
	bool ret;
	if(isReal()) {
		float val = what.realValue;
		ret = (obj->isReal() ? val > obj->what.realValue : val > obj->what.intValue);
	}
	else {
		int val = what.intValue;
		ret = (obj->isReal() ? val > obj->what.realValue : val > obj->what.intValue);
	}
	return ret;
}

bool Object::isLessThan(Object *obj) {
	bool ret;
	if(isReal()) {
		float val = what.realValue;
		ret = (obj->isReal() ? val < obj->what.realValue : val < obj->what.intValue);
	}
	else {
		int val = what.intValue;
		ret = (obj->isReal() ? val < obj->what.realValue : val < obj->what.intValue);
	}
	return ret;		
}

void Object::Add(Object *obj) {
	if(isReal()) {
		if(obj->isReal()) what.realValue += obj->what.realValue; else what.realValue += obj->what.intValue;
	}
	else {
		if(obj->isReal()) what.intValue += obj->what.realValue; else what.intValue += obj->what.intValue;
	}
}

bool Object::isComposite() {
	switch(GetType()) {
		case TOK_NAME:
		case TOK_STRING:
		case TOK_ARRAY:
		case TOK_DICT:
		case TOK_FILE:
		case TOK_GSTATE:
		case TOK_SAVE:
			return true;
	}
	return false;
}

void Object::copy(Object *obj) {
	token=obj->token;
	pad=obj->pad;
	start=obj->start;
	length=obj->length;
	what.intValue=obj->what.intValue;
}

byte   *Object::getStringPtr()   {return GVmem.getStringPtr(what.memIndex);}
Object *Object::getObjectPtr()   {return GVmem.getObjectPtr(what.memIndex);}

byte   *Object::getStringStartPtr() {return getStringPtr() + start;}
Object *Object::getObjectStartPtr() {return getObjectPtr() + start;}

unsigned Object::getTimeStamp() { return GVmem.getTimeStamp(what.memIndex);}

const char *Object::strType() {
	static const char *types[] = {   // DEPENDENT OF TOK_XXX order!!!
		"nulltype",
		"integertype",
		"realtype",
		"nametype",
		"booleantype",
		"stringtype",
		"ienametype",	// @TODO: remove		
		"dicttype",
		"arraytype",
		"marktype",
		"operatortype",
		"filetype",
		"gstatetype",
		"savetype",
		"loopmark",
		"stopmark"		
	};
	if(token==TOK_ARRAY+EXEC_BIT) 
		return "packedarraytype";	
    const int tt=GetType();
    if(tt >= 0 && tt < (int)(sizeof(types)/sizeof(char *)))
		return types[tt];
	
    return "errortype";
}

void Object::print_type(Object *obj) {
	printf("--%s--", obj->strType());
}

void Object::print(Object *obj) {
	switch(obj->GetType()) {
	case TOK_NULLA: printf("null"); break;
	case TOK_BOOL: printf( obj->what.intValue ? "true" : "false"); break;
	case TOK_INTEGER: printf("%d", obj->what.intValue); break;
	case TOK_REAL: printf("%.6f", obj->what.realValue); break;
	case TOK_NAME: 
		if(!obj->GetExecFlag()) printf("/");
		printf("%.*s",obj->length, (char *)obj->getStringStartPtr()); 
		break;
	case TOK_ENAME: printf("//%.*s",obj->length, (char *)obj->getStringStartPtr()); break;
	case TOK_STRING: printf("(%.*s)",obj->length, (char *)obj->getStringStartPtr()); break;	
	case TOK_OPERATOR: printf("--%s--", SystemTable::sysnameindex[obj->what.intValue].name); break;
	case TOK_ARRAY: 
	case TOK_DICT:
	case TOK_MARK:
	case TOK_FILE:
	case TOK_GSTATE:
	case TOK_SAVE:
	case TOK_LOOPMARK: 
		print_type(obj);  
		break;
	default: printf("unknown"); break;
	}
}

void Object::free() {
	if(isComposite()) {
		GVmem.FreeEntry(what.memIndex);
	}
}

/* static */ 

void Object::make_null(Object *obj) {
   	obj->token=TOK_NULLA;
   	obj->pad=0;
	obj->start=0;
   	obj->length=0;
   	obj->what.intValue=0;
}

void Object::MakeMarkObject(Object *obj) {
	make_null(obj);
	obj->token=TOK_MARK;
}

void Object::MakeBoolObject(Object *obj, bool val) {
	make_null(obj);
	obj->token=TOK_BOOL;
	obj->what.intValue=(int)val;
}

void Object::MakeIntObject(Object  *obj, int val) {
	make_null(obj);
	obj->token=TOK_INTEGER;
	obj->what.intValue=val;
}

void Object::MakeRealObject(Object *obj, float val) {
	make_null(obj);
	obj->token=TOK_REAL;
	obj->what.realValue=val;
}

void Object::checkrange(int x, int length){if(x<0 || x>=length) throw Error::RangeCheck();}

void Object::copystring(Object *obj, char *src) {
	int length = strlen(src);
	if(obj->length >= length) {
		obj->length = length;
		copybytes((byte *)src, obj->getStringStartPtr(), length);
	}
	else throw Error::RangeCheck();
}

void Object::copystring(Object *obj, const char *str) {
	copystring(obj, (char *)str);
}

void Object::copystring(Object *obj, Object *frm) {
	int length = frm->length;
	if(obj->length >= length) {
		obj->length = length;
		copybytes(frm->getStringStartPtr(), obj->getStringStartPtr(), length);
	}
	else throw Error::RangeCheck();
}

/*** derived classes ***/

BoolObject::BoolObject(bool b) {token=TOK_BOOL;what.intValue=(int)b;}
IntegerObject::IntegerObject(int i) {token=TOK_INTEGER;what.intValue=i;}
RealObject::RealObject(float f) {token=TOK_REAL;what.realValue=f;}
MarkObject::MarkObject() {token=TOK_MARK;}

OperatorObject::OperatorObject(int x) {
	token=TOK_OPERATOR|EXEC_BIT;
	pad=0;
	start=0;
	length=-1;
	what.intValue=x;// Value is a system name index
}

NameObject::NameObject(char *name) {setNameObject(name,strlen(name));}
NameObject::NameObject(char *name, int length) {setNameObject(name,length);}
NameObject::NameObject(const char *name) {
	token=TOK_NAME;
	pad=0;
	start=0;
	length=strlen(name);
	what.memIndex=GVmem.GetSetString((byte *)name);
}

void NameObject::setNameObject(char *name, int len) {
	token=TOK_NAME;
	pad=0;
	start=0;
	length=len;
	what.memIndex=GVmem.AllocNameGet((byte *)name, len);
}

StringObject::StringObject() {
	token=TOK_STRING;
	pad=0;
	start=0;
	length=0;
	what.memIndex=VMEntry::UNUSED;
}
StringObject::StringObject(Object *obj) {copy(obj);}
StringObject::StringObject(char *name) {
	token=TOK_STRING;
	pad=0;
	start=0;
	length=strlen(name);
	what.memIndex=GVmem.AllocString((byte *)name, length);
}
StringObject::StringObject(int size) {
	if(size < 0) throw Error::RangeCheck();
	token=TOK_STRING;
	pad=0;
	start=0;
	length=size;
	what.memIndex=GVmem.AllocString(0, size);
}
char *StringObject::getString(char *buff, int buflen) {
	char *src = (char *)getStringStartPtr();
	char *dst = buff;
	for(int i=0; i<length && i<buflen-1; i++) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return buff;
}
int StringObject::strchr(int c) {
	byte *ptr = getStringStartPtr();
	for(int i=0; i<length; i++, ptr++) {
		if(c==*ptr) return i;
	}
	return -1;
}
int StringObject::get(int x){
	checkrange(x, length);
	return *(getStringStartPtr() + x);
}
void StringObject::put(int x, int c){
	checkrange(x, length);
	*(getStringStartPtr() + x)=c;
}
void StringObject::tailor() {
	start++;
	length--;
}

/*** ArrayObject ***/

ArrayObject::ArrayObject(int count) {
	if(count < 0) throw Error::RangeCheck();
	token=TOK_ARRAY;
	pad=0;
	start=0;
	length=count;
	what.memIndex=GVmem.AllocObject(count);
}
ArrayObject::ArrayObject( Object *obj) {copy(obj);}
ArrayObject::ArrayObject( Object *obj, bool execFlag ) {
	copy(obj);
	SetExecFlag(execFlag);
}
Object *ArrayObject::get(int x){
	checkrange(x,length);
	Object *array = getObjectStartPtr();
	return &array[x];
}
void ArrayObject::put(int x,Object* a) {
	checkrange(x,length); 
	*(getObjectStartPtr() + x) = *a;
}
void ArrayObject::tailor() {
	start++;
	length--;
}

/*** DictObject ***/

DictObject::DictObject(Object *obj){copy(obj);}
DictObject::DictObject(int size, Access access){
	if(size < 0) throw Error::RangeCheck();
	token=TOK_DICT;
	pad=access;
	start=0;
	length=size;
	what.memIndex=GVmem.AllocObject(2*size);
	clear();
}

void DictObject::clear() {
	Object *ptr=getObjectPtr();
	for(int j=0;j<length;j++) {
		make_null(ptr++);
		make_null(ptr++);
	}
}

Object *DictObject::lookup(char *name) {
	int namelen=strlen(name);
	Object *ptr=getObjectPtr();
	for(int j=0; j<length; j++, ptr+=2) {
		if(ptr->token!=TOK_NULLA && !compare((byte *)name, namelen, ptr->getStringStartPtr(), ptr->length))
			return ptr+1;
	}
	return 0;
}

Object *DictObject::index(Object *obj){
	Object *ptr=getObjectPtr();
	for(int j=0;j<length;j++, ptr+=2) {
		if(*ptr==*obj)
			return ptr+1;
	}
	return 0;
}

void DictObject::get(int x, Object *key, Object *val) {
	checkrange(x,length);
	Object *ptr=getObjectPtr() + 2*x;
	*key=*ptr++;
	*val=*ptr;
}

void DictObject::put(Object *key, Object *val) {
	if(key->GetType()!=TOK_NAME) throw Error::TypeCheck();
	Object *ptr=index(key);
	if(!ptr) { // Search for an empty slot
		ptr=getObjectPtr();
		for (int j=0; j<length; j++, ptr+=2) {
			if(ptr->token==TOK_NULLA) {
				*ptr++ = *key;
				*ptr = *val;
				return;
			}
		}
		throw Error::DictFull();
	}
	else *ptr=*val;
}

void DictObject::putsys(const char *name, Object *val) {
	Object obj=NameObject(name);
	obj.pad=Access::ACCESS_EXECONLY;
	put(&obj,val);
}

int DictObject::size() {
	int len = 0;
	Object *ptr=getObjectPtr();
	for(int j=0;j<length;j++, ptr+=2) {
		if(ptr->token!=TOK_NULLA)
			len++;
	}
	return len;
}

void DictObject::print() {
	Object *ptr=getObjectPtr();
	for (int j=0; j<length; j++, ptr+=2) {
		if(ptr->token!=TOK_NULLA) {
			printf("%3d. ",j);
			Object::print(ptr);
			printf(" => ");
			Object::print(ptr+1);
			printf("\n");
		}
	}
}

SaveObject::SaveObject() {
	token=TOK_SAVE;
	what.uintValue=Timer::time();
}

GStateObject::GStateObject() {token=TOK_GSTATE;}

/*
* FileObject 
*/

void Object::CheckRes() {
	if(!what.lexp) throw Error::UndefinedResource();
}

FileObject::FileObject() {
	make_null(this);
	token=TOK_FILE;
}

FileObject::FileObject(char *fname, const char *fmode) {
	make_null(this);
	token=TOK_FILE;
	pad=fmode[0];
	what.lexp = new Lexx(fname, fmode);
}

int FileObject::getNextChar() {
	CheckRes();
	return what.lexp->getNextChar();
}

bool FileObject::getToken(Token& token) {
	CheckRes();
	return what.lexp->getToken(token)!=Token::LEX_EOF;
}

bool FileObject::getString(byte *buff, int maxlength, int *retlength) {
	CheckRes();
	return what.lexp->getStringBuffer(buff, maxlength, retlength);
}

bool FileObject::getLine(byte *buff, int maxlength, int *retlength) {
	CheckRes();
	return what.lexp->getLineBuffer(buff, maxlength, retlength);
}

int  FileObject::getFilePos() {
	CheckRes();
	return what.lexp->getFilePos();
}
void FileObject::setFilePos(int pos) {
	CheckRes();
	what.lexp->setFilePos(pos);
}

int FileObject::getBytesAvailable() {
	CheckRes();
	return what.lexp->getBytesAvailable();
}

void FileObject::resetBuffer() {
	CheckRes();
	what.lexp->resetBuffer();
}

void FileObject::flushFile() {
	CheckRes();
	what.lexp->flushFile();
}
	
void FileObject::close() {
	delete what.lexp;
	what.lexp = 0; // guard
}

// The constructor of the derived class gets called after the constructor of the base class
LoopMarkObject::LoopMarkObject() {token=TOK_LOOPMARK;}

/* EOF */
#ifndef ObjectH
#define ObjectH

#include "Main.h"
#include "VMem.h"
#include "Lexx.h"
#include "Error.h"
#include "utils.h"

class SystemTable;

extern VMem GVmem;

class Object {
protected:
	void CheckRes();
	
public:

	enum {
		TOK_NULLA=0,
		TOK_INTEGER=1,
		TOK_REAL=2,
		TOK_NAME=3,
		TOK_BOOL=4,
		TOK_STRING=5,
		TOK_ENAME=6,	// Immediate evaluated name	@TODO: remove		
		TOK_DICT=7,
		TOK_ARRAY=8,
		TOK_MARK=9,		
		TOK_OPERATOR=10,
		TOK_FILE=11,
		TOK_GSTATE=12, //(Level 2)
		TOK_SAVE=13,
		TOK_LOOPMARK=14,
		
		EXEC_BIT = 0x80
	};
	
	enum Access {
		ACCESS_UNLIMITED=0,
		ACCESS_READONLY='r',
		ACCESS_EXECONLY='x',
		ACCESS_NONE='0'
	};

	byte token;		// nulla, mark. TokenType | EXEC_BIT
   	byte pad;		// Padding or access for composite objects
	word start;		// string, name, array
    word length;	// string, literal /names : # of chars. array, dict : # of elements.
		
	union {
   		int intValue;		// bool, integer, operator
		unsigned uintValue;	// timestamp
        float realValue;	// real
		VMDesc memIndex;	// VMem descriptor  for composite objects
		Lexx *lexp;
   	} what;

	friend bool operator ==(const Object& a, const Object& b);
	friend bool operator !=(const Object& a, const Object& b);
	
	Object();
	virtual ~Object();
	
	 int size();			// subclasses may overload
	 int GetType(void);	
	void CheckExArray();
	void CheckNumber();
	void CheckType(int needType);
	void CheckInt();
	void CheckName();
	void CheckString();
	void CheckFile(int acc=0);
	bool GetExecFlag(void);
	void SetExecFlag(bool x);
	bool isBool();
	bool isInt();
	bool isReal();
	bool isPositive();
	bool isNegative();
	bool isGreaterThan(Object *obj);
	bool isLessThan(Object *obj);
	void Add(Object *obj);
	bool isComposite();	// @NOTE: includes NAME objects	
	void copy(Object *obj);	// @NOTE: Ezt az ertekadas is megcsinalja!
  	const char *strType();
	void free();

	// Only for composite objects		
	byte   *getStringPtr();
	Object *getObjectPtr();	
	byte   *getStringStartPtr();
	Object *getObjectStartPtr();
	
	unsigned getTimeStamp();
	
	static void copystring(Object *obj, char *str);
	static void copystring(Object *obj, const char *str);
	static void copystring(Object *obj, Object *src);
	static void make_null(Object *obj);
	static void MakeMarkObject(Object *obj);
	static void MakeBoolObject(Object *obj, bool val);
	static void MakeIntObject(Object  *obj, int val);
	static void MakeRealObject(Object *obj, float val);
	static void checkrange(int x, int length);
	static void print(Object *obj);
	static void print_type(Object *obj);
};

// Constructs Bool objects
class BoolObject : public Object {
public:	
	BoolObject(bool b);
};

class IntegerObject : public Object {
public:
	IntegerObject(int x);
};

class RealObject : public Object {
public:	
	RealObject(float f);
};

class MarkObject : public Object {
public:
	MarkObject();
};

class OperatorObject : public Object {
public:
	OperatorObject(int x);
};

// Unlike a string, a name is a simple object not made up of other objects
class NameObject : public Object {
private:
	void setNameObject(char *name, int len);
	
public:
	NameObject(char *name);
	NameObject(char *name, int length);
	NameObject(const char *name);
};

class StringObject : public Object {
public:
	StringObject();
    StringObject(Object *obj);
	StringObject(char *name);
    StringObject(int size);
	char *getString(char *buff, int buflen);
	int strchr(int c);	// -1 if not found
	int get(int x);
    void put(int x, int c);	
	void tailor();
};

class ArrayObject : public Object {
public:
	ArrayObject(int count);
	ArrayObject( Object *obj);
	ArrayObject( Object *obj, bool execFlag );
	//int size(); -- common
    Object *get(int x);
    void put(int x,Object* a);
	void tailor();
};

class DictObject : public Object {
private:
	void clear();
	
public:
	DictObject(Object *obj);
	DictObject(int size, Access access);
	
	Object *lookup(char *name);
	Object *index(Object *obj);

  	void get(int x, Object *key, Object *val);
    void put(Object *key, Object *val);
    void putsys(const char *name, Object *val);
	int size();
	void print();
};

class SaveObject : public Object {
public:
	SaveObject();
};

class GStateObject : public Object {
public:
	GStateObject();
};

class FileObject : public Object {
public:
	FileObject();	
	FileObject(char *fname, const char *fmode);
	
	int  getNextChar();
	bool getToken(Token& token);
	bool getString(byte *buff, int maxlength, int *retlength);
	bool getLine(byte *buff, int maxlength, int *retlength);
	
	int  getFilePos();
	void setFilePos(int pos);
	int  getBytesAvailable();
	void resetBuffer();
	void flushFile();
	void close();
};

class LoopMarkObject : public Object {
public:
	LoopMarkObject();
};

#endif
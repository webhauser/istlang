#ifndef ObjectStackH
#define ObjectStackH

#include <stdio.h>
#include "Object.h"
#include "Error.h"

class ObjectStack {
private:

	Object *stack;
    int top;
    int max_size;

public:

	ObjectStack(int size) : top(0), max_size(size) {
		fprintf(stderr, "DEBUG: ObjectStack(%d)\n", size);
		stack=new Object[size];
	}
	
	~ObjectStack() {
		fprintf(stderr, "DEBUG: ~ObjectStack()\n");
		delete[] stack;
	}
   	
   	inline void clear(){top=0;}
	inline bool empty(){return (top==0);}
	inline 	int size(){return top;}

	void checkstackempty(void){if(empty()) throw Error::StackUnderflow();}
    void checkstackfull(void){if(top==max_size) throw Error::StackOverflow();}
	
	void push(Object *obj);
	Object *pop();
	int  popint();
	bool popbool();
	
    // index operator overload kell
    Object& operator[](int x){if(x>=0 && x<top)return stack[x];else throw Error::RangeCheck();}
    Object* topitem(){checkstackempty();return &stack[top-1];}
	
    void pushbool(bool b);
	void pushint (int x);
	void pushreal(float f);
	void pushmark();

	void print();
};

#endif
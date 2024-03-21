#include "ObjectStack.h"

void ObjectStack::push(Object *obj) {
	checkstackfull();
	stack[top++] = *obj;
}

Object* ObjectStack::pop() {
	checkstackempty();
	return &stack[--top];
}

int ObjectStack::popint() {
	Object *obj=pop();
	obj->CheckInt();	
	return obj->what.intValue;
}

bool ObjectStack::popbool() {
	Object *obj=pop();
	obj->CheckType(Object::TOK_BOOL);
	return obj->what.intValue;
}

void ObjectStack::pushbool(bool b) {
	checkstackfull();
	Object::MakeBoolObject(&stack[top++], b);
}

void ObjectStack::pushint (int x)  {
	checkstackfull();
	Object::MakeIntObject(&stack[top++], x);
}

void ObjectStack::pushreal(float f) {
	checkstackfull();
	Object::MakeRealObject(&stack[top++], f);
}

void ObjectStack::pushmark() {
	checkstackfull();
	Object::MakeMarkObject(&stack[top++]);
}

void ObjectStack::print() {
	for(int x=0; x<top; x++) {
    	printf("%3d. ", x+1);
		Object::print(&stack[x]);
		printf("\n");
	}
}

/* EOF */
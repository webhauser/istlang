/* 2007 (c) webHauser. All rights reserved. Irta: Hauser Istvan */

#include "Interpreter.h"

/*
 * Interpreter static variables
 */
Interpreter *Interpreter::instance = 0;
Object Interpreter::nullObject;
BoolObject Interpreter::trueObject  = BoolObject(true);
BoolObject Interpreter::falseObject = BoolObject(false);
LoopMarkObject Interpreter::loopmark;
Object Interpreter::currObject;
Token Interpreter::token;
unsigned Interpreter::_seed = 0;

/* Ennek a tombnek az elemei az
 * enum OperatorIndex tipus OP_INDEX_XXXX
 * sorrendben kell hogy legyenek feltoltve!!!
 * Mert ez van a system name objektumok cimmezojeben!
 * system name index.
 */
Procedure Interpreter::operators[] = {
Interpreter::exec_not,
Interpreter::exec_xor,
Interpreter::exec_and,
Interpreter::exec_or,
Interpreter::exec_bitshift,
Interpreter::exec_add,
Interpreter::exec_sub,
Interpreter::exec_mul,
Interpreter::exec_div,
Interpreter::exec_idiv,
Interpreter::exec_mod,
Interpreter::exec_abs,
Interpreter::exec_neg,

// Stack manipulation
Interpreter::exec_pop,
Interpreter::exec_exch,
Interpreter::exec_dup,
Interpreter::exec_roll,
Interpreter::exec_index,
Interpreter::exec_clear,
Interpreter::exec_count,
Interpreter::exec_array,
Interpreter::exec_aload,
Interpreter::exec_astore,
Interpreter::exec_mark,
Interpreter::exec_cleartomark,
Interpreter::exec_counttomark,
Interpreter::exec_put,
Interpreter::exec_get,
Interpreter::exec_getinterval,
Interpreter::exec_putinterval,

// Conversion
Interpreter::exec_xcheck,
Interpreter::exec_cvi,
Interpreter::exec_cvn,
Interpreter::exec_cvs,
Interpreter::exec_cvx,
Interpreter::exec_cvlit,

// Mathematical
Interpreter::exec_ceiling,
Interpreter::exec_floor,
Interpreter::exec_round,
Interpreter::exec_truncate,
Interpreter::exec_sqrt,
Interpreter::exec_atan,
Interpreter::exec_cos,
Interpreter::exec_sin,
Interpreter::exec_exp,
Interpreter::exec_ln,
Interpreter::exec_log,
Interpreter::exec_rand,
Interpreter::exec_srand,
Interpreter::exec_rrand,

// Dictionary
Interpreter::exec_def,
Interpreter::exec_dict,
Interpreter::exec_begin,
Interpreter::exec_end,
Interpreter::exec_dictstack,
Interpreter::exec_countdictstack,
Interpreter::exec_currentdict,
Interpreter::exec_cleardictstack,
Interpreter::exec_load,
Interpreter::exec_store,
Interpreter::exec_where,
Interpreter::exec_undef,
Interpreter::exec_maxlength,
Interpreter::exec_known,

// Logical tests
Interpreter::exec_eq,
Interpreter::exec_ne,
Interpreter::exec_gt,
Interpreter::exec_lt,
Interpreter::exec_ge,
Interpreter::exec_le,

// Control
Interpreter::exec_if,
Interpreter::exec_ifelse,

// loop
Interpreter::exec_for,
Interpreter::exec_for_cont,
Interpreter::exec_loop,
Interpreter::exec_loop_cont,
Interpreter::exec_repeat,
Interpreter::exec_repeat_cont,
Interpreter::exec_forall,
Interpreter::exec_forall_string_cont,
Interpreter::exec_forall_array_cont,
Interpreter::exec_forall_dict_cont,
Interpreter::exec_exit,
Interpreter::exec_stop,
Interpreter::exec_stopped,
Interpreter::exec_stopped_cont,

// File
Interpreter::exec_file,
Interpreter::exec_closefile,
Interpreter::exec_read,
Interpreter::exec_write,
Interpreter::exec_readhexstring,
Interpreter::exec_writehexstring,
Interpreter::exec_readstring,
Interpreter::exec_writestring,
Interpreter::exec_readline,
Interpreter::exec_bytesavailable,
Interpreter::exec_flush,
Interpreter::exec_flushfile,
Interpreter::exec_resetfile,
Interpreter::exec_status,
Interpreter::exec_currentfile,
Interpreter::exec_deletefile,
Interpreter::exec_renamefile,
Interpreter::exec_filenameforall,
Interpreter::exec_setfileposition,
Interpreter::exec_fileposition,

Interpreter::exec_token,
Interpreter::exec_save,
Interpreter::exec_restore,
Interpreter::exec_meminfo,
Interpreter::exec_stack,
Interpreter::exec_pstack,
Interpreter::exec_pexecstack,

//etc
Interpreter::exec_bind,
Interpreter::exec_copy,
Interpreter::exec_anchorsearch,
Interpreter::exec_search,
Interpreter::exec_length,
Interpreter::exec_string,
Interpreter::exec_type,
Interpreter::exec_print,
Interpreter::exec_kiiras,
Interpreter::exec_exec,
Interpreter::exec_countexecstack,
Interpreter::exec_execstack,
Interpreter::exec_quit,

Interpreter::exec_mark,				// [
Interpreter::exec_array_token_end,	// ]
Interpreter::exec_mark,				// {
Interpreter::exec_array_token_exe_end, // }
Interpreter::exec_mark,				// <<
Interpreter::exec_dict_token_end	// >>
};

void Interpreter::systemdict_setup() {	
	for (int x=0; x<SystemTable::size(); x++) {
    	SystemTable *rec=(SystemTable*)SystemTable::sysnameindex+x;
		systemdict.putsys(SystemTable::sysnameindex[x].name, &rec->obj);
    }
    systemdict.putsys("null",  &nullObject);
	systemdict.putsys("true",  &trueObject);
    systemdict.putsys("false", &falseObject);
	systemdict.putsys("userdict",  &userdict);
	systemdict.putsys("systemdict",&systemdict);
	systemdict.putsys("errordict", &errordict);
}

// Walk through the dictionary stack till name found.
Object *Interpreter::bind(char *name, bool except) {
	for (int x=instance->dictstack.size()-1; x>=0; --x) {
        Object* obj = &instance->dictstack[x];
    	DictObject *dictobj=(DictObject *)obj;
	   	if((obj=dictobj->lookup(name))!=NULL)
        	return obj;
    }
	if(except) throw Error::Undefined();
	return NULL;
}

Object *Interpreter::bind(Object *key, bool except) {
	key->CheckName(); // key must be a name
	return bind((char *)key->getStringStartPtr(), except);	
}

// exec_bind helper instance method
void Interpreter::bind_recursive(ArrayObject *arr) {
	Object *obj, *val;
	//printf("bind_recursive(%d): ", arr->size()); Object::print(arr); printf("\n");	
	for(int x=0; x<arr->size(); x++) {
		obj=arr->get(x);
		if(obj->token==Object::EXEC_BIT+Object::TOK_NAME) {
			val=bind(obj,false);
			if(val && val->token==Object::EXEC_BIT+Object::TOK_OPERATOR) {		
				arr->put(x,val);
				//printf("bind_recursive: %d. replace with ",x); Object::print(val); printf("\n");	
			}
		}
		else if(obj->token==Object::EXEC_BIT+Object::TOK_ARRAY) {
			bind_recursive((ArrayObject *)obj);
		}
	}
}

// 12345 => 23451
/* static */ void Interpreter::scroll(Object *array, int n) {
	Object tmp=array[0];
	for(int i=0; i<n-1; i++) {
		array[i]=array[i+1];
	}
	array[n-1]=tmp;
}

/* static */void Interpreter::push_operand(Object *obj) {instance->opstack.push(obj);}
/* static */void Interpreter::push_execstack(Object *obj) {instance->exstack.push(obj);}
/* static */Object *Interpreter::pop_operand() {return instance->opstack.pop();}
/* static */FileObject *Interpreter::pop_operand_file(int acc) {
	Object *obj=instance->opstack.pop();
	obj->CheckFile(acc);
	return (FileObject *)obj;
}
/* static */Object *Interpreter::pop_execstack() {return instance->exstack.pop();}
/* static */void Interpreter::push_operand_int(int x) {instance->opstack.pushint(x);}
/* static */ int Interpreter::pop_operand_int() {return instance->opstack.popint();}
/* static */void Interpreter::push_execstack_int(int x) {instance->exstack.pushint(x);}
/* static */int Interpreter::pop_execstack_int() {return instance->exstack.popint();}
/* static */void Interpreter::pop_loopmark() {if(instance->exstack.pop()->GetType()!=Object::TOK_LOOPMARK) throw Error::UnmatchedMark();}

/*======================== System operators =========================*/

// Arithmetical & logical functions
void Interpreter::exec_not() {
	Object *a=pop_operand();
    if(a->isBool()) a->what.intValue = !(bool)a->what.intValue; else    
    if(a->isInt())  a->what.intValue = ~a->what.intValue; else throw Error::TypeCheck();
	push_operand(a);
}

void Interpreter::exec_xor() {
    Object *b=pop_operand();
	Object *a=pop_operand();
    if(a->isBool() && b->isBool())
    	instance->opstack.pushbool((bool)a->what.intValue ^ (bool)b->what.intValue);
    else if(a->isInt() && b->isInt())
		push_operand_int(a->what.intValue ^ b->what.intValue);
    else throw Error::TypeCheck();
}

void Interpreter::exec_and() {
    Object *b=pop_operand();
	Object *a=pop_operand();
    if(a->isBool() && b->isBool())
    	instance->opstack.pushbool((bool)a->what.intValue && (bool)b->what.intValue);
    else if(a->isInt() && b->isInt())
		push_operand_int(a->what.intValue & b->what.intValue);
    else throw Error::TypeCheck();
}

void Interpreter::exec_or() {
    Object *b=pop_operand();
	Object *a=pop_operand();
    if(a->isBool() && b->isBool())
    	instance->opstack.pushbool((bool)a->what.intValue || (bool)b->what.intValue);
    else if(a->isInt() && b->isInt())
		push_operand_int(a->what.intValue | b->what.intValue);
    else throw Error::TypeCheck();
}
/* int1 shift bitshift int2 */
void Interpreter::exec_bitshift() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	a->CheckInt(); b->CheckInt();
	int shift=b->what.intValue;
	push_operand_int(shift>0 ? a->what.intValue << shift : a->what.intValue >> shift);	
}

void Interpreter::exec_add() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	if(a->isReal() && b->isReal())instance->opstack.pushreal(a->what.realValue + b->what.realValue); else
	if(a->isReal() && b->isInt()) instance->opstack.pushreal(a->what.realValue + b->what.intValue); else 	
	if(a->isInt() && b->isReal()) instance->opstack.pushreal(a->what.intValue + b->what.realValue); else 	
	if(a->isInt() && b->isInt()) push_operand_int(a->what.intValue + b->what.intValue); else 
		throw Error::TypeCheck();
}

void Interpreter::exec_sub() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	if(a->isReal() && b->isReal()) instance->opstack.pushreal(a->what.realValue - b->what.realValue); else 
	if(a->isInt() && b->isReal())  instance->opstack.pushreal(a->what.intValue - b->what.realValue); else 
	if(a->isReal() && b->isInt()) instance->opstack.pushreal(a->what.realValue - b->what.intValue); else 
	if(a->isInt() && b->isInt()) push_operand_int(a->what.intValue - b->what.intValue); else 
		throw Error::TypeCheck();
}

void Interpreter::exec_mul() {
	Object *a=pop_operand();
	Object *b=pop_operand();
	a->CheckNumber(); b->CheckNumber();
	if(a->isReal() && b->isReal()) instance->opstack.pushreal(a->what.realValue * b->what.realValue); else 
	if(a->isInt() && b->isReal())  instance->opstack.pushreal(a->what.intValue * b->what.realValue); else 
	if(a->isReal() && b->isInt())  instance->opstack.pushreal(a->what.realValue * b->what.intValue); else 
	if(a->isInt() && b->isInt())   push_operand_int(a->what.intValue * b->what.intValue);
}

/** divides num1 by num2, producing a result that is always a real number even if both operands are integers */
void Interpreter::exec_div() {
	Object *b=pop_operand();
	Object *a=pop_operand();	
	if((b->isReal()    && b->what.realValue==0.0f) || (b->isInt() && b->what.intValue==0)) throw Error::UndefinedResult();
	float x;
	if(a->isReal()    && b->isReal())    x = a->what.realValue / b->what.realValue; else 
	if(a->isInt() && b->isReal())    x = a->what.intValue  / b->what.realValue; else 
	if(a->isReal()    && b->isInt()) x = a->what.realValue / b->what.intValue;  else
	if(a->isInt() && b->isInt()) x = (float)a->what.intValue / (float)b->what.intValue; else throw Error::TypeCheck();
	instance->opstack.pushreal(x);
}

void Interpreter::exec_idiv() {
	int b=pop_operand_int();
	int a=pop_operand_int();
	if(b==0) throw Error::UndefinedResult();
	push_operand_int(a / b);
}

void Interpreter::exec_mod() {
	int b=pop_operand_int();
	int a=pop_operand_int();
	push_operand_int(a % b);
}

void Interpreter::exec_abs() {
	Object *a=pop_operand();
	if(a->isInt()) a->what.intValue = ABS(a->what.intValue);  else
	if(a->isReal())   a->what.realValue = ABS(a->what.realValue); else throw Error::TypeCheck();
	push_operand(a);
}

void Interpreter::exec_neg() {
	Object *a=pop_operand();    
    if(a->isInt()) a->what.intValue  = -a->what.intValue;  else
	if(a->isReal())    a->what.realValue = -a->what.realValue; else throw Error::TypeCheck();
    push_operand(a);
}

/*
* Stack manipulation
*/
void Interpreter::exec_pop() {pop_operand();}
void Interpreter::exec_exch() {
	Object a=*pop_operand();
	Object b=*pop_operand();
	push_operand(&a);
	push_operand(&b);
}
void Interpreter::exec_dup()   {push_operand(instance->opstack.topitem());}

/**
0 0 0 1 2 3 4 5  5 1 roll % 0 0 0 5 1 2 3 4
(a) (b) (c) 3 −1 roll => (b) (c) (a)
(a) (b) (c) 3  1 roll => (c) (a) (b)
(a) (b) (c) 3  0 roll => (a) (b) (c) */
void Interpreter::exec_roll() {	
	Object *oj=pop_operand();
	Object *on=pop_operand();
	oj->CheckInt();
	on->CheckInt();
	int j=oj->what.intValue;
	int n=on->what.intValue;
	if(n<0) throw Error::RangeCheck();
	if(n==0 || j==0) return;
	if(j<0) j=n+j; 
	if((j%n)==0) return;	
	Object *array = new Object[n];
	int i;
	// stack: 1 2 3 4 5 |
	// array: 5 4 3 2 1
	for(i=0; i<n; i++) array[i] = *pop_operand();
	// array: 4 3 2 1 5
	while(j-- > 0)  scroll(array, n);
	// stack: 4 3 2 1 5
	for(i=0; i<n; i++) push_operand(&array[n-i-1]);
	delete [] array;
}

/**
index any n ... any0 n index anyn ... any0 anyn
removes the nonnegative integer n from the operand stack, counts down to the
nth element from the top of the stack, and pushes a copy of that element on the stack */
void Interpreter::exec_index() {
	int n=pop_operand_int();
	if(n<0) throw Error::RangeCheck();
	push_operand(&instance->opstack[instance->opstack.size() - n-1]);
}
void Interpreter::exec_clear() {instance->opstack.clear();}
void Interpreter::exec_count() {push_operand_int(instance->opstack.size());}
void Interpreter::exec_array() {
	ArrayObject obj = ArrayObject(pop_operand_int());
	push_operand(&obj);
}
/** array --aload-- any0 ... any(n−1) array
successively pushes all n elements of array on the operand stack
(where n is the length of the operand), and then pushes the operand itself.
FIGYELEM!!! A muvelet atirja az operandus stacket, ezert az array mutato mevaltozik!!! */
/* operator */void Interpreter::exec_aload() {
	Object aob=*pop_operand();
	aob.CheckType(Object::TOK_ARRAY);
	ArrayObject *arr=(ArrayObject *)&aob;
	for(int x=0; x<arr->size(); x++) {
		push_operand(arr->get(x));
	}
	push_operand(arr);
}
/** any0 ... any(n-1) array --astore-- array
Stores the objects any0 through any(n-1) from the operand stack into array, where n is the length of the array. 
Packed array objects always have read-only (or even more restricted) access, so
the put, putinterval, and astore operations are not allowed on them. */
void Interpreter::exec_astore() {
	ArrayObject *array=(ArrayObject *)pop_operand();
	array->CheckType(Object::TOK_ARRAY);
	if(array->GetExecFlag()) throw Error::InvalidAccess(); // Packed Array	
    for(int x=0; x<array->length; x++) {
		Object *obj=pop_operand();
        array->put(array->length-1-x,obj);// ez igy jo, mert *obj-al rakja be!
    }
    push_operand(array);
}

void Interpreter::exec_mark() {instance->opstack.pushmark();}
void Interpreter::exec_cleartomark() {
	Object *obj;
    do {
		obj=pop_operand();
    } while (obj->GetType()!=Object::TOK_MARK);
}

void Interpreter::exec_counttomark() {
    Object *obj;
	int count=0;
	int x=instance->opstack.size();
	do {
        ++count;
		obj=&instance->opstack[--x];
    } while(obj->GetType()!=Object::TOK_MARK);
    push_operand_int(count-1);
}
/*
 * array  index any put
 * string index int put
 * dict  key   any put
 */
void Interpreter::exec_put() {
	Object *any=pop_operand();
    Object *key=pop_operand();
    Object *obj=pop_operand();
    switch(obj->GetType()) {
		case Object::TOK_ARRAY:
			((ArrayObject *)obj)->put(key->what.intValue, any);
			break;
		case Object::TOK_STRING:
			((StringObject *)obj)->put(key->what.intValue, any->what.intValue);
			break;
		case Object::TOK_DICT:
			if(obj->pad!=0) throw Error::InvalidAccess();
			((DictObject *)obj)->put(key, any);
			break;
		default: 
			throw Error::TypeCheck();
	}
}
/*
 * array index - get - any
 * string index- get - any
 * dict   key -  get - any
 */
void Interpreter::exec_get() {
    Object *key=pop_operand();
    Object *obj=pop_operand();
    switch(obj->GetType()) {
		case Object::TOK_ARRAY:
			push_operand(((ArrayObject *)obj)->get(key->what.intValue));
			break;
		case Object::TOK_STRING:
			push_operand_int(((StringObject *)obj)->get(key->what.intValue));
			break;
		case Object::TOK_DICT: {
			Object *any=((DictObject *)obj)->index(key);
			if(!any) throw Error::Undefined();
			push_operand(any);
			break;
		}
		default:
			throw Error::TypeCheck();
    }
}

/* OK
string|array index count -getinterval- subarray
getinterval creates a new object that shares a subinterval of an array, a packed
array, or a string. This operator does not apply to dictionary objects. */
void Interpreter::exec_getinterval() {
	int count=pop_operand_int();
	int index=pop_operand_int();
	Object *obj=pop_operand();	
	if(index<0 || count<0) throw Error::RangeCheck();
	switch(obj->GetType()) {
		case Object::TOK_STRING: {			
			if(index + count > obj->length) throw Error::RangeCheck();
			StringObject ret = StringObject(obj);
			ret.length = count;
			ret.start = index;
			push_operand(&ret);
			break;
		}
		case Object::TOK_ARRAY: {
			if(index + count > obj->length) throw Error::RangeCheck();			
			ArrayObject ret = ArrayObject(obj);
			ret.length = count;
			ret.start = index;
			push_operand(&ret);
			break;
		}
		default:
			throw Error::TypeCheck();
    }	
}

/* OK
array1 index array2 |packedarray2 -putinterval-
putinterval overwrites a subinterval of one array or string with the contents of
another. This operator does not apply to dictionary or packed array objects, although 
it can overwrite a subinterval of an array with the contents of a packedarray. 
Packed array objects always have read-only (or even more restricted) access, so
the put, putinterval, and astore operations are not allowed on them. */
void Interpreter::exec_putinterval() {
	Object *o2=pop_operand();
	int index=pop_operand_int();
	Object *o1=pop_operand();
	if(o1->GetExecFlag()) throw Error::InvalidAccess();
	if(index < 0) throw Error::RangeCheck();		
	int type=o1->GetType();
	switch(type) {
		case Object::TOK_STRING:
		case Object::TOK_ARRAY:
			if(type!=o2->GetType()) throw Error::TypeCheck();
			if(index + o2->length > o1->length) throw Error::RangeCheck();
			if(type==Object::TOK_STRING) {
				copybytes(o2->getStringStartPtr(), o1->getStringStartPtr() + index, o2->length);
			}
			else if(type==Object::TOK_ARRAY) {
				int count = o2->length;
				Object *src = o2->getObjectStartPtr();
				Object *dst = o1->getObjectStartPtr() + index;
				while(count-- > 0) *dst++ = *src++;
			}			
			break;
		default:
			throw Error::TypeCheck();
	}
}

// any xcheck bool ;Test executable attribute
void Interpreter::exec_xcheck() {
	Object *obj=pop_operand();
	instance->opstack.pushbool(obj->GetExecFlag());
}

/**
@@@TODO: Check - rounds it toward 0 and converts it to an integer.
num cvi int
string cvi int */
void Interpreter::exec_cvi() {
	int ival;
	Object *obj=pop_operand();
	switch(obj->GetType()) {
		case Object::TOK_INTEGER:
			ival=obj->what.intValue;
			break;
		case Object::TOK_REAL:			
			ival=(int)obj->what.realValue;
			break;
		case Object::TOK_STRING: {
			StringObject *str=(StringObject *)obj;
			char buff[32];
			str->getString(buff,32);
			if(str->strchr('.') >= 0 || str->strchr('e') || str->strchr('E'))
				 ival=(int)atof(buff);
			else ival=atoi(buff);
			break;
		}
		default:
			throw Error::TypeCheck();
	}
	push_operand_int(ival);
}

// string -cvn- name
void Interpreter::exec_cvn() {
	Object *obj=pop_operand();
    obj->CheckString();	
    NameObject name=NameObject((char *)obj->getStringStartPtr(), obj->length);
    name.SetExecFlag(obj->GetExecFlag());
	name.length = obj->length;
    push_operand(&name);
}

/* any string --cvs-- substring 
If string is too small to hold the result of the conversion, a rangecheck error occurs. */
void Interpreter::exec_cvs() {
	char buff[TOKEN_BUFLEN];
	Object *str=pop_operand();    
	Object *obj=pop_operand();
	str->CheckString();
	switch(obj->GetType()) {
	case Object::TOK_NULLA:		Object::copystring(str,"--null--"); break;
    case Object::TOK_MARK:		Object::copystring(str,"--mark--"); break;
	case Object::TOK_BOOL:      Object::copystring(str, obj->what.intValue ? "--true--" : "--false--"); break;
	case Object::TOK_STRING:
	case Object::TOK_NAME:		Object::copystring(str,obj); break;	
	case Object::TOK_OPERATOR:	
		sprintf(buff,"--%s--", SystemTable::sysnameindex[obj->what.intValue].name);
		Object::copystring(str,buff);
		break;
    case Object::TOK_INTEGER: 	
		sprintf(buff,"%d",obj->what.intValue);
		Object::copystring(str,buff);
		break;
    case Object::TOK_REAL:		
		sprintf(buff,"%f", obj->what.realValue); 
		Object::copystring(str,buff);
		break;
    default:
    	Object::copystring(str,"--nostringval--");
		break;
    }
    push_operand(str);
}

void Interpreter::exec_cvx()   {instance->opstack.topitem()->token |= Object::EXEC_BIT;}
void Interpreter::exec_cvlit() {instance->opstack.topitem()->token &= Object::EXEC_BIT-1;}

/*
// Arithmetic and Math Operators
*/

/* num1 ceiling num2 ; Return ceiling of num1 */
void Interpreter::exec_ceiling() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	instance->opstack.pushreal(ceilf(x));
}

/* num1 floor num2 ; Return floor of num1 */
void Interpreter::exec_floor() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	instance->opstack.pushreal(floorf(x));
}

/* num1 round num2 ; Round num1 to nearest integer */
void Interpreter::exec_round() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	instance->opstack.pushreal(roundf(x));
}

/* num1 truncate num2 ; Remove fractional part of num1 */
void Interpreter::exec_truncate() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	instance->opstack.pushreal(truncf(x));
}

/*
-num- sqrt -real- 
returns the square root of num, which must be a nonnegative number. The result is a real number.
Errors: rangecheck, stackunderflow, typecheck */
void Interpreter::exec_sqrt() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	if(x>=0) instance->opstack.pushreal(sqrtf(x)); else throw Error::RangeCheck();
}
/* 
num den -atan- angle ; Return arctangent of num/den in (in degrees between 0 and 360) 
returns the angle (in degrees between 0 and 360) whose tangent is num divided by den. 
Either num or den may be 0, but not both. The signs of num and den determine 
the quadrant in which the result will lie: a positive num yields a result in the
positive y plane, while a positive den yields a result in the positive x plane. 
The result is a real number.
Examples:
0 1 atan ⇒ 0.0
1 0 atan ⇒ 90.0
−100 0 atan ⇒ 270.0
4 4 atan ⇒ 45.0
Errors: stackunderflow, typecheck, undefinedresult
*/
void Interpreter::exec_atan() {
	throw Error::Undefined(); // @TODO
}

/* angle cos real ; Return cosine of angle degrees */
void Interpreter::exec_cos() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	instance->opstack.pushreal(cosf(deg2radf(x)));
}

/* angle sin real ; Return sine of angle degrees */
void Interpreter::exec_sin() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	instance->opstack.pushreal(sinf(deg2radf(x)));
}

/* base exponent exp real ; Raise base to exponent power 
raises base to the exponent power. The operands may be either integers or real numbers. 
If the exponent has a fractional part, the result is meaningful only if the base is nonnegative. 
The result is always a real number.
C 2018 7.12.7.4 specifies powf, pow, and powl:
-The pow functions compute x raised to the power y. 
A domain error occurs if x is finite and negative and y is finite and not an integer value..
Examples:
9 0.5 exp ⇒ 3.0
−9 −1 exp ⇒ −0.111111
Errors: stackunderflow, typecheck, undefinedresult */
void Interpreter::exec_exp() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	a->CheckNumber(); b->CheckNumber();	
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	float y=(b->isInt() ? b->what.intValue : b->what.realValue);
	if(x<0 && b->GetType()!=Object::TOK_INTEGER) throw Error::Undefined();
	instance->opstack.pushreal(powf(x,y));
}

/* num ln real ; Return natural logarithm (base e) */
void Interpreter::exec_ln() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();	
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	if(x<=0) throw Error::RangeCheck();
	instance->opstack.pushreal(logf(x));
}

/* num log real ; Return common logarithm (base 10) */
void Interpreter::exec_log() {
	Object *a=pop_operand();	// stackunderflow
	a->CheckNumber();	
	float x=(a->isInt() ? a->what.intValue : a->what.realValue);
	if(x<=0) throw Error::RangeCheck();
	instance->opstack.pushreal(log10f(x));	
}
void Interpreter::exec_rand()  {push_operand_int(rand());}
void Interpreter::exec_rrand() {push_operand_int(_seed);}
void Interpreter::exec_srand() {_seed=pop_operand_int();srand(_seed);}
void Interpreter::exec_def() {
	Object *value = pop_operand();
	Object *key   = pop_operand();	
	// The topmost dictionary on the stack is the current dictionary
	DictObject *currentdict = (DictObject *)instance->dictstack.topitem();
	if(currentdict->pad!=0) throw Error::InvalidAccess();
	currentdict->put(key,value);
}

void Interpreter::exec_dict() {
	DictObject dict=DictObject(pop_operand_int(), Object::Access::ACCESS_UNLIMITED);
	push_operand(&dict);
}

void Interpreter::exec_begin() {
	Object *obj=pop_operand();
	obj->CheckType(Object::TOK_DICT);
	instance->dictstack.push(obj);
}

void Interpreter::exec_end() {instance->dictstack.pop();}
/** array --dictstack-- subarray % Copy dictionary stack into array */
void Interpreter::exec_dictstack() {
	ArrayObject *arr = (ArrayObject *)pop_operand();
	arr->CheckType(Object::TOK_ARRAY);
	int n=instance->dictstack.size();
	if(arr->length < n) throw Error::RangeCheck();	
	for(int i=0; i<n; i++) {
		arr->put(i, &instance->dictstack[i]);
	}
	ArrayObject subarray(arr);
	subarray.length=n;
	push_operand(&subarray);
}
void Interpreter::exec_countdictstack() {push_operand_int(instance->dictstack.size());}
void Interpreter::exec_currentdict()    {push_operand(instance->dictstack.topitem());}
void Interpreter::exec_cleardictstack() {
	int x=instance->dictstack.size() - 2;
	while(x-- > 0) instance->dictstack.pop();
}

/** key -load- value
searches for key in each dictionary on the dictionary stack, starting with the topmost (current) dictionary. 
If key is found in some dictionary, load pushes the associated value on the operand stack. 
If key is not found in any dictionary on the dictionary stack, load executes the error undefined.
load looks up key the same way the interpreter looks up executable names that it encounters during execution.
However, load always pushes the associated value on the operand stack; it never executes that value. */
void Interpreter::exec_load() {
	Object *key=pop_operand();
	push_operand(instance->bind(key,true));
}

/** key value -store-
searches for key in each dictionary on the dictionary stack, starting with the topmost 
(current) dictionary. If key is found in some dictionary, store replaces its value by 
the value operand. If key is not found in any dictionary on the dictionary stack, store 
creates a new entry with key and value in the current dictionary */
void Interpreter::exec_store() {	
	Object *val=pop_operand();
	Object *key=pop_operand();
	Object *obj=instance->bind(key,false);
	if(!obj) {
		DictObject *currentdict = (DictObject *)instance->dictstack.topitem();
		currentdict->put(key,val);
	}
	else *obj = *val;
}

/** key where ==> dict true (if found) | false (if not found)
determines which dictionary on the dictionary stack, if any, contains an entry
whose key is key. where searches for key in each dictionary on the dictionary stack,
starting with the topmost (current) dictionary. If key is found in some dictionary,
where returns that dictionary object and the boolean value true; otherwise, where
simply returns false. */	
void Interpreter::exec_where() {
	Object *key=pop_operand();
	key->CheckName(); // key must be a name
	for (int x=instance->dictstack.size()-1; x>=0; --x) {
        Object *obj = &instance->dictstack[x];
    	DictObject *dict=(DictObject *)obj;
	   	if(dict->lookup((char *)key->getStringStartPtr())!=NULL) {
			push_operand(dict);
			instance->opstack.pushbool(true);
        	return;
		}
    }
	instance->opstack.pushbool(false);
}

/** dict key -undef- 
removes an individual key from a dictionary. */
void Interpreter::exec_undef() {
	Object *key=pop_operand();
	DictObject *dict=(DictObject *)pop_operand();
	key->CheckName();
	dict->CheckType(Object::TOK_DICT);
	if(dict->pad!=0) throw Error::InvalidAccess();
	Object *obj=dict->lookup((char *)key->getStringStartPtr());
	if(obj) Object::make_null(obj);
}

/** dict -maxlength- int 
obtains a dictionary’s maximum capacity. */
void Interpreter::exec_maxlength() {
	Object *obj=pop_operand();
	obj->CheckType(Object::TOK_DICT);
	push_operand_int(obj->length);
}

/**
dict key -known- bool
returns true if there is an entry in the dictionary dict whose key is key; otherwise, it
returns false. dict does not have to be on the dictionary stack. */
void Interpreter::exec_known() {
	Object *key=pop_operand();
	DictObject *dict=(DictObject *)pop_operand();
	key->CheckName();
	dict->CheckType(Object::TOK_DICT);	
	instance->opstack.pushbool( dict->lookup((char *)key->getStringStartPtr())!=NULL );
}

// Logical tests
void Interpreter::exec_eq() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	int aa,bb; bool ok;
	do {		
		if(*a==*b) {
			ok=true;
			break;
		}
		aa=a->GetType(); bb=b->GetType();
		if(aa==Object::TOK_INTEGER && bb==Object::TOK_REAL) {
			ok=a->what.intValue == (int)b->what.realValue;
			break;
		}
		if(aa==Object::TOK_REAL && bb==Object::TOK_INTEGER) {
			ok=b->what.intValue == (int)a->what.realValue;
			break;			
		}
		if((aa==Object::TOK_NAME || aa==Object::TOK_STRING) && 
		   (bb==Object::TOK_NAME || bb==Object::TOK_STRING)) {
			ok=compare(a->getStringStartPtr(), a->length, b->getStringStartPtr(), b->length)==0;
			break;
		}
		ok=false;
	} while(false);
	instance->opstack.pushbool(ok);
}

void Interpreter::exec_ne() {
	exec_eq();
    exec_not();
}

void Interpreter::exec_gt() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	bool ok;
    if(a->GetType()==Object::TOK_STRING && b->GetType()==Object::TOK_STRING)
    	ok = compare(a->getStringStartPtr(), a->length, b->getStringStartPtr(), b->length) > 0;
	else if(a->isInt() && b->isInt()) ok = a->what.intValue > b->what.intValue;
    else if(a->isInt() && b->isReal()) ok = a->what.intValue > b->what.realValue;
    else if(a->isReal() && b->isInt()) ok = a->what.realValue > b->what.intValue;
    else if(a->isReal() && b->isReal()) ok = a->what.realValue > b->what.realValue;
    else throw Error::TypeCheck();
    instance->opstack.pushbool(ok);
}

void Interpreter::exec_lt() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	bool ok;
    if(a->GetType()==Object::TOK_STRING && b->GetType()==Object::TOK_STRING)
    	ok = compare(a->getStringStartPtr(), a->length, b->getStringStartPtr(), b->length) < 0;
    else if(a->isInt() && b->isInt()) ok = a->what.intValue < b->what.intValue;
    else if(a->isInt() && b->isReal()) ok = a->what.intValue < b->what.realValue;
    else if(a->isReal() && b->isInt()) ok = a->what.realValue < b->what.intValue;
    else if(a->isReal() && b->isReal()) ok = a->what.realValue < b->what.realValue;
    else throw Error::TypeCheck();
    instance->opstack.pushbool(ok);
}

void Interpreter::exec_ge() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	bool ok;
    if(a->GetType()==Object::TOK_STRING && b->GetType()==Object::TOK_STRING)
    	ok = compare(a->getStringStartPtr(), a->length, b->getStringStartPtr(), b->length) >= 0;
    else if(a->isInt() && b->isInt())  ok = a->what.intValue >= b->what.intValue;
    else if(a->isInt() && b->isReal()) ok = a->what.intValue >= b->what.realValue;
    else if(a->isReal() && b->isInt()) ok = a->what.realValue >= b->what.intValue;
    else if(a->isReal() && b->isReal())ok = a->what.realValue >= b->what.realValue;
    else throw Error::TypeCheck();
    instance->opstack.pushbool(ok);
}

void Interpreter::exec_le() {
	Object *b=pop_operand();
	Object *a=pop_operand();
	bool ok;
    if(a->GetType()==Object::TOK_STRING && b->GetType()==Object::TOK_STRING)
    	ok = compare(a->getStringStartPtr(), a->length, b->getStringStartPtr(), b->length) <= 0;
    else if(a->isInt() && b->isInt())  ok = a->what.intValue  <= b->what.intValue;
    else if(a->isInt() && b->isReal()) ok = a->what.intValue  <= b->what.realValue;
    else if(a->isReal() && b->isInt()) ok = a->what.realValue <= b->what.intValue;
    else if(a->isReal() && b->isReal())ok = a->what.realValue <= b->what.realValue;
    else throw Error::TypeCheck();
    instance->opstack.pushbool(ok);
}

/*
* array length int
*  dict length int
*string length int
*  name length int
*/
void Interpreter::exec_length() {
	int len;
	Object *obj=pop_operand();
    switch(obj->GetType()) {
    case Object::TOK_ARRAY:    
    case Object::TOK_STRING:
    case Object::TOK_NAME:
		len = obj->length;    	
    	break;
	case Object::TOK_DICT: {
		DictObject *dict = (DictObject *)obj;
		len = dict->size();
		break;
	}
	default:
    	throw Error::TypeCheck();
    }
	push_operand_int(len);
}
void Interpreter::exec_string() {
	StringObject obj=StringObject(pop_operand_int());
    push_operand(&obj);
}

/** any -type- name
returns a name object that identifies the type of the object any. 
The returned name has the executable attribute. */
void Interpreter::exec_type() {	
	Object *obj=pop_operand();
	NameObject name=NameObject(obj->strType());	// ez jo, mert bemasoljuk mielott megszunne..	
	name.SetExecFlag(true);
	push_operand(&name);
}

// prints a string
void Interpreter::exec_print() {
	Object *obj=pop_operand();
	obj->CheckString();
	char *str = (char *)obj->getStringStartPtr();
	for(int i=0; i<obj->length; i++) {
		putchar(*str++);
	}
}

void Interpreter::exec_array_token_end() {
	exec_counttomark();
    exec_array();
    exec_astore();
    exec_exch();
    exec_pop();
}
/* Creates an executable array object on top of operand stack
 * lehet, hogy ezeknek csak a gepi kodjat kellene letenni es exec() ?
 *--interpreter_state;
 */
void Interpreter::exec_array_token_exe_end() {
	exec_array_token_end();
	exec_cvx();
}
/**
counttomark 2 idiv dup dict begin {def} repeat pop currentdict end */
void Interpreter::exec_dict_token_end() {
	exec_counttomark();
    push_operand_int(2);
    exec_idiv();    // # of key-value pairs
    exec_dup();
    exec_dict();
    exec_begin();	// making it the currentdict
	int count = pop_operand_int();
	while(count-- > 0) {
		exec_def();
	}	
    exec_pop();			// removes starting mark
    exec_currentdict();	// push the current dictionary to the operand stack
    exec_end(); 		// pops the current dictionary off the dictionary stack, making the dictionary below it the current dictionary.
}

void Interpreter::exec_kiiras() { // operator: any = % (____) cvs print (\n) print
	StringObject temp = StringObject(TOKEN_BUFLEN);
	push_operand(&temp);
	exec_cvs();
	exec_print();
	temp.free(); // destroy temporary string object
	putchar('\n');
}

/* operator */void Interpreter::exec_exec() {
	push_execstack(pop_operand());
}

void Interpreter::exec_countexecstack() {push_operand_int(instance->exstack.size());}
/** array -execstack- subarray % Copy execution stack into array */
void Interpreter::exec_execstack() {
	ArrayObject *arr = (ArrayObject *)pop_operand();
	arr->CheckType(Object::TOK_ARRAY);
	int n=instance->exstack.size();
	if(arr->length < n) throw Error::RangeCheck();
	for(int i=0; i<n; i++) {
		arr->put(i, &instance->exstack[i]);
	}
	ArrayObject subarray(arr);
	subarray.length=n;
	push_operand(&subarray);
}

/* operator */void Interpreter::exec_quit() {instance->interpreter_state=QUIT_STATE;}
/** proc -bind- proc
replaces executable operator names with their values within a procedure. 
bind applies to all nested procedure bodies recursively. */
void Interpreter::exec_bind() {
	Object *obj=pop_operand();
	obj->CheckExArray();
	instance->bind_recursive((ArrayObject *)obj);
	push_operand(obj);
}

/** @@@
array1 array2 copy subarray2
dict1 dict2 copy dict2
string1 string2 copy substring2
packedarray1 array2 copy subarray2
gstate1 gstate2 copy gstate2 */
/* operator */void Interpreter::exec_copy() {
	Object obj2 = *pop_operand();
	if(obj2.isInt()) { /*
		(a) (b) (c) 2 copy => (a) (b) (c) (b) (c)
		(a) (b) (c) 0 copy => (a) (b) (c) */
		int n = obj2.what.intValue;
		if(n < 0) throw Error::RangeCheck();
		int x=instance->opstack.size() - n;
		while(n-- > 0) {
			push_operand( &instance->opstack[x++] );
		}		
	}
	else {
		Object obj1 = *pop_operand();
		if(obj1.GetType()!=obj2.GetType()) throw Error::TypeCheck();
		if(obj1.length > obj2.length) throw Error::RangeCheck();
		switch(obj1.GetType()) {
			case Object::TOK_STRING: {
				byte *src=obj1.getStringStartPtr();
				byte *dst=obj2.getStringStartPtr();
				for(int x=0; x<obj1.length; x++) *dst++ = *src++;
				obj2.length=obj1.length;
				push_operand(&obj2);
				break;
			}
			case Object::TOK_ARRAY: {
				if(obj2.GetExecFlag()) throw Error::InvalidAccess();
				Object *src=obj1.getObjectStartPtr();
				Object *dst=obj2.getObjectStartPtr();
				for(int x=0; x<obj1.length; x++) *dst++ = *src++;
				obj2.length=obj1.length;
				push_operand(&obj2);
				break;
			}
			case Object::TOK_DICT: {
				Object *src=obj1.getObjectStartPtr();
				Object *dst=obj2.getObjectStartPtr();
				for(int x=0; x<obj1.length; x++) {
					*dst++ = *src++;
					*dst++ = *src++;
				}
				obj2.length=obj1.length;
				push_operand(&obj2);
				break;
			}
			case Object::TOK_GSTATE:
				break;
			default:
				throw Error::TypeCheck();
		}
	}	
}

/** string seek	--anchorsearch--	post match true  | string false	% Search for seek at start of string */
/* operator */void Interpreter::exec_anchorsearch() {
	Object *seek = pop_operand();
	Object *string = pop_operand();
	if(string->length >= seek->length) {
		if(anchorsearch(string->getStringStartPtr(), seek->getStringStartPtr(), seek->length)) {
			StringObject post = StringObject(string);
			post.start=seek->length;
			post.length-=seek->length;
			push_operand(&post);
			push_operand(seek);
			instance->opstack.pushbool(true);
			return;
		}
	}
	push_operand(string);
	instance->opstack.pushbool(false);
}

/* string seek	--search-- 		post match pre true | string false	% Search for seek in string */
/* operator */void Interpreter::exec_search() {
	throw Error::Undefined();	// @TODO
}

/** 
file --token-- any true | false
string --token-- post any true | false */
/* operator */void Interpreter::exec_token() {
	Object *obj=pop_operand();	
	bool ok;
	switch(obj->GetType()) {
		case Object::TOK_FILE:
			ok=Scanner::GetStreamToken((FileObject *)obj, &token);			
			break;
		case Object::TOK_STRING: {
			StringObject post;
			ok=Scanner::GetStringToken((StringObject *)obj, &post, &token);
			if(ok) push_operand(&post);
			break;
		}			
		default:
			throw Error::TypeCheck();
	}
	if(ok) {
		Object any=instance->CreateObject(&token);
		push_operand(&any);
	}
	instance->opstack.pushbool(ok);
}

/* operator */void Interpreter::exec_save() {
	SaveObject save;	
	push_operand(&save);
}

/* operator */void Interpreter::exec_restore() {	
	Object *obj;
    do {
		obj=pop_operand();
		instance->FreeObject(obj);
    } while (obj->GetType()!=Object::TOK_SAVE);	
	// free and clean entries from dictionaries created since the last save
	Object *ptr;
	DictObject *dict;
	unsigned ts = obj->what.uintValue;
	for (int i=instance->dictstack.size()-1; i>0; --i) {
		dict = (DictObject *) &instance->dictstack[i];
		// Search for obj as key in the next dictionary
		ptr=dict->getObjectPtr();
		for(int j=0; j<dict->length; j++, ptr+=2) {
			obj=ptr;
			if(obj->isComposite() && obj->getTimeStamp() > ts) {
				obj->free();
				Object::make_null(obj);
			}
			obj=ptr+1;
			if(obj->isComposite() && obj->getTimeStamp() > ts) {
				obj->free();
				Object::make_null(obj);
			}
		}
	}
	// Remove dictionaries above userdict created since the last save
	while(instance->dictstack.size() > 2) {
		obj = instance->dictstack.topitem();
		if(obj->getTimeStamp() > ts) {
			obj->free();
			instance->dictstack.pop();
		}
	}
}

/* operator */void Interpreter::exec_meminfo() {GVmem.print_free();}

/* static */void Interpreter::exec_operator(int index) {
    if (index>=0 && index<(int)(sizeof(Interpreter::operators)/sizeof(Procedure))) {
		(*Interpreter::operators[index])();
    }
}

/* operator */void Interpreter::exec_stack() {
	printf("Operand stack(%d):\n", instance->opstack.size());
	instance->opstack.print();
}

/** @@@ TODO == */
/* operator */void Interpreter::exec_pstack() {
	printf("Operand stack(%d):\n", instance->opstack.size());
	instance->opstack.print();
}

/* operator */void Interpreter::exec_pexecstack() {
	printf("Execution stack(%d):\n", instance->exstack.size());
	instance->exstack.print();
}

/*
* Control statements
*/

/* operator */void Interpreter::exec_if() {
	Object *obj=pop_operand();
	obj->CheckExArray();
	if(instance->opstack.popbool())
		push_execstack(obj);
}
/* operator */void Interpreter::exec_ifelse() {
	Object *obj2=pop_operand();
	Object *obj1=pop_operand();
	obj2->CheckExArray();obj1->CheckExArray();
	push_execstack( instance->opstack.popbool() ? obj1 : obj2 );
}

/**
If the first operand is empty (that is, has length 0), forall does not execute proc at all.
array proc forall -
 dict proc forall -
string proc forall -
input: array|proc|dict|string proc forall ==>
exec stack init:
	@forall-<string|array|dict>
	{dict-index}
	obj <array|proc|dict|string>
	proc
	@loop-mark
*/
void Interpreter::exec_forall() {
	Object *proc = pop_operand();
	Object *obj = pop_operand();
	proc->CheckExArray();
	SystemTable::OperatorIndex conx;
	int type=obj->GetType();
	switch(type) {
		case Object::TOK_STRING:
			if(obj->length==0) return;
			conx = SystemTable::op_forall_string_cont; 
			break;
		case Object::TOK_ARRAY:	
			if(obj->length==0) return;
			conx = SystemTable::op_forall_array_cont;
			break;
		case Object::TOK_DICT: {
			DictObject *dict = (DictObject *)obj;
			if(dict->size()==0) return;
			conx = SystemTable::op_forall_dict_cont;
			break;
		}
		default: throw Error::TypeCheck();
	}
	push_execstack(&loopmark);
	push_execstack(proc);
	push_execstack(obj);
	if(type==Object::TOK_DICT) push_execstack_int(0);
	push_execstack(&SystemTable::sysnameindex[conx].obj);
}

void Interpreter::exec_forall_string_cont() {
	StringObject *obj = (StringObject *)pop_execstack();
	Object *proc= pop_execstack();
	if(obj->size() > 0) {
		push_operand_int(obj->get(0));
		obj->tailor();
		push_execstack(proc);
		push_execstack(obj);
		push_execstack(&SystemTable::sysnameindex[SystemTable::op_forall_string_cont].obj);
		push_execstack(proc);
	}
	else pop_loopmark();
}

void Interpreter::exec_forall_array_cont() {
	ArrayObject *obj = (ArrayObject *)pop_execstack();
	Object *proc= pop_execstack();
	if(obj->size() > 0) {
		push_operand(obj->get(0));
		obj->tailor();
		push_execstack(proc);
		push_execstack(obj);
		push_execstack(&SystemTable::sysnameindex[SystemTable::op_forall_array_cont].obj);
		push_execstack(proc);
	}
	else pop_loopmark();
}

void Interpreter::exec_forall_dict_cont() {
	int x=pop_execstack_int();
	DictObject *dict = (DictObject *)pop_execstack();
	Object *proc= pop_execstack();
	if(x < dict->size()) {
		Object key, val;
		dict->get(x, &key, &val);
		push_operand(&key);
		push_operand(&val);
		push_execstack(proc);
		push_execstack(dict);
		push_execstack_int(++x);
		push_execstack(&SystemTable::sysnameindex[SystemTable::op_forall_dict_cont].obj);
		push_execstack(proc);		
	}
	else pop_loopmark();	
}

/** 
input: 5 proc repeat	==>
exec stack init:
	@repeat
	proc
	5
	@loop-mark
*/	
void Interpreter::exec_repeat() {
	Object *proc = pop_operand();
	int count = pop_operand_int();
	proc->CheckExArray();
	push_execstack(&loopmark);
	push_execstack_int(count);
	push_execstack(proc);
	exec_repeat_cont();
}

void Interpreter::exec_repeat_cont() {
	Object *proc = pop_execstack();
	int count = pop_execstack_int();	
	if(count!=0) {
		push_execstack_int(--count);
		push_execstack(proc);
		push_execstack(&SystemTable::sysnameindex[SystemTable::op_repeat_cont].obj);
		push_execstack(proc);
	}
	else pop_loopmark();
}

/**
input: init inc limit proc for ==>
exec stack init:
	@for
	proc
	limit
	inc
	var
	@loop-mark
*/
void Interpreter::exec_for() {
	Object *proc  = pop_operand();
	Object *limit = pop_operand();
	Object *inc  = pop_operand();
	Object *init = pop_operand();
	Object var;
	proc->CheckExArray();
	limit->CheckNumber();
	inc->CheckNumber();
	init->CheckNumber();
	if(init->isReal() || inc->isReal()) {
		float fval;
		if(init->isReal()) fval=init->what.realValue; else fval=init->what.intValue;
		Object::MakeRealObject(&var, fval);
	} 
	else var.copy(init);
	push_execstack(&loopmark);
	push_execstack(&var);
	push_execstack(inc);
	push_execstack(limit);
	push_execstack(proc);
	exec_for_cont();
}

void Interpreter::exec_for_cont() {
	Object *proc = pop_execstack();
	Object *limit = pop_execstack();
	Object *inc = pop_execstack();
	Object *var = pop_execstack();
	bool terminate;
	if(inc->isPositive()) {
		terminate = var->isGreaterThan(limit);
	}
	else if(inc->isNegative()) {
		terminate = var->isLessThan(limit);
	}
	if(!terminate) {
		Object nxt=*var;
		nxt.Add(inc);
		push_operand(var);
		push_execstack(&nxt);
		push_execstack(inc);
		push_execstack(limit);
		push_execstack(proc);
		push_execstack(&SystemTable::sysnameindex[SystemTable::op_for_cont].obj);
		push_execstack(proc);
	} 
	else pop_loopmark();
}

/**
input: proc loop ==>
exec stack init:
	@loop
	proc
	@loop-mark
*/
void Interpreter::exec_loop() {
	Object *proc = pop_operand();
	proc->CheckExArray();
	push_execstack(&loopmark);
	push_execstack(proc);
	exec_loop_cont();
}

void Interpreter::exec_loop_cont() {
	Object *proc = instance->exstack.topitem();
	push_execstack(&SystemTable::sysnameindex[SystemTable::op_loop_cont].obj);
	push_execstack(proc);
}

void Interpreter::exec_exit() {
	Object *obj = pop_execstack();
	while(obj->GetType()!=Object::TOK_LOOPMARK)
		obj = pop_execstack();
}

/**
search for @stopped mark down in execstack. */
void Interpreter::exec_stop() {
	bool found = false;
	Object *stopped = &SystemTable::sysnameindex[SystemTable::op_stopped_cont].obj;
	while(!instance->exstack.empty()) {	
		Object *obj = pop_execstack();
		if(*obj==*stopped) {
			found = true;
			break;
		}
	}
	if(found) {
		// continue after stopped context
		// @TODO: prepare errordict with stop
		instance->opstack.pushbool(true);
	}
	else {
		// exit from main loop
		// @TODO: prepare errordict with stop
		exec_quit();
	}
}

/** input: any -stopped- bool %% any=(executable array|executable file|executable string)
exec stack init:
	any
	@stopped
*/
void Interpreter::exec_stopped() {
	Object *any = pop_operand();
	push_execstack(&SystemTable::sysnameindex[SystemTable::op_stopped_cont].obj);
	push_execstack(any);
}

void Interpreter::exec_stopped_cont() {
	instance->opstack.pushbool(false);
}

/** filename access --file-- file % creates a file object for the file identified by filename, accessing it as specified by access. */
/* operator */void Interpreter::exec_file() {
	char filename[MAX_FILENAME], fmode;
	StringObject *sao = (StringObject *)pop_operand();
	StringObject *sbo = (StringObject *)pop_operand();
	sao->CheckString();
	sbo->CheckString();
	fmode = (char)sao->get(0);
	FileObject fileobj = FileObject(sbo->getString(filename, MAX_FILENAME), &fmode);	
	push_operand(&fileobj);
}

/** file --closefile-- */
/* operator */void Interpreter::exec_closefile() {
	FileObject *foo=pop_operand_file();	
	foo->close();
}

/** 
file --read-- int true (if not end-of-file)
                  false (if end-of-file)
reads the next character from the input file file, pushes it on the operand stack as
an integer, and pushes true as an indication of success. If an end-of-file indication
is encountered before a character has been read, read returns false. */
/* operator */void Interpreter::exec_read() {
	FileObject *fob=pop_operand_file('r');	
	int ch=fob->getNextChar();
	bool noeof = (ch!=ISEOF);
	if(noeof) instance->opstack.pushint(ch);
	instance->opstack.pushbool(noeof);
}

/** --write--
Description */
/* operator */void Interpreter::exec_write() {
	throw Error::Unknown(); // @TODO
}

/** file string --readhexstring-- substring bool
reads characters from file, expecting to encounter a sequence of hexadecimal
digits 0 to 9 and A through F (or a through f). readhexstring interprets each successive 
pair of digits as a two-digit hexadecimal number representing an integer
value in the range 0 to 255. It then stores these values into successive elements of
string starting at index 0 until either the entire string has been filled or an end-of-file 
indication is encountered in file. Finally, readhexstring returns the substring
of string that was filled and a boolean value indicating the outcome (true normally, 
false if end-of-file was encountered before the string was filled).
readhexstring ignores any characters that are not valid hexadecimal digits, so the
data in file may be interspersed with spaces, newlines, and so on, without changing 
the interpretation of the data */
/* operator */void Interpreter::exec_readhexstring() {
	throw Error::Unknown(); // @TODO
}

/** --writehexstring--
Description */
/* operator */void Interpreter::exec_writehexstring() {
	throw Error::Unknown(); // @TODO
}

/** file string --readstring-- substring bool
reads characters from file and stores them into successive elements of string until either
the entire string has been filled or an end-of-file indication is encountered in file. 
readstring then returns the substring of string that was filled and a boolean value indicating the outcome
(true normally, false if end-of-file was encountered before the string was filled). 
If string is zero-length, a rangecheck error occurs.
All character codes are treated the same—as integers in the range 0 to 255. */
/* operator */void Interpreter::exec_readstring() {
	StringObject *sobj=(StringObject *)instance->opstack.pop();
	sobj->CheckString();
	if(sobj->length==0) throw Error::RangeCheck();	
	FileObject *fobj=pop_operand_file('r');
	int bytesread;
	bool noeof = Scanner::GetStreamString(fobj, sobj, &bytesread);	// true if normal
	StringObject subs=StringObject(sobj);
	subs.length=bytesread;
	instance->opstack.push(&subs);
	instance->opstack.pushbool(noeof);
}

/** --writestring--
Description */
/* operator */void Interpreter::exec_writestring() {
	throw Error::Unknown(); // @TODO
}

/** file string --readline-- substring bool
reads a line of characters (terminated by a newline character) from file and stores
them into successive elements of string. readline returns the substring of string
that was filled and a boolean value indicating the outcome (true normally, false if
end-of-file was encountered before a newline character was read).
The terminating newline character is not stored into string or included at the end
of the returned substring. If readline completely fills string before encountering a
newline character, a rangecheck error occurs */
/* operator */void Interpreter::exec_readline() {
	Object *sobj=pop_operand();
	sobj->CheckString();
	if(sobj->length==0) throw Error::RangeCheck();	
	FileObject *fobj=pop_operand_file('r');
	int bytesread;	
	bool noeof = Scanner::GetStreamLine(fobj, (StringObject *)sobj, &bytesread);	// true if normal
	StringObject subs=StringObject(sobj);
	subs.length=bytesread;
	instance->opstack.push(&subs);
	instance->opstack.pushbool(noeof);
}

/** file --bytesavailable-- int */
/* operator */void Interpreter::exec_bytesavailable() {
	FileObject *fob=pop_operand_file();
	push_operand_int(fob->getBytesAvailable());
}

/** --flush-- */
/* operator */void Interpreter::exec_flush() {}

/** file --flushfile-- */
/* operator */void Interpreter::exec_flushfile() {
	FileObject *fob=pop_operand_file();
	fob->flushFile();
}

/** file --resetfile-- */
/* operator */void Interpreter::exec_resetfile() {
	FileObject *fob=pop_operand_file();
	fob->resetBuffer();
}

/** --status--
Description */
/* operator */void Interpreter::exec_status() {
	throw Error::Unknown(); // @TODO
}

/** --currentfile-- file % Return file currently being executed
returns the file object the topmost file object on the execution stack. 
The returned file has the literal attribute.
If there is no file object on the execution stack, returns an invalid file object.*/
/* operator */void Interpreter::exec_currentfile() {
	Object* obj;
	bool found = false;	
	for (int x=instance->exstack.size()-1; x>=0; --x) {
        obj = &instance->exstack[x];
		if(obj->GetType()==Object::TOK_FILE) {
			found = true;
			break;
    	}
    }		
	if(found) { // FIXED Segmentation fault: lit.copy(obj);
		FileObject lit;
		lit.token=Object::TOK_FILE;
		lit.pad=obj->pad;
		lit.start=obj->start;
		lit.length=obj->length;
		lit.what.lexp=obj->what.lexp;	// Ez a megoldas itt !!!		
		push_operand(&lit);
	} else {
		FileObject nofile=FileObject();
		push_operand(&nofile);
	}
}

/** filename --deletefile-- */
/* operator */void Interpreter::exec_deletefile() {
	Object *obj=pop_operand();
	obj->CheckString();
	throw Error::Unknown(); // @TODO
}

/** filename1 filename2 --renamefile-- */
/* operator */void Interpreter::exec_renamefile() {
	Object *obj2=pop_operand();
	Object *obj1=pop_operand();
	obj2->CheckString();
	obj1->CheckString();
	throw Error::Unknown(); // @TODO
}

/** template proc scratch --filenameforall--
enumerates all files whose names match the specified template string. For each
matching file, filenameforall copies the file’s name into the supplied scratch string,
pushes a string object designating the substring of scratch actually used, and calls proc. 
filenameforall does not return any results of its own, but proc may do so. 

*/
/* operator */void Interpreter::exec_filenameforall() {
	throw Error::Unknown(); // @TODO
}

/** file pos --setfileposition-- */
/* operator */void Interpreter::exec_setfileposition() {
	int pos=pop_operand_int();
	FileObject *fob=pop_operand_file();
	fob->setFilePos(pos);
}

/** file --fileposition-- position
returns the current position in an existing open file. The result is a nonnegative
integer interpreted as number of bytes from the beginning of the file. If the file
object is not valid or the underlying file is not positionable, an ioerror occurs */
/* operator */void Interpreter::exec_fileposition() {
	FileObject *fob=pop_operand_file();
	push_operand_int(fob->getFilePos());
}

Object Interpreter::CreateObject(Token* token) {
    switch(token->token_type) {
    	case Token::LEX_INT:
        	return IntegerObject(token->uni.intValue);
    	case Token::LEX_REAL:
        	return RealObject(token->uni.realValue);
    	case Token::LEX_STRING:
        case Token::LEX_HEXSTRING:
        case Token::LEX_STRING85:
        	return StringObject(token->uni.strValue);
	    case Token::LEX_NAME_LIT:
        	return NameObject(token->uni.strValue);
	    case Token::LEX_NAME: {			
        	NameObject name=NameObject(token->uni.strValue);
            name.SetExecFlag(true);
            return name;
        }
	    case Token::LEX_NAME_IE:{	// Immediate evaluated names ???
        	Object *obj=bind(token->uni.strValue,true);
            return *obj;
        }
	    case Token::LEX_ARRAY:
	    case Token::LEX_ARRAY_EXE:
	    case Token::LEX_ARRAY_END:
	    case Token::LEX_ARRAY_EXE_END:
	    case Token::LEX_DICT:
	    case Token::LEX_DICT_END://mert ilyen okos vagyok
        	return *SystemTable::bind(token->uni.strValue);
    	default: 
			// Token::LEX_EOF
			// Token::LEX_ERROR
			throw Error::Unknown();
    }
}

void Interpreter::FreeObject(Object *obj) {
	switch(obj->GetType()) {
		//case Object::TOK_FILE: break;
		//case Object::TOK_SAVE: break;
		//case Object::TOK_GSTATE: break;
		case Object::TOK_NAME: {
			if(obj->pad!=0) return;	// Don't free system operator names
			/**
			 * Search the name from top until userdict..
			 * Don't search in the system dict at stack[0]
			 */
			DictObject *dict;
			for (int i=instance->dictstack.size()-1; i>0; --i) {
				dict = (DictObject *) &instance->dictstack[i];
				// Search for obj as key in the dictionary				
				bool found=false;
				Object *ptr=dict->getObjectPtr();
				for(int j=0; j<dict->length; j++, ptr+=2) {
					if(*ptr==*obj) {
						found=true;
						break;
					}
				}
				if(found) {
					// The name object found in the dict
					// free key, value pair
					ptr->free(); Object::make_null(ptr++);
					ptr->free(); Object::make_null(ptr);
					break;
				}
			}
			break;
		}
		case Object::TOK_DICT:
			if(dictstack.size() > 2) {
				Object *top = dictstack.topitem();
				if(*top==*obj) dictstack.pop();		// remove from the dictionary stack
			}
			// fallthrough
		case Object::TOK_STRING:
		case Object::TOK_ARRAY:
			obj->free();
			break;
	}	
}

/*
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
*/

void Interpreter::ProcessObject(Object *obj) {    
	if(obj->token==Object::EXEC_BIT+Object::TOK_OPERATOR) {
    	switch(obj->what.intValue){
			case SystemTable::op_array_exe_token:	// '{'
				interpreter_state++;
				push_execstack(obj);
				return;
			case SystemTable::op_array_exe_end_token: // '}'
				interpreter_state--;
				push_execstack(obj);
				return;
        }
    }
    if (!interpreter_state && obj->GetExecFlag()) {
		switch(obj->GetType()) {
			case Object::TOK_NULLA:
				return; 
			case Object::TOK_MARK:
			case Object::TOK_INTEGER:
			case Object::TOK_BOOL:
			case Object::TOK_REAL:				
			case Object::TOK_STRING:			// Executable string
				return push_operand(obj);
			case Object::TOK_OPERATOR:
			case Object::TOK_NAME:
				push_execstack(obj);	// Executable name
				return;
			case Object::TOK_ARRAY:				// Executable array
				push_operand(obj);
				push_execstack(obj);	// let's main_loop handle it..
				return;
			default:
				throw Error::Unknown();
		}
    }
    else push_operand(obj);
}

/* Handle executable name in loop */
void Interpreter::handle_name(Object *key) {	
	Object *obj=bind(key,true);
	if(obj->GetExecFlag())
		push_execstack(obj);
	else push_operand(obj);	
}

void Interpreter::handle_operator(Object *obj) {
	exec_operator(obj->what.intValue);
}

/** Hogy kerult ide ??? pl. (10 20 add) cvx exec */
void Interpreter::handle_string(StringObject *obj) {
	StringObject post;
	if (Scanner::GetStringToken(obj, &post, &token)) {
		push_execstack(&post);				// visszatenni a string maradekat
		Object any = CreateObject(&token);
		ProcessObject(&any);				// visszatenni a tokenbol keszult objektumot		
	}
}

/* @TODO: Review handling of normal and execurable arrays */
void Interpreter::handle_array(ArrayObject *arr) {
	Object *obj;
	switch(arr->size()) {
		case 0:	// exst: { } =>
			return;
		case 1: // exst: { one } => one
			obj=arr->get(0);
			break;
		default: // exst: { one two three } => { two three } one
			obj=arr->get(0);	// head
			arr->tailor();
			push_execstack(arr);	// tail
			break;
	}
	if(obj->GetType()!=Object::TOK_ARRAY && obj->GetExecFlag())
		 push_execstack(obj);
	else push_operand(obj);
}

void Interpreter::handle_file(FileObject *file) {
	if (file->pad!='r') throw Error::InvalidFileAccess();
	if (Scanner::GetStreamToken(file, &token)) {
		// @DEBUG
		if(tokenFlag) token.print("==> ");
		exstack.push(file);	// Amig nincs eof visszarakjuk a file object
		Object obj=CreateObject(&token);	// Create object from token
		ProcessObject(&obj);			// Push what it needs to ..
	}
	else file->close();
}

/**
feldolgozza az execstack top elemet.
feldolgozas utan kiveszi a stackbol.
Hiba vagy megszakitas eseten az obj marad.
file object
	write v stdout csak fileclose
	read v stdin csak eof 
utan kerul ki.
array csak miutan elfogyott azutan kerul ki.
operator csak vegrehajtas utan kerul ki.

any -exec-
packedarray -exec-
string -exec-
file exec % applied to an input file, causes the PostScript interpreter to execute a program.
executing a literal object will cause it only to be pushed back on the operand stack. 
Executing a procedure, however,will cause the procedure to be called. */
void Interpreter::exec_step() {
	Object *obj=exstack.pop();
	currObject=*obj;
	if(obj->GetExecFlag()) {		
		switch(obj->GetType()) {
			case Object::TOK_FILE:
				handle_file((FileObject *)obj);
				break;
			case Object::TOK_OPERATOR:
				handle_operator(obj);
				break;
			case Object::TOK_NAME:
				handle_name(obj);
				break;
			case Object::TOK_ARRAY:
				handle_array((ArrayObject *)obj);
				break;
			case Object::TOK_STRING:
				handle_string((StringObject *)obj);	
				break;
			case Object::TOK_NULLA: // An executable null object causes the interpreter to perform no action
				break;
			default: // other executable NUMBER, MARK, DICT, SAVE, GSTATE
				opstack.push(obj);
				break;
		}
	} 
	else opstack.push(obj);	// literal objects are put onto operand stack
}

void Interpreter::start() {
	while(interpreter_state!=QUIT_STATE && !exstack.empty()) {
		if(debugFlag) debug(true);
		try {
			exec_step();
		}
		catch(const Error::Exception &e) {
			/** Standard error handler process:
			1. Push the object caused the error on the stack.
			2. Lookup the error name in the dictionaries and execute its handler.
			3. The handler will prepare the error info, prints error message and call stop. */
			opstack.push(&currObject);
			Object *obj = bind((char *)e.what(), false);
			if(!obj) obj = errordict.lookup((char *)e.what());
			ProcessObject(obj);
			/*
			printf("*** Exception: %s -- %s\n", e.what(), e.message());
			exec_stop();
			*/
		}
		interpreter_steps++;
	}
}

void Interpreter::run(char *filename) {
	FileObject fileobj=FileObject(filename,"rt");
	fileobj.SetExecFlag(true);
	exstack.push(&fileobj);
	start();
}

void Interpreter::debug(bool need_pause) {
	puts("--------------------");
	printf("Operand stack: %d\n", opstack.size()); opstack.print();
	printf("Exec stack: %d\n", exstack.size()); exstack.print();
	if(need_pause) pause();
}

/* EOF */
#ifndef Interpreter_H
#define Interpreter_H

#include "Main.h"
#include "VMem.h"
#include "Scanner.h"
#include "ObjectStack.h"
#include "SystemTable.h"
#include "utils.h"

extern VMem GVmem;

typedef void (*Procedure)();

class Interpreter {
public:

	enum { QUIT_STATE=-999 };

	ObjectStack exstack;
	ObjectStack opstack;
	ObjectStack dictstack;
	DictObject userdict;
	DictObject systemdict;
	DictObject errordict;

	Interpreter() :
		exstack(DEFAULT_EXSTACK_SIZE),
		opstack(DEFAULT_OPSTACK_SIZE),
		dictstack(DEFAULT_DICTSTACK_SIZE),
		userdict(DEFAULT_USERDICT_SIZE,     Object::Access::ACCESS_UNLIMITED),
		systemdict(DEFAULT_SYSTEMDICT_SIZE, Object::Access::ACCESS_UNLIMITED),
		errordict(DEFAULT_ERRORDICT_SIZE,   Object::Access::ACCESS_UNLIMITED) {
			instance = this;
			interpreter_state=0;// NORMAL EXECUTION MODE, DEFERRED MODE<0, QUIT_STATE=-999
			interpreter_steps=0;
			systemdict_setup();
			// init dictionary stack with userdict on top like "systemdict begin userdict begin"
			dictstack.push( &systemdict );
			dictstack.push( &userdict );
			////////////////////////////////////////////////
			bool DEBUG_tokenFlag=tokenFlag; tokenFlag=false;
			bool DEBUG_debugFlag=debugFlag; debugFlag=false;
			run((char *)"init.ps");
			tokenFlag=DEBUG_tokenFlag;
			debugFlag=DEBUG_debugFlag;
			systemdict.pad=Object::Access::ACCESS_READONLY;
	}
	
	~Interpreter() {
		fprintf(stderr, "DEBUG: ~Interpreter()\n");
	}
	
	void start();
	void run(char *fname);
	void debug(bool need_pause = false);

private:
	static Interpreter *instance;
	static Object nullObject;
	static BoolObject trueObject;
	static BoolObject falseObject;
	static LoopMarkObject loopmark;
	static Object currObject;
	static Token token;
	static unsigned _seed;
	static Procedure operators[];
	
	static void exec_operator(int idx);	
	static void scroll(Object *array, int length);	// up
	
	static void push_execstack(Object *obj);	
	static void push_operand(Object *obj);
	static void push_operand_int(int x);
	static void push_execstack_int(int x);
	static Object *pop_execstack();
	static Object *pop_operand();
	static FileObject *pop_operand_file(int acc=0);
	static int pop_execstack_int();
	static int pop_operand_int();
	static void pop_loopmark();
	
	/*
	* instance variables and methods
	*/	
	short interpreter_state;
	int   interpreter_steps;
	
	void systemdict_setup();
	
	Object *bind(char *name, bool except);
	Object *bind(Object *key, bool except);
	void bind_recursive(ArrayObject *arr);
		
	Object CreateObject(Token *token);
	void FreeObject(Object *obj);
	void ProcessObject(Object *obj);
	void handle_name(Object *key);
	void handle_operator(Object *obj);
	void handle_string(StringObject *obj);
	void handle_array(ArrayObject *arr);
	void handle_file(FileObject *file);
	
	void exec_step();
	
/*
WARNING: Operators table functions must be in order!!!	
*/

// Arithmetical & logical functions
static void exec_not();
static void exec_xor();
static void exec_and();
static void exec_or();
static void exec_bitshift();
static void exec_add();
static void exec_sub();
static void exec_mul();
static void exec_div();
static void exec_idiv();
static void exec_mod();
static void exec_abs();
static void exec_neg();

// Stack manipulation
static void exec_pop();
static void exec_exch();
static void exec_dup();
static void exec_roll();
static void exec_index();
static void exec_clear();
static void exec_count();
static void exec_array();
static void exec_aload();
static void exec_astore();
static void exec_mark();
static void exec_cleartomark();
static void exec_counttomark();
static void exec_put();
static void exec_get();
static void exec_getinterval();
static void exec_putinterval();

static void exec_xcheck();
static void exec_cvi();
static void exec_cvn();
static void exec_cvs();
static void exec_cvx();
static void exec_cvlit();

// Mathematical
static void exec_ceiling();
static void exec_floor();
static void exec_round();
static void exec_truncate();
static void exec_sqrt();
static void exec_atan();
static void exec_cos();
static void exec_sin();
static void exec_exp();
static void exec_ln();
static void exec_log();
static void exec_rand();
static void exec_srand();
static void exec_rrand();

// Dictionary
static void exec_def();
static void exec_dict();
static void exec_begin();
static void exec_end();
static void exec_dictstack();
static void exec_countdictstack();
static void exec_currentdict();
static void exec_cleardictstack();
static void exec_load();
static void exec_store();
static void exec_where();
static void exec_undef();
static void exec_maxlength();
static void exec_known();

// Logical tests
static void exec_eq();
static void exec_ne();
static void exec_gt();
static void exec_lt();
static void exec_ge();
static void exec_le();

// Control
static void exec_if();
static void exec_ifelse();

// Loop
static void exec_for();
static void exec_for_cont();
static void exec_loop();
static void exec_loop_cont();
static void exec_repeat();
static void exec_repeat_cont();
static void exec_forall();
static void exec_forall_string_cont();
static void exec_forall_array_cont();
static void exec_forall_dict_cont();
static void exec_exit();
static void exec_stop();
static void exec_stopped();
static void exec_stopped_cont();

// File
static void exec_file();
static void exec_closefile();
static void exec_read();
static void exec_write();
static void exec_readhexstring();
static void exec_writehexstring();
static void exec_readstring();
static void exec_writestring();
static void exec_readline();
static void exec_bytesavailable();
static void exec_flush();
static void exec_flushfile();
static void exec_resetfile();
static void exec_status();
static void exec_currentfile();
static void exec_deletefile();
static void exec_renamefile();
static void exec_filenameforall();
static void exec_setfileposition();
static void exec_fileposition();

static void exec_token();
static void exec_save();
static void exec_restore();
static void exec_meminfo();
static void exec_stack();
static void exec_pstack();
static void exec_pexecstack();

//etc
static void exec_bind();
static void exec_copy();
static void exec_anchorsearch();
static void exec_search();
static void exec_length();
static void exec_string();
static void exec_type();
static void exec_print();	// string print -
static void exec_kiiras();
static void exec_exec();
static void exec_countexecstack();
static void exec_execstack();
static void exec_quit();

//Token Control
static void exec_array_token_end();//counttomark ?array astore exch pop
static void exec_array_token_exe_end();
static void exec_dict_token_end();
};

#endif
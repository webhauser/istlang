#ifndef SystemTableH
#define SystemTableH

#include "Object.h"

class SystemTable {
public:

// ez most a megvalositas sorrendjeben van
enum OperatorIndex {
op_not=0,
op_xor,
op_and,
op_or,
op_bitshift,
op_add,
op_sub,
op_mul,
op_div,
op_idiv,
op_mod,
op_abs,
op_neg,

// stack manipulation
op_pop,
op_exch,
op_dup,
op_roll,
op_index,
op_clear,
op_count,
op_array,
op_aload,
op_astore,
op_mark,			// mark
op_cleartomark,
op_counttomark,
op_put,
op_get,
op_getinterval,
op_putinterval,

// conversion
op_xcheck,
op_cvi,
op_cvn,
op_cvs,
op_cvx,
op_cvlit,

// mathematical
op_ceiling,
op_floor,
op_round,
op_trunc,
op_sqrt,
op_atan,
op_cos,
op_sin,
op_exp,
op_ln,
op_log,
op_rand,
op_srand,
op_rrand,

// dictionary
op_def,
op_dict,
op_begin,
op_end,
op_dictstack,
op_countdictstack,
op_currentdict,
op_cleardictstack,
op_load,
op_store,
op_where,
op_undef,
op_maxlength,
op_known,

// logical tests
op_eq,
op_ne,
op_gt,
op_lt,
op_ge,
op_le,

// control
op_if,
op_ifelse,

// loop
op_for,
op_for_cont,
op_loop,
op_loop_cont,
op_repeat,
op_repeat_cont,
op_forall,
op_forall_string_cont,
op_forall_array_cont,
op_forall_dict_cont,
op_exit,
op_stop,
op_stopped,
op_stopped_cont,

// file
op_file,
op_closefile,
op_read,
op_write,
op_readhexstring,
op_writehexstring,
op_readstring,
op_writestring,
op_readline,
op_bytesavailable,
op_flush,
op_flushfile,
op_resetfile,
op_status,
op_currentfile,
op_deletefile,
op_renamefile,
op_filenameforall,
op_setfileposition,
op_fileposition,

op_token,
op_save,
op_restore,
op_meminfo,
op_stack,
op_pstack,
op_pexecstack,

//etc
op_bind,
op_copy,
op_anchorsearch,
op_search,
op_length,
op_string,
op_type,
op_print,	// OK
op_kiiras,
op_exec,
op_countexecstack,
op_execstack,
op_quit,

op_array_token,			// [ : mark
op_array_end_token, 	// ] : counttomark array astore exch pop
op_array_exe_token,		// { : mark
op_array_exe_end_token,	// } :
op_dict_token,			// << : mark
op_dict_token_end		// >> :
};

const char *name;
Object obj;
OperatorIndex index;	
SystemTable(const char *namep, Object o, OperatorIndex num=op_quit) : name(namep), obj(o), index(num) {}
static SystemTable sysnameindex[];
static const int size();    
static const Object *bind(char *name);
};

#endif
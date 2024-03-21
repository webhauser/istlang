#include "SystemTable.h"

/* new nelkul inicializalja a statikus objektum tombot, 
amibol  csak egyetlenegy peldany letezik az egesz osztalyban.
Most egyebkent is csak egy objektum lesz letrehozva a SystemTable konkret osztalybol.

ha mar kesz van akkor meg lehetne csinalni ujra jobban:

	a tomb elem indexe maga a System Name Index.

const SystemTable SystemTable::sysnameindex[] = {
SystemTable("abs",  OperatorObject(0), 0),	// igy van..
SystemTable("add",   OperatorObject(OPX_ADD), OPX_ADD),
SystemTable("aload", OperatorObject(2), 2),
SystemTable("and", OperatorObject(-1), -1),
SystemTable("array", OperatorObject(9), -1),	// 9
SystemTable("astore", OperatorObject(11), -1),
SystemTable("begin", OperatorObject(-1), -1),	// 13
SystemTable("bind", OperatorObject(-1), -1),
SystemTable("bitshift", OperatorObject(-1), -1),
SystemTable("ceiling", OperatorObject(-1), -1),
SystemTable("clear", OperatorObject(-1), -1),
SystemTable("cleartomark", OperatorObject(-1), -1),
SystemTable("concat", OperatorObject(-1), -1),
SystemTable("copy", OperatorObject(-1), -1),
SystemTable("count", OperatorObject(-1), -1),
SystemTable("counttomark", OperatorObject(-1), -1),
SystemTable("currentdict", OperatorObject(-1), -1),
SystemTable("cvi", OperatorObject(-1), -1),
SystemTable("cvlit", OperatorObject(-1), -1),
SystemTable("cvn", OperatorObject(-1), -1),
SystemTable("cvr", OperatorObject(-1), -1),
SystemTable("cvrs", OperatorObject(-1), -1),
SystemTable("cvs", OperatorObject(-1), -1),
SystemTable("cvx", OperatorObject(-1), -1),
SystemTable("def", OperatorObject(-1), -1),
SystemTable("defineusername", OperatorObject(-1), -1),
SystemTable("dict", OperatorObject(-1), -1),
SystemTable("div", OperatorObject(-1), -1),
SystemTable("dup", OperatorObject(-1), -1),
SystemTable("end", OperatorObject(-1), -1),
SystemTable("eq", OperatorObject(-1), -1),
SystemTable("exch", OperatorObject(-1), -1),
SystemTable("exec", OperatorObject(-1), -1),
SystemTable("exit", OperatorObject(-1), -1),
SystemTable("file", OperatorObject(-1), -1),
SystemTable("floor", OperatorObject(-1), -1),
SystemTable("flush", OperatorObject(-1), -1),
SystemTable("flushfile", OperatorObject(-1), -1),
SystemTable("for", OperatorObject(-1), -1),
SystemTable("forall", OperatorObject(-1), -1),
SystemTable("ge", OperatorObject(-1), -1),
SystemTable("get", OperatorObject(-1), -1),
SystemTable("getinterval", OperatorObject(-1), -1),
SystemTable("gt", OperatorObject(-1), -1),
SystemTable("idiv", OperatorObject(-1), -1),
SystemTable("if", OperatorObject(-1), -1),
SystemTable("ifelse", OperatorObject(-1), -1),
SystemTable("index", OperatorObject(-1), -1),
SystemTable("known", OperatorObject(-1), -1),
SystemTable("le", OperatorObject(-1), -1),
SystemTable("length", OperatorObject(-1), -1),
SystemTable("load", OperatorObject(-1), -1),
SystemTable("loop", OperatorObject(-1), -1),
SystemTable("lt", OperatorObject(-1), -1),
SystemTable("mod", OperatorObject(-1), -1),
SystemTable("mul", OperatorObject(-1), -1),
SystemTable("ne", OperatorObject(-1), -1),
SystemTable("neg", OperatorObject(-1), -1),
SystemTable("not", OperatorObject(-1), -1),
SystemTable("null", Object(), 113),	// Special: null object
SystemTable("or", OperatorObject(-1), -1),
SystemTable("pop", OperatorObject(-1), -1),
SystemTable("print", OperatorObject(-1), -1),
SystemTable("printobject", OperatorObject(-1), -1),
SystemTable("put", OperatorObject(-1), -1),
SystemTable("putinterval", OperatorObject(-1), -1),
SystemTable("read", OperatorObject(-1), -1),
SystemTable("readhexstring", OperatorObject(-1), -1),
SystemTable("readline", OperatorObject(-1), -1),
SystemTable("readstring", OperatorObject(-1), -1),
SystemTable("repeat", OperatorObject(-1), -1),
SystemTable("roll", OperatorObject(-1), -1),
SystemTable("round", OperatorObject(-1), -1),
SystemTable("save", OperatorObject(-1), -1),
SystemTable("search", OperatorObject(-1), -1),
SystemTable("stop", OperatorObject(-1), -1),
SystemTable("string", OperatorObject(-1), -1),
SystemTable("sub", OperatorObject(-1), -1),
SystemTable("systemdict", OperatorObject(-1), -1),
SystemTable("token", OperatorObject(-1), -1),
SystemTable("truncate", OperatorObject(-1), -1),
SystemTable("type", OperatorObject(-1), -1),
SystemTable("undef", OperatorObject(-1), -1),	// 180
SystemTable("userdict", OperatorObject(-1), -1),
SystemTable("where", OperatorObject(-1), -1),
SystemTable("write", OperatorObject(-1), -1),
SystemTable("writehexstring", OperatorObject(-1), -1),
SystemTable("writeobject", OperatorObject(-1), -1),
SystemTable("writestring", OperatorObject(-1), -1),
SystemTable("xor", OperatorObject(-1), -1),
SystemTable("execuserobject", OperatorObject(-1), -1),
SystemTable("currentcolor", OperatorObject(-1), -1),
SystemTable("currentglobal", OperatorObject(-1), -1),
SystemTable("execform", OperatorObject(-1), -1),
SystemTable("filter", OperatorObject(-1), -1),
SystemTable("findresource", OperatorObject(-1), -1),
SystemTable("globaldict", OperatorObject(-1), -1),
SystemTable("setcolor", OperatorObject(-1), -1),
SystemTable("setglobal", OperatorObject(-1), -1),
SystemTable("=", OperatorObject(256), -1),	//////////////////////////////
SystemTable("==", OperatorObject(257), -1),	//
SystemTable("[", OperatorObject(260), -1),	//
SystemTable("]", OperatorObject(261), -1),	//
SystemTable("atan", OperatorObject(-1), -1),
SystemTable("bytesavailable", OperatorObject(-1), -1),
SystemTable("closefile", OperatorObject(-1), -1),
SystemTable("cos", OperatorObject(-1), -1),
SystemTable("countdictstack", OperatorObject(-1), -1),
SystemTable("currentobjectformat", OperatorObject(-1), -1),
SystemTable("deletefile", OperatorObject(-1), -1),
SystemTable("detach", OperatorObject(-1), -1),
SystemTable("deviceinfo", OperatorObject(-1), -1),
SystemTable("dictstack", OperatorObject(-1), -1),
SystemTable("echo", OperatorObject(-1), -1),
SystemTable("errordict", OperatorObject(-1), -1),
SystemTable("execstack", OperatorObject(-1), -1),
SystemTable("executeonly", OperatorObject(-1), -1),
SystemTable("exp", OperatorObject(-1), -1),
SystemTable("false", BoolObject(false), 301),	// Itt jon ki szepen, mert a harmadik a system name index
SystemTable("filenameforall", OperatorObject(-1), -1),
SystemTable("fileposition", OperatorObject(-1), -1),
SystemTable("fork", OperatorObject(-1), -1),
SystemTable("initgraphics", OperatorObject(-1), -1),
SystemTable("join", OperatorObject(-1), -1),
SystemTable("ln", OperatorObject(-1), -1),
SystemTable("lock", OperatorObject(-1), -1),
SystemTable("log", OperatorObject(-1), -1),
SystemTable("mark", OperatorObject(-1), -1),
SystemTable("monitor", OperatorObject(-1), -1),
SystemTable("noaccess", OperatorObject(-1), -1),
SystemTable("notify", OperatorObject(-1), -1),
SystemTable("nulldevice", OperatorObject(-1), -1),
SystemTable("quit", OperatorObject(-1), -1),
SystemTable("rand", OperatorObject(-1), -1),
SystemTable("rcheck", OperatorObject(-1), -1),
SystemTable("readonly", OperatorObject(-1), -1),
SystemTable("realtime", OperatorObject(-1), -1),
SystemTable("renamefile", OperatorObject(-1), -1),
SystemTable("resetfile", OperatorObject(-1), -1),
SystemTable("rrand", OperatorObject(-1), -1),
SystemTable("setfileposition", OperatorObject(-1), -1),
SystemTable("setobjectformat", OperatorObject(-1), -1),
SystemTable("sin", OperatorObject(-1), -1),
SystemTable("sqrt", OperatorObject(-1), -1),
SystemTable("srand", OperatorObject(-1), -1),
SystemTable("stack", OperatorObject(-1), -1),
SystemTable("status", OperatorObject(-1), -1),
SystemTable("true", BoolObject(true), 360),
SystemTable("usertime", OperatorObject(-1), -1),
SystemTable("version", OperatorObject(-1), -1),
SystemTable("vmreclaim", OperatorObject(-1), -1),
SystemTable("vmstatus", OperatorObject(-1), -1),
SystemTable("wait", OperatorObject(-1), -1),
SystemTable("defineuserobject", OperatorObject(-1), -1),
SystemTable("undefineuserobject", OperatorObject(-1), -1),
SystemTable("UserObjects", OperatorObject(-1), -1),
SystemTable("cleardictstack", OperatorObject(-1), -1),
SystemTable("A", OperatorObject(-1), -1), // 376
SystemTable("<<", OperatorObject(429), -1),// ennek a kodja 429
SystemTable(">>", OperatorObject(430), -1)// ennek a kodja 430
};

----- ez nem mindegyik, csak amit megcsinaltam:  */

/* Ebben a listaban csak a system operatorok szerepelnek!
 * Az olyan rendszer nevek mint: null, true, false a
 * system dictionary ban bejegyzeskent szerepelnek.
 * Tehat amikhez tenyleg kell procedura
 */

SystemTable SystemTable::sysnameindex[] = {
SystemTable("not",OperatorObject(op_not), op_not),
SystemTable("xor",OperatorObject(op_xor), op_xor),
SystemTable("and",OperatorObject(op_and), op_and),
SystemTable("or", OperatorObject(op_or),  op_or),
SystemTable("bitshift",OperatorObject(op_bitshift), op_bitshift),
SystemTable("add",OperatorObject(op_add), op_add),
SystemTable("sub",OperatorObject(op_sub), op_sub),
SystemTable("mul",OperatorObject(op_mul), op_mul),
SystemTable("div",OperatorObject(op_div), op_div),
SystemTable("idiv",OperatorObject(op_idiv), op_idiv),
SystemTable("mod",OperatorObject(op_mod), op_mod),
SystemTable("abs",OperatorObject(op_abs), op_abs),
SystemTable("neg",OperatorObject(op_neg), op_neg),

// stack manipulation
SystemTable("pop",OperatorObject(op_pop), op_pop),
SystemTable("exch",OperatorObject(op_exch), op_exch),	// ok
SystemTable("dup",OperatorObject(op_dup), op_dup),
SystemTable("roll", OperatorObject(op_roll), op_roll),
SystemTable("index", OperatorObject(op_index),op_index),
SystemTable("clear",OperatorObject(op_clear), op_clear),
SystemTable("count",OperatorObject(op_count), op_count),
SystemTable("array",OperatorObject(op_array), op_array),
SystemTable("aload", OperatorObject(op_aload), op_aload),
SystemTable("astore",OperatorObject(op_astore), op_astore),
SystemTable("mark",OperatorObject(op_mark), op_mark),
SystemTable("cleartomark",OperatorObject(op_cleartomark), op_cleartomark),
SystemTable("counttomark",OperatorObject(op_counttomark), op_counttomark),
SystemTable("put",OperatorObject(op_put), op_put),
SystemTable("get",OperatorObject(op_get), op_get),
SystemTable("getinterval",OperatorObject(op_getinterval), op_getinterval),
SystemTable("putinterval",OperatorObject(op_putinterval), op_putinterval),

// convert
SystemTable("xcheck",OperatorObject(op_xcheck), op_xcheck),
SystemTable("cvi",OperatorObject(op_cvi), op_cvi),
SystemTable("cvn",OperatorObject(op_cvn), op_cvn),
SystemTable("cvs",OperatorObject(op_cvs), op_cvs),
SystemTable("cvx",OperatorObject(op_cvx), op_cvx),
SystemTable("cvlit", OperatorObject(op_cvlit), op_cvlit),

// mathematical
SystemTable("ceiling",OperatorObject(op_ceiling), op_ceiling),
SystemTable("floor",OperatorObject(op_floor), op_floor),
SystemTable("round",OperatorObject(op_round), op_round),
SystemTable("truncate",OperatorObject(op_trunc), op_trunc),
SystemTable("sqrt",OperatorObject(op_sqrt), op_sqrt),
SystemTable("atan",OperatorObject(op_atan), op_atan),
SystemTable("cos",OperatorObject(op_cos), op_cos),
SystemTable("sin",OperatorObject(op_sin), op_sin),
SystemTable("exp",OperatorObject(op_exp), op_exp),
SystemTable("ln",OperatorObject(op_ln), op_ln),
SystemTable("log",OperatorObject(op_log), op_log),
SystemTable("rand",OperatorObject(op_rand), op_rand),
SystemTable("srand",OperatorObject(op_srand), op_srand),
SystemTable("rrand",OperatorObject(op_rrand), op_rrand),

// dictionary
SystemTable("def",OperatorObject(op_def), op_def),
SystemTable("dict",OperatorObject(op_dict), op_dict),
SystemTable("begin",OperatorObject(op_begin), op_begin),
SystemTable("end",OperatorObject(op_end), op_end),
SystemTable("dictstack",OperatorObject(op_dictstack),op_dictstack),
SystemTable("countdictstack",OperatorObject(op_countdictstack), op_countdictstack),
SystemTable("currentdict",OperatorObject(op_currentdict), op_currentdict),
SystemTable("cleardictstack",OperatorObject(op_cleardictstack), op_cleardictstack),
SystemTable("load",OperatorObject(op_load), op_load),
SystemTable("store",OperatorObject(op_store), op_store),
SystemTable("where",OperatorObject(op_where), op_where),
SystemTable("undef", OperatorObject(op_undef), op_undef),
SystemTable("maxlength", OperatorObject(op_maxlength), op_maxlength),
SystemTable("known", OperatorObject(op_known), op_known),

// logical tests
SystemTable("eq",OperatorObject(op_eq), op_eq),
SystemTable("ne",OperatorObject(op_ne), op_ne),
SystemTable("gt",OperatorObject(op_gt), op_gt),
SystemTable("lt",OperatorObject(op_lt), op_lt),
SystemTable("ge",OperatorObject(op_ge), op_ge),
SystemTable("le",OperatorObject(op_le), op_le),

// control
SystemTable("if",OperatorObject(op_if), op_if),
SystemTable("ifelse",OperatorObject(op_ifelse), op_ifelse),

// loop
SystemTable("for",OperatorObject(op_for), op_for),
SystemTable("@for",OperatorObject(op_for_cont), op_for_cont),
SystemTable("loop",OperatorObject(op_loop), op_loop),
SystemTable("@loop",OperatorObject(op_loop_cont), op_loop_cont),
SystemTable("repeat",OperatorObject(op_repeat), op_repeat),
SystemTable("@repeat",OperatorObject(op_repeat_cont), op_repeat_cont),
SystemTable("forall",OperatorObject(op_forall), op_forall),
SystemTable("@forall-string",OperatorObject(op_forall_string_cont), op_forall_string_cont),
SystemTable("@forall-array",OperatorObject(op_forall_array_cont), op_forall_array_cont),
SystemTable("@forall-dict",OperatorObject(op_forall_dict_cont), op_forall_dict_cont),
SystemTable("exit",OperatorObject(op_exit), op_exit),
SystemTable("stop",OperatorObject(op_stop),op_stop),
SystemTable("stopped",OperatorObject(op_stopped),op_stopped),
SystemTable("@stopped",OperatorObject(op_stopped_cont),op_stopped_cont),

// File
SystemTable("file", OperatorObject(op_file), op_file),
SystemTable("closefile", OperatorObject(op_closefile), op_closefile),
SystemTable("read", OperatorObject(op_read), op_read),
SystemTable("write", OperatorObject(op_write), op_write),
SystemTable("readhexstring", OperatorObject(op_readhexstring), op_readhexstring),
SystemTable("writehexstring", OperatorObject(op_writehexstring), op_writehexstring),
SystemTable("readstring", OperatorObject(op_readstring), op_readstring),
SystemTable("writestring", OperatorObject(op_writestring), op_writestring),
SystemTable("readline", OperatorObject(op_readline), op_readline),
SystemTable("bytesavailable", OperatorObject(op_bytesavailable), op_bytesavailable),
SystemTable("flush", OperatorObject(op_flush), op_flush),
SystemTable("flushfile", OperatorObject(op_flushfile), op_flushfile),
SystemTable("resetfile", OperatorObject(op_resetfile), op_resetfile),
SystemTable("status", OperatorObject(op_status), op_status),
SystemTable("currentfile", OperatorObject(op_currentfile), op_currentfile),
SystemTable("deletefile", OperatorObject(op_deletefile), op_deletefile),
SystemTable("renamefile", OperatorObject(op_renamefile), op_renamefile),
SystemTable("filenameforall", OperatorObject(op_filenameforall), op_filenameforall),
SystemTable("setfileposition", OperatorObject(op_setfileposition), op_setfileposition),
SystemTable("fileposition", OperatorObject(op_fileposition), op_fileposition),

SystemTable("token",OperatorObject(op_token), op_token),
SystemTable("save", OperatorObject(op_save), op_save),
SystemTable("restore", OperatorObject(op_restore), op_restore),
SystemTable("meminfo", OperatorObject(op_meminfo), op_meminfo),
SystemTable("stack", OperatorObject(op_stack),op_stack),
SystemTable("pstack", OperatorObject(op_pstack),op_pstack),
SystemTable("pexecstack", OperatorObject(op_pexecstack),op_pexecstack),

//etc
SystemTable("bind",OperatorObject(op_bind),op_bind),
SystemTable("copy",OperatorObject(op_copy),op_copy),
SystemTable("anchorsearch",OperatorObject(op_anchorsearch),op_anchorsearch),
SystemTable("search",OperatorObject(op_search),op_search),
SystemTable("length",OperatorObject(op_length), op_length),
SystemTable("string",OperatorObject(op_string), op_string),
SystemTable("type",OperatorObject(op_type), op_type),
SystemTable("print",OperatorObject(op_print), op_print),// string print -
SystemTable("=", OperatorObject(op_kiiras), op_kiiras),
SystemTable("exec", OperatorObject(op_exec), op_exec),
SystemTable("countexecstack",OperatorObject(op_countexecstack), op_countexecstack),
SystemTable("execstack",OperatorObject(op_execstack), op_execstack),
SystemTable("quit", OperatorObject(op_quit)),

SystemTable("[", OperatorObject(op_array_token), op_array_token),
SystemTable("]", OperatorObject(op_array_end_token), op_array_end_token),
SystemTable("{", OperatorObject(op_array_exe_token), op_array_exe_token),
SystemTable("}", OperatorObject(op_array_exe_end_token), op_array_exe_end_token),
SystemTable("<<", OperatorObject(op_dict_token), op_dict_token),
SystemTable(">>", OperatorObject(op_dict_token_end), op_dict_token_end)
};

const int SystemTable::size(void) {
	return sizeof(sysnameindex) / sizeof(SystemTable);
}

const Object *SystemTable::bind(char *name) {
	int sz = size();
    for (int x=0; x<sz; x++) {
		if(!strcmp(name,sysnameindex[x].name))
        	return &sysnameindex[x].obj;
    }
	return 0;
}

/* EOF */
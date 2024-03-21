#ifndef Error_H
#define Error_H

namespace Error {
	
struct Exception {
	const char *error_name;
	const char *error_message;	
	Exception(const char *errname, const char *errmsg) : error_name(errname), error_message(errmsg) {}
	const char *what() const {return error_name;}
	const char *message() const {return error_message;}
};

struct DictFull           : Exception {DictFull()           : Exception("dictfull", "No more room in dictionary") {}};
struct DictStackOverflow  : Exception {DictStackOverflow()  : Exception("dictstackoverflow", "Too many begin operators") {}};
struct DictStackUnderflow : Exception {DictStackUnderflow() : Exception("dictstackunderflow", "Too many end operators") {}};
struct ExecStackOverflow  : Exception {ExecStackOverflow()  : Exception("execstackoverflow", "Executive stack nesting too deep") {}};
struct Interrupt          : Exception {Interrupt()          : Exception("interrupt", "External interrupt request") {}};
struct InvalidAccess      : Exception {InvalidAccess()      : Exception("invalidaccess", "Attempt to violate access attribute") {}};
struct InvalidExit        : Exception {InvalidExit()        : Exception("invalidexit", "exit not in loop") {}};
struct InvalidFileAccess  : Exception {InvalidFileAccess()  : Exception("invalidfileaccess", "Unacceptable access string") {}};
struct InvalidFont        : Exception {InvalidFont()        : Exception("invalidfont", "Invalid Font resource name or font or CIDFont dictionary") {}};
struct InvalidRestore     : Exception {InvalidRestore()     : Exception("invalidrestore", "Improper restore") {}};
struct IO                 : Exception {IO()                 : Exception("ioerror", "Input/output error") {}};
struct LimitCheck         : Exception {LimitCheck()         : Exception("limitcheck", "Implementation limit exceeded") {}};
struct NoCurrentPoint     : Exception {NoCurrentPoint()     : Exception("nocurrentpoint", "Current point undefined") {}};
struct RangeCheck         : Exception {RangeCheck()         : Exception("rangecheck", "Operand out of bounds") {}};
struct StackOverflow      : Exception {StackOverflow()      : Exception("stackoverflow", "Operand stack overflow") {}};
struct StackUnderflow     : Exception {StackUnderflow()     : Exception("stackunderflow", "Operand stack underflow") {}};
struct SyntaxError        : Exception {SyntaxError()        : Exception("syntaxerror", "PostScript language syntax error") {}};
struct Timeout            : Exception {Timeout()            : Exception("timeout", "Time limit exceeded") {}};
struct TypeCheck          : Exception {TypeCheck()          : Exception("typecheck", "Operand of wrong type") {}};
struct Undefined          : Exception {Undefined()          : Exception("undefined", "Name not known") {}};
struct FileNotFound       : Exception {FileNotFound()       : Exception("undefinedfilename", "File not found") {}};
struct UndefinedResource  : Exception {UndefinedResource()  : Exception("undefinedresource", "Resource instance not found") {}};
struct UndefinedResult    : Exception {UndefinedResult()    : Exception("undefinedresult", "Overflow, underflow, or meaningless result") {}};
struct UnmatchedMark      : Exception {UnmatchedMark()      : Exception("unmatchedmark", "Expected mark not on stack") {}};
struct Unknown            : Exception {Unknown()            : Exception("unregistered", "Internal error") {}};
struct OutOfMemory        : Exception {OutOfMemory()        : Exception("VMerror", "Virtual memory exhausted") {}};
};

#endif
	
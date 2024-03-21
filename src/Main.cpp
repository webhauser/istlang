/*---------------------------------------------------------------------------
2007 (c) webHauser
All rights reserved.
Minden jog fenntartva.
Szoftver tervezés, programozás, forráskód by ifj.Hauser István, webHauser.
---------------------------------------------------------------------------*/
#include "Main.h"
#include "VMem.h"
#include "Interpreter.h"
#include "Error.h"
#include "utils.h"

bool test;
bool debugFlag;
bool compileFlag;
bool scriptFlag;
bool tokenFlag;

VMem GVmem(DEFAULT_VMEM_ITEMS, DEFAULT_VMEM_SIZE);

void usage(void) {
	printf( "2007-2023 (c) Copyright by webHauser. Visit http://www.webHauser.com\n" VERSION
			"\nUsage: main [Options] [scriptfile]"
			"\nUsage: main [Options] \"script\"\n"
			"Options:\n"
			"\t/h - help (this)\n"
			"\t/i - info\n"
			"\t/d - debug\n"
			"\t/t - test\n"
			"\t/c - compile\n"
			"\t/p - print tokens\n"
			"\t/s - following command line script\n"			
			"\t     The script parameter shold be the last!\n");
	exit(0);
}

void info() {
	printf("Short size is %d\n", (int)sizeof(short));
	printf("Integer size is %d\n", (int)sizeof(int));
	printf("Long size is %d\n", (int)sizeof(long));
	printf("Float size is %d\n", (int)sizeof(float));
	printf("Double size is %d\n\n", (int)sizeof(double));
	
	printf("Pointer size is %d\n", (int)sizeof(char *));	// 4
	printf("Object size is %d\n", (int)sizeof(Object));	// 4
	printf("IntegerObject size is %d\n", (int)sizeof(IntegerObject));
	printf("NameObject size is %d\n", (int)sizeof(NameObject));
	printf("String Object size is %d\n", (int)sizeof(StringObject));	// 4
	printf("Array Object size is %d\n", (int)sizeof(ArrayObject));	// 4
	printf("Dictionary Object size is %d\n\n", (int)sizeof(DictObject));	// 4
	
	printf("InputStream buffer size: %d\n", STREAM_BUFSIZE);
	printf("Execution stack size: %d\n", DEFAULT_EXSTACK_SIZE);
	printf("Operand stack size: %d\n", DEFAULT_OPSTACK_SIZE);
	printf("Dictionary stack size: %d\n", DEFAULT_DICTSTACK_SIZE);
	
	printf("User dictionary size: %d\n", DEFAULT_USERDICT_SIZE);
	printf("System dictionary size: %d\n",DEFAULT_SYSTEMDICT_SIZE);
}

int test_file(char *filename) {
	int st=0;
	try {
		Token token;
		FileObject fileobj=FileObject(filename,"rt");
		Scanner::GetStreamToken(&fileobj, &token);
		while (!token.eof()) {
			token.print("Token: ");
			Scanner::GetStreamToken(&fileobj, &token);
		}
	}
	catch(const Error::Exception &e) {
		printf("*** Exception: %s -- %s\n", e.what(), e.message());
		st = -1;
	}
	return st;
} 

int test_string(char *string) {
	int st=0;
	try {
		Token token;
		StringObject post, obj=StringObject(string);	
		Scanner::GetStringToken(&obj, &post, &token);
		//while (post.length != 0) {
		while (!token.eof()) {
			printf("String: "); Object::print(&obj);printf("\n");
			token.print("Token: ");
			printf("Remain: "); Object::print(&post);printf("\n");
			obj=post;
			Scanner::GetStringToken(&obj, &post, &token);
		}
	}
	catch(const Error::Exception &e) {
		/* look up the error name in the errordict and call it.
		the default error handling procedure will take snapshots of all the stack
		and place them in $error dictionary. Finally, the default procedure will call 
		stop which pops the user program from the exec stack and executes the interpreter's
		error printing procedure called handleerror in errordict. */
		printf("*** Exception: %s -- %s\n", e.what(), e.message());
		st = -1;
	}
	return st;
}

int run_string(char *string) {
	int st=0;
	Interpreter interp=Interpreter();
	StringObject obj=StringObject(string);
	obj.SetExecFlag(true);
	interp.exstack.push(&obj);
	try {
		interp.start();
	}
	catch(const Error::Exception &e) {
		printf("*** Exception(main): %s -- %s\n", e.what(), e.message());
		st = -1;
	}
	interp.debug();
	return st;
}

int run_file(char *filename) {
	int st=0;
	Interpreter interp=Interpreter();
	try {
		interp.run(filename);
	}
	catch(const Error::Exception &e) {
		printf("*** Exception(main): %s -- %s\n", e.what(), e.message());
		st = -1;
	} /*
	interp.debug();
	if(debugFlag) {
		printf("Dictionary stack: %d\n", interp.dictstack.size());
		while(!interp.dictstack.empty()) {
			printf("============================\n");
			DictObject *dict = (DictObject *)interp.dictstack.pop();
			dict->print();		
		}
	}*/
	return st;
}

int main(int argc, char* argv[]) {
char *lastArg=NULL; int st=-1;

	if(argc >= 2) {
		bool quit=false;		
		while(!quit && --argc > 0) {
			switch(**++argv) {
				case '-':
				case '/':
					for(int i=1; i<(int)strlen(*argv); i++) {
						switch((*argv)[i]) {							
							case 'i': info();  return 0;
							case 'c': compileFlag = true; break;
							case 's': scriptFlag = true; break;
							case 'd': debugFlag = true; break;
							case 'p': tokenFlag = true; break;
							case 't': test = true; break;							
							case 'h': 
							default : usage();							
						}
					}
					break;
				default:
					lastArg = *argv;
					quit=true;
					break;
			}
		}
	}

	if(lastArg)     fprintf(stderr, "Script: %s\n", lastArg);
	if(debugFlag)   fprintf(stderr, "d : Debug   flag is ON\n");
	if(compileFlag) fprintf(stderr, "c : Compile flag is ON\n");
	if(scriptFlag)  fprintf(stderr, "s : Script  flag is ON\n");
	if(scriptFlag && lastArg==NULL) return -1;	
	if(test) {
		st = scriptFlag ? test_string(lastArg) : test_file(lastArg);
	}
	else {
		st = scriptFlag ? run_string(lastArg) : run_file(lastArg);
	}
	fprintf(stderr, "Exit status: %d\n", st);
	return st;
}

/* EOF */
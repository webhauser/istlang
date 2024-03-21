2007 (c) webHauser All rights reserved. Minden jog fenntartva.
Szoftver tervezés, programozás, forráskód by ifj.Hauser István, webHauser.
=================================================================
////////////////////////////////////////////////////////////////////////////////////////////////
HOW-TO use ?
$ ./istlang /h
$ ./istlang /s "10 20 add ="
$ ./istlang scripts/test.txt
$ ./istlang scripts/array.ps 
HOW-TO Redirect standard output and error into files ?
$ ./istlang scripts/array.ps 2> /dev/null
$ ./istlang scripts/array.ps 2> error.log 1> output.log
HOW-TO debug ?
$ nemiver ./istlang /s "10 20 add ="
////////////////////////////////////////////////////////////////////////////////////////////////
HOW-TO implement a new operator XXX ?

1.a) SystemTable.h:   Insert the new named operator number into enum OperatorIndex list as op_XXX
1.b) SystemTable.cpp: Insert the new entry to SystemTable::sysnameindex array as SystemTable("XXX",OperatorObject(op_XXX), op_XXX)
2.a) Interpreter.h:   Declare the new implementer method as static void exec_XXX();
2.b) Interpreter.cpp: Implement the method as void Interpreter::exec_XXX() { /* @TODO */ }
2.c) Interpreter.cpp: Insert the method into Interpreter::operators array as Interpreter::exec_XXX

WARNING: The entries must be in same place in order to array indexes work properly !!!
////////////////////////////////////////////////////////////////////////////////////////////////

	ERROR HANDLING

(the_command) /nocurrentpoint .error

$error 
	/newerror	=>	true
	/errorname	=>	/nocurrentpoint
	/command	=>	(the_command)
	...

//$error 	% /nocurrentpoint D$error
exch 		% D$error /nocurrentpoint
/errorname	% D$error /nocurrentpoint /errorname
exch		% D$error /errorname /nocurrentpoint
put

//$error	% (the_command) D$error
exch		% D$error (the_command)
/command	% D$error (the_command) /command
exch		% D$error /command (the_command)
put

////////////////////////////////////////////////////////////////////////////////////////////////
HOW-TO implement the following operators ?

exec_array		; letesszuk az exec stackbe es amig van, kivesszuk a legelso elemet (amit vegrehajtunk ) a tobbit visszatesszuk az exstack-be
exec_exec		; ha operator az exec stack-be kerul. proc eseten az opstack-be megy!
exec_cvx		; make the top item on the opstack executable
exec_token		; StreamToken, StringToken

	(15 (St1) {1 2 add}) token => ((St1) {1 2 add}) 15 true
	((St1) {1 2 add}) token => ({1 2 add}) (St1) true
	({1 2 add}) token => ( ) {1 2 add} true
	( ) token => false

@NOTE: Names are NOT composite objects!
@NOTE: Strings are of unsigned char
@NOTE: Strings and names are interchangeable when used as keys in dictionaries.
@NOTE: 6 Maximum number of open files.
@NOTE: 8 byte per object
@NOTE: dictionary value occupies about 20 bytes per entry.
@NOTE: Each distinct name occupies about 40 bytes plus the number of characters in the name.
@NOTE: 127 Maximum length of a name, in characters.
@NOTE: string is similar to an array, but its elements must be integers in the range 0 to 255.
@NOTE: 65535 Maximum elements in an array or string.
@NOTE: 240000 Maximum size of virtual memory, in bytes.

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

@KELL: VMem ne legyen globalis hanem az Interpreter resze legyen, mert ekkor parancssorbol dinamikusan lehetne megadni a memoria meretet!!!
	VMem globalis deklaracio eltavolitasa
	VMem dinamikus pointer felvetele az Interpreterbe
	VMem konstruktor meghivasa az Interpreterbol
	Mindenutt ahol hasznaljuk a Vmem-et at kell adni (A kontextust vagy csak a memoria pointert)

@KELL: Automatic test cases with result verification
	prepare test cases with correct required results under directories
		test/testfile.ps	% test program
		test/testfile.ok	% result
		test/testfile.txt	% output
	filenameforall
	run
	redirect output to directory/file
	compare text files

@KELL: recursive string token
	Now using Static global Lexx lexx

@KELL: Rewrite class FileObject
	A file-hoz kell class InputStream helyett class FileHandler
	what.filep = new FileHandler();
	
LAST UPDATES:

	Version 2.4


2023.05.31 DONE Error handler TESTS
$ ./istlang scripts/ok/handleerror.ps 2> /dev/null
$ ./istlang scripts/ok/handleerror1.ps 2> /dev/null
$ ./istlang scripts/ok/error.ps 2> /dev/null
$ ./istlang scripts/ok/errorproc.ps 2> /dev/null

2023.05.31 @KELL: Error handler, ehhez kellenek:
@NOTE: Nem kell a Main-bol kivenni az exception handlert, mert a file not found-ot kapja el!
	Exception eseten az $error dictionary feltoltese
	++	file ; Only Reading
	OK	run
	OK	errordict
	OK	$error
	OK	handleerror
	OK	.error
	OK	init.ps bootstrap loader file
	
2023.05.30 DONE: Main-ban kilepeskor nem kell stack dump info ezt nyelvi elemekkel kell megoldani
	Továbbá nem C++ ban kell feltetlenul megvalositani minden operatort, hanem istlang-ban.	
	Startup file init.ps:
		systemdict	entries
		errordict	entries
		$error		entries

2023.05.30 FIXED: dictstack operator did not return subarray as a result.
2023.05.29 DONE TESTS
$ ./istlang scripts/readline.ps   2> /dev/null
$ ./istlang scripts/readstring.ps 2> /dev/null
	
2023.05.28 DONE: file operators skeleton and implementation:
OK: filename access --file-- file
OK: file --closefile--
OK:	--currentfile-- file
OK: filename --run-- % (r) file cvx exec
OK:	read
OK:	readline
OK:	readstring
	readhexstring
OK:	bytesavailable
OK: flush
OK:	flushfile
OK:	resetfile
OK:	setfileposition
OK:	fileposition	
	write			; FileHandler
	writestring		; FileHandler
	writehexstring	; FileHandler
	status			; filenameforall
	filenameforall
	filename --deletefile--
	filename1 filename2 --renamefile--	

2023.05.28 DONE: recursive bind operator	
2023.05.28 DONE: Immediate evaluation names - syntax feature
	 When the PostScript language scanner encounters a token of the form //name 
	 it immediately looks up the name and substitutes the corresponding value. 
	 This lookup occurs in the dictionaries on the dictionary stack at the time 
	 the scanner encounters the token. If it cannot find the name, an undefined error occurs.
	 
	 TEST $ ./istlang scripts/ok/immeval.ps 2> /dev/null

2023.05.14 FIXED: >>, forall dict, dictionary get
2023.05.14 FIXED: dictionary length
2023.05.14 DONE: cvi, known, index, bind ; null
2023.05.13 FIXED: for loop
2023.05.12 DONE: Dictionary AccessMode { UNLIMITED, READ_ONLY, EXEC_ONLY, NONE };
2023.05.12 DONE: save/restore - mark and sweep - free memory entries created since the last save.

	Hol lehetnek az objektumok ?
		Operand stack (any)
		Execution stack <operator, packedarray, string, file>
		Dictionary stack (dict)
			User Dictionary (key, value<any>)

	Mi legyen az azota letrehozott composite objektumokkal ? (szabaduljanak fel)
	Mi legyen az azota letrehozott string objektumokkal ?
	Mi legyen az azota letrehozott array objektumokkal ?
	Mi legyen az azota letrehozott dictionary-kkal es dictionary elemekkel ?
		dictionary-t fel lehet szabaditani, a benne levo elemeket csak akkor ha kesobb jottek letre.
	Mi legyen az azota letrehozott file objektumokkal ?
	Hogy legyen a dictionary stack ?
	Hogy legyen az operand stack ?
	Hogy legyen az exec stack ?
	
	Egy composite objektum felszabaditasa:
		Az ebbol duplikalt objektumokat is mind fel kell szabaditani.
		Minden dictionary-t at kell nezni es ahol megvan, ki kell nullazni a key,value-t.
		Az exec stacket nem szabad bantani !

2023.05.12 DONE: New operators: meminfo
2023.05.12 DONE: Version 2.1 Destructors memory error free
2023.05.02 DONE: Reading file object by tokens Scanner::GetStreamToken(FileObject *file, Token *token) reused Lexx and InputStream
2023.05.01 DONE: Reading string object by tokens with Scanner::GetStringToken(&obj, &post, &token) reused Lexx and InputStream
			DONE: bool Scanner::StreamToken(FileObject *src, Object *ret);
			DONE: bool Scanner::StringToken(StringObject *src, StringObject *rem, Object *ret);
2023.04.29 DONE: class Object composite storage rewritten
2023.04.29 DONE: class VMem 
2023.04.16 DONE: Version 1.9
	OK	Main loop // ha bemenet van le kell tenni eloszor a file-t
	OK	File objects
	OK	Execution stack	
	OK	Exec loop
	OK	StringObject
	OK	ArrayObject
	OK	Rewrite Loops
	OK	Resolve recursion calls
	OK	Memory management
	OK		memory reservation info
	OK		register composite objects
2023.04.12 DONE: Operators: undef, maxlength added
2023.04.11 DONE: Operators: cleardictstack, dictstack, countdictstack, execstack, countexecstack added
2023.04.10 DONE: TOK_OPERATOR bevezetese es hasznalata
	Tehat meg kell kulonboztetni az Executable Name-t az Operatortol.
	Jelenleg mindketto tarolasa token=TOK_NAME | EXEC_BIT
2023.04.07	FIXED: roll % 0 0 0 1 2 3 4 5   5 1 roll % 0 0 0 5 1 2 3 4
2023.04.07	FIXED: roll % <list> n j roll
2023.04.05  @ERROR: rgb.ps typeswitch2.ps /* {<< ... >>} */
2023.04.05  TEST: exec.ps
2023.04.05  FIXED: Interpreter
2023.04.05  FIXED: Simplified code format of interpreter
2023.04.05  FIXED: Lexx lines longer than buffer
2023.04.05  FIXED: Lexx script buffer scanner last char 
2023.04.05  TEST: array.ps compare.ps dict2.ps
2023.04.04  @TODO: execstack
2023.04.04  FIXED operator eq, idiv, mod, abs, div, 
2023.04.04  operator cvlit, getinterval,\ putinterval, where added
2023.04.03  @TEST: operator roll added 
2023.04.02  @INFO: errordict, $error
2023.04.02  @INFO: stop, stopped Exception kezeles
2023.04.02  @INFO: Az exit utasitashoz kell az execstack !
2023.04.02  @ERROR: << >> hibasan mukodik egy proc-ban
2023.04.02  operator load, store added
2023.04.02  operator xcheck added
2023.04.02  type operator now returns executable name. test: typeswitch.ps
2023.04.01  FIXED: A print operator nem hajt vegre soremelest.
2023.03.30  lexical parser extended with radix numbers
2023.03.30  FIXED Base85 string handling <~ ... ~>
2023.03.30  FIXED Lexx string literals, lexx buffer size
2023.03.26  SYSTEM REVIEW
2007.11.08  @ERROR Ha a String konstans mérete túl nagy elszáll a program.
			VMem-bol kell allokálni, throw exception amennyiben nincs.
2007.11.08  FIXED: length operator keveredés hiba kijavítva a SystemTable-ban és az Interpreter-ben.
2007.11.08  quit operator added.
2007.11.07  FIXED: for utasitas ciklusváltozó stack hiba kijavitva

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
	
MEMORY HANDLING
	
	@TODO: Mark and sweep, reference counting
	
	save, save object, save level - ettol a ponttol kezdve kell majd felszabaditast vegezni..
	restore - objektumok VMem felszabaditasa es megszuntetese ha a legutolso save ota feleslegesse valnak.
		1. A memoria nyilvantartasban vegigmegyunk a bejegyzeseken es ha a save idejenel tobb az ido akkor felszabaditjuk es toroljuk.
		2. Megnezzuk a rekordot, hogy hasznaljuk-e a stackekben (dict, exec, operand) ha nem akkor felszabaditjuk es toroljuk.
		
	Q: String objektumok nagyok, mi tortenik veluk ?
	A: save/restore -kor szabadulnak fel ha nincsenek hasznalatban.
	
	Q: Ha egy objektum a stackba kerul masolodik vagy csak a mutato ?
	A: Simple object masolodik, Composite objectben csak a mutato.
	
	Memoria nyilvantartas
	
		Ha egy objektumot lefoglalunk akkor egy uj rekordot is hozzaadunk a memoria nyilvantartashoz (timestamp, mutato, darabszam)
	
		Only for composite objects
	
		Memory reservation record:
			-sorszama	ID
			-ideje		TS
			-mutato		PTR
			-darabszam	NUM
			-tipusa		TYPE (S,O)
			-hol szerepel ? 
			-mely objektumok hivatkoznak rá ?
			-hány db hivatkozas van rá ?

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

<< % SVG
/Header (<svg height="800" width="800">)
  <circle cx="50" cy="50" r="40" stroke="black" stroke-width="3" fill="red" />
  Sorry, your browser does not support inline SVG.  
/Footer (</svg>)
>>

x y r circle
x y moveto
x y lineto
??? curveto

x y rmoveto
x y rlineto
??? rcurveto 

%% add an arc of a circle to the current path.
?? arc
?? arcn
?? arct
?? arcto

setcolor, setgray, setrgbcolor
1.0 setlinewidth
stroke
fill

-text- show

% transformations
x y translate
a rotate
x y scale
concat	% applies an arbitrary linear transformation to the user coordinate system

/////////////////////////////////////////////////////////////////////////////////
#define HANDLER _E_RESTORE; } else { _E_RESTORE;
#define END_HANDLER }}
/////////////////////////////////////////////////////////////////////////////////
EditPlus Search & Replace:
	^(op_[^,]+)
    SystemTable("\0",OperatorObject(\0), \0)

#!/usr/bin/perl

use strict;

my @operators = qw(
file
closefile
read
write
readhexstring
writehexstring
readstring
writestring
readline
bytesavailable
flush
flushfile
resetfile
status
run
currentfile
deletefile
renamefile
filenameforall
setfileposition
fileposition
);

print "\n// SystemTable.h\n\n";

foreach my $op (@operators) {
	print "op_$op,\n";	
}

print "\n// SystemTable.cpp\n\n";

foreach my $op (@operators) {
	print "SystemTable(\"$op\", OperatorObject(op_$op), op_$op),\n";
}

print "\n// Interpreter.h\n\n";

foreach my $op (@operators) {
	print "static void exec_$op();\n";
}

print "\n// Interpreter.cpp\n\n";

foreach my $op (@operators) {
	print "Interpreter::exec_$op,\n";	
}

print "\n// Interpreter.cpp\n\n";

foreach my $op (@operators) {
	print "/** --$op--\nDescription */\n/* operator */";
	print "void Interpreter::exec_$op() {\n\t// \@TODO\n}\n\n";	
}

#!/usr/bin/bash
g++ -Wall -g Main.cpp InputStream.cpp Lexx.cpp Object.cpp ObjectStack.cpp SystemTable.cpp Interpreter.cpp Scanner.cpp VMem.cpp Timer.cpp utils.cpp -lm -o istlang

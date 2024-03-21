#ifndef MainH
#define MainH

#include <stdio.h>
#include <stdlib.h>		// rand(), srand(), atoi, atof
#include <string.h>
#include <math.h>
#include "Timer.h"

const int MAX_FILENAME=64;
const int STREAM_BUFSIZE=80;		// InputStream max buffer size
const int TOKEN_BUFLEN=255+1;		// Token max size

const int DEFAULT_EXSTACK_SIZE=256;
const int DEFAULT_OPSTACK_SIZE=128;
const int DEFAULT_DICTSTACK_SIZE=8;
const int DEFAULT_USERDICT_SIZE=128;
const int DEFAULT_SYSTEMDICT_SIZE=150;
const int DEFAULT_ERRORDICT_SIZE=32;

const int DEFAULT_MAXOPENFILES=4;
const int DEFAULT_VMEM_ITEMS=4096;
const int DEFAULT_VMEM_SIZE=240000;


typedef unsigned char byte;
typedef unsigned short word;
typedef word VMDesc;

extern bool debugFlag;
extern bool tokenFlag;

#define VERSION "Version 2.4 Last update 2023.05.30"

#endif
#include <stdio.h>
#include <stdlib.h>

#include "components.h"
#include "cycleindex.h"
#include "isnt1.h"

#define REGISTER_SIZE 16
#define TRACE_FILENAME "trace.out"

void simul_ooo(struct Config* config);
#ifndef _FILE_READ_
#define _FILE_READ_
//for prevent multifle include


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatypes.h"

//inst file reading
int read_instruction(FILE *in_filename, struct INST *out_inst);
int make_inst_array(char* filename, struct INST *out_inst);

//config file reading
int config_reader(struct Config *config);

#endif // !_FILE_READ_
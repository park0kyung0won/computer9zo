#ifndef _FILE_READ_
#define _FILE_READ_
//for prevent multifle include


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatypes.h"

//inst file reading

bool read_instruction(FILE *in_filename, struct INST * out_inst);// bool is_readed, read one line and convert INST form
struct Instruction* make_inst_array(char* filename, int *lines);
//bool make_inst_array(char* filename, struct INST ** out_inst);// bool is_readed read whole file and convert INST array

//config file reading
bool config_reader(char* filename, struct Config *out_config);// bool is_readed, read whole file and convert Config file

#endif // !_FILE_READ_
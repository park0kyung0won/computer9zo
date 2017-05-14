// Computer Architecture Homework #2
// Noh Dong Ju & Park Kyung Won & Yu Jin woo

#include <stdio.h>
#include <stdlib.h>
//#include "read.h" //read config and instruction
//#include "simul.h"//func related simulation

typedef struct Config { int justtest;  } Config;

int main(int argc, char* argv[])
{
	//
	// Read config file and make config array
	//
	struct Config* configs;
	if (argc == 1)
	{//if there is no input
		configs = malloc(sizeof(struct Config));
		read_config("our_config_file.txt",configs);
	}
	else
	{//if there is input from argv
		configs = malloc(sizeof(struct Config)*(argc-1));
		for (int idx = 1; idx < argc; )//every config_file 
			read_config(argv[idx], (configs+idx) );//config array elem init
	}

	//
	// simulate and for every config combination
	//
	for (int idx_conf = 0; idx_conf < argv ; ++idx_conf)
	{
		simul_ooo(configs[idx_conf]);//simulate
	}

	
	free(configs);// free config

	return 0;

}

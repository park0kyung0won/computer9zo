// Computer Architecture Homework #2
// Noh Dong Ju & Park Kyung Won & Yu Jin woo

#include <stdio.h>
#include <stdlib.h>
#include "simul.h"



//#include "read.h" //read config and instruction
//#include "simul.h"//func related simulation

//typedef struct Config { int justtest;  } Config;

int main(int argc, char* argv[])
{
	//
	// Read config file and make config array
	//
	struct Config* configs;
	if (argc == 1)
	{//if there is no input
		configs = malloc(sizeof(struct Config));
		if (configreader("config.txt", configs) != 0)
		{	return 1;	}
	}
	else
	{//if there is input from argv
		configs = malloc(sizeof(struct Config)*(argc-1));
		for (int idx = 1; idx < argc; ++idx)//every config_file 
		{
			if (configreader(argv[idx], (configs + idx)) != 0)//config array elem init
			{	return 1;	}
		}
	}

	//
	// simulate and for every config combination
	//
	for (int idx_conf = 1; idx_conf < argc ; ++idx_conf)
	{
		printf("conf_%d : %s \n",idx_conf, argv[idx_conf]);
		configprinter(configs + idx_conf);
		simul_ooo(configs + idx_conf);//simulate
	}

	free(configs);// free config

	return 0;
}


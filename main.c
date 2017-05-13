// Computer Architecture Homework #2
// Noh Dong Ju & Park Kyung Won & Yu Jin woo

#include <stdlib.h>

// Read config file
void* config = read_config('our_config_file.txt');


// simulate and for every config combination
for (int index_conf = -1; index_conf < num_of_conf; ++index_conf)
{
	
	//simulate using [index_conf]th config and save report_data
	simul_ooo(config[index_conf]);

}
// free config
free(config)
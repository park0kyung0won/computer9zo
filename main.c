// Computer Architecture Homework #2
// Noh Dong Ju & Park Kyung Won & Yu Jin woo

#include <stdio.h>
#include <stdlib.h>

#include "datatypes.h"
#include "fileread.h"

//#include "read.h" //read config and instruction
//#include "simul.h"//func related simulation

//typedef struct Config { int justtest;  } Config;

int main(int argc, char* argv[])
{
	//
	// Print intro message
	//
	printf("+------------------------------------------------------------------------------+\n");
	printf("                     Computer Architecture Homework #2\m");
	printf("                 Noh Dong Ju & Park Kyung Won & Yu Jin woo\n");
	printf("+------------------------------------------------------------------------------+\n");
	printf(" Usage : 1) Just excute this program, \n");
	printf("            then program take 'config.conf' and 'instruction.inst'\n");
	printf("            and simulate OoO. when end simulation, program will print out\n");
	printf("            '[dump]_[width]_[ROS_size]_[RS_size]_output.out'\n");
	printf("\n");
	printf("         2) drag and drop on exe.file or give filepath as argument,\n");
	printf("            then program take every '~.conf' and '~.inst' as input,\n");
	printf("            and simulate all combinaion of these. when end simulation,\n");
	printf("            program will print out\n");
	printf("            '[dump]_[width]_[ROS_size]_[RS_size]_[inst_name]_output.out'\n");
	printf("+------------------------------------------------------------------------------+\n");

	// 
	// Read config and Data file and make config array
	//
	printf("\n");
	printf("Read Config and Instruction files : \n");
	
	//이후 입력 인수로 파일을 받기 위해 필요한 변수 선언
	char** conf_name; int conf_len;//name = 파일경로, len= 총 개수,
	char** inst_name;  int inst_len; char** inst_filename;
	conf_name = malloc(sizeof(char*)*argc);
	inst_name = malloc(sizeof(char*)*argc);//공간 낭비지만 그렇게 크지는 않음
	inst_filename = malloc(sizeof(char*)*argc);
	conf_len = 0;
	inst_len = 0;

	if (argc == 1)
	{//if there is no input, set basic value;
		
		conf_len = 1;
		conf_name[0] = "config.conf";

		inst_len = 1;
		inst_name[0] = "instruction.inst";
		inst_filename[0] = "instruction.inst";
	}
	else
	{//if there is input from argv,  모든 입력 인수를 읽어 .conf는 conf에 .inst는 inst에, 나머지는 무시.

		char* file_name = 0;//확장자나 파일 명을 기록할 
		for (int idx = 1; idx < argc; ++idx)//every config_file 
		{
			file_name = argv[idx] + (strlen(argv[idx]) - 5); // 맨 끝 5자리를 가져온다. 그 후 .conf인지 .inst인지 둘다 아닌지를 확인한다. 
			//printf("%s", file_name);

			if (0 == strcmp(file_name, ".conf"))//if conf file
			{
				//printf("is conf", file_name);
				conf_name[conf_len] = argv[idx];
				++conf_len;
			}
			else if (0 == strcmp(file_name, ".inst"))
			{
				//printf("is inst", file_name);
				inst_name[inst_len] = argv[idx];
				inst_filename[inst_len] = get_filename(argv[idx]);
				//printf("%s", inst_filename[inst_len]);
				++inst_len;
			}
			else
			{
				printf("File  %s is nethier .conf and .inst! \n", argv[idx]);
			}
		}
	}
	printf("Total %3d Config and %3d Instruction finded\n", conf_len, inst_len);
	//읽을 파일의 개수와 경로 파악 끝

	//
	// simulate and for every config combination
	//


	//config 파일을 읽어 저장
	struct Config* configs;
	configs = malloc(sizeof(struct Config)*(conf_len));
	
	int for_ignore = 0;
	for (int idx = 0; idx < conf_len; ++idx)
	{
		if (!config_reader(conf_name[idx], configs + idx - for_ignore))
		{
			printf("Cannot find file %s\n", conf_name[idx]); 
			++for_ignore;
		}
	}
	conf_len -= for_ignore;

	//inst 파일은 필요할 때 한개씩 읽어 저장
	struct INST* inst_arr; int inst_arr_len;

	for (int inst_idx = 0; inst_idx < inst_len; ++inst_idx)
	{//모든 인스트럭션에 대해 
		if (!make_inst_array(inst_name[inst_idx], &inst_arr, &inst_arr_len)) 
		{
			printf("Error dectection - ignore this file %s\n", inst_filename[inst_idx]);
			continue;
			//return 1;
		}
		//printf("inst_%d : %s\n", inst_idx + 1, inst_filename[inst_idx]);

		for (int conf_idx = 0; conf_idx < conf_len; ++conf_idx)
		{//모든 config에 대해

			printf("Task %d in total %d\n", inst_idx + 1, (inst_idx+1)*(conf_idx+1));
			printf("inst_%d : %s\n", inst_idx + 1, inst_filename[inst_idx]);
			printf("conf_%d : ", conf_idx+1 );
			Config_printer(configs + conf_idx);
			//simul_ooo(configs + idx_conf);//simulate
		}

		free(inst_arr);

	}

	free(configs);// free config
	free(conf_name);
	free(inst_name);
	free(inst_filename);

	printf("Program done - Press any key to quit");
	getchar();

	return 0;
}


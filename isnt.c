#ifndef ISNT
#define ISNT


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

struct Instruction
{
 int inst_type;
 int destination;
 int src1;
 int src2;
// char addr[12];
};

struct Config
{
 int Dump;
 int Width;
 int ROB_size;
 int Res_size;
};

int read_instruction(FILE *in_filename, struct Instruction *out_inst)
{
 char buffer[30];
 if (fgets(buffer, 30, in_filename) != NULL)
 {
	 char inst_now[8];
	 strcpy(inst_now, strtok(buffer, " "));
	 printf("%s\n", inst_now);
	 if (strcmp(inst_now, "IntAlu") != 1)
		 out_inst->inst_type = 0;
	 else if (strcmp(inst_now, "MemRead") != 1)
		 out_inst->inst_type = 1;
	 else
		 out_inst->inst_type = 2;

	 out_inst->destination = atoi(strtok(NULL, " "));
	 out_inst->src1 = atoi(strtok(NULL, " "));
	 out_inst->src2 = atoi(strtok(NULL, " "));
	 if (out_inst->inst_type != 0)
	 {
		 out_inst->src2 = atoi(strtok(NULL, " "));
	 }

	 /*  if (strcmp(out_inst->inst_type, "MemRead") || strcmp(out_inst->inst_type, "MemWrite"))
	   {strcpy(out_inst->addr,strtok(NULL," "));
	   }
	 */

	 return 0;
 }
 else
 {
	 return 1;
 }
}

int configreader(char* config_filename, struct Config* out_config )
{
	char buffer[5];
	FILE *configp;
	configp=fopen(config_filename,"r");
	
	for (int idx = 0; idx < 4; ++idx)
	{
		fgets(buffer, 5, configp);
		switch(idx)
		{
			case 0 : out_config->Dump=atoi(buffer);	break;
			case 1 : out_config->Width=atoi(buffer); break;
			case 2 : out_config->ROB_size=atoi(buffer);	break;
			case 3 : out_config->Res_size=atoi(buffer); return 0;
		}
	}
	return 1; //if "fgets"function cannot get any value, then while phrase would not activated. In that case this function return 0 meaning false. if once while phrase activated, it finally goes to switch case 4 and return 1.
}
void configprinter(struct Config* config)
{
	printf("-> Dump : %d | Width : %d | ROB_size : %d | Res_size : %d |",
		config->Dump, config->Width, config->ROB_size, config->Res_size);
}
//
//void main()
//{
//	file* in_file=fopen("hw2_trace_bzip2.out","r");
//	if(in_file==null)
//	{
//		fputs("cannot open input file...\n",stderr);
//		exit(1);
//	}
//	struct instruction a;
//	int b;
//	b=read_instruction(in_file,&a);
///*	printf("%d\n",a.inst_type);
//	printf("%d\n",a.destination);
//	printf("%d\n",a.src1);
//	printf("%d\n",a.src2);
//*/
//	
//	b=read_instruction(in_file,&a);
///*	printf("%d\n",a.inst_type);
//	printf("%d\n",a.destination);
//	printf("%d\n",a.src1);
//	printf("%d\n",a.src2);
//*/
//	b=read_instruction(in_file,&a);
//
//	struct config config;
//	configreader(&config);
//	printf("%d\n",config.dump);
//	printf("%d\n",config.width);
//	printf("%d\n",config.rob_size);
//	printf("%d\n",config.res_size);
//	fclose(in_file);
//}
//

#endif // !ISNT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct INST
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

int read_instruction(FILE *in_filename, struct INST *out_inst)
{
 char buffer[30];
 if (fgets(buffer,30,in_filename) !=NULL)
 {	
 	char inst_now[8];
	strcpy(inst_now,strtok(buffer," "));
	printf("%s\n",inst_now);
 	if(strcmp(inst_now,"IntAlu")!=1)
	 out_inst->inst_type=0;
	else if(strcmp(inst_now,"MemRead")!=1)
	out_inst->inst_type=1;
	else
	out_inst->inst_type=2;
	
 	out_inst->destination=atoi(strtok(NULL," "));
 	out_inst->src1=atoi(strtok(NULL," "));
 	out_inst->src2=atoi(strtok(NULL," "));
 	
/*  if (strcmp(out_inst->inst_type, "MemRead") || strcmp(out_inst->inst_type, "MemWrite"))
  {strcpy(out_inst->addr,strtok(NULL," "));
  }
*/

  return 1;
}
 else
 {return 0;}
}

int configreader(struct Config *config)
{
	char buffer[5];
	FILE *configp;
	configp=fopen("config.txt","r");
	int switcher=1;
	while(fgets(buffer,5,configp)!=NULL)
	{
		switch(switcher)
		{
		case 1 : config->Dump=atoi(buffer);	switcher;
		case 2 : config->Width=atoi(buffer); switcher++;
		case 3 : config->ROB_size=atoi(buffer);	switcher++;
		case 4 : config->Res_size=atoi(buffer); return 1;
		}
	}
	return 0; //if "fgets"function cannot get any value, then while phrase would not activated. In that case this function return 0 meaning false. if once while phrase activated, it finally goes to switch case 4 and return 1.
}

void make_inst_array(char* filename, struct INST *out_inst)
{	int lines=0;
	char tmp;
	char buffer[30];
	FILE* p_file;
	p_file=fopen(filename,"r");
	
	while (!feof(p_file))
	{
		tmp=fgetc(p_file);
		if(tmp=='\n')
		{
			lines++;
		}
	}
	fclose(p_file);
	FILE* pfile;
	printf("does file read?");
	pfile=fopen(filename,"r");
	printf("yes it is!");
	out_inst=(struct INST*)malloc(sizeof(struct INST)*lines);
	for(int i = 0; i<lines; i++)
	{
		read_instruction(pfile,&out_inst[i]);
	}
	fclose(pfile);
}

int main(void)
{
/*	FILE* in_file=fopen("hw2_trace_bzip2.out","r");
	if(in_file==NULL)
	{
		fputs("cannot open input file...\n",stderr);
		exit(1);
	}
	*/
	extern struct INST *ptr_instruction;
	make_inst_array("hw2_trace_bzip2.out", ptr_instruction);
//	fclose(in_file);
	return 0;
}


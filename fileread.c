#include "fileread.h"

bool read_instruction(FILE *in_filename, struct INST * out_inst)
{// bool is_readed, read one line and convert INST form
	
	char buffer[30];//buffer for read one line (inst)
	if (fgets(buffer, 30, in_filename) != NULL)
	{//if we get string

		char* inst_name = strtok(buffer, " ");

		switch (inst_name[3])
		{
		case 'A':
			out_inst->op = IntAlu;
			break;
		case 'R':
			out_inst->op = MemRead;
			break;
		case 'W':
			out_inst->op = MemWrite;
			break;
		default:
			return false;
		}
		out_inst->dest = atoi(strtok(NULL, " "));
		out_inst->oprd1 = atoi(strtok(NULL, " "));
		out_inst->oprd2 = atoi(strtok(NULL, " "));
		//if (out_inst->op != IntAlu) { out_inst->oprd2 = atoi(strtok(NULL, " ")); }

		return true;
	}
	else
	{//if we can't read (get eof)
		return false;
	}
}

bool config_reader(char* filename, struct Config *out_config)
{
    char buffer[5];
    FILE *configp;
	if ((configp = fopen(filename, "r")) == NULL)
	{
		for (int idx = 0; idx < 4; ++idx)
		{
			fgets(buffer, 5, configp);
			switch (idx)
			{
			case 0: out_config->Dump = atoi(buffer); break;
			case 1: out_config->Width = atoi(buffer); break;
			case 2: out_config->ROB_size = atoi(buffer); break;
			case 3: out_config->RS_size = atoi(buffer); return true;
			}
		}
	}
	else
	{
    return false; //if "fgets"function cannot get any value, then while phrase would not activated. In that case this function return 0 meaning false. if once while phrase activated, it finally goes to switch case 4 and return 1.
	}
}


struct Instruction* make_inst_array(char* filename, int *lines)
{	
	int line=0;
	char tmp;
	char buffer[30];
	FILE* p_file;
	p_file=fopen(filename,"r");
	
	while (!feof(p_file))
	{
		tmp=fgetc(p_file);
		if(tmp=='\n')
		{
			line++;
		}
	}
	line++;
	fclose(p_file);
	p_file=fopen(filename,"r");
	struct Instruction *out_inst=(struct Instruction*)malloc(sizeof(struct Instruction)*line);
	printf("in make fuction %x\n",out_inst);
	
	for(int i=0; i<line; i++)
	{
		read_instruction(p_file,out_inst+i);
	}
	fclose(p_file);
	*lines=line;
	return out_inst;
}

void main()
{
/*	FILE* in_file=fopen("hw2_trace_bzip2.out","r");
	if(in_file==NULL)
	{
		fputs("cannot open input file...\n",stderr);
		exit(1);
	}
	*/
	 struct Instruction *ptr_instruction;
	 int length;
	ptr_instruction=make_inst_array("hw2_trace_bzip2.out",&length);
	printf("in main %x",ptr_instruction);
	 struct Config config;
	configreader("config.txt",&config);

//	fclose(in_file);
}

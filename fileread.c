#include "fileread.h"

bool char_to_INST(char* buffer, struct INST * out_inst);

bool read_instruction(FILE *in_filename, struct INST * out_inst)
{// bool is_readed, read one line and convert INST form
	
	char buffer[30];//buffer for read one line (inst)
	if (fgets(buffer, 30, in_filename) == NULL) { return false; }//if we can't read (get eof)

	return char_to_INST(buffer, out_inst);
}

bool config_reader(char* filename, struct Config *out_config)
{
    char buffer[5];
    FILE *configp;
	if ((configp = fopen(filename, "r")) == NULL) { return false; }//if configp == null, it mean fail.
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
	
	
	//if "fgets"function cannot get any value, then while phrase would not activated. In that case this function return 0 meaning false. 
	//if once while phrase activated, it finally goes to switch case 4 and return 1.
}


bool make_inst_array(char* filename, struct INST ** out_inst_arr, int *len)
{		

	printf("Instruction File Reader\n");
	printf("Open %s ",filename);

	FILE* p_file;
	if ((p_file = fopen(filename, "r")) == NULL) { printf("- Failed \n"); return false; }//if fail to open file, return false
	printf("- Done \n");

	//get file size
	printf("File size = ");
	fseek(p_file, 0, SEEK_END);
	long int p_len = ftell(p_file);
	fseek(p_file, 0, SEEK_SET);
	printf("%lKB\n", sizeof(char)*p_len/1024);

	//all file load on memory
	printf("Loading file on memory ");
	char* p_file_buffer = malloc(sizeof(char)*(p_len+1));
	fread(p_file_buffer, sizeof(char), p_len, p_file);
	p_file_buffer[p_len] = '\0';
	printf("- Done \n");

	//close file
	fclose(p_file);
	

	//Make blank instruction array
	printf("Allocate Memory for Instruction Array");
	int length = 0;
	for (long int p_idx = 0; p_idx < p_len; ++p_idx)
	{
		if (p_file_buffer=='\n') { ++length; }
	}//get line number
	if 
	(*out_inst_arr)=(struct INST*)malloc(sizeof(struct INST)*length);
	printf("- Done \n");

	//Translate File and Fill Instruction
	printf("Make Instruction Array -   0%");

	bool is_worked = true;

	char* p_line = NULL;//file_pointer_in_memory
	char* p_token_line;//Temp_char_pointer used in tokenizing

	p_line = strtok(p_file_buffer, "\n");
	int token_length = 0;
	
	while (p_line !=NULL)
	{
		p_token_line = p_line;
		is_worked = ( is_worked && char_to_INST(p_token_line,(*out_inst_arr)+token_length) );
		++token_length;
		printf("\b\b\b\b%3d\%", token_length/length);
	}
	(*len) = length;
	printf("\b\b\b\bDone\n");

	return is_worked;
}


bool char_to_INST(char* buffer, struct INST * out_inst)
{
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


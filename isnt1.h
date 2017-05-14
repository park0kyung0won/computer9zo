#ifndef IST
#define IST

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

int read_instruction(FILE *in_filename, struct INST *out_inst);

int make_inst_array(char* filename, struct INST *out_inst);

int configreader(struct Config *config);

void configprinter(struct Config* config);

//int main(void)
//{
///*	FILE* in_file=fopen("hw2_trace_bzip2.out","r");
//	if(in_file==NULL)
//	{
//		fputs("cannot open input file...\n",stderr);
//		exit(1);
//	}
//	*/
//	extern struct INST *ptr_instruction;
//	make_inst_array("hw2_trace_bzip2.out", ptr_instruction);
////	fclose(in_file);
//	return 0;
//}


#endif // !INST

// Data struectures for OoO Execution CPU
#ifndef DATA_TYPES
#define DATA_TYPES



#include <stdbool.h>

enum flag { Q = 0, V = 1 }; // has V? or Q?
enum instruction { IntAlu = 0, MemRead = 1, MemWrite = 2 }; // Instruction Type
enum is_complete { P = 0, C = 1 }; // P means pending, C means completed
const char* instruction_name[3] = { "IntALU", "MemRead" , "MemWrite" };

struct INST;//instructrion
struct FQ;//fetch_Queue, it same INST
struct Config;//config
struct RAT;//Register AT
struct RS;//Reservation Staton
struct ROB;// Reorder Buffer

//for debuging
void INST_printer(const struct INST* printed); 
void FQ_printer(const struct FQ* printed);
void Config_printer(const struct Config* printed);
void RAT_printer(const struct RAT* printed);
void RS_printer(const struct RS* printed);
void ROB_printer(const struct ROB* printed);

//for reporting
//void INST_printer(struct INST* printed, FILE* fileID);
//void FQ_printer(struct FQ* printed, FILE* fileID);
//void Config_printer(struct Config* printed, FILE* fileID);
//void RAT_printer(struct RAT* printed, FILE* fileID);
void RS_fprinter(const struct RS* printed, FILE* fileID);
void ROB_fprinter(const struct ROB* printed, FILE* fileID);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

struct INST//instructrion
{
	enum instruction op;
	int dest;
	int oprd1;
	int oprd2;
};

struct FQ//equal INST, it mean fetch queue
{
	enum instruction op;
	int dest;
	int oprd1;
	int oprd2;
};

struct Config//config
{
	int Dump;
	int Width;
	int ROB_size;
	int RS_size;
};

struct RAT//Register AT
{
	bool RF_valid;
	int Q;
};

struct RS//Res Staton
{
	int target_rob; // ROB index for linking
	bool is_valid; // Busy or not
	enum instruction op;
	int time_left; // Shows cycles left to be finished. -1 means not started
	struct
	{
		enum flag state;
		union 
		{
			int v;
			int q;
		} data;
	} oprd_1;
	struct
	{
		enum flag state;
		union
		{
			int v;
			int q;
		} data;
	} oprd_2;
};

//int rob_point = 0; // points first element in ROB. Value must come btw 0 and N-1

struct ROB// Reorder Buffer
{
	enum instruction op; // operation
//	int value; // data 
	int dest; // Destination ( Must point to one of elements in reg[16] !
	enum is_complete status; // status = P; means pending, status = C; means completed
};

#endif // !DATA_TYPES
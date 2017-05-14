// Data struectures for OoO Execution CPU

#include <stdbool.h>

enum flag { Q = 0, V = 1 }; // has V? or Q?
enum instruction { IntAlu = 0, MemRead = 1, MemWrite = 2 }; // Instruction Type
enum is_complete { P = 0, C = 1 }; // P means pending, C means completed

//int N; // Window Size
//int reg[16]; // Architectural Register File

struct FI//fetched instruction
{
	enum instruction op;
	int dest;
	int oprd1;
	int oprd2;
};

struct RAT
{
	bool RF_valid;
	int Q;
}; // RAT

struct RS
{
	int index_rob; // ROB index for linking
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
};//ResvStation[N];

int rob_point = 0; // points first element in ROB. Value must come btw 0 and N-1

struct ROB
{
	enum instruction op; // operation
	int value; // data 
	int dest; // Destination ( Must point to one of elements in reg[16] !
	enum is_complete status; // status = P; means pending, status = C; means completed
};
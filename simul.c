#include "components.h"


#define REGISTER_SIZE 16

struct Config

void simul_ooo(struct Config* config)
{
	//
	// Init simulator
	//

	//Init report data
	struct Status {
		int cycle;//num of cycle
		struct
		{
			int total;//totalnum
			int intalu;//alu num
			int memread;//read mem
			int memwrite;//write mem
		} inst;//num of instructions
	};
	
	// Init OoO
	enum Debug { None = 0; ROB = 1, RSROB = 2 } debug = (config*).debug; //set Debug_mode from config
	int N = (config*).width;
	struct ROB	rob[(config*).rob_size];
	struct RS	rs[(config*).rs_size];
	struct RAT	rat[REGISTER_SIZE];//
	
}
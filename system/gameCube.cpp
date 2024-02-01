#include "gameCube.h"


gameCube_t::gameCube_t(){
	cpu.p_soc = &soc;
}

void gameCube_t::clock() {
	cpu.clock();
}

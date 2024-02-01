#pragma once

#include "cpu/gekko.h"
#include "soc/flipper.h"

class gameCube_t {
public:

	gameCube_t();

	gekko_t cpu;
	flipper_t soc;



	void clock();
};
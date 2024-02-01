#include "system/gameCube.h"

gameCube_t g_console;
bool g_pause = false;

void main() {


	while (!g_pause)
		g_console.clock();
}
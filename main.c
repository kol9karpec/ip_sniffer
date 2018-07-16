#include "lib.h"

int main(int argc, const char * argv[]) {

	//TODO: fill config with arguments if needed
	//TODO: CTRL+C signal handler(deinit gconf + close fds)
	run_daemon();

	return 0;
}

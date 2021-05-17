#include <winsock.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdint>

#include "Inputs.h"
#include "Magic.h"
#include "Game.h"
#include "Trig.h"
#include "Macros.h"
#include "Brute.h"
#include "Movement.h"
#include "Start.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {
	FILE* f;
	size_t err;

	vector<Start> starts = read_starts(".\\starts_input.txt");

	try {
		if ((err = fopen_s(&f, ".\\starts_output.txt", "wb")) != 0) {
			cerr << "Bad open: " << err << endl;
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < starts.size(); i++) {
			char buffer[1024];
			sprintf(buffer, "Solutions for Start number %d\n", i);
			vector<Slot> backups = starts[i].set_start(&game, ".\\BitFS_poc_final_6.m64", ".\\sm64_jp.dll");

			fprintf(stderr, "starting brute force\n");

			calc_next_node(false, &backups[0], true, 133, &backups[1], &backups[2]);

			sprintf(buffer, "\n");
			fwrite(buffer, sizeof(char), 1, f);

			fprintf(stderr, "finished\n\n");
		}

		fclose(f);
	}
	catch (...) {
		cerr << "error" << endl;
		exit(1);
	}

	return 0;
}

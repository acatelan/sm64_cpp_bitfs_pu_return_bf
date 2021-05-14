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

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

vector<Inputs> load_m64(const char* filename) {
	vector<Inputs> frames;
	FILE* f;

	uint16_t buttons;
	int8_t stick_x, stick_y;

	size_t err;

	try {
		if ((err = fopen_s(&f, filename, "rb")) != 0) {
			cerr << "Bad open: " << err << endl;
			exit(EXIT_FAILURE);
		}

		fseek(f, 0x400, SEEK_SET);

		while (true) {
			uint16_t bigEndianButtons;

			fread(&bigEndianButtons, sizeof(uint16_t), 1, f);

			if (feof(f) != 0 || ferror(f) != 0) {
				break;
			}

			buttons = ntohs(bigEndianButtons);

			fread(&stick_x, sizeof(int8_t), 1, f);

			if (feof(f) != 0 || ferror(f) != 0) {
				break;
			}

			fread(&stick_y, sizeof(int8_t), 1, f);

			if (feof(f) != 0 || ferror(f) != 0) {
				break;
			}

			frames.push_back(Inputs(buttons, stick_x, stick_y));
		}
	}
	catch (invalid_argument& e)
	{
		cerr << e.what() << endl;
	}

	return frames;
}

int main(int argc, char* argv[]) {
	vector<Inputs> m64 = load_m64("BitFS_poc_final_6.m64");

	Slot backup = game.alloc_slot();
	Slot backupTemp = game.alloc_slot();
	Slot backupL2 = game.alloc_slot();

	//Run through the m64
	for (int frame = 0; frame < m64.size(); frame++) {
		//hacks to get m64 to sync
		if (frame == 3020) {
			*marioX(game) = -2250.1f;
			*marioZ(game) = -715;
		}

		if (frame == 3051) {
			*bullyX(game) = -2289.91f;
			*bullyY(game) = -2950.0;
			*bullyZ(game) = -731.34f;
			*bullyYaw1(game) = 16384;
			*bullyHSpd(game) = 50000000.0;
		}

		if (frame == 3076) {
			*bullyX(game) = -2239.0;
			*bullyY(game) = -2950.0;
			*bullyZ(game) = -573.61f;
			*bullyYaw1(game) = 0;
			*bullyHSpd(game) = 50000000.0;
		}
			

		if (frame == 3099) {
			*trackPlatAction(game) = 2;
			*trackPlatX(game) = -1331.77f;
		}
				

		if (frame == 3104) {
			*bullyX(game) = -1719.7f;
			*bullyY(game) = -2950.0;
			*bullyZ(game) = -461.0;
			*bullyYaw1(game) = -32768;
			*bullyHSpd(game) = 14063606.0;
		}
					
		//Deactivating all of the objects but the bully, Mario, the
		//tilting pyramid platforms, and the track platform
		if (frame == 3110) {
			for (int obj = 0; obj < 108; obj++) {
				if (obj == 27 || obj == 89 || obj == 83 || obj == 84 || obj == 85) {
					continue;
				}

				//seems to be either 48 or 100 to deactivate. I think it's 180
				short* active_flag = (short*)(game.addr("gObjectPool") + (obj * 1392 + 180));
				*active_flag = *active_flag & 0xFFFE;
			}
				
		}
						
		if (frame == 3274) {
			game.save_state(&backup);
			break;
		}

		int16_t num_stars = *(int16_t*)(game.addr("gMarioStates") + 230);

		if (frame % 1000 == 0) {
			printf("Frame %05d stars %02d\n", frame, num_stars);
		}
										

		set_inputs(game, m64.at(frame));
		game.advance_frame();
	}

	fprintf(stderr, "starting brute force\n");

	calc_next_node(false, &backup, true, 133, &backupTemp, &backupL2);

	fprintf(stderr, "finished or crashed\n");

	return 0;
}

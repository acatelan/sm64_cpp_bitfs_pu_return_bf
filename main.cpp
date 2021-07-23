#include <winsock.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <iomanip>

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

/*
void to_json(json& j, const Mario& m) {
	j = json{ {"x", m.x}, {"z", m.z} };
}

void to_json(json& j, const Bully& b) {
	j = json{ {"x", b.x}, {"y", b.y}, {"z", b.z}, {"spd", b.spd}, {"yaw", b.yaw} };
}

void to_json(json& j, const Platform& p) {
	//if we need more variables, update this
	j = json{ {"action", p.action}, {"x", p.x} };
}

void to_json(json& j, const Start& s) {
	j = json{ {"Mario", s.mario}, {"Bullies", s.bullies}, {"Platform", s.platform} };
}

int main(int argc, char* argv[]) {
	json j;

	vector<Start> starts;

	Mario mario = Mario(-2250.1f, -715);
	vector<Bully> bullies;
	bullies.push_back(Bully(-2289.91f, -2950.0, -731.34f, 50000000.0, 16384));
	bullies.push_back(Bully(-2239.0, -2950.0, -573.61f, 50000000.0, 0));
	bullies.push_back(Bully(-1719.7f, -2950.0, -461.0, 14063606.0, -32768));

	Platform platform = Platform(2, -1331.77f);

	starts.push_back(Start(mario, bullies, platform));

	j = starts;

	cout << setw(4) << j << endl;

	return 0;
}*/

int main(int argc, char* argv[]) {
	vector<Slot> backups;

	backups.push_back(game.alloc_slot());
	backups.push_back(game.alloc_slot());
	backups.push_back(game.alloc_slot());

	vector<Inputs> m64 = load_m64("Debug\\BitFS_poc_final_6.m64");

	sync_game(m64, &backups[0]);

	calc_next_node(false, &backups[0], true, 133, &backups[1], &backups[2]);

	/*
	vector<Start> starts = read_starts("starts.json");

	for (int i = 0; i < starts.size(); i++) {
		printf("Solutions for Start #%d\n", i);

		vector<Slot> backups = starts[i].set_start(&game, "Debug\\BitFS_poc_final_6.m64", "Debug\\sm64_jp.dll");

		fprintf(stderr, "starting brute force\n");

		calc_next_node(false, &backups[0], true, 133, &backups[1], &backups[2]);

		printf("\n");

		fprintf(stderr, "finished\n\n");
	}*/

	return 0;
}
#pragma once

#include "json.hpp"

// for convenience
using json = nlohmann::json;

#include "Game.h"
#include "Inputs.h"
#include "Macros.h"
#include "Movement.h"

#ifndef START_H
#define START_H

class Mario {
public:
	float x;
	float z;

	Mario(float mX, float mZ) {
		x = mX;
		z = mZ;
	}

	Mario() {
	}
};

class Bully {
public:
	float x;
	float y;
	float z;
	float spd;
	uint16_t yaw;

	Bully(float bX, float bY, float bZ, float bSpd, uint16_t bYaw) {
		x = bX;
		y = bY;
		z = bZ;
		spd = bSpd;
		yaw = bYaw;
	}

	Bully() {
	}
};

class Platform {
public:
	uint32_t action;
	float x;

	Platform(uint32_t pAction, float pX) {
		action = pAction;
		x = pX;
	}

	Platform() {
	}
};

class Start
{
public:
	Mario mario;
	vector<Bully> bullies;
	Platform platform;

	Start(Mario m, vector<Bully> b, Platform p) {
		mario = m;
		bullies = b;
		platform = p;
	}

	Start() {
	}

	vector<Slot> set_start(Game* game, const char* filename, const char* dll_filename);
};

void sync_game(vector<Inputs> m64, Slot* backup);
vector<Inputs> load_m64(const char* filename);
vector<Start> read_starts(const char* filename);

#endif
#pragma once

#include "Game.h"
#include "Inputs.h"
#include "Macros.h"
#include "Movement.h"

#ifndef START_H
#define START_H

class Start
{
public:
	float marioX;
	float marioY;
	float marioZ;

	float bullyX;
	float bullyY;
	float bullyZ;
	uint16_t bullyYaw;
	float bullySpd;

	uint32_t platAction;
	float platX;
	float platZ;

	Start(float mX, float mY, float mZ, float bX, float bY, float bZ, uint16_t bYaw, float bSpd, uint32_t pAction, float pX, float pZ) {
		marioX = mX;
		marioY = mY;
		marioZ = mZ;
		
		bullyX = bX;
		bullyY = bY;
		bullyZ = bZ;
		bullyYaw = bYaw;
		bullySpd = bSpd;

		platAction = pAction;
		platX = pX;
		platZ = pZ;
	}

	Start() {
	}

	vector<Slot> set_start(Game* game, const char* filename, const char* dll_filename);
};

vector<Inputs> load_m64(const char* filename);
vector<Start> read_starts(const char* filename);

#endif
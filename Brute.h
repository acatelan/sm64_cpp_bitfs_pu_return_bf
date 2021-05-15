#pragma once
#include <cstdint>
#include <functional>

#include "Slot.h"

#ifndef BRUTE_H
#define BRUTE_H

void calc_next_node(bool isLeaf, Slot* saveState, bool recurse = false, int16_t startIndex = 0, Slot* saveStateTemp = NULL, Slot* saveStateNext = NULL);
bool input_precheck(float* marioFloorNormalX, float* marioFloorNormalY, float* marioFloorNormalZ, int16_t* marioFloorType);
void update_mario_state(float x, float y, float z, float hSpd, int16_t fYaw);
bool check_if_pos_in_main_universe(float x, float z);
bool check_freefall_outcome(int16_t input_x, int16_t input_y, int16_t fYaw, bool input_matters, bool isLeaf, bool recurse, Slot* saveStateTemp, Slot* saveStateNext, std::function<void(bool, Slot*)> execute_recursion);

#endif


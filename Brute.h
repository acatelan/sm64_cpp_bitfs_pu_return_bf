#pragma once
#include <cstdint>

#include "Slot.h"

#ifndef BRUTE_H
#define BRUTE_H

void calc_next_node(bool isLeaf, Slot* saveState, bool recurse = false, int16_t startIndex = 0, Slot* saveStateTemp = NULL, Slot* saveStateNext = NULL);

#endif


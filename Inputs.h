#pragma once
#include <windows.h>

#include <stdint.h>

#ifndef INPUTS_H
#define INPUTS_H

class Inputs {
public:
	uint16_t buttons;
	int8_t stick_x;
	int8_t stick_y;

	Inputs(uint32_t butts, int8_t xStick, int8_t yStick) {
		buttons = butts;
		stick_x = xStick;
		stick_y = yStick;
	}
};

#endif
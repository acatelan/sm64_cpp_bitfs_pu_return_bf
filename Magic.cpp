#include "Magic.h"
#include "Trig.h"

int16_t atan2_lookup(float z, float x) {
	int16_t angle = 0;

	if (x == 0) {
		angle = gArctanTable[0];
	}
	else {
		angle = gArctanTable[uint16_t(float(float(z / x) * 1024.0 + 0.5))];
	}

	return angle;
}

int16_t atan2s(float z, float x) {
	int16_t angle = 0;

	if (x >= 0) {
		if (z >= 0) {
			if (z >= x) {
				angle = atan2_lookup(x, z);
			}
			else {
				angle = 0x4000 - atan2_lookup(z, x);
			}
		}
		else {
			z = -z;

			if (z < x) {
				angle = 0x4000 + atan2_lookup(z, x);
			}
			else {
				angle = 0x8000 - atan2_lookup(x, z);
			}
		}
	}
	else {
		x = -x;

		if (z < 0) {
			z = -z;

			if (z >= x) {
				angle = 0x8000 + atan2_lookup(x, z);
			}
			else {
				angle = 0xC000 - atan2_lookup(z, x);
			}
		}
		else {
			if (z < x) {
				angle = 0xC000 + atan2_lookup(z, x);
			}
			else {
				angle = -atan2_lookup(x, z);
			}
		}
	}

	return ((angle + 32768) % 65536) - 32768;
}

bool check_if_inbounds(float x, float z) {
	float x_mod = fmodf(x + 32768, 65536) - 32768;
	float z_mod = fmodf(z + 32768, 65536) - 32768;

	if (abs(x_mod) < 8192 && abs(z_mod) < 8192) {
		return true;
	}
	else {
		return false;
	}
}
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <map>
#include <utility>
#include <set>

#include "Brute.h"
#include "Game.h"
#include "Macros.h"
#include "Movement.h"
#include "Trig.h"
#include "Magic.h"
#include "Inputs.h"

void calc_next_node(bool isLeaf, Slot* saveState, bool recurse, int16_t startIndex, Slot* saveStateTemp, Slot* saveStateNext) {
	/*start with hacked position
	* game.load_state(saveState);
	* for (int i = 0; i < 30; i++) {
	*     *marioX(game) = 1434318.625;
	*	  *marioY(game) = 2972.337646484375;
	*	  *marioZ(game) = 6946403.0;
	*	  *marioHSpd(game) = -160000;
	*	  *marioAction(game) = 0x04000440;
	*	  set_inputs(game, Inputs(0b0000000000010000, 0, 100));
	*	  game.advance_frame();
	* marioX(game) = 1434318.625;
	* *marioY(game) = 2972.337646484375;
	* *marioZ(game) = 6946403.0;
	* *marioHSpd(game) = -36758512.0;
	* *marioAction(game) = 0x04000440;
	* game.save_state(saveState);
	*/

	//favor angles facing the real universe
	game.load_state(saveState);
	float fyaw_to_main_uni = atan2(*marioX(game), *marioZ(game)) * 32768 / M_PI;
	fyaw_to_main_uni = fyaw_to_main_uni - fmodf(fyaw_to_main_uni, 16);

	int16_t hau_index = startIndex;
	int hau_offset_sign = 1;

	/*
	printf("marioX - %.8f, marioX^2 - %.8f\n", *marioX(game), pow(*marioX(game), 2));
	printf("marioY - %.8f, marioY^2 - %.8f\n", *marioY(game), pow(*marioY(game), 2));
	printf("marioZ - %.8f, marioZ^2 - %.8f\n", *marioZ(game), pow(*marioZ(game), 2));
	*/

	if (isLeaf == false) {
		printf("starting dist: %.8f\n", sqrt(pow(*marioX(game), 2) + pow(*marioY(game), 2) + pow(*marioZ(game), 2)));
		printf("starting yaw: %.1f\n", fyaw_to_main_uni - fmodf(fyaw_to_main_uni, 16));
	}

	map<pair<int8_t, int8_t>, pair<int16_t, float>> input_yawmag_map;

	while (true) {
		int16_t fYaw = int(fyaw_to_main_uni) + 16 * hau_index * hau_offset_sign;
		fYaw = ((fYaw + 32768) % 65536) - 32768;

		if (fYaw == int(fyaw_to_main_uni) && hau_index > 0) {
			break;
		}

		if (isLeaf == false) {
			printf("updating fYaw to %d\n", fYaw);
		}

		//check if Mario moves directly into freefall
		bool is_freefall_angle = false;
		game.load_state(saveState);
		float hSpd = *marioHSpd(game);
		float x = *marioX(game);
		float y = *marioY(game);
		float z = *marioZ(game);

		float* marioFloorNormalX = (float*)(*marioFloorPtr(game) + 0x1C);
		float* marioFloorNormalY = (float*)(*marioFloorPtr(game) + 0x20);
		float* marioFloorNormalZ = (float*)(*marioFloorPtr(game) + 0x24);
		int16_t* marioFloorType = (int16_t*)(*marioFloorPtr(game) + 0x0);
		uint64_t* marioAreaCameraPtr = (uint64_t*)(*marioAreaPtr(game) + 0x48);
		int16_t* marioAreaCameraYaw = (int16_t*)(*marioAreaCameraPtr + 0x2);
		int16_t camYaw = *marioAreaCameraYaw;

		*marioFYaw(game) = fYaw;
		set_inputs(game, Inputs(0b0000000000010000, 0, 0));
		game.advance_frame();

		if (*marioAction(game) == 0x0100088C) { //freefall
			is_freefall_angle = true;

			while (true) {
				set_inputs(game, Inputs(0b0000000000010000, 0, 0));
				game.advance_frame();

				if (abs(*marioX(game)) < 10000.0 && abs(*marioZ(game)) < 10000.0) {
					printf("MAIN MAP: %.8f %.8f %.8f %d", *marioX(game), *marioY(game), *marioZ(game), fYaw);
					fprintf(stderr, "found something\n");
				}

				if (abs(*marioHSpd(game)) < 1000.0) {
					break;
				}

				if (*marioAction(game) == 0x04000471) { //freefall land
					game.advance_frame();

					if (*marioAction(game) == 0x04000471) { //freefall land
						if (isLeaf == false) {
							float new_fyaw_to_main_uni = float(atan2(*marioX(game), *marioZ(game)) * 32768.0 / M_PI);
							new_fyaw_to_main_uni = new_fyaw_to_main_uni - fmodf(new_fyaw_to_main_uni, 16);

							printf("%.8f %d\n", *marioY(game), fYaw);
							printf("distance to main uni: %.8f\n", sqrt(pow(*marioX(game), 2) + pow(*marioY(game), 2)));
							printf("yaw to main uni: %.8f\n", new_fyaw_to_main_uni);

							if (recurse == true) {
								//test if this node will return to main map
								game.save_state(saveStateTemp);

								//allow camera yaw to stabilize
								for (int i = 0; i < 10; i++) {
									set_inputs(game, Inputs(0b0000000000010000, 0, 0));
									*marioActionTimer(game) = 1;
									game.advance_frame();
								}

								*marioAction(game) = 0x04000440;
								game.save_state(saveStateNext);
								calc_next_node(true, saveStateNext);
								game.load_state(saveStateTemp);
							}
						}
					}

					break;
				}
			}
		}

		if (is_freefall_angle == false) {
			set<pair<int16_t, float>> yawmags_tested;

			for (int16_t input_x = -128; input_x < 128; input_x++) {
				for (int16_t input_y = -128; input_y < 128; input_y++) {
					//avoid testing redundant inputs
					if (input_yawmag_map.count(pair<int8_t, int8_t>(input_x, input_y)) == 1 && yawmags_tested.count(input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)]) == 1) {
						continue;
					}

					//add to mapping on first hau iteration
					*marioAreaCameraYaw = camYaw;

					if (input_yawmag_map.count(pair<int8_t, int8_t>(input_x, input_y)) == 0) {
						calc_intended_yawmag(input_x, input_y, *marioAreaCameraYaw);
						input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)] = pair<int16_t, float>(*marioIntYaw(game), *marioIntMag(game));

						if (yawmags_tested.count(input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)]) == 1) {
							continue;
						}
					}

					//input inbounds precheck
					*marioIntYaw(game) = int(input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)].first);
					*marioIntMag(game) = float(input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)].second);
					*marioFYaw(game) = fYaw;
					*marioMYaw(game) = fYaw;
					*marioHSpd(game) = hSpd;
					*marioXSlidSpd(game) = *marioHSpd(game) * gSineTable[(uint16_t)(*marioFYaw(game)) >> 4];
					*marioZSlidSpd(game) = *marioHSpd(game) * gCosineTable[(uint16_t)(*marioFYaw(game)) >> 4];

					*marioX(game) = x;
					*marioY(game) = y;
					*marioZ(game) = z;

					//mark input as tested
					yawmags_tested.insert(pair<int16_t, float>(*marioIntYaw(game), *marioIntMag(game)));

					update_sliding(*marioFloorNormalX, *marioFloorNormalZ, *marioFloorType);
					*marioX(game) = float(*marioX(game) + *marioXSlidSpd(game) * *marioFloorNormalY / 4.0);
					*marioZ(game) = float(*marioZ(game) + *marioZSlidSpd(game) * *marioFloorNormalY / 4.0);

					if (check_if_inbounds(*marioX(game), *marioZ(game)) == true) {
						//printf("Step 1 Predicted: %.8f %.8f %d %d\n", *marioX(game), *marioZ(game), *marioFYaw(game), *marioMYaw(game));

						*marioX(game) = float(*marioX(game) + *marioHSpd(game) * gSineTable[(uint16_t)(*marioFYaw(game)) >> 4] / 4.0);
						*marioZ(game) = float(*marioZ(game) + *marioHSpd(game) * gCosineTable[(uint16_t)(*marioFYaw(game)) >> 4] / 4.0);

						if (check_if_inbounds(*marioX(game), *marioZ(game)) == false) {
							continue;
						}
						//printf("Step 2 Predicted: %.8f %.8f %d %d\n", *marioX(game), *marioZ(game), *marioFYaw(game), *marioMYaw(game));
					}
					else {
						continue;
					}

					game.load_state(saveState);
					*marioFYaw(game) = fYaw;
					*marioMYaw(game) = fYaw;

					*marioXSlidSpd(game) = *marioHSpd(game) * gSineTable[(uint16_t)(*marioFYaw(game)) >> 4];
					*marioZSlidSpd(game) = *marioHSpd(game) * gCosineTable[(uint16_t)(*marioFYaw(game)) >> 4];

					set_inputs(game, Inputs(0b0010000000010000, input_x, input_y));
					game.advance_frame();

					//printf("%d %d\n", *marioFYaw(game) + 32768, *marioMYaw(game));
					//printf("Step 1 Actual: %.8f %.8f %d %d\n", *marioX(game), *marioZ(game), *marioFYaw(game), *marioMYaw(game));

					if (abs(*marioHSpd(game)) < 1000.0) {
						continue;
					}
					else if (*marioAction(game) == 0x0100088C) {
						/*
						* if (input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)] != pair<int16_t, float>(*marioIntYaw(game), *marioIntMag(game))) {
						*     printf("(%d, %.8f) (%d, %.8f)\n", input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)].first, input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)].second, *marioIntYaw(game), *marioIntMag(game));
						*     printf("%#X %#X\n", input_yawmag_map[pair<int8_t, int8_t>(input_x, input_y)].first - 1187, *marioIntYaw(game) - 1187);
						*     printf("%d %d\n", input_x, input_y);
						* }
						*/
						while (true) {
							set_inputs(game, Inputs(0b0000000000010000, 0, 0));
							game.advance_frame();

							//printf("Step 2+ Actual: %.8f %.8f %d %d\n", *marioX(game), *marioZ(game), *marioFYaw(game), *marioMYaw(game));
							if (abs(*marioX(game)) < 10000.0 && abs(*marioZ(game)) < 10000.0) {
								printf("MAIN MAP: %.8f %.8f %.8f %d %d %d\n", *marioX(game), *marioY(game), *marioZ(game), fYaw, input_x, input_y);
								fprintf(stderr, "found something\n");
							}

							if (abs(*marioHSpd(game)) < 1000.0) {
								break;
							}

							if (*marioAction(game) == 0x04000471) {
								game.advance_frame();

								if (*marioAction(game) == 0x04000471) { //freefall land
									if (isLeaf == false) {
										float new_fyaw_to_main_uni = float(atan2(*marioX(game), *marioZ(game)) * 32768.0 / M_PI);
										new_fyaw_to_main_uni = new_fyaw_to_main_uni - fmodf(new_fyaw_to_main_uni, 16);

										printf("%.8f %d %d %d\n", *marioY(game), fYaw, input_x, input_y);
										printf("distance to main uni: %.8f\n", sqrt(pow(*marioX(game), 2) + pow(*marioY(game), 2) + pow(*marioZ(game), 2)));
										printf("yaw to main uni: %.8f\n", new_fyaw_to_main_uni);

										if (recurse == true) {
											//test if this node will return to main map
											game.save_state(saveStateTemp);

											//allow camera yaw to stabilize
											for (int i = 0; i < 10; i++) {
												set_inputs(game, Inputs(0b0000000000010000, 0, 0));
												*marioActionTimer(game) = 1;
												game.advance_frame();
											}

											*marioAction(game) = 0x04000440; //walking
											game.save_state(saveStateNext);
											calc_next_node(true, saveStateNext);
											game.load_state(saveStateTemp);
										}
									}
								}

								break;
							}
						}
					}
					else if (isLeaf == false) {
						printf("wtf %u %d %d\n", *marioAction(game), fYaw, *marioFYaw(game));
					}
				}
			}
		}

		if (isLeaf == false) {
			printf("tested all inputs for fYaw %d\n", fYaw);
		}

		if (hau_offset_sign == 1 && hau_index != 0) {
			hau_offset_sign = -1;
		}
		else {
			hau_offset_sign = 1;
			hau_index += 1;
		}

		if (isLeaf == true && hau_index > 3) {
			printf("Tested L2 node.\n");
			return;
		}
	}

	printf("fin\n");
	return;
}
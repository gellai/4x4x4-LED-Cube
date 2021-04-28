/*
 * 4x4x4_led_cube.h
 * ATmega328
 *
 *  Created on: 14 Jun 2020
 *  Author: gellai.com
 */

#ifndef MAIN_H_
#define MAIN_H_

#define FF_OE 		0x01			// B00000001 - Flip-Flops On/Off
#define FF_CLK_1 	0x02			// B00000010 - Flip-Flop 1 Clock
#define FF_CLK_2 	0x04			// B00000100 - Flip-Flop 2 Clock
#define FF_CLK_MASK	0x06			// B00000110 - Flip-Flop Clock Mask

#define LAYER_E 			0x04	// B00000100 - Layer On/Off
#define LAYER_SELECT_MASK 	0x03	// B00000011 - Layer Mask

volatile unsigned char cube_buffer[4][4];
volatile int current_layer;

void set_all_voxels(unsigned char mask);
void set_voxel(int z, int y, int x);
void clear_voxel(int z, int y, int x);
void clear_voxel(int z, int y, int x);
unsigned char get_voxel(int z, int y, int x);
void set_small_cube(int z, int y, int x);

void invert_voxel(int z, int y, int x);
void layer_demo();
void layer_stepping(int speed_ms, int iteration);
void blinking(int speed_ms, int iteration);
void pulsing(int speed_ms, int iteration);
void random_voxel(int speed_ms, int iteration, int voxels);
void rain(int speed_ms, int iterations);
void cube_in_cube(int speed_ms, int iteration);
void boingboing(uint16_t iterations, int speed_ms, unsigned char mode, unsigned char drawmode);

#endif /* MAIN_H_ */

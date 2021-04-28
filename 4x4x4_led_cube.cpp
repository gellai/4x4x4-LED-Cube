/*
 * 4x4x4_led_cube.cpp
 *
 * Arduino 4x4x4 LED Cube
 * ATmega328
 *
 * Created on: 14 Jun 2020
 * Author: gellai.com
 */

#include <Arduino.h>
#include <time.h>
#include "4x4x4_led_cube.h"

/*
 * Arduino's built in setup function where
 * the pins and interrupt are configured.
 */
void setup() {
  // Data bus D0 - D7
  DDRD = 0xFF;
  PORTD = 0x00;

  // Flip-Flop control C0 - C2
  // C0: OE output (High = Off / Low = On)
  // C1: Flip-Flop 1 Clock Select
  // C2: Flip-Flop 2 Clock Select
  DDRC = 0xFF;
  PORTC = 0x01;

  // Layer Control B0 - B2
  // B0: Layer A
  // B1: Layer B
  // B2: Layer IC On/Off (High = Off / Low = On)
  DDRB = 0xFF;
  PORTB = 0x04;

  // Reset PWM
  TCCR2A = 0x00;
  TCCR2B = 0x00;

  TCCR2A |= (0x01 << WGM21);                  // CTC mode. clear counter on TCNT2 == OCR2A
  OCR2A = 10;                                 // Interrupt every 25600th cpu cycle (256*100)
  TCNT2 = 0x00;                               // start counting at 0
  TCCR2B |= (0x01 << CS22) | (0x01 << CS21);  // Start the clock with a 256 prescaler

  TIMSK2 |= (0x01 << OCIE2A);

  current_layer = 0;

  srand(time(NULL));
}

/*
 * Main loop.
 */
void loop() {
  while(true) {
    //layer_demo();
    layer_stepping(300, 30);
    blinking(500, 10);
    pulsing(150, 10);
    random_voxel(200, 30, 50);
    rain(75, 200);
    cube_in_cube(250, 30);
    cube_buffer[3][0] = 0x01;
  }
}

/*
 * VISUAL EFFECTS
 * =========================================
 */

/*
 * Cube layer speedup - never ending loop
 */
void layer_demo() {
  int z, speed_ms = 200;

  set_all_voxels(0x00);

  while(true) {
    if(speed_ms > 0) {
      for(z = 0; z < 4; z++) {
        set_all_voxels(0x00);
        cube_buffer[z][0] = 0x0F;
        cube_buffer[z][1] = 0x0F;
        cube_buffer[z][2] = 0x0F;
        cube_buffer[z][3] = 0x0F;
        delay(speed_ms);
        speed_ms -= 2;
      }
    }
    else {
      set_all_voxels(0x0F);
    }
  }
}


/*
 * Layer stepping
 */
void layer_stepping(int speed_ms, int iteration) {
  int i, l;

  for(i = 0; i < iteration; i++) {
    set_all_voxels(0x00);

    for(l = 0; l < 4; l++) {
      cube_buffer[l][0] = 0x0F;
      cube_buffer[l][1] = 0x0F;
      cube_buffer[l][2] = 0x0F;
      cube_buffer[l][3] = 0x0F;
      delay(speed_ms);
      set_all_voxels(0x00);
    }
  }
}


/*
 * Blinking the whole cube
 */
void blinking(int speed_ms, int iteration) {
  int i;

  for(i = 0; i < iteration; i++) {
    set_all_voxels(0x00);
    delay(speed_ms);
    set_all_voxels(0x0F);
    delay(speed_ms);
  }

  set_all_voxels(0x0F);
}

/*
 * Pulsing from inside to outside
 */
void pulsing(int speed_ms, int iteration) {
  int i, s;
  set_all_voxels(0x00);

  for(i = 0; i < iteration; i++) {
    // Step 0 - Blank
    // Step 1 - 4 internal voxels on
    // Step 2 - Whole cube is on
    // Step 3 - 4 internal voxels on
    // Back to step 0
    for(s=0; s < 4; s++) {
      if(s == 0) {
        set_all_voxels(0x00);
        delay(speed_ms * 2);
      }
      else if(s == 1 || s == 3) {
        set_all_voxels(0x00);
        set_small_cube(1, 1, 1);
        cube_buffer[1][1] = 0x06; // B00000110
        cube_buffer[1][2] = 0x06;
        cube_buffer[2][1] = 0x06;
        cube_buffer[2][2] = 0x06;
        delay(speed_ms);
      }
      else if(s == 2) {
        set_all_voxels(0x0F);
        delay(speed_ms * 2);
      }
    }
  }
}

/*
 * Randomly set and clear voxels
 */
void random_voxel(int speed_ms, int iteration, int voxels) {
  int i, v;

  for(i = 0; i < iteration; i++) {
    set_all_voxels(0x00);
    for(v = 0; v < voxels; v++)
      invert_voxel( rand()%4, rand()%4, rand()%4 );

    delay(speed_ms);
  }
}
/*
 * Rain
 */
void rain(int speed_ms, int iteration) {
  int rand_y, rand_x;
  int i, p, z, y;

  set_all_voxels(0x00);

  for(i = 0; i < iteration; i++) {
    rand_y = rand() % 4;
    rand_x = rand() % 4;

    if( !get_voxel(2, rand_y, rand_x) && !get_voxel(1, rand_y, rand_x) && !get_voxel(0, rand_y, rand_x) )
        set_voxel(3, rand_y, rand_x);

    for(p = 1; p < 4; p++) {
      for(z = 0; z < 3; z++) {
        for(y = 0; y < 4; y++) {
          cube_buffer[z][y] = cube_buffer[z+1][y];
        }
      }

      cube_buffer[3][0] = 0x00;
      cube_buffer[3][1] = 0x00;
      cube_buffer[3][2] = 0x00;
      cube_buffer[3][3] = 0x00;

      rand_y = rand() % 4;
      rand_x = rand() % 4;

      if( !get_voxel(2, rand_y, rand_x) && !get_voxel(1, rand_y, rand_x) && !get_voxel(0, rand_y, rand_x) )
          set_voxel(3, rand_y, rand_x);

      delay(speed_ms + (speed_ms / p));
    }

  }
}

/*
 * Randomly moving 2x2x2 cube
 */
void cube_in_cube(int speed_ms, int iteration) {
  int i;
  int z, y, x;        // Random coordinates
  int axis_z, axis_y, axis_x; // Random directions, 0 - down/back/left 1 - up/forward/right

  z = rand() % 3;
  y = rand() % 3;
  x = rand() % 3;

  for(i = 0; i < iteration; i++) {
    axis_z = rand() % 2;
    axis_y = rand() % 2;
    axis_x = rand() % 2;

    set_all_voxels(0x00);
    set_small_cube(z, y, x);
    delay(speed_ms);

    // Z Axis movement
    if(axis_z == 1) // Move up
      if((z+1) < 2)
        z++;
      else
        z--;
    else            // Move down
      if((z-1) > 0)
        z--;
      else
        z++;

    set_all_voxels(0x00);
    set_small_cube(z, y, x);
    delay(speed_ms);

    // Y Axis movement
    if(axis_y == 1)
      if((y+1) < 2)
        y++;
      else
        y--;
    else
      if((y-1) > 0)
        y--;
      else
        y++;

    set_all_voxels(0x00);
    set_small_cube(z, y, x);
    delay(speed_ms);

    // X Axis movement
    if(axis_x == 1)
      if((x+1) < 2)
        x++;
      else
        x--;
    else
      if((x-1) > 0)
        x--;
      else
        x++;
  }
}

/*
 * VOXEL FUNCTIONS
 * =========================================
 */

/*
 * Setting all voxels to be ON (0x0F) or OFF (0x00)
 */
void set_all_voxels(unsigned char mask) {
  int z, y;

  for(z = 0; z < 4; z++)
    for(y = 0; y < 4; y++)
      cube_buffer[z][y] = mask;
}

/*
 * Setting a voxel in the cube_buffer
 * z - Layers 0-3
 * y - Rows   0-3
 * x - Set one voxel by position 0-3
 */
void set_voxel(int z, int y, int x) {
  cube_buffer[z][y] |= (1 << x);
}

/*
 * Clearing a voxel in the cube_buffer
 * z - Layers 0-3
 * y - Rows   0-3
 * x - Clear one voxel by position 0-3
 */
void clear_voxel(int z, int y, int x) {
  cube_buffer[z][y] &= ~(1 << x);
}

/*
 * Returning the status of a voxel
 * from the cube_buffer
 */
unsigned char get_voxel(int z, int y, int x) {
  if( cube_buffer[z][y] & (1 << x) )
    return 0x01;
  else
    return 0x00;
}

/*
 * Invert a voxel in the cube_buffer
 */
void invert_voxel(int z, int y, int x) {
  if( cube_buffer[z][y] & (1 << x) )
    clear_voxel(z, y, x);
  else
    set_voxel(z, y, x);
}

/*
 * Set a 2x2x2 cube
 * z, y, x are coordinates, max values 2
 */
void set_small_cube(int z, int y, int x) {
  cube_buffer[z][y] = ((1 << x) | (1 << (x+1)));
  cube_buffer[z][y+1] = ((1 << x) | (1 << (x+1)));
  cube_buffer[z+1][y] = ((1 << x) | (1 << (x+1)));
  cube_buffer[z+1][y+1] = ((1 << x) | (1 << (x+1)));
}

/*
 * Timer Interrupt
 * =========================================
 */
ISR(TIMER2_COMPA_vect)
{
  // Disable Flip-Flops & Layers
  PORTC |= FF_OE;         // Flip-Flops OFF
  PORTB |= LAYER_E;       // Layers OFF

  // AXIS_Z - Setting layer first
  PORTB &= ~LAYER_SELECT_MASK;                  // Clear layer select
  PORTB |= (LAYER_SELECT_MASK & current_layer); // Set Layer

  // Setting row 1 & 2 AXIS_X
  PORTD = (0x0F & cube_buffer[current_layer][0]) | (0xF0 & (cube_buffer[current_layer][1] << 4)); // Merge row 1 & 2

  // Register data to flip-flip 1 AXIS_Y
  PORTC &= ~FF_CLK_MASK;      // Reset flip-flip selection
  PORTC |= FF_CLK_1;          // Select Flip-Flop 1

  // Setting row 3 & 4 AXIS_X
  PORTD = (0x0F & cube_buffer[current_layer][2]) | (0xF0 & (cube_buffer[current_layer][3] << 4)); // Merge row 3 & 4

  // Register data to latch 2 AXIS_Y
  PORTC &= ~FF_CLK_MASK;      // Reset flip-flop selection
  PORTC |= FF_CLK_2;          // Select Flip-Flop 2

  // Enable Layers & Flip-Flops
  PORTB &= ~LAYER_E;        // Layers ON
  PORTC &= ~FF_OE;          // Flip-Flops ON

  current_layer++;

  if(current_layer == 4)
    current_layer = 0;
}

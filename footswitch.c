/***************************************************************
License goes here. Free for all?
****************************************************************/

/*=============================================================================
footswitch.c
Pieter E. Sartain  pesartain@googlemail.com
pesartain.com
=============================================================================*/

/******************************************************************************
Project level include files.
******************************************************************************/

#include <ecog.h>
#include <registers.h>
#include <stdio.h>
#include <core_lib.h>

/******************************************************************************
Include files for public interfaces from other modules.
******************************************************************************/

/******************************************************************************
Declaration of public functions.
******************************************************************************/

/******************************************************************************
Private constants and types.
******************************************************************************/

// Button state values
enum
{
	E_OPEN = 0,
	E_DOWN,
	E_HELD,
	E_UP
};

/******************************************************************************
Declaration of static functions.
******************************************************************************/

/******************************************************************************
Global variables.
******************************************************************************/

/******************************************************************************
Module global variables.
******************************************************************************/

/******************************************************************************
Definition of API functions.
******************************************************************************/

/******************************************************************************
******************************************************************************/

int main(int argc, char * argv[])
{
	int i = 0;
	int btn_val;
	int drive_line;

	int btn_state[10];	
	int sw_state   = 0x0000;
	int btn_pushed = 0xFFFF;

	int channel    = 0x96; // channel 7
	int vel_toggle = 0x01;
	int velocity[2][10];
	
	int btn_cfg[10];	

	// Initialize button number output
	btn_cfg[0] = 0x45;
	btn_cfg[1] = 0x46;
	btn_cfg[2] = 0x47;
	btn_cfg[3] = 0x48;
	btn_cfg[4] = 0x49;

	btn_cfg[5] = 0x4A;
	btn_cfg[6] = 0x4B;
	btn_cfg[7] = 0x4C;
	btn_cfg[8] = 0x4D;
	btn_cfg[9] = 0x4E;
	
	// Initialize velocity
	for (i = 0; i<10; i++) {
		velocity[0][i] = 0x01;
		velocity[1][i] = 0x01;
	}
	
	// Initialize the starting drive line
	drive_line = 0;

	// fd.* deals with bit fields only (convenient)
	// rg.* deals with entire register (more efficient if changing a lot of stuff at once)
	
	// GPIOs 9 and 11 are the drive lines, so are set to output only
	fd.io.gp8_11_out.en9  = 1;
	fd.io.gp8_11_out.en11 = 1;
	
	// GPIOs 0-5 are the poll lines, so are set to input by disabling the output part
	rg.io.gp0_3_out = 0x8888;
	rg.io.gp4_7_out = 0x8888;

	// Configured GPIOs 0-7 to generate no interupts
	rg.io.gp0_3_cfg = 0x0000;
	rg.io.gp4_7_cfg = 0x0000;

	// main loop
	while (1)
	{	
		// Drive one line high, the other low
		if (drive_line == 0) {
			// set9; clr11
			rg.io.gp8_11_out = 0x2010;
			//drive_0_prev = btn_state_now;
		} else {
			// clr9; set11
			rg.io.gp8_11_out = 0x1020;
			//drive_1_prev = btn_state_now;
		}

		// Now see if anyone pushed a button on the high line
		sw_state = rg.io.gp0_7_sts;
		
		// Now parse the status bits to find which key was pressed
		btn_pushed = 0xFFFF;
		
		// Check each status bit and, coupled with the drive_line,
		// spit out a unique number corresponding to a switch.
		for (i = 0; i <= 8; i = i + 2) {
			btn_val = ( (sw_state >> i) & 0x003);
			if ( btn_val == 0x0000 ) {
				btn_pushed = (i + 2)/2;
				btn_pushed = btn_pushed + (drive_line*5);
				btn_pushed--;
				break;
			}
		}

		// At this point, btn_pushed contains a value from 0-9, or 0xFFFF if nothing's been pushed.

		// This is the state machine that determines various factors about the switching.
		// Like when to go up, go down, and when to do nothing.
		for (i = (drive_line*5); i < ((drive_line*5)+5); i++) {
			
			if (btn_pushed == i) {

				switch(btn_state[i]) {
				case E_OPEN:
					// Button pushed, marking for down
					btn_state[i] = E_DOWN;
					break;
				case E_DOWN:
					// This went down last cycle, so push to hold this cycle
					btn_state[i] = E_HELD;
					break;
				case E_HELD:
					// We're still leaning on this button
					btn_state[i] = E_HELD;
					break;
				case E_UP:
					// On it's way up, I changed my mind, so reset to held
					btn_state[i] = E_HELD;
					break;
				default:
					btn_state[i] = E_HELD;
					break;
				}

			} else {

				switch(btn_state[i]) {
				case E_OPEN:
					// Do nothing
					btn_state[i] = E_OPEN;
					break;
				case E_DOWN:
					// We didn't wait very long ... going up!
					btn_state[i] = E_OPEN;
					break;
				case E_HELD:
					// We've been leaning on this one for a while. Going up!
					btn_state[i] = E_UP;
					break;
				case E_UP:
					// This is already marked to go up, so reset to open
					btn_state[i] = E_OPEN;
					break;
				default:
					btn_state[i] = E_OPEN;
					break;
				}

			}
			
			// Having calculated the new state, now trigger any events
			// attached to that. This is where the messages are sent.
			
// HW version mark I:
/*
	   /|\ (wire)
  4  3  2  1  0      drive_line = 0
  *  *  *  *  *
  *  *  *  *  *
  9  8  7  6  5      drive_line = 1
*/

// HW version mark II:
/*
	   /|\ (wire)
  4  3  2  1  0      drive_line = 0
  *  *  *  *  *
  *  *  *  *  *
  9  8  7  6  5      drive_line = 1
*/

			if (btn_state[i] == E_UP) {
				if (i == 0) {
					// Channel change button
					if (channel == 0x96) {
						channel = 0x97;
					} else {
						channel = 0x96;
					}
					vel_toggle = (~vel_toggle) & 0x0001;
				} else {

					// Every other button
					duart_a_write(channel);
					duart_a_write(btn_cfg[i]);
					duart_a_write(velocity[vel_toggle][i]);
					
					if (velocity[vel_toggle][i] == 0x01) {
						velocity[vel_toggle][i] = 0x7F;
					} else {
						velocity[vel_toggle][i] = 0x01;
					}
				}
			}
	
		}

		// Switch drive lines. For convenience this effectively toggles between 1 and 0.
		drive_line = (~drive_line) & 0x0001;
	}
    
    return (0);
}

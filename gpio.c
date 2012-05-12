/***************************************************************

Date: September 8th, 2006.
Copyright (c) 2006 Cyan Technology Limited. All Rights Reserved.

Cyan Technology Limited, hereby grants a non-exclusive license
under Cyan Technology Limited's copyright to copy, modify and
distribute this software for any purpose and without fee,
provided that the above copyright notice and the following
paragraphs appear on all copies.

Cyan Technology Limited makes no representation that this
source code is correct or is an accurate representation of any
standard.

In no event shall Cyan Technology Limited have any liability
for any direct, indirect, or speculative damages, (including,
without limiting the forgoing, consequential, incidental and
special damages) including, but not limited to infringement,
loss of use, business interruptions, and loss of profits,
respective of whether Cyan Technology Limited has advance
notice of the possibility of any such damages.

Cyan Technology Limited specifically disclaims any warranties 
including, but not limited to, the implied warranties of
merchantability, fitness for a particular purpose, and non-
infringement. The software provided hereunder is on an "as is"
basis and Cyan Technology Limited has no obligation to provide
maintenance, support, updates, enhancements, or modifications.

****************************************************************/

/*=============================================================================
Cyan Technology Limited

FILE - gpio.c
    Example eCOG1 application.

DESCRIPTION
    Test GPIO pins and interrupts.

MODIFICATION DETAILS
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

// Error codes
enum
{
	E_NONE = 0,
	E_WRAP,
	E_OVFL
};

// Button state values
enum
{
	E_OPEN = 0,
	E_DOWN,
	E_HELD,
	E_UP
};

/*
typedef struct {
	int trigger;
	int note;
} config;
*/

/******************************************************************************
Declaration of static functions.
******************************************************************************/

void __irq_code pattern(void);
void __irq_code gpio_wr(unsigned int gpio, unsigned int out);
void __irq_code duart_a_wr(int c);

//void set_config(int idx, int trigger, int note, config* cfg);

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
NAME
    main

SYNOPSIS
    int main(int argc, char * argv[])

FUNCTION
    Sit in a loop and call function pattern when the interrupt signals.

RETURNS
    Exit code.
******************************************************************************/

int main(int argc, char * argv[])
{
	int i = 0;
	int btn_val;
	int drive_line;

	int btn_state[10];	
	int sw_state = 0x0000;
	int btn_pushed = 0xFFFF;

	int channel  = 0x96; // channel 7
	int note     = 0x45;
	int vel_toggle = 0x01;
	int velocity[2][10];
	
	int btn_cfg[10];	

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

	duart_a_write(0x99);
	duart_a_write(0x45);
	duart_a_write(0x45);

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

/******************************************************************************
NAME
    int_handler

SYNOPSIS
    void __irq_entry gpio_handler(void)

FUNCTION
    Interrupt handler for gpio.

RETURNS
    Nothing.
******************************************************************************/

void __irq_entry gpio_handler(void)
{
    volatile unsigned int btn_pressed;
	
    // Clear gpio interrupt by reading the register
    btn_pressed = rg.io.gp0_7_sts;
}

// Copy of duart_a_write called from ISR
void __irq_code duart_a_wr(int c)
//int duart_a_write(int c)
{
    // Wait for the UART to be ready to transmit
    while (0 == fd.duart.a_sts.tx_rdy)
    {
        // Do nothing
    }

    rg.duart.a_tx8 = c;
}

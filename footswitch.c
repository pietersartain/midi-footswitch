/******************************************************************************
The MIT License (MIT)
Copyright (c) 2012 Pieter Sartain

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

/*=============================================================================
footswitch.c
=============================================================================*/

/******************************************************************************
Project level include files.
******************************************************************************/

#include <ecog.h>
#include <registers.h>
#include <stdio.h>
#include <core_lib.h>

/******************************************************************************
Private constants and types.
******************************************************************************/

// Button state values
enum
{
	E_OPEN = 0,
	E_DOWN,
	E_HELD,
	E_UP,
	E_TAP,
	E_LONG
};

// Used to enter and leave the sleep state
static void power_down (void);
static void power_up (void);

/******************************************************************************
Main function / entry point
******************************************************************************/

// Main reference clock = 4.9152 MHz
// CPU clock = 4.9152 /2 /2 = 1.228 MHz (1,228,800 Hz)
// 1,228,800 / 2048 = 600 counts
// 1 ms = 1 kHz

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
	int TOP_RIGHT  = 5;
	
	int btn_cfg[10];
	
	// Initialize button number output
	btn_cfg[0] = 104;
	btn_cfg[1] = 117;
	btn_cfg[2] = 101;
	btn_cfg[3] = 111;
	btn_cfg[4] = 97;

	btn_cfg[5] = 103;
	btn_cfg[6] = 112;
	btn_cfg[7] = 46;
	btn_cfg[8] = 44;
	btn_cfg[9] = 39;
	
	// Initialize velocity and debounce state
	for (i = 0; i<10; i++) {
		velocity[0][i] = 0x01;
		velocity[1][i] = 0x01;
		
		btn_bounce[i] = 0;
	}
	
	// Initialize the starting drive line
	drive_line = 0;

	// fd.* deals with bit fields only (more convenient)
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

    // Configure operation of clocks during sleep and wakeup modes
    fd.ssm.clk_sleep_dis.uarta = 1;
    fd.ssm.clk_sleep_dis.uartb = 1;
    fd.ssm.clk_sleep_dis.timeout = 1;
    fd.ssm.clk_wake_en.uarta = 1;
    fd.ssm.clk_wake_en.uartb = 1;

    // Counter 1 is initialised for a 1Hz tick interrupt in configurator
    // Start counter and enable interrupt
    rg.tim.ctrl_en = TIM_CTRL_EN_CNT1_CNT_MASK;
    rg.tim.int_en1 = TIM_INT_EN1_CNT1_EXP_MASK;	
	
	// main loop
	while (1)
	{
	    power_down();
        sleep();
        power_up();
		
		// Drive one line high, the other low
		if (drive_line == 0) {
			// set9; clr11
			rg.io.gp8_11_out = 0x2010;
		} else {
			// clr9; set11
			rg.io.gp8_11_out = 0x1020;
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
		
		// Count the number of cycles to make sure the push was not just an erroneous contact
		// This counts as debounce code.
		//if ( (btn_pushed != 0xFFFF) && (++btn_bounce[btn_pushed] == 4) ) {
		//	btn_bounce[btn_pushed] = 0;
			
			// At this point, btn_pushed contains a value from 0-9, or 0xFFFF if nothing's been pushed.
	
			// This is the state machine that determines various factors about the switching.
			// Like when to go up, go down, and when to do nothing.
			for (i = (drive_line*5); i < ((drive_line*5)+5); i++) {
				
				if (btn_pushed == i) {
					//if (++btn_bounce[i] == 4) {
					//	btn_bounce[i] = 0;
					//} else {
					//	continue;
					//}

					switch(btn_state[i]) {
					case E_OPEN:
						btn_state[i] = E_DOWN;
						break;
					default:
						btn_state[i] = E_HELD;
						break;
					}
					
					//btn_state[i] = E_TAP;
	
				} else {
					btn_bounce[i] = 0;

					switch(btn_state[i]) {
					case E_HELD:
						btn_state[i] = E_UP;
						break;
					default:
						btn_state[i] = E_OPEN;
						break;
					}
					
					//btn_state[i] = E_OPEN;
	
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
  9  8  7  6  5      drive_line = 1
  *  *  *  *  *
  *  *  *  *  *
  4  3  2  1  0      drive_line = 0
*/

				if (btn_state[i] == E_DOWN) {
					if (i == TOP_RIGHT) {
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
			} // end for each button

		//} // end debounce
		

		// Switch drive lines. For convenience this effectively toggles between 1 and 0.
		drive_line = (~drive_line) & 0x0001;
	}
    
    return (0);
}

static void power_down(void)
{
    // Make sure the DUART has finished sending any pending character
    while (fd.duart.a_sts.tx_act || fd.duart.b_sts.tx_act)
        ;
    
    // Set cpu clock to 16kHz
    ssm_cpu_clk_config(SSM_LOW_REF, 7, 7);
    // Disable high ref oscillator and high pll to save power
    rg.ssm.clk_dis = SSM_CLK_DIS_HIGH_OSC_MASK | SSM_CLK_DIS_HIGH_PLL_MASK;
}

static void power_up(void)
{
    // Enable high ref oscillator and high pll
    rg.ssm.clk_en = SSM_CLK_EN_HIGH_OSC_MASK | SSM_CLK_EN_HIGH_PLL_MASK;
    // Set cpu clock to 25MHz
    ssm_cpu_clk_config(SSM_HIGH_PLL, 7, 6);
    // Set evening bit to enable next sleep
    rg.ssm.ex_ctrl = SSM_EX_CTRL_EVENING_MASK;
}

void __irq_entry tick_handler(void) {
    // Clear interrupt status
    fd.tim.int_clr1.cnt1_exp = 1;
    // Set morning bit to wake up cpu
    rg.ssm.ex_ctrl = SSM_EX_CTRL_MORNING_MASK;
}

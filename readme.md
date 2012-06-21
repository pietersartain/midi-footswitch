# midi footswitch

Embedded code to drive a custom built midi footswitch.

The interesting code is in footswitch.c, the rest is support for the [Cyan Technologies](http://www.cyantechnology-ir.com/html/about/company_history.asp) eCog1k eval board I used as the brains of the project.

This code is used as part of the MarkII pedal, which provides midi directly out from the pedal.

## Theory of operation

* Connect 10 switches as a matrix to 7 GPIO: 2 'outputs' and 5 'inputs'.So each switch, when depressed, makes a connection between one output and one input GPIO.
* Designate the 2 output that each connect 5 switches the polling lines, A and B.
* Drive A high and B low, then poll the other 5 GPIO to look for a high value, record that in software.
* Drive B high and A low, then poll again, and record that as a different switch.
* Abstract away the polling into a pseudo-event driven state machine that lets you attach MIDI outputs to a particular button state (OPEN, DOWN, HELD, UP).

Most of the buttons are an on/off toggle, where 'on' is a midi note at 127 velocity and 'off' is the same note at velocity 1. The exception to this is the top-right switch (button number 5 with the MarkII hardware), which changes the midi channel the note is sent from between channel 7 and 8, providing my pedal with two 'banks' of 9 switches.

## Improvements/TODOs

* Slow down the clock. There's no need to run it at full speed, my feet aren't that fast.
* Add some LEDs to the hardware and drive the on/off state of each button.
* Replace the eCog1k eval board with something more appropriate, like an MSP430 or an AVR.

## References / Resources

Sending MIDI messages is done simply by soldering some extra components onto the proper socket, and hooking that up to a serial driver and a 5v line.

* [Sending MIDI from an Arduino][1]
* [Wiring a MIDI interface][2]
* [MIDI message reference][3]

[1]: http://itp.nyu.edu/physcomp/Labs/MIDIOutput
[2]: http://www.tigoe.net/pcomp/code/communication/midi/
[3]: http://www.cs.cf.ac.uk/Dave/Multimedia/node158.html#SECTION04133000000000000000
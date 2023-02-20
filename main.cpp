/*	RGB_Sequence_Direction_Change
*	=============================
*
*	Function:               Cycle through a RGB LED sequence where the input of a button changes the direction of the sequence
*	Circuit Schematic No.:  1 : https://github.com/ELECXJEL2645/Circuit_Schematics
*	Required Libraries:     
*
*	Authored by:            Dr Craig Evans
*	Date:                   
*	Collaberators:          Andrew Knowles
*				            Dr Tim Amsdon
*	Version:                1.0
*	Revision Date:          02/2023
*	MBED Studio Version:    1.4.1
*	MBED OS Version:        6.12.0
*	Board:	                NUCLEO L476RG	*/
 
#include "mbed.h"
 
// defines directions as 0/1. Note UPPERCASE
#define UP 0
#define DOWN 1
 
//led colours    R      G      B
BusOut LEDS(PA_15, PA_14, PA_13); //define output bus pins 
InterruptIn Joystick_Button(PC_12);
 
// array of states in the FSM, each element is the output of the counter
// set the output in binary to make it easier, 1 is LED on, 0 is LED off
int g_fsm[4] = {0b000,0b001,0b010,0b100};
 
// flag - must be volatile as changes within ISR
// g_ prefix makes it easier to distinguish it as global
volatile int g_Joystick_Button_flag = 0;

void init_buttons();
// Button A interrupt service routine
void Joystick_Button_isr();
void init_led();
 
int main(){
	init_buttons();
	init_led();
	
    // Button A has a pull-up resistor, so the pin will be at 3.3 V by default
    // and fall to 0 V when pressed. We therefore need to look for a falling-edge
    // on the pin to fire the interrupt
    Joystick_Button.fall(&Joystick_Button_isr);
  
    // set inital state
    int state = 0;
    // set initial direction
    int direction = UP;
 
    while(1){
        // check if flag i.e. interrupt has occured
        if (g_Joystick_Button_flag) {
            g_Joystick_Button_flag = 0;  // if it has, clear the flag
 
            // swap direction when button has been pressed
            // (could just use ! but want this to be explicit to aid understanding)
            if(direction == UP){
                direction = DOWN;
            }else{
                direction = UP;
            }
        }
 
        LEDS = g_fsm[state];  // output current state
 
        // check which state we are in and see which the next state should be next depending on direction
        switch(state) {
            case 0:
                switch(direction) {
                    case UP:
                        state = 1;
                        break;
                    case DOWN:
                        state = 3;
                        break;
                }
                break;
				
            case 1:
                switch(direction) {
                    case UP:
                        state = 2;
                        break;
                    case DOWN:
                        state = 0;
                        break;
                }
                break;
				
            case 2:
                switch(direction) {
                    case UP:
                        state = 3;
                        break;
                    case DOWN:
                        state = 1;
                        break;
                }
                break;
				
            case 3:
                switch(direction) {
                    case UP:
                        state = 0;
                        break;
                    case DOWN:
                        state = 2;
                        break;
                }
                break;
				
            default:  // default case
                error("Invalid state!");  //invalid state - call error routine
                state = 0;
                break;
        }
        ThisThread::sleep_for(500ms);
    }
}
 
// Button A event-triggered interrupt
void Joystick_Button_isr(){
    g_Joystick_Button_flag = 1;   // set flag in ISR
}

void init_buttons(){
	// Set the joystick button to internal pull-up resistor
    Joystick_Button.mode(PullUp);
}

void init_led(){
    LEDS.write(0);   //turn off all leds by writing the decimal equivalent of 0b111
}

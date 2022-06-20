/*  FSM Up Down counter Example
*   Operate an RGB LED using a finite state machine with a button input 
*	where the input from the button changes the direction of the FSM.
*
*   Board: NUCLEO L476RG
*   Author: Dr Craig Evans
*	Editied: Andrew Knowles 2022
*	MBED OS: 6.14
*/
 
#include "mbed.h"
 
// defines directions as 0/1. Note UPPERCASE
#define UP 0
#define DOWN 1
 
//led colours    R      G      B
BusOut RGB_LED(PA_15, PA_14, PA_13); //define output bus pins 
InterruptIn buttonA(PC_12);
 
// array of states in the FSM, each element is the output of the counter
// set the output in binary to make it easier, 1 is LED on, 0 is LED off
int g_fsm[4] = {0b000,0b001,0b010,0b100};
 
// flag - must be volatile as changes within ISR
// g_ prefix makes it easier to distinguish it as global
volatile int g_buttonA_flag = 0;

void init_buttons();
// Button A interrupt service routine
void buttonA_isr();
void init_led();
 
int main(){
	init_buttons();
	init_led();
	
    // Button A has a pull-down resistor, so the pin will be at 0 V by default
    // and rise to 3.3 V when pressed. We therefore need to look for a rising edge
    // on the pin to fire the interrupt
    buttonA.rise(&buttonA_isr);
  
    // set inital state
    int state = 0;
    // set initial direction
    int direction = UP;
 
    while(1){
        // check if flag i.e. interrupt has occured
        if (g_buttonA_flag) {
            g_buttonA_flag = 0;  // if it has, clear the flag
 
            // swap direction when button has been pressed
            // (could just use ! but want this to be explicit to aid understanding)
            if(direction == UP){
                direction = DOWN;
            }else{
                direction = UP;
            }
        }
 
        RGB_LED = g_fsm[state];  // output current state
 
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
void buttonA_isr(){
    g_buttonA_flag = 1;   // set flag in ISR
}

void init_buttons(){
	// since Button A has an external pull-down, we should disable to internal pull-down
    // resistor that is enabled by default using InterruptIn
    buttonA.mode(PullNone);
}

void init_led(){
    RGB_LED.write(0);   //turn off all leds by writing the decimal equivalent of 0b111
}


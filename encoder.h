#ifndef _ENCODER_H_
#define _ENCODER_H_
	
#define MIN 0
#define MAX 999
#define pin_A 6
#define pin_B 5
#define PIN PIND //or PINC or PINB

volatile uint8_t RON_mode = 0;
volatile uint8_t A_state = 0;
volatile uint8_t B_state = 0;
volatile int R_count = 0;	
	
void encoder_rotary(void)
{

	if (!(PIN & (1<<pin_B)) && (PIN & (1<<pin_A)) && !B_state) {  // detect direction
		B_state = 0;
		A_state = 1;
	}

	if (!(PIN & (1<<pin_A)) && (PIN & (1<<pin_B)) && !A_state) {  // detect direction
		A_state = 0;
		B_state = 1;
	}

	if ((PIN & (1<<pin_B)) && (PIN & (1<<pin_A))) {  //both are zero = reset
		A_state = 0;
		B_state = 0;
	}


	if (!(PIN & (1<<pin_B)) && !(PIN & (1<<pin_A))) {  //check if encoder movment continues
		
		if (A_state) {
			if (R_count < MAX)
			{
				R_count++;
			}
			A_state = 0;  //reset
		}

		if (B_state) {
			if ((R_count > MIN) | (RON_mode && (R_count > MIN-1)))
			{
				R_count--;			  
			}
			B_state = 0;  // reset
		}
	}
}


#endif

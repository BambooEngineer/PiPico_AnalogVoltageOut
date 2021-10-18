/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"








/*static int fade = 0;

void on_pwm_wrap() {
    //static int fade = 0;
    static bool going_up = true;
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(0));

    if (going_up) {
        fade=fade+1; // this rate can change frequency but not by alot  
        if (fade > 255) {
            fade = 255;
            going_up = false;
        }
    } else {
        fade=fade-1;
        if (fade < 0) {
            fade = 0;
            going_up = true;
        }
    }
   
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(0, fade * fade); // 255 * 255 at max 
}
*/

int DAC_wave[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224,223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208,207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192,191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,175,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160,159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144,143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128,127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

int sin_table[] = {128,133,138,143,149,154,159,164,
169,174,179,184,189,194,198,202,
207,211,215,219,222,226,229,232,
235,238,240,243,245,247,249,250,
252,253,254,254,255,255,255,255,
254,254,253,252,250,249,247,245,
243,240,238,235,232,229,226,222,
219,215,211,207,202,198,194,189,
184,179,174,169,164,159,154,149,
143,138,133,128,122,117,112,106,
101,96,91,86,81,76,71,66,
61,57,53,48,44,40,36,33,
29,26,23,20,17,15,12,10,
8,6,5,3,2,1,1,0,
0,0,0,1,1,2,3,5,
6,8,10,12,15,17,20,23,
26,29,33,36,40,44,48,53,
57,61,66,71,76,81,86,91,
96,101,106,112,117,122,128};

int tri_wav[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 240, 230, 220, 210, 200, 190, 180, 170, 160, 150, 140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10};

int value = 50;


int main() {						// 8 bits for DAC are GPIO 0-7, instead of gpio register manipulation like avr, the rp2040 can control all gpio at same time 


	gpio_init(0); // using these 3 in parallel 8 times works 
	gpio_init(1);
	gpio_init(2);
	gpio_init(3);
	gpio_init(4);
	gpio_init(5);
	gpio_init(6);
	gpio_init(7);

	gpio_init(16);
	gpio_set_dir(16, 0);
	gpio_pull_up(16);

	gpio_set_dir(0,1);
	gpio_set_dir(1,1);
	gpio_set_dir(2,1);
	gpio_set_dir(3,1);
	gpio_set_dir(4,1);
	gpio_set_dir(5,1);
	gpio_set_dir(6,1);
	gpio_set_dir(7,1);

	int i = 0;
	int I = 40;

	int mode = 0;
	bool de_b = false;
	
	//DAC_wave[i];
//DontTouchThatDial_raw[] 329158
    while(1){
		//------------------------------------------------------------------------
		// 8 bit binary individual bit values = 128, 64, 32, 16, 8, 4, 2, 1
			
		// past a certain amount of array variables the waves distort !?!?!? bypass capacitors have been our current solution
		 

			//value = DAC_wave[++i]; // array bitbanger	
			//value = tri_wav[++i];		// crappy triangle wave with 2x 104 caps 		
			//value = sin_table[++i]; 
			
			sleep_us(1); // delay -- frequency setter for rote  !!!!!! this sets the correct frequency for audio sampling = 8 microseconds

			if(mode >= 0 && mode < 5){
				value = DAC_wave[++i];
				if(i>510) i = 380; // falling sawtooth 
			}
			if(mode >= 5 && mode < 10){
				value = DAC_wave[++i];
				if(i>100) i = 0; // rising sawtooth 
			}
			if(mode >= 10 && mode < 15){
				value = sin_table[++i]; 
				if(i>151) i=0; // sinewave 
			}
			if(mode >= 15 && mode < 20){
				value = tri_wav[++i];
				if(i>49) i=0; // triangle 
			}
			if(mode >= 20){
				mode = 0;
			}

			if(!(gpio_get(16))){ // debounced and extra debounce above was needed
				if(!de_b){
					mode=mode+1;
					de_b = true;
				}
			}
			else{
				de_b = false;
			}

					if(value >= 128){		 // Decimal to GPIO Binary 
						gpio_put(7,1);		
						value = value - 128;		
					}
					else{
						gpio_put(7,0);		
					}
					/////////////////////////
					if(value >= 64 && value < 128){
						gpio_put(6,1);		
						value = value - 64;
					}
					else{
						gpio_put(6,0);		
					}
					/////////////////////////
					if(value >= 32 && value < 64){
						gpio_put(5,1);		
						value = value - 32;
					}
					else{
						gpio_put(5,0);		
					}
					///////////////////////
					if(value >= 16 && value < 32){
						gpio_put(4,1);		
						value = value - 16;
					}
					else{
						gpio_put(4,0);		
					}
					/////////////////////
					if(value >= 8 && value < 16){
						gpio_put(3,1);		
						value = value - 8;
					}
					else{
						gpio_put(3,0);		
					}
					/////////////////////
					if(value >= 4 && value < 8){
						gpio_put(2,1);		
						value = value - 4;
					}
					else{
						gpio_put(2,0);		
					}
					/////////////////////
					if(value > 1 && value < 4){
						gpio_put(1,1);		
						value = value - 2;
					}
					else{
						gpio_put(1,0);		
					}
					//////////////////////
					if(value == 1){
						gpio_put(0,1);		
						value = value - 1;
					}
					else{
						gpio_put(0,0);		
					}
					//////////////////////
				
		

		
		
		

	}
        //tight_loop_contents();
}

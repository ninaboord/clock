#include "gpio.h"
#include "timer.h"

unsigned char codes[16] = {0b00111111, 0b00110000, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001};
unsigned int segment[8] = {GPIO_PIN26, GPIO_PIN19, GPIO_PIN13, GPIO_PIN6, GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};
unsigned int buttons[2] = {GPIO_PIN2, GPIO_PIN3};
unsigned char dash = 0b01000000;
unsigned int start = 0;
// BLUE button is on the left and connected to GPIO 3, WHITE button is on the right and connected to GPIO 2

void turn_off_digit(unsigned char code) // turns off segments of a specific character
{ 
    for (int j = 0; j < 8; j++)
    {
        if ((code & (1 << j)) != 0)
        {
            gpio_write(segment[j], 0);
        }
    }
}

void display_digit(unsigned char code)
{ // displays segments of a specific character
    for (int j = 0; j < 8; j++)
    {
        if ((code & (1 << j)) != 0)
        { // find out if a specific bit is on or off through the and operation with a single 1 in a specific place
            gpio_write(segment[j], 1);
        }
    }
}

void display_refresh_loop(unsigned char code1, unsigned char code2, unsigned char code3, unsigned char code4, unsigned int ref_rate_ms)
{
    unsigned char digs[4] = {code1, code2, code3, code4};
    for (int i = 0; i < 4; i++)
    { // quickly iterate to display multiple digits at once
        gpio_write(digit[i], 1);
        display_digit(digs[i]);
        timer_delay_ms(ref_rate_ms);
        turn_off_digit(digs[i]);
        gpio_write(digit[i], 0);
        
    }
}

void go_time(unsigned int start, unsigned int extra_time) // starts clock function given time of button press and the time that the user set to the display (extension)
{
    while (1)
    {
        unsigned int seconds = (timer_get_ticks() - start) / 1000000 + extra_time; // subtract time of button press from the time that the pi was plugged in to get current time
        if (start != 0)
        {
            int dig0 = (seconds % 10); // math to find each digit based on the number of seconds
            int dig1 = (seconds % 60) / 10;
            int dig2 = (seconds / 60) % 10;
            int dig3 = (seconds / 600) % 10;
            display_refresh_loop(codes[dig3], codes[dig2], codes[dig1], codes[dig0], 0);
        }
    }
}

void setup() // configures GPIOs to input or output as needed
{
    for (int i = 0; i < 8; i++)
    { // configure segments
        gpio_set_output(segment[i]);
    }
    for (int i = 0; i < 4; i++)
    { // configure digits
        gpio_set_output(digit[i]);
    }
    gpio_set_input(buttons[0]); // configure buttons
    gpio_set_input(buttons[1]);
}

unsigned int set_display[4] = {0, 0, 0, 0}; // this holds the starting digits in order after the user sets a custom time

unsigned int get_time_s(){ // takes display and gets current time from that by translating digits to seconds
    unsigned int t_min = set_display[3] * 600;
    unsigned int min = set_display[2] * 60;
    unsigned int t_secs = set_display[1] * 10;
    unsigned int secs = set_display[0];
    return (t_min + min + t_secs + secs);
    
}

void set_time(){ //unsigned int set_display
    unsigned int d = 0;
    while(d < 4){
        display_refresh_loop(codes[set_display[3]], codes[set_display[2]], codes[set_display[1]], codes[set_display[0]], 0);
        if (gpio_read(buttons[0]) == 0){ // if white button is pressed
        timer_delay_ms(75);
        if(gpio_read(buttons[0]) == 0){ // double check if button is pressed
            set_display[d] += 1; // increment a specific display digit 
            if (set_display[d] > 9){ // go back to 0 on next button press at 9
                set_display[d] = 0;
            }
            if (d == 1 && set_display[d] > 5){ // special case for tens of seconds digit so that it does not increment past 6
                set_display[d] = 0;
            }
        }
        } 
        if(gpio_read(buttons[1]) == 0){ // if blue button is pressed
        timer_delay_ms(75);
        if(gpio_read(buttons[1]) == 0){ // double check if button is pressed
            d += 1;
        }
        } 
    }
    unsigned int extra_time = get_time_s();
    unsigned int start = timer_get_ticks();
    while(1){
        go_time(start, extra_time); // enter regular clock function
    }

}

void ready_set_time(void){ // this displays the flashing 0s that indicate that you are in "setting clock mode"
    while(1){
        display_refresh_loop(codes[0], codes[0], codes[0], codes[0], 100);
        if (gpio_read(buttons[1]) == 0){ // if white button is pressed again
            break;
        }
    }
    timer_delay_ms(500);
    set_time();
}


void clock_start() // this branches to regular clock function if white button is pressed and to the set time function if blue button is pressed
{
    display_refresh_loop(dash, dash, dash, dash, 0);
    if (gpio_read(buttons[0]) == 0) // if white button pressed
    {
        start = timer_get_ticks(); // set start to the time when the button was pressed
        go_time(start, 0);
        return;
    }
        if (gpio_read(buttons[1]) == 0) // if blue button pressed
    {
        ready_set_time();
        return;
    }
}

void display_all_digits(unsigned int digit) { // display all characters in sequence (for testing purposes only! I included this just to show that I conducted testing before continuing)
    gpio_write(digit, 1);
    for (int i = 0; i < 16; i++)
    {
        display_digit(codes[i]);
        timer_delay_ms(1000);
        turn_off_digit(codes[i]);
    }
}

void main(void)
{
    setup();
    while (1)
    {
        clock_start();
    }
}

#include "gpio.h"

struct gpio
{
    unsigned int fsel[6];
    unsigned int reservedA;
    unsigned int set[2];
    unsigned int reservedB;
    unsigned int clr[2];
    unsigned int reservedC;
    unsigned int lev[2];
};

volatile struct gpio *gpio = (struct gpio *)0x20200000;

void gpio_init(void)
{
    // no initialization required for this peripheral
}

void gpio_set_function(unsigned int pin, unsigned int function)
{
    if (function > 7 || function < 0)
    {
        return;
    }
    unsigned int FSEL_number = pin / 10;
    unsigned int fsel_copy = gpio->fsel[FSEL_number];
    unsigned int command = function << (3 * (pin % 10)); // center the function so it aligns with where to place it in FSEL
    unsigned int B = 7 << (3 * (pin % 10));
    unsigned int notB = ~B;
    unsigned int temp = fsel_copy & notB; // FSEL but with 000 where we want function
    unsigned int want = temp | command;   // put function into FSEL
    gpio->fsel[FSEL_number] = want;
}

unsigned int gpio_get_function(unsigned int pin)
{
    unsigned int FSEL_number = pin / 10;
    unsigned int fsel_copy = gpio->fsel[FSEL_number];
    unsigned int B = 7 << (3 * (pin % 10));
    unsigned int func = fsel_copy & B;             // isolates function with zeros on both sides
    unsigned int final = func >> (3 * (pin % 10)); // right shift
    return final;
}

void gpio_set_input(unsigned int pin)
{
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

void gpio_set_output(unsigned int pin)
{
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value)
{
        int reg = 0;
        if (pin > 32) 
        {
            pin = pin - 33;
            reg = 1; // if pin num greater than 32 we want either set1 or clr1
        }

        if (value == 1)
        {
            gpio->set[reg] = 1 << pin; // ask about GPIO pins, can we reset?
        }
        else
        {
            gpio->clr[reg] = 1 << pin;
        }
}

unsigned int gpio_read(unsigned int pin)
{
    int reg = 0;
    if (pin > 32)
    {
        pin = pin - 33; 
        reg = 1; // if pin num greater than 32 we want lev1
    }
    unsigned int lev_copy = gpio->lev[reg];
    unsigned int shift = 1 << pin; // find bit that we want to read
    unsigned int levPin = lev_copy & shift; // create binary number that has a 1 in pin slot if ON and is all 0s if OFF
    if (levPin !=0){ 
        return 1;
    }
    else {
        return 0;
    }
    
}

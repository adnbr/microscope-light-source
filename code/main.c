/* ---------------------------------------------------------------------
 * PWM LED Brightness control for ATtiny13.
 * Datasheet for ATtiny13: http://www.atmel.com/images/doc2535.pdf
 * 
 * Pin configuration -
 * PB1/OC0B: LED output (Pin 6)
 * PB2/ADC1: Potentiometer input (Pin 7)
 * 
 * Find out more: http://bit.ly/1d51tgB
 * -------------------------------------------------------------------*/

// 9.6 MHz, built in resonator
#define F_CPU 9600000
#define LED PB1 

// Point on the scale (0-255) that corresponds to "OFF" on the knob.
#define OFF_BELOW 25

#include <avr/io.h>

void adc_setup (void)
{
    // Set the ADC input to PB2/ADC1, left align result
    ADMUX |= (1 << MUX0) | (1 << ADLAR);

    // Set the prescaler to clock/128 & enable ADC
    // At 9.6 MHz this is 75 kHz.
    // See ATtiny13 datasheet, Table 14.4.
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

    // Start the first conversion
    ADCSRA |= (1 << ADSC);
}

int adc_read (void)
{
    // Get the 8-bit result.
    int adc_result = ADCH;

    // Start the next conversion
    ADCSRA |= (1 << ADSC);

    return adc_result;
}

void pwm_setup (void)
{
    // Set Timer 0 prescaler to clock/8.
    // At 9.6 MHz this is 1.2 MHz.
    // See ATtiny13 datasheet, Table 11.9.
    TCCR0B |= (1 << CS01);

    // Set to 'Fast PWM' mode
    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    // Clear OC0B output on compare match, upwards counting.
    TCCR0A |= (1 << COM0B1);
}

void pwm_write (int val)
{
    OCR0B = val;
}

int main (void)
{
    int brightness = 0;
    int adc_in = 0;

    // LED is an output.
    DDRB |= (1 << LED);  

    adc_setup();
    pwm_setup();

    pwm_write(0);
  
    while (1) {
        // Get the ADC value
        adc_in = adc_read();

        // Smooth the original input to stop jumping around.
        // ADC input is quite noisy, needs calming down.
        brightness = (brightness - (brightness / 4)) + (adc_in / 4);

        if (brightness < OFF_BELOW) {
            pwm_write(0);
        } else {
            pwm_write(brightness);
        }
    }
}
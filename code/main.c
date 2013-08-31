/* ---------------------------------------------------------------------
 * PWM LED Brightness control for ATtiny13.
 * 
 * Pin configuration -
 * PB1/OC0B: LED output (Pin 6)
 * PB2/ADC1: Potentiometer input (Pin 7)
 * 
 * Find out more: http://bit.ly/1d51tgB
 * -------------------------------------------------------------------*/

// 9.6 MHz, built in resonator
#define F_CPU 9600000

#define true 1
#define false 0
 
#include <avr/io.h>
#include <util/delay.h>

// Define led ext output pin on PB2
#define LED PB1 
// Point on the scale (0-1023) that corresponds to "OFF" on the knob.
#define OFF_BELOW 100

// Map function from Arduino project: http://bit.ly/18taXex
long map(long x, long i_min, long i_max, long o_min, long o_max)
{
  return (x - i_min) * (o_max - o_min) / (i_max - i_min) + o_min;
}

int main(void)
{
    int brightness = 0;
    int mapped = 0; 
    int adc_in;
   
    // LED is an output.
    DDRB |= (1 << LED);
    
    // ADC setup code
    // -----------------------------------------------------------------
    
    // Set ADC prescaler to clock/128.
    // At 9.6 MHz this is 75 kHz.
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
    
    // ADC input is PB2/ADC1.
    ADMUX |= (1 << MUX0);

    // Start ADC conversions.
    ADCSRA |= (1 << ADSC); 
    
    // PWM setup code
    // -----------------------------------------------------------------
    
    // Set Timer 0 prescaler to clock/8.
    TCCR0B |= (1 << CS01) | (1<<CS00);
    
    // Set to 'Phase Correct PWM' mode
    TCCR0A |= (1 << WGM02) | (1 << WGM00);

    TCCR0A |= (1 << COM0B1);
    
    // Set PWM to 0 brightness.
    OCR0B = 0;
    
    // Infinite loop
    while (1) {
        
        // Wait for the previous conversion to finish
        while (ADCSRA & (1 << ADSC));
        // Start a conversion
        ADCSRA |= (1 << ADSC);
        
        // Get the 10 bit ADC value
        adc_in = ADC;
       
        // Smooth the original input to stop jumping around.
        // ADC input is quite noisy, needs calming down.
        brightness = (brightness - (brightness / 4)) + (adc_in / 4);
 
        // There is a section of the original knob that is marked "OFF"
        // so if the pot is in that section, turn off.
        // The rest of the time, remap the pot so that we still have a 
        // low brightness setting.
        if (brightness < OFF_BELOW) {
            OCR0B = 0;
        } else {
            // Smooth out any rounding issues caused by a moving ADC
            // value and mapping.
            mapped = (mapped - (mapped / 4)) + 
                     (map(brightness, OFF_BELOW, 1023, 0, 255) / 4);
            OCR0B = mapped;
        }
      
    }
}
